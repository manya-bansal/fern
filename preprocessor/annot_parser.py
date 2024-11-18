from dataclasses import dataclass
from typing import List, Union, Optional
from enum import Enum
import re

class TokenType(Enum):
    IDENTIFIER = "IDENTIFIER"
    NUMBER = "NUMBER"
    SYMBOLIC = "SYMBOLIC"
    COLON = "COLON"
    DOT = "DOT"
    PLUS = "PLUS"
    MUL = "MUL"
    SUB = "SUB"
    DIV = "DIV"
    COMMA = "COMMA"
    LEFT_BRACE = "LEFT_BRACE"
    RIGHT_BRACE = "RIGHT_BRACE"
    LEFT_BRACKET = "LEFT_BRACKET"
    RIGHT_BRACKET = "RIGHT_BRACKET"
    FOR = "FOR"
    IN = "IN"
    PRODUCE = "PRODUCE"
    WHEN = "WHEN"
    CONSUME = "CONSUME"
    ANNOTATION_START = "ANNOTATION_START"
    ANNOTATION_END = "ANNOTATION_END"

@dataclass
class Token:
    type: TokenType
    value: str
    line: int
    column: int

@dataclass
class ObjectDef:
    name: str
    properties: dict

@dataclass
class ProduceConsumeBlock:
    produce_objects: List[ObjectDef]
    consume_objects: List[ObjectDef]

@dataclass
class ForLoop:
    iterator: str
    range_values: List[str]
    nested_loops: List['ForLoop']
    produce_consume_block: Optional[ProduceConsumeBlock] = None

@dataclass
class FernAnnotation:
    symbols: List[str]
    loops: List[ForLoop]

class Tokenizer:
    def __init__(self, text: str):
        self.text = text
        self.pos = 0
        self.line = 1
        self.column = 1
        self.tokens = []

    def add_token(self, type: TokenType, value: str):
        self.tokens.append(Token(type, value, self.line, self.column))

    def skip_whitespace(self):
        while self.pos < len(self.text) and self.text[self.pos].isspace():
            if self.text[self.pos] == '\n':
                self.line += 1
                self.column = 1
            else:
                self.column += 1
            self.pos += 1

    def tokenize(self) -> List[Token]:
        while self.pos < len(self.text):
            self.skip_whitespace()
            if self.pos >= len(self.text):
                break

            char = self.text[self.pos]
            
            if self.text[self.pos:].startswith("/*"):
                self.pos += len("/*")
                self.column += len("/*")
                continue
            
            if self.text[self.pos:].startswith("*/"):
                self.pos += len("*/")
                self.column += len("*/")
                continue
            
            # Handle annotation tags
            if self.text[self.pos:].startswith("<Fern Annotation>"):
                self.add_token(TokenType.ANNOTATION_START, "<Fern Annotation>")
                self.pos += len("<Fern Annotation>")
                self.column += len("<Fern Annotation>")
                continue
            
            if self.text[self.pos:].startswith("</Fern Annotation>"):
                self.add_token(TokenType.ANNOTATION_END, "</Fern Annotation>")
                self.pos += len("</Fern Annotation>")
                self.column += len("</Fern Annotation>")
                continue

            # Handle keywords and identifiers
            if char.isalpha():
                identifier = ""
                while self.pos < len(self.text) and (self.text[self.pos].isalnum() or self.text[self.pos] == '_'):
                    identifier += self.text[self.pos]
                    self.pos += 1
                    self.column += 1

                if identifier in ["for", "in", "produce", "when", "consume", "Symbolic"]:
                    self.add_token(TokenType[identifier.upper()], identifier)
                else:
                    self.add_token(TokenType.IDENTIFIER, identifier)
                continue

            # Handle numbers
            if char.isdigit():
                number = ""
                while self.pos < len(self.text) and self.text[self.pos].isdigit():
                    number += self.text[self.pos]
                    self.pos += 1
                    self.column += 1
                self.add_token(TokenType.NUMBER, number)
                continue

            # Handle special characters
            char_tokens = {
                ':': TokenType.COLON,
                '.': TokenType.DOT,
                '+': TokenType.PLUS,
                '-': TokenType.SUB,
                '*': TokenType.MUL,
                '/': TokenType.DIV,
                ',': TokenType.COMMA,
                '{': TokenType.LEFT_BRACE,
                '}': TokenType.RIGHT_BRACE,
                '[': TokenType.LEFT_BRACKET,
                ']': TokenType.RIGHT_BRACKET,
            }

            if char in char_tokens:
                self.add_token(char_tokens[char], char)
                self.pos += 1
                self.column += 1
                continue

            # Skip comments and unknown characters
            self.pos += 1
            self.column += 1

        return self.tokens

class Parser:
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.current = 0

    def match(self, *types: TokenType) -> Optional[Token]:
        if self.current < len(self.tokens):
            current_token = self.tokens[self.current]
            if current_token.type in types:
                self.current += 1
                return current_token
        return None

    def expect(self, *types: TokenType) -> Token:
        token = self.match(*types)
        if token is None:
            raise SyntaxError(f"Expected one of {types} but got {self.tokens[self.current].type}")
        return token

    def peek(self) -> Optional[Token]:
        if self.current < len(self.tokens):
            return self.tokens[self.current]
        return None

    def parse_object_def(self) -> ObjectDef:
        name = self.expect(TokenType.IDENTIFIER).value
        self.expect(TokenType.LEFT_BRACE)
        
        properties = {}
        while self.current < len(self.tokens) and self.tokens[self.current].type != TokenType.RIGHT_BRACE:
            prop_name = self.expect(TokenType.IDENTIFIER).value
            self.expect(TokenType.COLON)
            
            value = []
            while self.current < len(self.tokens):
                token = self.tokens[self.current]
                if token.type in [TokenType.COMMA, TokenType.RIGHT_BRACE]:
                    break
                value.append(token.value)
                self.current += 1
                
            properties[prop_name] = " ".join(value)
            
            if self.match(TokenType.COMMA):
                continue
            
        self.expect(TokenType.RIGHT_BRACE)
        return ObjectDef(name, properties)

    def parse_for_loop(self) -> ForLoop:
        self.expect(TokenType.FOR)
        iterator = self.expect(TokenType.IDENTIFIER).value
        self.expect(TokenType.IN)
        self.expect(TokenType.LEFT_BRACKET)
        
        range_values = []
        while self.current < len(self.tokens) and self.tokens[self.current].type != TokenType.RIGHT_BRACKET:
            value = []
            while self.current < len(self.tokens):
                token = self.tokens[self.current]
                if token.type in [TokenType.COMMA, TokenType.RIGHT_BRACKET]:
                    break
                value.append(token.value)
                self.current += 1
            
            range_values.append("".join(value))
            
            if self.match(TokenType.COMMA):
                continue
                
        self.expect(TokenType.RIGHT_BRACKET)
        self.expect(TokenType.LEFT_BRACE)
        
        nested_loops = []
        produce_consume_block = None
        
        # Check for nested for loops or produce/consume block
        while self.current < len(self.tokens) and self.tokens[self.current].type != TokenType.RIGHT_BRACE:
            if self.peek().type == TokenType.FOR:
                nested_loops.append(self.parse_for_loop())
            elif self.peek().type == TokenType.PRODUCE:
                # Parse produce/consume block
                self.expect(TokenType.PRODUCE)
                self.expect(TokenType.LEFT_BRACE)
                produce_objects = []
                while self.current < len(self.tokens) and self.tokens[self.current].type != TokenType.RIGHT_BRACE:
                    produce_objects.append(self.parse_object_def())
                    if self.match(TokenType.COMMA):
                        continue
                self.expect(TokenType.RIGHT_BRACE)
                
                self.expect(TokenType.WHEN)
                self.expect(TokenType.CONSUME)
                self.expect(TokenType.LEFT_BRACE)
                consume_objects = []
                while self.current < len(self.tokens) and self.tokens[self.current].type != TokenType.RIGHT_BRACE:
                    consume_objects.append(self.parse_object_def())
                    if self.match(TokenType.COMMA):
                        continue
                self.expect(TokenType.RIGHT_BRACE)
                
                produce_consume_block = ProduceConsumeBlock(produce_objects, consume_objects)
            else:
                self.current += 1
        
        self.expect(TokenType.RIGHT_BRACE)
        return ForLoop(iterator, range_values, nested_loops, produce_consume_block)

    def parse(self) -> FernAnnotation:
        self.expect(TokenType.ANNOTATION_START)
        
        symbols = []
        loops = []
        
        while self.current < len(self.tokens) and self.tokens[self.current].type != TokenType.ANNOTATION_END:
            if self.match(TokenType.IDENTIFIER):
                self.current -= 1
                symbol = self.expect(TokenType.IDENTIFIER).value
                self.expect(TokenType.COLON)
                self.expect(TokenType.SYMBOLIC)
                symbols.append(symbol)
            elif self.tokens[self.current].type == TokenType.FOR:
                loops.append(self.parse_for_loop())
            else:
                self.current += 1
        
        self.expect(TokenType.ANNOTATION_END)
        return FernAnnotation(symbols, loops)

def parse_annotation_structure(text: str) -> FernAnnotation:
    tokenizer = Tokenizer(text)
    tokens = tokenizer.tokenize()
    parser = Parser(tokens)
    return parser.parse()


# # Example usage
# if __name__ == "__main__":
#     test_input = """
#     <Fern Annotation>
#     len : Symbolic
#     size : Symbolic
#     for i in [A.idx, A.idx + A.size, len]{
#         for j in [A.idx, A.idx + A.size, len]{
#             produce{
#                 A {
#                     idx: i + 4,
#                     length: len
#                 }
#             }
#             when consume{
#                 B {
#                     idx: i + 2,
#                     length: len
#                 },
#                 C {
#                     idx: i,
#                     length: len + 1
#                 }
#             }
#         }
#     }
#     </Fern Annotation>
#     """
    
#     try:
#         result = parse_fern_annotation(test_input)
#         print("Parsing successful!")
#         print("Symbols:", result.symbols)
        
#         def print_loop_structure(loop: ForLoop, indent: str = ""):
#             print(f"{indent}Loop (iterator: {loop.iterator})")
#             print(f"{indent}Range values: {loop.range_values}")
            
#             if loop.nested_loops:
#                 print(f"{indent}Nested loops:")
#                 for nested_loop in loop.nested_loops:
#                     print_loop_structure(nested_loop, indent + "  ")
                    
#             if loop.produce_consume_block:
#                 print(f"{indent}Produce block:")
#                 for obj in loop.produce_consume_block.produce_objects:
#                     print(f"{indent}  {obj.name}: {obj.properties}")
#                 print(f"{indent}Consume block:")
#                 for obj in loop.produce_consume_block.consume_objects:
#                     print(f"{indent}  {obj.name}: {obj.properties}")
        
#         print("\nLoop Structure:")
#         for loop in result.loops:
#             print_loop_structure(loop)
            
#     except Exception as e:
#         print(f"Parsing failed: {str(e)}")