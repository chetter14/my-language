grammar MyLanguage;
 
options
{
    language=C;
	output=AST;
	ASTLabelType = pANTLR3_BASE_TREE;
	backtrack=true;
}
 
tokens
{
	Source;
	FuncSignature;
	ArgList;
	Arg;
	SourceItem;
}

/*------------------------------------------------------------------
 * LEXER RULES
 *------------------------------------------------------------------*/
			
Str : '"' ( ~('"'|'\\') | ('\\'.) )* '"'; // строка, окруженная двойными кавычками

Char: '\''~('\'')'\''; // одиночный символ в одинарных кавычках

Hex: '0'('x' | 'X')(DIGITS | 'A'..'F' | 'a'..'f')+; // шестнадцатеричный литерал

Bits: '0'('b' | 'B')('0' | '1')+; // битовый литерал

Dec: (DIGITS)+; // десятичный литерал

fragment DIGITS : '0'..'9';

Bool: 'true' | 'false'; // булевский литерал

WS  :  (' '|'\r'|'\t'|'\u000C'|'\n') { $channel=HIDDEN; }
  ;

Identifier : ('a'..'z' | 'A'..'Z' | '_')('a'..'z' | 'A'..'Z' | '_' | DIGITS)*; // идентификатор
 
/*------------------------------------------------------------------
 * PARSER RULES
 *------------------------------------------------------------------*/
		
source: sourceItem* -> ^(Source sourceItem*);
	
typeRef
	: 	('bool'|'byte'|'int'|'uint'|'long'|'ulong'|'char'|'string'
		| Identifier
		) ('array' '[' Dec ']')? // число - размерность
	;

funcSignature
	: Identifier '(' argList? ')' ('of' typeRef)? -> ^(FuncSignature Identifier argList? typeRef?)
	;

argList	
	: arg (',' arg)* -> ^(ArgList arg+)
	;

arg
	: Identifier ('of' typeRef)? -> ^(Arg Identifier typeRef?)
	;

sourceItem
	: 'def' funcSignature statement* 'end' -> ^(SourceItem funcSignature statement*)
	;
	
// STATEMENTS:

statement // присваивание через '='   
	: break_statement
	| expr_statement
	| if_statement
	| while_statement
	| repeat_statement
	| block_statment
	;

block_statment : ('begin'|'{') (statement | sourceItem)* ('end'|'}') ;

if_statement : 'if' expr 'then' statement ('else' statement)? ;

while_statement : ('while'|'until') expr statement* 'end' ;

repeat_statement 
	: (if_statement
	| while_statement
	| block_statment
	| break_statement
	| expr_statement
	) ('while'|'until') expr ';' 
	;
	
break_statement : 'break' ';' ;

expr_statement : expr ';' ;

// EXPRESSIONS:

expr : assignment_expr ;

assignment_expr : logical_expr ( '='^ expr )? ;

logical_expr : comparison_expr ( ('&&' | '||')^ expr)* ;

comparison_expr
	: additive_expr ( ( '==' | '!=' | '<' | '>' | '<=' | '>=' )^ additive_expr )? 
    ;

additive_expr : multiplicative_expr ( ('+' | '-')^ multiplicative_expr )* ;

multiplicative_expr : unary_expr ( ('*' | '/')^ unary_expr )* ;

unary_expr : (un_op^ unary_expr | primary_expr) ;

primary_expr
	: (braces
	| place
	| literal
	) (
		( '(' (expr (',' expr)*)? ')' )?	// call handling
		| ( '[' expr ('..' expr)? ']' )?	// slice handling
	)
    ;

braces : '(' expr ')' ;

place : Identifier ;

literal: Char | Hex | Bits | Dec | Bool | Str ;

un_op : '-' | '!' ;
