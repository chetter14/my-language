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
	ParameterList;
	Parameter;
	TypeRef;
	ArrayType;
	Array;
	Function;
	BlockStatement;
	Condition;
	Body;
	Then;
	Else;
	Statement;
	IfStatement;
	WhileStatement;
	RepeatCondition;
	BreakStatement;
	ExpressionStatement;
	Index;
	Args;
	FunctionCall;
	ArrayAccess;
	Slice;
}

/*------------------------------------------------------------------
 * LEXER RULES
 *------------------------------------------------------------------*/
			
BuiltInType: 'bool'|'byte'|'int'|'uint'|'long'|'ulong'|'char'|'string' ;
			
Str : '"' ( ~('"'|'\\') | ('\\'.) )* '"'; // строка, окруженная двойными кавычками

Char: '\''~('\'')'\''; // одиночный символ в одинарных кавычках

Hex: '0'('x' | 'X')(DIGITS | 'A'..'F' | 'a'..'f')+; // шестнадцатеричный литерал

Bits: '0'('b' | 'B')('0' | '1')+; // битовый литерал

Dec: (DIGITS)+; // десятичный литерал

fragment DIGITS : '0'..'9';

Bool: 'true' | 'false'; // булевский литерал

WS  :  (' '|'\r'|'\t'|'\u000C'|'\n') { $channel=HIDDEN; } ;

SINGLE_COMMENT
    :   '//' ~('\n' | '\r')* '\r'? '\n' { $channel = HIDDEN; }
    ;

MULTI_COMMENT
    :   '/*' ( options {greedy=false;} : . )* '*/' { $channel = HIDDEN; }
    ;

Identifier : ('a'..'z' | 'A'..'Z' | '_')('a'..'z' | 'A'..'Z' | '_' | DIGITS)*; // идентификатор
 
/*------------------------------------------------------------------
 * PARSER RULES
 *------------------------------------------------------------------*/
		
source: sourceItem* -> ^(Source sourceItem*);
	
typeRef	: (builtInType | identifier | arrayType)
		-> ^(TypeRef builtInType? identifier? arrayType?)
	;

builtInType : BuiltInType ;

identifier : Identifier ;

arrayType : (builtInType | identifier) arraySuf -> ^(ArrayType builtInType? identifier? arraySuf) ;

arraySuf : 'array' '[' Dec ']' (arraySuf)? -> ^(Array Dec arraySuf?);

funcSignature
	: Identifier '(' parameterList? ')' ('of' typeRef)? -> ^(FuncSignature Identifier parameterList? typeRef?)
	;

parameterList	
	: parameter (',' parameter)* -> ^(ParameterList parameter+)
	;

parameter
	: Identifier ('of' typeRef)? -> ^(Parameter Identifier typeRef?)
	;

sourceItem
	: 'def' funcSignature statement* 'end' -> ^(Function funcSignature statement*)
	;
	
// STATEMENTS:

statement : repeat_statement;

block_statment : ('begin'|'{') (statement | sourceItem)* ('end'|'}') -> ^(BlockStatement statement* sourceItem*);

if_statement : 'if' expr 'then' statement ('else' statement)? -> ^(IfStatement ^(Condition expr) ^(Then statement) ^(Else statement)?);

while_statement : ('while'|'until') expr statement* 'end' -> ^(WhileStatement ^(Condition expr) ^(Body statement*));

repeat_statement 
	: (if_statement
	| while_statement
	| block_statment
	| break_statement
	| expr_statement
	) (('while'|'until') expr ';')? 
	-> ^(Statement if_statement? while_statement? block_statment? break_statement? expr_statement? ^(RepeatCondition expr)?)
	;
	
break_statement : 'break' ';' -> ^(BreakStatement);

expr_statement : expr ';' -> ^(ExpressionStatement expr);

// EXPRESSIONS:

expr : assignment_expr;

assignment_expr : logical_or_expr ( '='^ expr )? ;

logical_or_expr: logical_and_expr ('||'^ expr)*;

logical_and_expr : comparison_expr ('&&'^ expr)* ;

comparison_expr
	: additive_expr ( ( '==' | '!=' | '<' | '>' | '<=' | '>=' )^ additive_expr )? 
    ;

additive_expr : multiplicative_expr ( ('+' | '-')^ multiplicative_expr )* ;

multiplicative_expr : bitwise_or_expr ( ('*' | '/' | '%')^ bitwise_or_expr )* ;

bitwise_or_expr : bitwise_xor_expr ('|'^ bitwise_xor_expr)* ;

bitwise_xor_expr : bitwise_and_expr ('^'^ bitwise_and_expr)* ;

bitwise_and_expr : unary_expr ('&'^ unary_expr)* ;

unary_expr : (un_op^ unary_expr | primary_expr) ;

primary_expr 
	: base_expr '(' (expr (',' expr)*)? ')' expr_suf? -> ^(FunctionCall base_expr ^(Args expr*) expr_suf?)
	| base_expr '[' indexing (',' indexing)* ']' expr_suf? -> ^(ArrayAccess base_expr indexing+ expr_suf?)
	| base_expr
	;
	
base_expr : braces | place | literal ;

expr_suf 
	: '(' (expr (',' expr)*)? ')' expr_suf? -> ^(FunctionCall ^(Args expr*) expr_suf?)
	| '[' indexing (',' indexing)* ']' expr_suf? -> ^(ArrayAccess indexing+ expr_suf?)
	;

indexing
	: expr ('..' expr)? -> ^(Index expr expr?)
	;
	
braces : '('! expr ')'! ;

place : Identifier ;

literal: Char | Hex | Bits | Dec | Bool | Str ;

un_op : '-' | '!' | '~';
