import torch
import torch.fx
from torch.fx import symbolic_trace
import fern_py
import operator

torch_to_fern = {
    torch.add: fern_py.torch_madd,
    operator.add: fern_py.torch_madd,
    torch.matmul: fern_py.torch_mmul,
    operator.matmul: fern_py.torch_mmul
}

shared_len = fern_py.Variable("shared_len", True)

class M(torch.nn.Module):
    def forward(self, x, y, z):
        # return torch.add(x, y)
        return x @ y @ z

module = M()
symbolic_traced : torch.fx.GraphModule = symbolic_trace(module)

# a = torch.rand(10)
# b = torch.rand(10)
# print(a)
# print(b)
# print(module(a, b))
# print(symbolic_traced.code)

env = {}
fn_calls = []

for node in symbolic_traced.graph.nodes:
    print(node.name)
    print(node.op)
    if node.op == "placeholder":
        env[node.name] = fern_py.DataStructureArg(fern_py.PyMatrix.init(node.name))
    elif node.op == "call_function":
        print(node.target)
        print(node.args)
        if node.target in torch_to_fern:
            fern_func = torch_to_fern[node.target]
            evaled_args = [env[arg.name] if isinstance(arg, torch.fx.Node) else arg for arg in node.args]
            result = fern_py.DataStructureArg(fern_py.PyMatrix.init(node.name))
            fn_calls.append(fern_func(*evaled_args, shared_len, result))
            print("FERN FUNC", fern_func)
            print("args", (*evaled_args, result))
            env[node.name] = result

print(fn_calls)

pipeline = fern_py.Pipeline(fn_calls)

pipeline.constructPipeline()
pipeline.finalize(True)

fern_py.printToFile(pipeline, "pytorch_pipeline.ir")
code = fern_py.CodeGenerator(pipeline)
fern_py.printToFile(code, "pytorch_pipeline_fused.cpp")
printer = fern_py.PyCodeGenPrint(code)
fern_py.printToFile(printer, "pytorch_pipeline_fused.py")