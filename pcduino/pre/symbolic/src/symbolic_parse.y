%name-prefix="symbolic_parse_"
%{
#include "symbolic_parse_internal.hpp"
using namespace std;
using namespace symbolic;

extern YYSTYPE symbolic_parse_lval;
%}


%token NUMBER VARIABLE

%left '+' '-'
%left '*' '/'
%left NEGATE
%nonassoc '^'

%%

start : expression { symbolic_parse_result=$1; $1=NULL; }

expression : 
  expression '+' expression 
  { 
    $$ = new Sum($1,$3); 
    $1=$3=NULL;
  }
|
  expression '-' expression
  { 
    $$ = new Sum($1,new Product(new Constant(-1.0),$3));
    $1=$3=NULL;
  }
|
expression '*' expression 
  { 
    $$ = new Product($1,$3);
    $1=$3=NULL;
  }
|
expression '/' expression
{
  $$ = new Product($1,new Power($3,new Constant(-1)));
  $1=$3=NULL;
}
|
expression '^' expression 
  {  
    $$ = new Power($1,$3);
    $1=$3=NULL;
  }
|
  '+' expression %prec NEGATE
  { 
    $$=$2;
    $2=NULL;
  }
|
  '-' expression %prec NEGATE
  { 
    $$ = new Product(new Constant(-1.0),$2);
    $2=NULL;
  }
|
'(' expression ')' { $$=$2; $2=NULL; }
|
VARIABLE optional_args { 
  if ($2 == NULL) 
  {
    $$=$1;
    $1=NULL; 
  }
  else 
  {
    string name=((Variable*)$1)->name;
    if (name == "log") 
    {
      $$=new NaturalLog($2);
      $1=$2=NULL;
    }
    else if (name == "exp") 
    {
      $$=new Power(new Variable("%e"),$2);
      $1=$2=NULL;
    } 
    else if (name == "cos")
    {
      $$=new Cos($2);
      $1=$2=NULL;
    }
    else if (name == "sin")
    {
      $$=new Sin($2);
      $1=$2=NULL;
    }
    else 
    {
      std::ostringstream oss;
      oss << name << " function is not supported.";
      symbolic::Expression::UnsupportedSyntax exception;
      exception.message = oss.str();
      throw exception;
    }
  }
}
|
NUMBER { $$=$1; $1=NULL; }
;

optional_args :
/* null */ { $$=NULL; }
| 
 '(' expression ')' { $$=$2; $2=NULL; }
;
