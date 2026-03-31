# Hold

Hold is a scripting language.

# Why?

To learn how scripting languages work. Also to "fix" problems with other langauges by making different problems.

# Grammar

TODO:

```ebnf

program             = { use }, { top_level_stmnt } ;

(* Probably we need an option to alias these, I'm not sure how complicated that is *)
use                 = "use", "{", [ IDENTIFIER, { ",", IDENTIFIER } ], "}", "from", IDENTIFIER, ';' ;  

top_level_stmnt     = [ "export" ], (struct_decl | trait_decl | function_decl);

struct_decl         = "struct", IDENTIFIER, [ generic_decl ], [ ":", IDENTIFIER, [ generic_args ], 
                      { ",", IDENTIFIER, [ generic_args ] } ], [generic_filter],  "{", { struct_member_decl }, "}" ;
struct_member_decl  = [ IDENTIFIER, ":", IDENTIFIER ], { ",", IDENTIFIER, ":", IDENTIFIER }

(* TODO: add default values *)
generic_decl        = "<", IDENTIFIER, { ",", IDENTIFIER }, ">"             
generic_args        = "<", IDENTIFIER, { ",", IDENTIFIER }, ">"
generic_filter      = "where", IDENTIFIER, "is", IDENTIFIER, { ",", IDENTIFIER };

trait_decl          = "trait", IDENTIFIER, [generic_decl], "{", { trait_func_decl }, "}" ;

trait_func_decl     = IDENTIFIER, [generic_decl], "(", function_arg_list, ")", [ ":", IDENTIFIER ];

function_decl       = "fn", IDENTIFIER, [ "::", IDENTIFIER ], [generic_decl], "(", function_arg_list, ")", 
                      [ ":", IDENTIFIER ], [generic_filter], block;
function_arg_list   = [ "self" ], { ",", function_arg } ;
(* TODO: add default values *)
function_arg        = IDENTIFIER, ":", IDENTIFIER ;

(* This allows some invalid states like `fn foo() defer {}` :( *)
block               = [ "defer" ], "{", stmnt_list, "}" ;                   

stmnt_list          = { variable_decl | if | loop | while | "break" | "continue" | return | variable_assign | expression | ";" } ;

 (* It would be nice if the compiler could infer the type. *)
variable_decl       = "let", IDENTIFIER, ":", IDENTIFIER, [ "=", expression ] ;  

(* TODO: elif? else if? *)
if                  = "if", expression, block [ "else", block ] ;            

(* Is this even necessary? *)
loop                = "loop", block ;                                        

while               = "while", expression, block ;

return              = "return", expression, ";" ;

variable_assign     = IDENTIFIER, "=", expression ";" ;

(* TODO: add missing stuff *)

(* No emoji in names :( *)
IDENTIFIER          = IDENTIFIER_BASE, { IDENTIFIER_BASE | DEC_DIGIT } ;     
IDENTIFIER_BASE     = "_" | "a" ... "z" | "A" ... "Z" ;

CHAR_LITERAL        = "'", STR_CHAR, "'",
STRING_LITERAL      = '"', { STR_CHAR }, '"'     
NUMBER_LITERAL      = BIN_LITERAL | DEC_LITERAL | HEX_LITERAL ;

HEX_LITERAL         = "0x", HEX_DIGIT, { "_" | HEX_DIGIT } ;
(* TODO: 012 is a valid decimal number *)
DEC_LITERAL         = DEC_DIGIT, { "_" | DEC_DIGIT } ;                    
BIN_LITERAL         = "0b", BIN_DIGIT, { "_" | BIN_DIGIT } ;

(* TODO: add more escape chars *)
STR_CHAR            = ? any utf-8 character except " ? | "\"" | "\t" | "\n" | "\r", "\\"; 
HEX_DIGIT           = DEC_DIGIT | "a" ... "f" | "A" ... "F" ;
DEC_DIGIT           = BIN_DIGIT | "2" ... "9" ;
BIN_DIGIT           = "0" | "1" ;
```
