from dataclasses import dataclass
from typing import List, Dict, Set, Tuple, Optional
import re

@dataclass
class ArrayAccess:
    array_name: str
    indices: List[str]
    is_write: bool
    offset: List[int]

@dataclass
class LoopBound:
    start: str
    end: str
    step: str
    iterator: str

class PolyhedralAnalyzer:
    def __init__(self):
        self.loop_bounds: List[LoopBound] = []
        self.array_accesses: List[ArrayAccess] = []
        
    def parse_loop(self, code: str) -> None:
        """Parse loop structure from input code."""
        # Extract for loops using regex
        loop_pattern = r'for\s*\(([^;]+);([^;]+);([^)]+)\)'
        loops = re.finditer(loop_pattern, code)
        
        for loop in loops:
            init, cond, incr = loop.groups()
            
            # Parse iterator and bounds
            iterator = self._extract_iterator(init)
            start = self._extract_start(init)
            end = self._extract_end(cond)
            step = self._extract_step(incr)
            
            self.loop_bounds.append(LoopBound(start, end, step, iterator))
    
    def parse_array_accesses(self, code: str) -> None:
        """Parse array accesses from the code."""
        # Match array access patterns like A[i,j] or B[i-1,j+1]
        array_pattern = r'([A-Z])\[([^\]]+)\]'
        
        # Find all array accesses
        for line in code.split('\n'):
            assignment = '=' in line
            accesses = re.finditer(array_pattern, line)
            
            for access in accesses:
                array_name, index_str = access.groups()
                indices = [idx.strip() for idx in index_str.split(',')]
                is_write = assignment and line.index(access.group(0)) < line.index('=')
                offsets = self._calculate_offsets(indices)
                
                self.array_accesses.append(
                    ArrayAccess(array_name, indices, is_write, offsets)
                )
    
    def _extract_iterator(self, init: str) -> str:
        """Extract iterator variable from loop initialization."""
        return init.split('=')[0].strip()
    
    def _extract_start(self, init: str) -> str:
        """Extract loop start value."""
        return init.split('=')[1].strip()
    
    def _extract_end(self, cond: str) -> str:
        """Extract loop end value."""
        # Handle different comparison operators
        for op in ['<', '<=', '>', '>=']:
            if op in cond:
                parts = cond.split(op)
                return parts[1].strip()
        return ''
    
    def _extract_step(self, incr: str) -> str:
        """Extract loop step value."""
        if '+=' in incr:
            return incr.split('+=')[1].strip()
        elif '++' in incr:
            return '1'
        elif '--' in incr:
            return '-1'
        return ''
    
    def _calculate_offsets(self, indices: List[str]) -> List[int]:
        """Calculate offsets in array accesses."""
        offsets = []
        for idx in indices:
            offset = 0
            if '+' in idx:
                offset = int(idx.split('+')[1])
            elif '-' in idx:
                offset = -int(idx.split('-')[1])
            offsets.append(offset)
        return offsets
    
    def generate_access_maps(self) -> Dict[str, List[Tuple[str, List[str]]]]:
        """Generate access maps for each array."""
        access_maps = {}
        
        for access in self.array_accesses:
            if access.array_name not in access_maps:
                access_maps[access.array_name] = []
                
            access_type = "write" if access.is_write else "read"
            access_maps[access.array_name].append((access_type, access.indices))
            
        return access_maps
    
    def get_domain_constraints(self) -> List[str]:
        """Generate domain constraints for the loop nest."""
        constraints = []
        
        for bound in self.loop_bounds:
            iterator = bound.iterator
            if bound.start.isdigit() and bound.end.isdigit():
                constraints.append(
                    f"{bound.start} <= {iterator} <= {bound.end}"
                )
            else:
                constraints.append(
                    f"{bound.start} <= {iterator} < {bound.end}"
                )
                
        return constraints
    
    def analyze(self, code: str) -> str:
        """Perform complete analysis of the loop nest."""
        self.parse_loop(code)
        self.parse_array_accesses(code)
        
        # Generate output
        result = []
        
        # Domain constraints
        constraints = self.get_domain_constraints()
        result.append("Domain:")
        result.append("{ " + " and ".join(constraints) + " }")
        
        # Access maps
        result.append("\nArray Accesses:")
        access_maps = self.generate_access_maps()
        
        for array_name, accesses in access_maps.items():
            result.append(f"{array_name}:")
            for access_type, indices in accesses:
                index_str = ", ".join(indices)
                result.append(f"Access Map ({access_type}): {{{array_name}[{index_str}] }}")
                
        return "\n".join(result)

# Example usage
code = """
for(i = 0; i < D.cols; i++) {
    for(j = 0; j < D.rows; j++) {
        tmp = 0
        for(k = 0; k < i; k++) {
           tmp += A[i, k] + B[k, j]
           tmp += A[i, k] + B[k, j]
        }
        D[i,j] = tmp
    }
}
"""

analyzer = PolyhedralAnalyzer()
result = analyzer.analyze(code)
print(result)