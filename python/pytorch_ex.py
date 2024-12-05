import torch
import torch.fx
import fern_py
import operator
from generate_torch_compile_pipeline import FnInterface, gen
import math

add_fn_interface = FnInterface(fern_py.torch_madd, fern_py.torch_madd, (fern_py.Variable("shared_len", True),))
mmul_fn_interface = FnInterface(fern_py.torch_mmul, fern_py.mmul)
transpose_fn_interface = FnInterface(fern_py.torch_transpose, fern_py.transpose)
divn_fn_interface = FnInterface(fern_py.torch_divn, fern_py.divn)
softmax_fn_interface = FnInterface(fern_py.torch_softmax, fern_py.softmax)

torch_to_fern = {
    torch.add: add_fn_interface,
    operator.add: add_fn_interface,
    torch.matmul: mmul_fn_interface,
    operator.matmul: mmul_fn_interface,
    torch.t: transpose_fn_interface,
    operator.truediv: divn_fn_interface,
    torch.nn.functional.softmax: softmax_fn_interface
}

class M(torch.nn.Module):
    def forward(self, q, k, v):
        return torch.nn.functional.softmax((q @ torch.t(k)) / math.sqrt(q.size(dim=1))) @ v

if __name__ == "__main__":
    gen(M, torch_to_fern, torch.randn((1024, 64)), torch.randn((1024, 64)), torch.randn((1024, 64)), name="torch_compile_attention")

# class N(torch.nn.Module):
#     def forward(self, x, y, z):
#         return x @ y @ z

# gen(N, torch_to_fern, "matmul")
