import torch
from typing import List
import math

def custom_backend(gm: torch.fx.GraphModule, example_inputs: List[torch.Tensor]):
    print("custom backend called with FX graph:")
    gm.graph.print_tabular()
    return gm.forward


class foo(torch.nn.Module):
    def forward(self, q, k, v):
        return torch.nn.functional.softmax((q @ torch.t(k)) / math.sqrt(q.size(dim=1))) @ v

torch._dynamo.reset()

opt_foo1 = torch.compile(foo(), dynamic=True)

print(opt_foo1(torch.randn(10, 10), torch.randn(10, 10), torch.randn(10, 10)))