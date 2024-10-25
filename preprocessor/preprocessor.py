import annot_parser
import expr_parser
import signature_parse
import argparse
import re


def cpp_gen_expr(expr_str, declared):
    # print(expr_str)
    expr = expr_parser.parse_expression(expr_str)
    # print(expr)
    
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
                    raise Exception(f"You have not declared {string} before!")
                return string
        raise Exception("Unreachable!")
    
    return generate_string(expr)
    
def cpp_gen_data_dep(structure):
    declared = set()
    declarations = []
    constrained_subsets = set()
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
                constrained_subsets.add(obj.name)
            for obj in loop.produce_consume_block.consume_objects:
                declarations.append(f"fern::DataStructure block_{obj.name}(\"block_{obj.name}\", &{obj.name});")
                declared.add(f"block_{obj.name}")      
                constrained_subsets.add(obj.name)        
    
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
    func_def = f"fern::DependencySubset getDataRelationship() const override {{\n"+\
           f"\t{'\n\t'.join(declarations)}\n"+\
           f"\treturn {data_dep} }}"
    return constrained_subsets, func_def


def generate_full_func(signature, data_dep, constrained_subsets):
    func_name, args  = signature_parse.parse_function_signature(signature)
    
    fern_arguments = []
    private_vars = []
    
    for arg in args:
        if arg[0] == "int":
            fern_arguments.append(f"new fern::VariableArg(fern::getNode({arg[1]}))")
            private_vars.append(f"fern::Variable {arg[1]};")
        elif arg[1] not in constrained_subsets:
            fern_arguments.append(f"fern::DataStructureArg(fern::DataStructurePtr(&{arg[1]}))")
            private_vars.append(f"{arg[0]} {arg[1]};")
        else:
            fern_arguments.append(f"new fern::DataStructureArg(fern::DataStructurePtr(&{arg[1]}))")
            private_vars.append(f"{arg[0]} {arg[1]};")
    
    return f"class {func_name} : public fern::AbstractFunctionCall {{\n\n"\
           f"public:\n"\
           f"\t{func_name}() = default;\n"\
           f"\tstd::string getName() const override {{ return {{\"{func_name}\"}}; }}\n\n"\
           f"\t{data_dep}\n\n"\
           f"\tstd::vector<fern::Argument> getArguments() override {{\n"\
           f"\t\treturn {{ {',\n\t\t'.join(fern_arguments)} }}; }}\n\n"\
           f"{'\n'.join(private_vars)}\n"\
           f"}};"
    
    
def parse_fern_annotation(signature:str, annot: str):
    structure = annot_parser.parse_annotation_structure(annot)
    constrained_subsets, data_dep_func = cpp_gen_data_dep(structure)
    full_func = generate_full_func(signature, data_dep_func, constrained_subsets)
    return full_func
        

        
# parse_fern_annotation(signature, test_input)


def main():
    parser = argparse.ArgumentParser(description='Convert Fern Annotations into cpp classes automatically')
    parser.add_argument('--header_file', type=str, help='Path to the input file')
    parser.add_argument('--cpp_file', type=str, help='Path to the input file')
    args = parser.parse_args()
    
    # Read the file and print its contents
    try:
        with open(args.header_file, 'r') as file:
            with open(args.cpp_file, 'w') as out_file:
                file_contents = file.read()
                # Read all the relevant annotations from the file
                pattern = r'(/\*.*?\*/)\s*(void\s+\w+\(.*?\);)'
                matches = re.findall(pattern, file_contents, re.DOTALL)  
                parsed_results = [{"annotation": match[0], "function": match[1]} for match in matches]
                
                out_file.write(
                   "#include \"dependency_lang/data-structure-interface.h\"\n"\
                   "#include \"dependency_lang/dep_lang.h\"\n"\
                   "#include \"dependency_lang/dep_lang_nodes.h\"\n"\
                   "#include \"examples/common-ds.h\"\n"\
                   "#include \"utils/name.h\"\n"\
                   "\n\n"\
                   "namespace examples {\n\n"
                )
                for result in parsed_results:
                    out_file.write(parse_fern_annotation(result['function'], result['annotation']))
                    out_file.write("\n\n")
                
                # close namespace
                out_file.write("\n\n}")
            
    except FileNotFoundError:
        print(f"Error: The file '{args.header_file}' does not exist.")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == '__main__':
    main()