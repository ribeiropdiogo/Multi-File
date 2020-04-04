%option noyywrap

%{
#include <glib.h>
#include "Dictionary.h"
#include "TreeBuilder.h"

GString *curr;				// String buffer usado.
Dictionary meta;			// Dicionario de metados.
TreeBuilder tree;			// Arvore de diretorias.
char *file, *metadado;		// Nome de ficheiro e meta em memoria.
int stc;					// Estado a considerar.
int level = 0;				// Nivel de profundidade da diretoria.

/* 
	Regista nivel de profundidade da diretoria
	Passada como argumento.
*/
int track_level(char *str);

/*
	Adiciona os metadados ao dicionario.
*/
void add_to_meta(char *key, char *value);

/*
	Adiciona a diretoria inscrita em memoria
	à arvore de diretorias.
*/
void add_dir();

/*
	Adiciona o ficheiro inscrito em memoria
	à arvore de diretorias.
*/
void add_file();
%}

%x META FOLDER FILERD SKIP TITLE

wd  [a-zA-Z0-9.]+
ws  [^ \n\r\t{}]+
wt	[^ \n\r\r/{}]+
%%
<*>^=/==					{if(stc == FILERD)
								add_info_to_file(tree, file, strdup(curr->str), curr->len);
							BEGIN TITLE;}
							/* Inicio de titulo detetado, se estado anteriro for o de leitura de ficheiro, então adiciona a informação obtida à arvore de diretoria. */

<*>\{%{wd}%\}				{yytext[yyleng-2] = '\0';
							g_string_append_printf(curr, "%s", (char*)getValueDictionary(meta, yytext+2));}
							/* Quando encontra, em qualquer estado, metadados, faz append do correspondente valor. */

<TITLE>==[ ]*				{stc = SKIP;
							g_string_erase(curr,0,-1);}
							/* Estado predifinido é o de ignorar */

<TITLE>(?i:meta)			{stc = META;}
							/* Próximo estado é o analizador de METADADOS */

<TITLE>(?i:tree)			{stc = FOLDER;}
							/* Próximo estado é o analizador de TREE */

<TITLE>{wt}				  	{g_string_append_printf(curr,"%s",yytext);}
							/* Assume-se que próximo estado vai ser de ler ficheiros */

<TITLE>[ ]					{g_string_append_printf(curr, " ");}
							/* Sempre que encontra um espaço entre palavras, concatena ao buffer, permitindo assim nomear ficheiros com varias palavras */

<TITLE>[ ]*\n 				{if(stc == SKIP) {
								file = strdup(curr->str);
								
								if(contains_file(tree, file)) {
									stc = FILERD;
								}
							}
							g_string_erase(curr,0,-1);
							BEGIN stc;}
							/* Limpa o string buffer, antes do inicio do próximo estado */

<FILERD>.|\n                {g_string_append_printf(curr, "%s", yytext);}
							/* Qualquer informação deve ser adicionada ao fim do buffer associado ao ficheiro. */

<FILERD><<EOF>>				{add_info_to_file(tree, file, strdup(curr->str), curr->len);
							yyterminate();}
							/* Caso se encontre o EOF, durante o estado FILERD, entao a informação do ficheiro é adicionada à arvore de diretoria. */

<SKIP>.|\n              	{;}
							/* No estado SKIP só se ignora */

<META>\n\r?					{add_to_meta(metadado, strdup(curr->str));}
							/* Adiciona aos metadados válidos */

<META>^{wd}/[ ]*:[ ]*		{ metadado = strdup(yytext);}
							/* Armazena o metadado em memória */

<META>[ ]*:[ ]*				{;}
							/* Ignora os espaços entre atribuições */

<META,FOLDER>[ ]			{g_string_append_printf(curr, " ");}
							/* Espaços e tabs contribuem como tabs */

<META,FOLDER>^#[^\n]*\n     {;}
							/* No estado META e FOLDER, é possivel adicionar comentários de linha */

<FOLDER>\/\n\r?				{add_dir();}
							/* Deteta que se pretende adicionar uma diretoria, e adiciona */

<FOLDER>\n\r?				{add_file();}
							/* Deteta que se pretende adicionar um ficheiro, e adiciona */

<FOLDER>^-*[ ]*				{level = track_level(yytext);}
							/* Escreve o nivel de profundidade do que se pretende adicionar */

<META>{ws}					{g_string_append_printf(curr, "%s", yytext);}
							/* Cada vez que é capturada uma palavra, é adicionada ao string buffer */

<FOLDER>{wt}	  			{g_string_append_printf(curr, "%s", yytext);}
							/* Cada vez que é capturada uma palavra, é adicionada ao string buffer */


<*>.|\n                		{;}
							/* Pretende-se ignorar qualquer outro caso. */
%%

int track_level(char *str) {
	int i;

	for(i = 0; str[i] && str[i] == '-'; i++)
			;

	return i;
}

void add_to_meta(char *key, char *value) {
	if(curr->len > 0)
		insertDictionary(meta,key,value);
	g_string_erase(curr,0,-1);
}

void add_dir() {
	if(curr->len > 0)
		add_dir_tree_builder(tree, strdup(curr->str), level);
	g_string_erase(curr,0,-1);
	level = 0;
}

void add_file() {
	if(curr->len > 0)
		add_file_tree_builder(tree, strdup(curr->str), level);
	g_string_erase(curr,0,-1);
	level = 0;
}

int main(int argc, char **argv)
{ 
	if(argc != 3) {
		fprintf(stderr, "Wrong number of arguments!");
		return 1;
	}

	// Construcao das estruturas principais.
	meta = makeDictionary();
	tree = make_tree_builder();

	// Construcao do buffer utilizado.
	curr = g_string_new(NULL);

	// Insere o primeiro metadado, o nome do projeto.
	insertDictionary(meta, g_strdup("name"), g_strdup(argv[1]));

	yyin = fopen(argv[2],"r");

	stc = SKIP;

	yylex();

	// Constroi todas as diretorias e ficheiros em memoria.
	dump_tree_builder(tree);

	// Liberta memória associada.
	destroy_tree_builder(tree);
	destroyDictionary(meta);

	return 0; 
}