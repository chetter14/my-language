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
	TypeRef;
	ArrayType;
	SourceItem;
	BlockStatement;
	Condition;
	Body;
	Statement;
	IfStatement;
	WhileStatement;
	RepeatCondition;
	BreakStatement;
	ExpressionStatement;
	FunctionCall;
	ArrayAccess;
	Arguments;
	Indices;
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
	
typeRef	: (builtInType | identifier) arrayType?
		-> ^(TypeRef builtInType? identifier? arrayType?)
	;

builtInType : BuiltInType ;

identifier : Identifier ;

arrayType : 'array'! '['! Dec ']'!;

funcSignature
	: Identifier '(' argList? ')' ('of' typeRef)? -> ^(FuncSignature Identifier argList?)
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

statement : repeat_statement;

block_statment : ('begin'|'{') (statement | sourceItem)* ('end'|'}') -> ^(BlockStatement statement* sourceItem*);

if_statement : 'if' expr 'then' statement ('else' statement)? -> ^(IfStatement ^(Condition expr) ^(Body statement+));

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

assignment_expr : logical_expr ( '='^ expr )? ;

logical_expr : comparison_expr ( ('&&' | '||')^ expr)* ;

comparison_expr
	: additive_expr ( ( '==' | '!=' | '<' | '>' | '<=' | '>=' )^ additive_expr )? 
    ;

additive_expr : multiplicative_expr ( ('+' | '-')^ multiplicative_expr )* ;

multiplicative_expr : unary_expr ( ('*' | '/')^ unary_expr )* ;

unary_expr : (un_op^ unary_expr | primary_expr) ;

primary_expr
	: ( braces 
	| place 
	| literal
	) (function_call | array_access)*
	;
	
function_call : '(' (expr (',' expr)*)? ')' -> ^(FunctionCall expr*);

array_access : '[' expr ('..' expr)? ']' -> ^(ArrayAccess expr+);

braces : '('! expr ')'! ;

place : Identifier ;

literal: Char | Hex | Bits | Dec | Bool | Str ;

un_op : '-' | '!' ;
