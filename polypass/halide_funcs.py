import re
from dataclasses import dataclass
from typing import List, Dict, Set, Tuple, Optional
from collections import defaultdict

@dataclass
class Access:
    var_name: str
    offsets: Dict[str, int]  # dimension -> offset mapping
    multiplier: float = 1.0  # scalar multiplier

@dataclass
class StencilPattern:
    output: Access
    inputs: List[Access]
    dimensions: List[str]  # ordered list of dimension names

def parse_array_access(expr: str) -> Tuple[str, Dict[str, int], float]:
    """Parse array access like '2*x(i+1, j-2)' into variable name, offsets, and multiplier"""
    # First check for multiplier
    multiplier = 1.0
    array_expr = expr.strip()
    
    # Match potential multiplier pattern
    mult_match = re.match(r'(\d*\.?\d*)\s*\*\s*(\w+\s*\(.*\))', array_expr)
    if mult_match:
        multiplier = float(mult_match.group(1))
        array_expr = mult_match.group(2)
    
    # Match variable name and argument list
    match = re.match(r'(\w+)\s*\(\s*([\w\s,+\-\d]+)\s*\)', array_expr.strip())
    if not match:
        raise ValueError(f"Invalid array access format: {expr}")
    
    var_name, args = match.groups()
    dims = [arg.strip() for arg in args.split(',')]
    offsets = {}
    
    for dim in dims:
        # Parse dimension and optional offset
        dim_match = re.match(r'(\w+)\s*([+-]\s*\d+)?', dim)
        if not dim_match:
            raise ValueError(f"Invalid dimension format: {dim}")
            
        dim_name, offset = dim_match.groups()
        offset_val = int(offset.replace(' ', '')) if offset else 0
        offsets[dim_name] = offset_val
        
    return var_name, offsets, multiplier

def split_into_terms(expr: str) -> List[str]:
    """Split expression into terms, handling parentheses properly"""
    terms = []
    current_term = ""
    paren_count = 0
    
    for char in expr:
        if char == '(':
            paren_count += 1
            current_term += char
        elif char == ')':
            paren_count -= 1
            current_term += char
        elif char == '+' and paren_count == 0:
            if current_term.strip():
                terms.append(current_term.strip())
            current_term = ""
        elif char == '/' and paren_count == 0:
            if current_term.strip():
                terms.append(current_term.strip())
            current_term = ""
        else:
            current_term += char
            
    if current_term.strip():
        terms.append(current_term.strip())
        
    return terms

def parse_expression(expr: str) -> StencilPattern:
    """Parse a stencil expression with arbitrary dimensions"""
    # Split into LHS and RHS
    lhs, rhs = [s.strip() for s in expr.split('=')]
    
    # Parse output (LHS)
    out_var, out_offsets, _ = parse_array_access(lhs)  # Ignore multiplier for output
    output = Access(out_var, out_offsets)
    dimensions = list(out_offsets.keys())  # Keep track of dimension order
    
    # Parse inputs (RHS)
    terms = split_into_terms(rhs)
    inputs = []
    
    for term in terms:
        var_name, offsets, multiplier = parse_array_access(term)
        # Verify dimensions match
        # if set(offsets.keys()) != set(dimensions):
        #     raise ValueError(f"Inconsistent dimensions in term: {term}")
        inputs.append(Access(var_name, offsets, multiplier))
    
    return StencilPattern(output, inputs, dimensions)

def generate_annotation(pattern: StencilPattern) -> str:
    """Generate Fern annotation for multi-dimensional stencil"""
    # Calculate maximum offsets for each dimension
    
    inp_map = {}
    for inp in pattern.inputs:
        if inp.var_name in inp_map:
            max_offsets, min_offsets = inp_map[inp.var_name]
        else:
            max_offsets = defaultdict(int)
            min_offsets = defaultdict(int)
        print(inp)
        for dim, offset in inp.offsets.items():
            print(offset)
            max_offsets[dim] = max(max_offsets[dim], offset)
            min_offsets[dim] = min(min_offsets[dim], offset)
        # Just use name as a hashable string
        inp_map[inp.var_name] = (max_offsets, min_offsets)
    
    # Build annotation string
    lines = [
        "/*",
        "<Fern Annotation>",
    ]
    
    for dim in pattern.dimensions:
        lines.append(f"len_{dim}: Symbolic")
    
    # Generate nested loops for each dimension
    for dim in pattern.dimensions:
        lines.append(f"for {dim} in [0, {pattern.output.var_name}.dim[{pattern.dimensions.index(dim)}], len_{dim}]{{")
    
    # Indent level for nested content
    indent = "    " * len(pattern.dimensions)
    
    # Producer section
    lines.extend([
        f"{indent}produce{{",
        f"{indent}    {pattern.output.var_name} {{"
    ])
    
    # Add dimension mappings for output
    for dim in pattern.dimensions:
        lines.extend([
            f"{indent}        {dim}: {dim},",
            f"{indent}        length_{dim}: len_{dim}"
        ])
    
    lines.extend([
        f"{indent}    }}",
        f"{indent}}}",
        f"{indent}when consume{{"
    ])
    
    # Consumer section - collect unique input variables
    seen_vars = set()
    for inp in pattern.inputs:
        max_offsets, min_offsets = inp_map[inp.var_name]
        if inp.var_name not in seen_vars:
            seen_vars.add(inp.var_name)
            lines.extend([
                f"{indent}    {inp.var_name} {{",
            ])
            
            i=0
            # Add dimension mappings for input
            for offset in inp.offsets:
                dim = offset
                
                if dim not in pattern.dimensions:
                    lines.extend([
                        f"{indent}        {dim}: 0,",
                        f"{indent}        length_{dim}: {inp.var_name}.dim[{i}]"
                    ])
                else:
                    lines.extend([
                        f"{indent}        {dim}: {dim} + ({min_offsets[dim]}),",
                        f"{indent}        length_{dim}: len_{dim} + {max_offsets[dim]}"
                    ])
                i+=1
            
            lines.append(f"{indent}    }},")
    
    lines.append(f"{indent}}}")
    
    # Close all loops
    for _ in pattern.dimensions:
        lines.append("}")
    
    lines.extend([
        "</Fern Annotation>",
        "*/"
    ])
    
    return '\n'.join(lines)

def process_stencil(expr: str) -> str:
    """Main function to process stencil expression and generate annotation"""
    pattern = parse_expression(expr)
    return generate_annotation(pattern)

# Test cases
if __name__ == "__main__":
    test_cases = [
        "y(i, j) = x(i, j) + x(i+1, j) + x(i+2, j) + z(i+1, j)",
        "out(i, j, k) = in(i+1, j, k) + in(i, j, k+2)",
        "result(i, j) = input(i+1, j) + input(i, j) + input(i+1, j)",
        "result(i, j) = input(i+1, j) + input(i-1, j)",
        "result(i, j) = input(i)",
        "result(i, j) = input(k)",
    ]
    
    for test in test_cases:
        print(f"\nProcessing: {test}")
        print(process_stencil(test))