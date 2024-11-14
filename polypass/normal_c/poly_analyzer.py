import json
from dataclasses import dataclass
from typing import Dict, List, Tuple, Set, Optional

@dataclass
class IteratorBound:
    iterator: str  # The iterator variable (e.g., 'i0')
    coefficient: int = 1  # For cases like 2*i0
    constant: int = 0    # For cases like i0 + 1

    def __str__(self):
        parts = []
        if self.coefficient != 1:
            parts.append(f"{self.coefficient}*")
        parts.append(self.iterator)
        if self.constant > 0:
            parts.append(f" + {self.constant}")
        elif self.constant < 0:
            parts.append(f" - {abs(self.constant)}")
        return "".join(parts)

@dataclass
class IteratorDomain:
    iterator: str
    lower_bound: str
    upper_bound: str

    def __str__(self):
        return f"{self.lower_bound} ≤ {self.iterator} ≤ {self.upper_bound}"

@dataclass
class AccessBounds:
    iterator_expr: Optional[IteratorBound]
    value_bound: Optional[str]

@dataclass
class DimensionBounds:
    lower: AccessBounds
    upper: AccessBounds

@dataclass
class ArrayAccess:
    array_name: str
    dimensions: List[DimensionBounds]
    iterator_domains: List[IteratorDomain]

def parse_domain_constraints(domain_str: str) -> List[IteratorDomain]:
    """Parse domain constraints into iterator domains."""
    # Remove prefix and get the actual constraints
    constraints = domain_str.split(':')[1].strip()
    domains = {}
    
    # Parse each constraint
    for constraint in constraints.split('and'):
        constraint = constraint.strip()
        if '<=' in constraint:
            parts = constraint.split('<=')
            if len(parts) == 3:
                # Case: a <= x <= b
                iterator = parts[1].strip()
                lower = parts[0].strip()
                upper = parts[2].strip()
                domains[iterator] = IteratorDomain(iterator, lower, upper)
        elif '<' in constraint:
            # Case: 0 <= x < b
            left, right = constraint.split('<')
            if '<=' in left:
                iterator = left.split('<=')[1].strip()
                lower_bound = left.split('<=')[0].strip()
                upper_bound = f"{right.strip()}-1"
                domains[iterator] = IteratorDomain(iterator, lower_bound, upper_bound)
    
    return list(domains.values())

def parse_access_expr(expr: str) -> IteratorBound:
    """Parse an access expression like 'i0' or 'i0 + 1'."""
    expr = expr.strip()
    if '+' in expr:
        iterator, constant = expr.split('+')
        return IteratorBound(iterator.strip(), 1, int(constant))
    elif '-' in expr:
        iterator, constant = expr.split('-')
        return IteratorBound(iterator.strip(), 1, -int(constant))
    else:
        return IteratorBound(expr)

def extract_array_name(relation_str: str) -> str:
    """Extract the actual array name from the relation string."""
    parts = relation_str.split("->")
    if len(parts) >= 2:
        last_part = parts[-1].strip()
        array_ref = last_part.split("->")[-1].strip()
        array_name = array_ref.split("[")[0].strip("{ }")
        return array_name
    return ""

def analyze_polly_bounds(json_data: dict) -> Dict[str, ArrayAccess]:
    """Analyze Polly JSON output to determine array access bounds with iterator information."""
    array_accesses = {}
    
    # Initialize for each array
    for array in json_data['arrays']:
        array_name = array['name']
        dims = len(array['sizes'])
        array_accesses[array_name] = ArrayAccess(
            array_name=array_name,
            dimensions=[
                DimensionBounds(
                    lower=AccessBounds(None, "∞"),
                    upper=AccessBounds(None, "-∞")
                )
                for _ in range(dims)
            ],
            iterator_domains=[]
        )
    
    # Analyze each statement
    for stmt in json_data['statements']:
        # Get iterator domains
        iterator_domains = parse_domain_constraints(stmt['domain'])
        
        # Analyze each access
        for access in stmt['accesses']:
            array_name = extract_array_name(access['relation'])
            array_access = array_accesses[array_name]
            
            # Update iterator domains
            array_access.iterator_domains = iterator_domains
            
            # Extract index expressions
            indices_str = access['relation'].split('[')[-1].split(']')[0]
            indices = [idx.strip() for idx in indices_str.split(',')]
            
            # Update bounds for each dimension
            for dim, idx in enumerate(indices):
                # Parse the access expression
                iterator_bound = parse_access_expr(idx)
                
                # Update dimension bounds
                current_bounds = array_access.dimensions[dim]
                
                # Find value bounds from iterator domains
                for domain in iterator_domains:
                    if domain.iterator == iterator_bound.iterator:
                        if current_bounds.lower.value_bound == "∞":
                            current_bounds.lower.iterator_expr = iterator_bound
                            current_bounds.lower.value_bound = domain.lower_bound
                        if current_bounds.upper.value_bound == "-∞":
                            current_bounds.upper.iterator_expr = iterator_bound
                            current_bounds.upper.value_bound = domain.upper_bound
    
    return array_accesses

def format_access_analysis(accesses: Dict[str, ArrayAccess]) -> str:
    """Format the access analysis results."""
    result = []
    for array_name, access in accesses.items():
        result.append(f"Array: {array_name}")
        
        result.append("Iterator Domains:")
        for domain in access.iterator_domains:
            result.append(f"  {domain}")
        
        result.append("Access Bounds:")
        for dim, bounds in enumerate(access.dimensions):
            result.append(f"  Dimension {dim}:")
            if bounds.lower.iterator_expr:
                result.append(f"    Lower bound (iterator): {bounds.lower.iterator_expr}")
            result.append(f"    Lower bound (value): {bounds.lower.value_bound}")
            if bounds.upper.iterator_expr:
                result.append(f"    Upper bound (iterator): {bounds.upper.iterator_expr}")
            result.append(f"    Upper bound (value): {bounds.upper.value_bound}")
    
    return "\n".join(result)

# Example usage with your JSON
polly_data = {
    "arrays": [
        {
            "name": "MemRef0",
            "sizes": ["*", "(zext i32 %M to i64)"],
            "type": "i32"
        }
    ],
    "context": "[p_0, p_1] -> {  : -2147483648 <= p_0 <= 2147483647 and -2147483648 <= p_1 <= 2147483647 }",
    "statements": [
        {
            "accesses": [
                {
                    "kind": "read",
                    "relation": "[p_0, p_1] -> { Stmt2[i0, i1] -> MemRef0[i0, i1] }"
                },
                {
                    "kind": "write",
                    "relation": "[p_0, p_1] -> { Stmt2[i0, i1] -> MemRef0[i0, i1] }"
                }
            ],
            "domain": "[p_0, p_1] -> { Stmt2[i0, i1] : 0 <= i0 < p_0 and 0 <= i1 < p_1 }",
            "name": "Stmt2",
            "schedule": "[p_0, p_1] -> { Stmt2[i0, i1] -> [0, i0, i1] }"
        },
        {
            "accesses": [
                {
                    "kind": "read",
                    "relation": "[p_0, p_1] -> { Stmt6[i0, i1] -> MemRef0[i0, i1] }"
                },
                {
                    "kind": "write",
                    "relation": "[p_0, p_1] -> { Stmt6[i0, i1] -> MemRef0[i0, i1] }"
                }
            ],
            "domain": "[p_0, p_1] -> { Stmt6[i0, i1] : 0 <= i0 < p_0 and 0 <= i1 < p_1 }",
            "name": "Stmt6",
            "schedule": "[p_0, p_1] -> { Stmt6[i0, i1] -> [1, i0, i1] }"
        }
    ]
}

if __name__ == "__main__":
    accesses = analyze_polly_bounds(polly_data)
    print(format_access_analysis(accesses))