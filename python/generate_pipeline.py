import torch
import torch.fx
from torch.fx import symbolic_trace
import fern_py

class FnInterface:
    def __init__(self, py_fn, cpp_fn, extra_args=()):
        self.py_fn = py_fn
        self.cpp_fn = cpp_fn
        self.extra_args = extra_args

def gen(M, torch_to_fern, name="pytorch_pipeline_attention"):
    module = M()
    symbolic_traced : torch.fx.GraphModule = symbolic_trace(module)

    py_env = {}
    cpp_env = {}
    py_fn_calls = []
    cpp_fn_calls = []

    for node in symbolic_traced.graph.nodes:
        print("NODE NAME", node.name)
        print("NODE OP", node.op)
        if node.op == "placeholder":
            py_env[node.name] = fern_py.DataStructureArg(fern_py.PyMatrix.init(node.name))
            cpp_env[node.name] = fern_py.DataStructureArg(fern_py.Matrix.init(node.name))
        elif node.op == "call_function":
            print("CALL_FN")
            print(node.target)
            print(node.args)
            if node.target in torch_to_fern:
                fern_fn_interface = torch_to_fern[node.target]
                py_evaled_args = [py_env[arg.name] if isinstance(arg, torch.fx.Node) else arg for arg in node.args]
                py_result = fern_py.DataStructureArg(fern_py.PyMatrix.init(node.name))
                print(*py_evaled_args, *fern_fn_interface.extra_args, py_result)
                py_fn_calls.append(fern_fn_interface.py_fn(*py_evaled_args, *fern_fn_interface.extra_args, py_result))
                py_env[node.name] = py_result

                cpp_evaled_args = [cpp_env[arg.name] if isinstance(arg, torch.fx.Node) else arg for arg in node.args]
                cpp_result = fern_py.DataStructureArg(fern_py.Matrix.init(node.name))
                cpp_fn_calls.append(fern_fn_interface.cpp_fn(*cpp_evaled_args, *fern_fn_interface.extra_args, cpp_result))
                cpp_env[node.name] = cpp_result
        elif node.op == "call_method":
            print("CALL_METHOD")
            print(node.target)
            print(node.name)
            py_env[node.name] = fern_py.DummyDataStructureArg(fern_py.FloatArg.init(node.name))
            cpp_env[node.name] = fern_py.DummyDataStructureArg(fern_py.FloatArg.init(node.name))

   
    py_pipeline = fern_py.Pipeline(py_fn_calls)

    py_pipeline.constructPipeline()
    py_pipeline = py_pipeline.finalize(True)

    fern_py.printToFile(py_pipeline, f"{name}_py.ir")
    code = fern_py.CodeGenerator(py_pipeline)
    printer = fern_py.PyCodeGenPrint(code)
    fern_py.printToFile(printer, f"{name}_fused.py")

    cpp_pipeline = fern_py.Pipeline(cpp_fn_calls)
    cpp_pipeline.constructPipeline()
    cpp_pipeline = cpp_pipeline.finalize(True)

    fern_py.printToFile(cpp_pipeline, f"{name}_cpp.ir")
    code = fern_py.CodeGenerator(cpp_pipeline)
    fern_py.printToFile(code, f"{name}_fused.cpp")