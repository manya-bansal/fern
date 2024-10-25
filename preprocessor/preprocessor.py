import annot_parser
from annot_parser import parse_annotation_structure
import expr_parser
from expr_parser import parse_expression



test_input = """
    <Fern Annotation>
    len : Symbolic
    size : Symbolic
    for i in [A.idx, A.idx + A.size, len]{
        for j in [A.idx, A.idx + A.size, len]{
            produce{
                A {
                    idx: i + 4,
                    length: len
                }
            }
            when consume{
                B {
                    idx: i + 2,
                    length: len
                },
                C {
                    idx: i,
                    length: len + 1
                }
            }
        }
    }
    </Fern Annotation>
    """


def cpp_gen_expr(expr_str, declared):
    expr = parse_expression(expr_str)
    
    def generate_string(expr):
        match expr:
            case expr_parser.BinaryOp(op, left, right):
                return generate_string(left) + op + generate_string(right)
            
            case expr_parser.StructField(struct_name, field_name):
                return f"{struct_name}[\"{field_name}\"]"
            
            case int(value):
                return f"{value}"
            
            case str(string):
                if string not in declared:
                    raise(f"You have not declared {string} before!")
                return string
        raise Exception("Unreachable!")
    
    return generate_string(expr)
    
def cpp_gen_data_dep(structure):
    declared = set()
    declarations = []
    for symbol in structure.symbols:
        # Handle Arguments separately
        if symbol not in declared:
            declarations.append(f"fern::Variable {symbol}(false, false, false);")
            declared.add(symbol)
    

    def decl_loops(loop):
        if loop.iterator not in declared:
            declarations.append(f"fern::Variable {loop.iterator}(false, false, false);")
            declarations.append(f"fern::Interval {loop.iterator}_loop({loop.iterator}, " 
                f"{cpp_gen_expr(loop.range_values[0], declared)}, "
                f"{cpp_gen_expr(loop.range_values[1], declared)}, "
                f"{cpp_gen_expr(loop.range_values[2], declared)});")
            declared.add(loop.iterator)
            declared.add(loop.iterator+"_loop")
        if loop.nested_loops:
           for nested_loop in loop.nested_loops:
                    decl_loops(nested_loop)
                    
        if loop.produce_consume_block:
            for obj in loop.produce_consume_block.produce_objects:
                declarations.append(f"fern::DataStructure block_{obj.name}(\"block_{obj.name}\", &{obj.name});")
                declared.add(f"block_{obj.name}")
            for obj in loop.produce_consume_block.consume_objects:
                declarations.append(f"fern::DataStructure block_{obj.name}(\"block_{obj.name}\", &{obj.name});")
                declared.add(f"block_{obj.name}")              
    
    for loop in structure.loops:
        decl_loops(loop)
        
    def generate_data_dep(node):
        match node:
            case annot_parser.ForLoop(iterator, range_values, nested_loops, produce_consume_block):
                if nested_loops:
                    for nested_loop in nested_loops:
                        return f"{nested_loop.iterator}_loop({generate_data_dep(nested_loop)})"
                    
                if produce_consume_block:
                    assert len(produce_consume_block.produce_objects) == 1
                    produce = ""
                    for obj in produce_consume_block.produce_objects:
                        produce = generate_data_dep(obj)
                    
                    consume = []
                    for obj in produce_consume_block.consume_objects:
                        consume.append(generate_data_dep(obj))
                
                    return f"fern::ComputationAnnotation(\n"\
                       f"\t\tfern::Producer({produce}),\n"\
                       f"\t\tfern::Consumer({',\n\t\t'.join(consume)}))"

                        
            case annot_parser.ObjectDef(name, properties):
                assert f"block_{name}" in declared
                prop = []
                for v in properties:
                    prop.append(cpp_gen_expr(properties[v], declared))
                return f"block_{name}({','.join(prop)})"
    
    assert len(structure.loops) == 1    
    # print(declarations)    
    data_dep = f"{structure.loops[0].iterator}_loop({generate_data_dep(structure.loops[0])});"
    # print(data_dep)
    return f"fern::DependencySubset getDataRelationship() const override {{\n"\
           f"\t{'\n\t'.join(declarations)}\n"\
           f"\treturn {data_dep}"


    
def parse_fern_annotation(annot: str):
    try:
        structure = parse_annotation_structure(annot)
        data_dep_func =cpp_gen_data_dep(structure)
        print(data_dep_func)
    except Exception as e:
        print(f"Parsing failed: {str(e)}")
        
parse_fern_annotation(test_input)