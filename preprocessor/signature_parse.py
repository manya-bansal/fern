import re
from typing import List, Tuple

def parse_function_signature(signature: str) -> Tuple[str, List[Tuple[str, str]]]:
    # Match the function name
    func_name_match = re.match(r'\w+\s+(\w+)\s*\(', signature)
    if not func_name_match:
        raise ValueError("Invalid function signature format")
    
    # Extract the function name
    func_name = func_name_match.group(1)

    # Extract arguments
    args_str = signature[func_name_match.end():].rstrip(');')
    args = []
    for arg in args_str.split(','):
        # Split on first space to get type and name
        parts = arg.strip().split(' ')
        if len(parts) == 2:
            type_name, var_name = parts
            args.append((type_name, var_name))
        else:
            raise ValueError("Unexpected argument format")

    return func_name, args

# Example usage
signature = "void vadd(Array<float> B, Array<float> C, Array<float> A);"
func_name, args = parse_function_signature(signature)

print("Function Name:", func_name)
print("Arguments:")
for arg_type, arg_name in args:
    print(f"Type: {arg_type}, Name: {arg_name}")
