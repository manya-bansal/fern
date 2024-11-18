from dataclasses import dataclass
from typing import Union, Optional, List, Any
from enum import Enum
from abc import ABC, abstractmethod

class TokenType(Enum):
    NUMBER = "NUMBER"
    IDENTIFIER = "IDENTIFIER"
    DOT = "DOT"
    PLUS = "PLUS"
    MINUS = "MINUS"
    MULTIPLY = "MULTIPLY"
    DIVIDE = "DIVIDE"
    MODULO = "MODULO"
    EOF = "EOF"

@dataclass
class Token:
    type: TokenType
    value: str
    
@dataclass
class StructField:
    struct_name: str
    field_name: str
    
    def __repr__(self):
        return f"StructField({self.struct_name}, {self.field_name})"

@dataclass
class BinaryOp:
    op: str
    left: Union['BinaryOp', StructField, str, int]
    right: Union['BinaryOp', StructField, str, int]
    
    def __repr__(self):
        return f"BinaryOp({self.op}, {self.left}, {self.right})"

class Lexer:
    def __init__(self, text: str):
        self.text = text.replace(" ", "")
        self.pos = -1
        self.current_char = None
        self.advance()
        
    def error(self):
        raise Exception('Invalid character')
        
    def advance(self):
        self.pos += 1
        self.current_char = self.text[self.pos] if self.pos < len(self.text) else None
        
    def peek(self) -> Optional[str]:
        peek_pos = self.pos + 1
        return self.text[peek_pos] if peek_pos < len(self.text) else None
        
    def number(self) -> str:
        result = ''
        while self.current_char is not None and self.current_char.isdigit():
            result += self.current_char
            self.advance()
        return result
        
    def identifier(self) -> str:
        result = ''
        while self.current_char is not None and (self.current_char.isalnum() or self.current_char == '_'):
            result += self.current_char
            self.advance()
        return result
        
    def get_next_token(self) -> Token:
        while self.current_char is not None:
            if self.current_char.isdigit():
                return Token(TokenType.NUMBER, self.number())
                
            if self.current_char.isalpha() or self.current_char == '_':
                return Token(TokenType.IDENTIFIER, self.identifier())
                
            if self.current_char == '.':
                self.advance()
                return Token(TokenType.DOT, '.')
                
            if self.current_char == '+':
                self.advance()
                return Token(TokenType.PLUS, '+')
                
            if self.current_char == '-':
                self.advance()
                return Token(TokenType.MINUS, '-')
                
            if self.current_char == '*':
                self.advance()
                return Token(TokenType.MULTIPLY, '*')
                
            if self.current_char == '/':
                self.advance()
                return Token(TokenType.DIVIDE, '/')
                
            if self.current_char == '%':
                self.advance()
                return Token(TokenType.MODULO, '%')
                
            self.error()
            
        return Token(TokenType.EOF, None)

class Parser:
    def __init__(self, lexer: Lexer):
        self.lexer = lexer
        self.current_token = self.lexer.get_next_token()
        
    def error(self):
        raise Exception('Invalid syntax')
        
    def eat(self, token_type: TokenType):
        if self.current_token.type == token_type:
            self.current_token = self.lexer.get_next_token()
        else:
            self.error()
            
    def struct_field(self) -> StructField:
        """Parse a struct field of the form A.idx"""
        struct_name = self.current_token.value
        self.eat(TokenType.IDENTIFIER)
        if self.current_token.type != TokenType.DOT:
            return struct_name  # Return as regular identifier if no dot follows
        self.eat(TokenType.DOT)
        field_name = self.current_token.value
        self.eat(TokenType.IDENTIFIER)
        return StructField(struct_name, field_name)
        
    def factor(self) -> Union[StructField, str, int]:
        token = self.current_token
        
        if token.type == TokenType.NUMBER:
            self.eat(TokenType.NUMBER)
            return int(token.value)
            
        elif token.type == TokenType.IDENTIFIER:
            return self.struct_field()
                
        self.error()
        return None
        
    def term(self) -> Union[BinaryOp, StructField, str, int]:
        """Parse terms (multiplication, division, modulo)"""
        result = self.factor()
        
        while self.current_token.type in (TokenType.MULTIPLY, TokenType.DIVIDE, TokenType.MODULO):
            token = self.current_token
            if token.type == TokenType.MULTIPLY:
                self.eat(TokenType.MULTIPLY)
                result = BinaryOp('*', result, self.factor())
            elif token.type == TokenType.DIVIDE:
                self.eat(TokenType.DIVIDE)
                result = BinaryOp('/', result, self.factor())
            elif token.type == TokenType.MODULO:
                self.eat(TokenType.MODULO)
                result = BinaryOp('%', result, self.factor())
                
        return result
        
    def expr(self) -> Union[BinaryOp, StructField, str, int]:
        """Parse expressions (addition, subtraction)"""
        result = self.term()
        
        while self.current_token.type in (TokenType.PLUS, TokenType.MINUS):
            token = self.current_token
            if token.type == TokenType.PLUS:
                self.eat(TokenType.PLUS)
                result = BinaryOp('+', result, self.term())
            elif token.type == TokenType.MINUS:
                self.eat(TokenType.MINUS)
                result = BinaryOp('-', result, self.term())
                
        return result

def parse_expression(expr: str) -> Union[BinaryOp, StructField, str, int]:
    """
    Parse expressions with binary operations, struct fields, and integers.
    Supports operators: +, -, *, /, % with proper precedence rules.
    
    Args:
        expr: String expression to parse
        
    Returns:
        Parsed expression tree
        
    Examples:
        "len + A.idx * 2"  -> BinaryOp(+, len, BinaryOp(*, StructField(A, idx), 2))
        "10 * B.val / 5"   -> BinaryOp(/, BinaryOp(*, 10, StructField(B, val)), 5)
    """
    lexer = Lexer(expr)
    parser = Parser(lexer)
    return parser.expr()

# # Example usage
# def test_parser():
#     test_cases = [
#         "10 * B.val / 5",  # Testing complex struct field expressions
#         "len + A.idx",     # Simple struct field
#         "A.idx",          # Just a struct field
#         "len + A.idx * 2",
#         "count % Struct.field + 3",
#         "A.x * B.y",
#         "1 + 2 * 3",
#         "length",
#         "Table.column",
#         "x * stridearg"
#     ]
    
#     for test in test_cases:
#         try:
#             result = parse_expression(test)
#             print(f"Input: {test}")
#             print(f"Output: {result}\n")
#         except Exception as e:
#             print(f"Error parsing '{test}': {str(e)}\n")
            
# test_parser()