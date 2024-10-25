from dataclasses import dataclass
from enum import Enum, auto
from typing import List, Optional, Dict, Union
import re

# Token definitions
class TokenType(Enum):
    FERN_START = auto()        # <Fern Annotation>
    FERN_END = auto()          # </Fern Annotation>
    IDENTIFIER = auto()        # [a-zA-Z][a-zA-Z0-9_]*
    COLON = auto()            # :
    SYMBOLIC = auto()         # Symbolic
    FOR = auto()              # for
    IN = auto()               # in
    LBRACKET = auto()         # [
    RBRACKET = auto()         # ]
    LBRACE = auto()           # {
    RBRACE = auto()           # }
    DOT = auto()              # .
    PLUS = auto()             # +
    COMMA = auto()            # ,
    PRODUCE = auto()          # produce
    CONSUME = auto()          # when consume
    COMMENT_START = auto()    # /*
    COMMENT_END = auto()      # */
    WHITESPACE = auto()       # \s+
    EOF = auto()              # end of file

@dataclass
class Token:
    type: TokenType
    value: str
    line: int
    column: int

class Tokenizer:
    def __init__(self, text: str):
        self.text = text
        self.pos = 0
        self.line = 1
        self.column = 1
        self.tokens = []
        
        # Token patterns
        self.patterns = [
            (r'/\*', TokenType.COMMENT_START),
            (r'\*/', TokenType.COMMENT_END),
            (r'<Fern Annotation>', TokenType.FERN_START),
            (r'</Fern Annotation>', TokenType.FERN_END),
            (r'Symbolic', TokenType.SYMBOLIC),
            (r'for\b', TokenType.FOR),
            (r'in\b', TokenType.IN),
            (r'produce\b', TokenType.PRODUCE),
            (r'when\s+consume\b', TokenType.CONSUME),
            (r'[a-zA-Z][a-zA-Z0-9_]*', TokenType.IDENTIFIER),
            (r':', TokenType.COLON),
            (r'\[', TokenType.LBRACKET),
            (r'\]', TokenType.RBRACKET),
            (r'{', TokenType.LBRACE),
            (r'}', TokenType.RBRACE),
            (r'\.', TokenType.DOT),
            (r'\+', TokenType.PLUS),
            (r',', TokenType.COMMA),
            (r'\s+', TokenType.WHITESPACE),
        ]
        
        self.token_regex = '|'.join(f'(?P<{token_type.name}>{pattern})' 
                                   for pattern, token_type in self.patterns)
        self.regex = re.compile(self.token_regex)
    
    def tokenize(self) -> List[Token]:
        while self.pos < len(self.text):
            match = self.regex.match(self.text, self.pos)
            if match is None:
                raise ValueError(f"Invalid character at line {self.line}, column {self.column}")
            
            token_type_name = match.lastgroup
            token_value = match.group()
            
            if token_type_name != 'WHITESPACE':
                token = Token(
                    type=TokenType[token_type_name],
                    value=token_value,
                    line=self.line,
                    column=self.column
                )
                self.tokens.append(token)
            
            # Update position and line/column numbers
            if '\n' in token_value:
                self.line += token_value.count('\n')
                self.column = len(token_value.split('\n')[-1])
            else:
                self.column += len(token_value)
            
            self.pos = match.end()
        
        # Add EOF token
        self.tokens.append(Token(TokenType.EOF, "", self.line, self.column))
        return self.tokens

@dataclass
class VarRef:
    base: str
    field: str

@dataclass
class Expression:
    is_symbolic: bool
    var_ref: Optional[VarRef] = None
    raw_value: Optional[str] = None
    produce: Optional['ProduceClause'] = None
    consume: Optional['ConsumeClause'] = None
    loops: List['Loop'] = None

    def __post_init__(self):
        if self.loops is None:
            self.loops = []

@dataclass
class StructInstance:
    name: str
    fields: Dict[str, Expression]

@dataclass
class ProduceClause:
    instances: List[StructInstance]

@dataclass
class ConsumeClause:
    instances: List[StructInstance]

@dataclass
class Loop:
    iterator: str
    range_exprs: List[Expression]
    expression: Expression

@dataclass
class FernAnnotation:
    symbolic_vars: List[Expression]
    loops: List[Loop]

class Parser:
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.current = 0
    
    def parse(self) -> FernAnnotation:
        self.expect(TokenType.COMMENT_START)
        self.expect(TokenType.FERN_START)
        
        symbolic_vars = []
        loops = []
        
        while not self.match(TokenType.FERN_END):
            if self.check(TokenType.IDENTIFIER):
                # Parse symbolic variable
                var_name = self.expect(TokenType.IDENTIFIER).value
                self.expect(TokenType.COLON)
                self.expect(TokenType.SYMBOLIC)
                symbolic_vars.append(Expression(is_symbolic=True, raw_value=var_name))
            elif self.check(TokenType.FOR):
                loops.append(self.parse_loop())
            else:
                self.advance()
        
        self.expect(TokenType.COMMENT_END)
        return FernAnnotation(symbolic_vars=symbolic_vars, loops=loops)
    
    def parse_loop(self) -> Loop:
        self.expect(TokenType.FOR)
        iterator = self.expect(TokenType.IDENTIFIER).value
        self.expect(TokenType.IN)
        self.expect(TokenType.LBRACKET)
        
        range_exprs = []
        while not self.match(TokenType.RBRACKET):
            if self.check(TokenType.COMMA):
                self.advance()
                continue
            
            range_exprs.append(self.parse_range_expression())
        
        self.expect(TokenType.LBRACE)
        
        expr = Expression(is_symbolic=False)
        
        while not self.match(TokenType.RBRACE):
            if self.check(TokenType.PRODUCE):
                self.advance()
                expr.produce = self.parse_produce_consume_block()
            elif self.check(TokenType.CONSUME):
                self.advance()
                expr.consume = self.parse_produce_consume_block()
            elif self.check(TokenType.FOR):
                expr.loops.append(self.parse_loop())
            else:
                self.advance()
        
        return Loop(iterator=iterator, range_exprs=range_exprs, expression=expr)
    
    def parse_range_expression(self) -> Expression:
        if not self.check(TokenType.IDENTIFIER):
            raise ValueError(f"Expected identifier at {self.peek()}")
        
        first = self.advance()
        
        if self.match(TokenType.DOT):
            # Handle var ref (e.g., a.idx)
            field = self.expect(TokenType.IDENTIFIER).value
            expr = Expression(is_symbolic=False, var_ref=VarRef(first.value, field))
        else:
            # Handle raw value
            expr = Expression(is_symbolic=False, raw_value=first.value)
        
        # Handle compound expression (e.g., a.idx + a.size)
        if self.match(TokenType.PLUS):
            rest = self.parse_range_expression()
            expr.raw_value = f"{expr.raw_value or f'{expr.var_ref.base}.{expr.var_ref.field}'} + {rest.raw_value or f'{rest.var_ref.base}.{rest.var_ref.field}'}"
            expr.var_ref = None
        
        return expr
    
    def parse_produce_consume_block(self) -> Union[ProduceClause, ConsumeClause]:
        self.expect(TokenType.LBRACE)
        instances = []
        
        while not self.match(TokenType.RBRACE):
            if self.check(TokenType.IDENTIFIER):
                instances.append(self.parse_struct_instance())
            else:
                self.advance()
            
            # Skip commas between instances
            self.match(TokenType.COMMA)
        
        return ProduceClause(instances=instances)
    
    def parse_struct_instance(self) -> StructInstance:
        name = self.expect(TokenType.IDENTIFIER).value
        self.expect(TokenType.LBRACE)
        
        fields = {}
        while not self.match(TokenType.RBRACE):
            if self.check(TokenType.IDENTIFIER):
                field_name = self.advance().value
                print("field_name = ", field_name)
                self.expect(TokenType.COLON)
                
                if not self.check(TokenType.IDENTIFIER):
                    raise ValueError(f"Expected identifier at {self.peek()}")
                
                value = self.advance().value
                print("value_name = ", value)
                
                if self.match(TokenType.DOT):
                    print("NO...")
                    # Handle var ref
                    field = self.expect(TokenType.IDENTIFIER).value
                    fields[field_name] = Expression(
                        is_symbolic=False,
                        var_ref=VarRef(value, field)
                    )
                else:
                    print("YES...")
                    # Handle raw value
                    fields[field_name] = Expression(
                        is_symbolic=False,
                        raw_value=value
                    )
                    print(fields)
            else:
                self.advance()
            
            # Skip commas between fields
            self.match(TokenType.COMMA)
        
        return StructInstance(name=name, fields=fields)
    
    def match(self, type: TokenType) -> bool:
        if self.check(type):
            self.advance()
            return True
        return False
    
    def check(self, type: TokenType) -> bool:
        if self.is_at_end():
            return False
        return self.peek().type == type
    
    def advance(self) -> Token:
        if not self.is_at_end():
            self.current += 1
        return self.previous()
    
    def is_at_end(self) -> bool:
        return self.peek().type == TokenType.EOF
    
    def peek(self) -> Token:
        return self.tokens[self.current]
    
    def previous(self) -> Token:
        return self.tokens[self.current - 1]
    
    def expect(self, type: TokenType) -> Token:
        if self.check(type):
            return self.advance()
        raise ValueError(f"Expected {type} but got {self.peek()}")

def print_loop_structure(loop: Loop, indent: int = 0):
    indent_str = "  " * indent
    print(f"{indent_str}Loop iterator: {loop.iterator}")
    print(f"{indent_str}Range expressions: {[expr.raw_value or f'{expr.var_ref.base}.{expr.var_ref.field}' for expr in loop.range_exprs]}")
    
    if loop.expression.produce:
        for instance in loop.expression.produce.instances:
            print(f"{indent_str}Produce instance: {instance.name}")
            for field_name, field_value in instance.fields.items():
                value = field_value.raw_value or f'{field_value.var_ref.base}.{field_value.var_ref.field}' if field_value.var_ref else 'None'
                print(f"{indent_str}  {field_name}: {value}")
    
    if loop.expression.consume:
        for instance in loop.expression.consume.instances:
            print(f"{indent_str}Consume instance: {instance.name}")
            for field_name, field_value in instance.fields.items():
                value = field_value.raw_value or f'{field_value.var_ref.base}.{field_value.var_ref.field}' if field_value.var_ref else 'None'
                print(f"{indent_str}  {field_name}: {value}")
    
    for nested_loop in loop.expression.loops:
        print(f"{indent_str}Nested loop:")
        print_loop_structure(nested_loop, indent + 1)

# Example usage
if __name__ == "__main__":
    test_input = """
    /*
    <Fern Annotation>
    len : Symbolic
    len2 : Symbolic
    for i in [A.idx, A.idx + A.size, len]{
        for i in [A.idx, A.idx + A.size, len]{
            produce{
                A {
                    idx: i,
                    length: len
                }
            }
            when consume{
                B {
                    idx: i,
                    length: len
                },
                C {
                    idx: i,
                    length: len
                }
            }
        }
    }
    </Fern Annotation>
    */
    """
    
    try:
        # First tokenize
        tokenizer = Tokenizer(test_input)
        tokens = tokenizer.tokenize()
        
        # Then parse
        parser = Parser(tokens)
        result = parser.parse()
        
        # Print results
        print("Symbolic variables:")
        for var in result.symbolic_vars:
            print(f"  {var.raw_value}")
        
        print("\nLoop structure:")
        for loop in result.loops:
            print_loop_structure(loop)
            
    except ValueError as e:
        print(f"Error: {e}")