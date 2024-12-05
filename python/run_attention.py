import torch
import torch_compile_attention_fused
import pytorch_ex
import math
import torch.utils.benchmark as benchmark


def matrix_query(self, i, j, i_len, j_len):
    return self[i:i + i_len, j:j + j_len]

def matrix_insert(self, i, j, i_len, j_len, values):
    self[i:i + i_len, j:j + j_len] = values

torch.Tensor.matrix_query = matrix_query
torch.Tensor.matrix_insert = matrix_insert

class Tensor(torch.Tensor):
    @staticmethod
    def empty(i, j, i_len, j_len):
        return torch.empty(i_len, j_len)

    # Wrappers around pytorch functions
    @staticmethod
    def add(a, b, out):
        return torch.add(a, b, out=out)
    
    @staticmethod
    def matmul(a, b, out):
        return torch.matmul(a, b, out=out)
    
    @staticmethod
    def transpose(a, out):
        out.set_(a.t())
    
    @staticmethod
    def divn(a, n, out):
        out.set_(a / n)

    @staticmethod
    def softmax(a, out):
        out.set_(torch.nn.functional.softmax(a))

torch_compile_attention_fused.Tensor = Tensor

my_fused_impl = torch_compile_attention_fused.my_fused_impl

module = pytorch_ex.M()
q = torch.randn((1024, 64))
k = torch.randn((1024, 64))
v = torch.randn((1024, 64))
# out = module(q, k, v)

unfused_attention = benchmark.Timer(
    stmt="""
module(q, k, v)
""",
    globals={'q': q, 'k': k, 'v': v, 'module': module}
)

math_attention = benchmark.Timer(
    stmt="""
with sdpa_kernel(SDPBackend.MATH):
    scaled_dot_product_attention(q, k, v)
""",
    setup="""
from torch.nn.functional import scaled_dot_product_attention
from torch.nn.attention import SDPBackend, sdpa_kernel
""",
    globals={'q': q, 'k': k, 'v': v}
)

fused_out = torch.empty((1024, 64))


# https://pytorch.org/docs/2.2/generated/torch.nn.functional.scaled_dot_product_attention.html
def unfused_scaled_dot_product_attention(query, key, value, attn_mask=None, dropout_p=0.0, is_causal=False, scale=None) -> torch.Tensor:
    # Efficient implementation equivalent to the following:
    # L, S = query.size(-2), key.size(-2)
    scale_factor = 1 / math.sqrt(query.size(-1))
    # attn_bias = torch.zeros(L, S, dtype=query.dtype)
    # if is_causal:
    #     assert attn_mask is None
    #     temp_mask = torch.ones(L, S, dtype=torch.bool).tril(diagonal=0)
    #     attn_bias.masked_fill_(temp_mask.logical_not(), float("-inf"))
    #     attn_bias.to(query.dtype)

    # if attn_mask is not None:
    #     if attn_mask.dtype == torch.bool:
    #         attn_bias.masked_fill_(attn_mask.logical_not(), float("-inf"))
    #     else:
    #         attn_bias += attn_mask
    attn_weight = query @ key.transpose(-2, -1) * scale_factor
    # attn_weight += attn_bias
    attn_weight = torch.softmax(attn_weight, dim=-1)
    # attn_weight = torch.dropout(attn_weight, dropout_p, train=True)
    return attn_weight @ value

unfused_attention2 = benchmark.Timer(
    stmt='unfused_scaled_dot_product_attention(q, k, v)',
    setup='from __main__ import unfused_scaled_dot_product_attention',
    globals={'q': q, 'k': k, 'v': v}
)

# for i in range(10):
print(unfused_attention.timeit(1000))
print(math_attention.timeit(1000))
print(unfused_attention2.timeit(1000))

tile_sz = 16
while tile_sz <= 1024:
    print("TILE SIZE", tile_sz)
    fused_attention = benchmark.Timer(
        setup="""
        import math
        """,
        stmt="""
        my_fused_impl(k, q, v, math.sqrt(q.size(dim=1)), fused_out, tile_sz, v.size(dim=0), q.size(dim=1), q.size(dim=1))
        """,
        globals={'q': q, 'k': k, 'v': v, 'fused_out': fused_out, 'tile_sz': tile_sz, 'my_fused_impl': torch_compile_attention_fused.my_fused_impl}
    )
    tile_sz *= 2
    print(fused_attention.timeit(1000))

# print(out)
# print(fused_out)
# print(torch.equal(out, fused_out))
