import fern_py


a = fern_py.DataStructureArg(fern_py.PyArray.init("a"), "data")
b = fern_py.DataStructureArg(fern_py.PyArray.init("b"), "data")
out = fern_py.DataStructureArg(fern_py.PyArray.init("out"), "data")

add_call = fern_py.py_add(a, b, out)

pipeline = fern_py.Pipeline([add_call])

pipeline.constructPipeline()
pipeline.finalize(False)

fern_py.printToFile(pipeline, "pygen.ir")
code = fern_py.CodeGenerator(pipeline)
printer = fern_py.PyCodeGenPrint(code)
fern_py.printToFile(printer, "pygen.py")