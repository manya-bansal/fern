import torch
import pytorch_pipeline_fused

class Tensor(torch.Tensor):
    @staticmethod
    def empty(i, j, i_len, j_len):
        return Tensor(torch.empty(i_len, j_len))

    def matrix_query(self, i, j, i_len, j_len):
        return self[i:i + i_len, j:j + j_len]
    
    def matrix_insert(self, i, j, i_len, j_len, values):
        self[i:i + i_len, j:j + j_len]

    # Wrappers around pytorch functions
    @staticmethod
    def add(a, b, out):
        return torch.add(a, b, out=out)
    
    @staticmethod
    def matmul(a, b, shared_len, out):
        return torch.matmul(a, b, out=out)

pytorch_pipeline_fused.Tensor = Tensor

my_fused_impl = pytorch_pipeline_fused.my_fused_impl
    
a = Tensor(torch.randn(5, 7))
b = Tensor(torch.randn(7, 6))
c = Tensor(torch.randn(6, 5))
print("a", a)
print("b", b)
print("c", c)
print("a @ b @ c", a @ b @ c)

out1 = Tensor(torch.empty(5, 5))
my_fused_impl(a, b, c, out1, 7, 6, 1, 1)
print("fused", out1)

out2 = Tensor(torch.empty(5, 5))
my_fused_impl(a, b, c, out2, 7, 6, 1, 2)
print("fused 2", out2)