%{
#include <glib.h>
#include "Dictionary.h"
#include "TreeBuilder.h"

int track_level(char *str);
void add_to_meta(char *key, char *value);
void add_dir();
void add_file();

GString *curr;

Dictionary meta;
TreeBuilder tree;

char *file,*metadado, *valor;
int stc, level = 0;
%}

%x META FOLDER FILERD SKIP TITLE

wd  [a-zA-Z]+
ws  [0-9a-zA-Z|\@\.áéíãó]+
%%
<TITLE>==[ ]*				{stc = SKIP;g_string_erase(curr,0,-1);}
							/* Estado predifinido é o de ignorar */
<TITLE>(?i:meta)			{stc = META;}
							/* Próximo estado é o analizador de METADADOS */
<TITLE>(?i:tree)			{stc = FOLDER;}
							/* Próximo estado é o analizador de TREE */
<TITLE>{ws}				  	{g_string_append_printf(curr,"%s",yytext);}
							/* Assume-se que próximo estado vai ser de ler ficheiros */
<TITLE>[ ]					{g_string_append_printf(curr, " ");}
<TITLE>[ ]*\n 				{printf("finished title\n");
							if(stc == SKIP) {
								file = strdup(curr->str);
								printf("I have file |%s|\n",file);
								
								if(contains_file(tree, file)) {
									printf("it is\n");
									stc = FILERD;
								}
							}
							g_string_erase(curr,0,-1);
							BEGIN stc;}
							/* Limpa o string buffer, antes do inicio do próximo estado */

<FILERD>.|\n                  {g_string_append_printf(curr, "%s", yytext);}

<SKIP>.|\n              	{;}
							/* No estado SKIP só se ignora */



<META>\n\r?					{add_to_meta(metadado, strdup(curr->str));}
							/* Adiciona aos metadados válidos */
<META>^{wd}					{ metadado = strdup(yytext);}
							/* Armazena o metadado em memória */
<META>[ ]*:[ ]*				{;}


							/* Ignora os espaços entre atribuições */
<META,FOLDER>[ \t]			{g_string_append_printf(curr, " ");}
							/* Espaços e tabs contribuem como tabs */
<META,FOLDER>{ws}  			{g_string_append_printf(curr, "%s", yytext);}
							/* Cada vez que é capturada uma palavra, é adicionada ao string buffer */
<META,FOLDER>^#[^\n]*\n     {;}
							/* No estado META e FOLDER, é possivel adicionar comentários de linha */


<FOLDER>\/\n\r?				{add_dir();}
							/* Deteta que se pretende adicionar uma diretoria, e adiciona */
<FOLDER>\n\r?				{add_file();}
							/* Deteta que se pretende adicionar um ficheiro, e adiciona */
<FOLDER>^-*[ ]*				{level = track_level(yytext);}
							/* Escreve o nivel de profundidade do que se pretende adicionar */


<*>\{%{wd}%\}				{yytext[yyleng-2] = '\0';
							g_string_append_printf(curr, "%s", (char*)getValueDictionary(meta, yytext+2));}
							/* Quando encontra, em qualquer estado, metadados, faz append do correspondente valor */

<*>^=/==					{if(stc == FILERD)
								add_info_to_file(tree, file, strdup(curr->str), curr->len);
							BEGIN TITLE;}
<*>.|\n                		{;}
<FILERD><<EOF>>				{printf("file %s, len %d\ncheguei ao fim\n%s\n", file,curr->len,curr->str);
							add_info_to_file(tree, file, strdup(curr->str), curr->len);
							yyterminate();}
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

int yywrap()
{ 
	return(1); 
}

static void printMeta(gpointer key, gpointer value, gpointer userdata)
{
    printf("%s -> %s\n",(char*)key, (char*)value);
}

int main(int argc, char **argv)
{ 
	meta = makeDictionary();
	tree = make_tree_builder();
	curr = g_string_new(NULL);

	insertDictionary(meta, g_strdup("name"), g_strdup(argv[1]));

	printf("File is %s\n", argv[2]);

	yyin = fopen(argv[2],"r");


	stc = SKIP;

	yylex(); 


	//printf("There are %d keys in the hash table\n", sizeDictionary(meta));


	//foreachDictionary(meta, printMeta);

	printf("Dumping tree\n");

	dump_tree_builder(tree);

	printf("Finished dumping tree\n");

	destroy_tree_builder(tree);
	destroyDictionary(meta);

	return 0; 
}