import fern_py
import random

arg_len = fern_py.Variable("arg_len", True)

pipeline_funcs = []

class Node:
    def __init__(self, func, concrete_fn_call, args, kwargs):
        self.func = func
        self.concrete_fn_call = concrete_fn_call
        self.args = args
        self.kwargs = kwargs
        self.dependencies = []
        self.evaled = False
        self.output = None
        self.out_ptr = None
    
    def eval(self):
        if self.evaled:
            return self.output
        self.evaled = True
        evaled_args = []
        for arg in self.args:
            if isinstance(arg, Node):
                evaled_args.append(arg.eval())
            else:
                evaled_args.append(arg)
        print("HERE1", evaled_args)
        self.output = fern_py.DataStructureArg(fern_py.Array.init(), "data")
        print("HERE2", *evaled_args, arg_len, self.output)
        func_call = self.concrete_fn_call(*evaled_args, arg_len, self.output)

        pipeline_funcs.append(func_call)
        # self.output = self.func(*evaled_args, **self.kwargs)
        return self.output

# nodes that have no dependents
roots = []

def register(concrete_fn_call):
    def inner(func):
        def wrapper(*args, **kwargs):
            print("args", args)
            global roots
            print("roots", roots)
            node = Node(func, concrete_fn_call, args, kwargs)
           
            q = []
            visited = set()
            for root in roots:
                q.append(root)
            
            while q:
                curr = q.pop(0)
                print("curr", curr)
                # check if curr is a dependency of our current node
                if curr in args:
                    node.dependencies.append(curr)
                else:
                    for dep in curr.dependencies:
                        if dep not in visited:
                            visited.add(dep)
                            q.append(dep)
            
            roots = [root for root in roots if not root in args]
            roots.append(node)

            # print("wrapper")
            # print("args", args)
            return node
        return wrapper
    return inner

@register(fern_py.add_mock)
def vadd(a, b):
    print("vadd", a, b)
    c = []
    for idx in range(len(a)):
        c.append(a[idx] + b[idx])
    return c

@register(fern_py.addi_mock)
def vaddi(a, i):
    print("vaddi", a, i)
    out = []
    for idx in range(len(a)):
        out.append(a[idx] + i)
    return out

a = fern_py.DataStructureArg(fern_py.Array.init(), "data")
a = vaddi(a, 3.0)
b = vaddi(a, 2.0)
c = vadd(a, b)
d = vaddi(c, 1.0)

for root in roots:
    print("root", root.func)
    print('deps')
    for child in root.dependencies:
        print(child.func)

d.eval()
print("pipeline funcs", pipeline_funcs)

pipeline = fern_py.Pipeline(pipeline_funcs)

# print(pipeline)

pipeline.constructPipeline()
pipeline.finalize(False)

fern_py.printToFile(pipeline, "pipeline2.ir")
code = fern_py.CodeGenerator(pipeline)
fern_py.printToFile(code, "pipeline2.cpp")