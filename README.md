# Hold

Hold is a scripting language.

# Why?

To learn how scripting languages work. Also to "fix" problems with other langauges by making different problems.

# Grammar

TODO:

```ebnf

program             = { use }, { top_level_stmnt } ;

use                 = "use", "{", [IDENTIFIER], { ",", IDENTIFIER }, "}", ';' ;  (* Probably we need an option to alias these, I'm not sure how complicated that is *)

top_level_stmnt     = [ "export" ], (struct_decl | trait_decl | function_decl);

struct_decl         = "struct", IDENTIFIER, "{", { struct_member_decl }, "}" ;
struct_member_decl  = [ IDENTIFIER, ":", IDENTIFIER ], { ",", IDENTIFIER, ":", IDENTIFIER }

trait_decl          = "trait", IDENTIFIER, "{", { trait_func_decl }, "}" ;

trait_func_decl     = IDENTIFIER, "(", function_arg_list, ")", [ ":", IDENTIFIER ] ;

function_decl       = "fn", IDENTIFIER, [ "::", IDENTIFIER ], "(", function_arg_list, ")", [ ":", IDENTIFIER ], block;
function_arg_list   = [ "self" ], { ",", IDENTIFIER, ":", IDENTIFIER } ;    (* TODO: default values *)
function_arg        = IDENTIFIER, ":", IDENTIFIER ;

block               = "{", stmnt_list, "}" ;

stmnt_list          = { variable_decl | if | loop | while | break | continue | return | (( variable_assign | expression ), ";") } | ";" ;

variable_decl       = "let", IDENTIFIER, ":", IDENTIFIER, [ "=", expression ] ;   (* It would be nice if the compiler could infer the type. *)

if                  = "if", expression, block [ "else", block ] ;            (* TODO: elif? else if? *)

(* TODO: add missing stuff *)

IDENTIFIER          = IDENTIFIER_BASE, { IDENTIFIER_BASE | DEC_DIGIT } ;     (* No emoji in names :( *)
IDENTIFIER_BASE     = "_" | "a" ... "z" | "A" ... "Z" ;

STRING_LITERAL      = '"', { STR_CHAR | "\"" | "\n" | "\r", "\\"  }, '"'     (* TODO: add more escape chars *)
NUMBER_LITERAL      = BIN_LITERAL | DEC_LITERAL | HEX_LITERAL ;

HEX_LITERAL         = "0x", HEX_DIGIT, [ "_" ], { HEX_DIGIT } ;
DEC_LITERAL         = DEC_DIGIT, { [ "_" ], DEC_DIGIT } ;                    (* TODO: 012 is a valid decimal number *)
BIN_LITERAL         = "0b", BIN_DIGIT, [ "_" ], { BIN_DIGIT } ;

STR_CHAR            = ? any utf-8 character except " ? ;                     (* I dare you to make a generator that understands this. *)
HEX_DIGIT           = DEC_DIGIT | "a" ... "f" | "A" ... "F" ;
DEC_DIGIT           = BIN_DIGIT | "2" ... "9" ;
BIN_DIGIT           = "0" | "1" ;
```
