class Node:
    def __init__(self, func, args, kwargs):
        self.func = func
        self.args = args
        self.kwargs = kwargs
        self.dependencies = []
        self.evaled = False
        self.output = None
    
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
        self.output = self.func(*evaled_args, **self.kwargs)
        return self.output

# nodes that have no dependents
roots = []

def register():
    def inner(func):
        def wrapper(*args, **kwargs):
            print("args", args)
            global roots
            print("roots", roots)
            node = Node(func, args, kwargs)
           
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

@register()
def vadd(a, b):
    print("vadd", a, b)
    c = []
    for idx in range(len(a)):
        c.append(a[idx] + b[idx])
    return c

@register()
def vsubi(a, i):
    print("vsub", a, i)
    out = []
    for idx in range(len(a)):
        out.append(a[idx] - i)
    return out

a = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
b = [2, 4, 5, 6, 7, 8, 9, 5, 4, 3]
a = vsubi(a, 3)
b = vsubi(a, 2)
c = vadd(a, b)
d = vsubi(a, 1)

for root in roots:
    print("root", root.func)
    print('deps')
    for child in root.dependencies:
        print(child.func)

print(c.eval())