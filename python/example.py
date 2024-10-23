import fern_py

arg_len = fern_py.Variable("arg_len", True)

a_arg = fern_py.DataStructureArg(fern_py.Array.init("a"), "data")
b_arg = fern_py.DataStructureArg(fern_py.Array.init("b"), "data")
out_arg = fern_py.DataStructureArg(fern_py.Array.init("out"), "data")

add_call = fern_py.addi_mock(a_arg, 5.0, arg_len, b_arg)
sub_call = fern_py.addi_mock(b_arg, -3.0, arg_len, out_arg)

pipeline = fern_py.Pipeline([add_call, sub_call])

pipeline.constructPipeline()
pipeline.finalize(False)

fern_py.printToFile(pipeline, "pipeline1.ir")
code = fern_py.CodeGenerator(pipeline)
fern_py.printToFile(code, "pipeline1.cpp")