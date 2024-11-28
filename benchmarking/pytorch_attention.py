import math
import torch
import torch.utils.benchmark as benchmark
from torch.nn.functional import scaled_dot_product_attention
from torch.nn.attention import SDPBackend, sdpa_kernel

# https://pytorch.org/docs/2.2/generated/torch.nn.functional.scaled_dot_product_attention.html
def unfused_scaled_dot_product_attention(query, key, value, attn_mask=None, dropout_p=0.0, is_causal=False, scale=None) -> torch.Tensor:
    # Efficient implementation equivalent to the following:
    L, S = query.size(-2), key.size(-2)
    scale_factor = 1 / math.sqrt(query.size(-1)) if scale is None else scale
    attn_bias = torch.zeros(L, S, dtype=query.dtype)
    if is_causal:
        assert attn_mask is None
        temp_mask = torch.ones(L, S, dtype=torch.bool).tril(diagonal=0)
        attn_bias.masked_fill_(temp_mask.logical_not(), float("-inf"))
        attn_bias.to(query.dtype)

    if attn_mask is not None:
        if attn_mask.dtype == torch.bool:
            attn_bias.masked_fill_(attn_mask.logical_not(), float("-inf"))
        else:
            attn_bias += attn_mask
    attn_weight = query @ key.transpose(-2, -1) * scale_factor
    attn_weight += attn_bias
    attn_weight = torch.softmax(attn_weight, dim=-1)
    attn_weight = torch.dropout(attn_weight, dropout_p, train=True)
    return attn_weight @ value

q = torch.randn(1, 1, 1024, 256)
k = torch.randn(1, 1, 1024, 256)
v = torch.randn(1, 1, 1024, 256)

unfused_out = unfused_scaled_dot_product_attention(q, k, v)
with sdpa_kernel(SDPBackend.FLASH_ATTENTION):
    fused_out = scaled_dot_product_attention(q, k, v)

assert unfused_out.allclose(fused_out, atol=1e-6)


t0 = benchmark.Timer(
    stmt='unfused_scaled_dot_product_attention(q, k, v)',
    setup='from __main__ import unfused_scaled_dot_product_attention',
    globals={'q': q, 'k': k, 'v': v}
)

t1 = benchmark.Timer(
    stmt="""
with sdpa_kernel(SDPBackend.FLASH_ATTENTION):
    scaled_dot_product_attention(q, k, v)
""",
    setup="""
from torch.nn.functional import scaled_dot_product_attention
from torch.nn.attention import SDPBackend, sdpa_kernel
""",
    globals={'q': q, 'k': k, 'v': v}
)

print(t0.timeit(1000))
print(t1.timeit(1000))