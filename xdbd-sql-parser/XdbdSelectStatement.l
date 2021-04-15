
%option	outfile="sql-select.lex.c" header-file="sql-select.lex.h"
%option	reentrant bison-bridge
%option	noyywrap nounput noinput

%{
#include "sql-select.tab.h"

extern	void SaveLastName (void* obj, char* name);
extern	void SaveLastNumber (void* obj, char* name);
extern	void SaveLastString (void* obj, char* name);

%}

TSELECT                 ?i:SELECT
TUPDATE                 ?i:UPDATE
TINSERT                 ?i:INSERT
TDELETE                 ?i:DELETE
TCATALOG                ?i:CATALOG
TFROM                   ?i:FROM
TWHERE                  ?i:WHERE
TINTO                   ?i:INTO
TNULL                   ?i:NULL
TSET                    ?i:SET
TVALUES                 ?i:VALUES
TAND                    ?i:AND
TOR                     ?i:OR
TLIKE                   ?i:LIKE
TORDER                  ?i:ORDER
TBY                     ?i:BY
TASC                    ?i:ASC
TASCENDING              ?i:ASCENDING
TDESC                   ?i:DESC
TDESCENDING             ?i:DESCENDING
TAS                     ?i:AS
TIS                     ?i:IS
TNOT                    ?i:NOT

LATIN_CAPITAL_LETTER    [A-Z]
LATIN_SMALL_LETTER      [a-z]
DECIMAL_DIGIT           [0-9]
LATIN_LETTER            ({LATIN_CAPITAL_LETTER}|{LATIN_SMALL_LETTER})

QUOTATION_MARK          \"
APOSTROPHE              \'
AMPERSAND               \&
LEFT_PARENTHESIS        \(
RIGHT_PARENTHESIS       \)
ASTERISK                \*
COMMA                   \,
SEMICOLON               \;
MINUS_SIGN              \-
PLUS_SIGN               \+
FULL_STOP               \.
LESS_THAN_SIGN          \<
EQUALS_SIGN             \=
GREATER_THAN_SIGN       \>
LOW_LINE                \_
VERTICAL_LINE           \|
EXCLAMATION_MARK        \!
QUESTION_MARK           \?

EQ                      {EQUALS_SIGN}
NE                      {EXCLAMATION_MARK}{EQUALS_SIGN}|{LESS_THAN_SIGN}{GREATER_THAN_SIGN}
LT                      {LESS_THAN_SIGN}
LE                      {LESS_THAN_SIGN}{EQUALS_SIGN}
GT                      {GREATER_THAN_SIGN}
GE                      {GREATER_THAN_SIGN}{EQUALS_SIGN}
LBR                     {LEFT_PARENTHESIS}
RBR                     {RIGHT_PARENTHESIS}
AND                     {TAND}|{AMPERSAND}
OR                      {TOR}|{VERTICAL_LINE}

NAME                    {LATIN_LETTER}({LATIN_LETTER}|{FULL_STOP}|{DECIMAL_DIGIT}|{LOW_LINE})*
NUMBER                  ({MINUS_SIGN}|{PLUS_SIGN})?({DECIMAL_DIGIT})+
QSTRING                 {QUOTATION_MARK}([^{QUOTATION_MARK}]|{QUOTATION_MARK}{QUOTATION_MARK})*{QUOTATION_MARK}
ASTRING                 {APOSTROPHE}([^{APOSTROPHE}]|{APOSTROPHE}{APOSTROPHE})*{APOSTROPHE}

%%

{TSELECT}               return SELECT;
{TUPDATE}               return UPDATE;
{TINSERT}               return INSERT;
{TDELETE}               return DELETE;
{TCATALOG}              return CATALOG;
{TFROM}                 return FROM;
{TWHERE}                return WHERE;
{TINTO}                 return INTO;
{TNULL}                 return TNULL;
{TSET}                  return SET;
{TVALUES}               return VALUES;
{AND}                   return AND;
{OR}                    return OR;
{TLIKE}                 return LIKE;
{TORDER}                return ORDER;
{TBY}                   return BY;
{TASC}                  return ASC;
{TASCENDING}            return ASCENDING;
{TDESC}                 return DESC;
{TDESCENDING}           return DESCENDING;
{TAS}                   return AS;
{TIS}                   return IS;
{TNOT}                  return NOT;

{NAME}                  { SaveLastName (yyget_extra (yyscanner), yyget_text (yyscanner)); return NAME; }
{NUMBER}                { SaveLastNumber (yyget_extra (yyscanner), yyget_text (yyscanner)); return NUMBER; }
\"([^\"]|\"\")*\"       { SaveLastString (yyget_extra (yyscanner), yyget_text (yyscanner)); return STRING; }
\'([^\']|\'\')*\'       { SaveLastString (yyget_extra (yyscanner), yyget_text (yyscanner)); return STRING; }
{COMMA}                 return COMMA;
{SEMICOLON}             return SEMICOLON;
{FULL_STOP}             return DOT;
{ASTERISK}              return ASTERISK;
{QUESTION_MARK}         return QUESTION_MARK;

{EQ}                    return EQ;
{NE}                    return NE;
{LT}                    return LT;
{LE}                    return LE;
{GT}                    return GT;
{GE}                    return GE;
{LBR}                   return LBR;
{RBR}                   return RBR;

[ \t\r\n]               continue;

%%

extern int yyerror (char*msg)
{
	printf ("%s\n", msg);
	return	-1;
}
