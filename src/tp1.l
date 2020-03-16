%{
#include <glib.h>
#include "Dictionary.h"
#include "TreeBuilder.h"

int track_level(char *str);

GString *curr;

Dictionary meta;
TreeBuilder tree;

char *metadado, *valor;
int level = 0;
%}

%x META FOLDER

wd  [a-zA-Z]+
ws  [0-9a-zA-Z|\@\.áéíãó]+
%%
^===[ ]*(?i:meta)[ ]*\n 				{BEGIN META;}
<META>\n\r?				{if(curr->len > 0)
							insertDictionary(meta,metadado,strdup(curr->str));
						g_string_erase(curr,0,-1);}
<META>^{wd}				{ metadado = strdup(yytext);}
<META>[ ]*:[ ]*			{;}
<META,FOLDER>[ \t]		{g_string_append_printf(curr, " ");}
<META,FOLDER>{ws}  			{g_string_append_printf(curr, "%s", yytext);
						}
<META,FOLDER>^#[^\n]*\n        {;}
<META,FOLDER>.*/===			{BEGIN INITIAL;}

^===[ ]*(?i:tree)[ ]*\n 				{BEGIN FOLDER;}
<FOLDER>\/\n\r?			{printf("adding dir\n");
						if(curr->len > 0)
							add_dir_tree_builder(tree, strdup(curr->str), level);
						g_string_erase(curr,0,-1);
						level = 0;}
<FOLDER>\n\r?				{printf("adding file\n");
						if(curr->len > 0)
							add_file_tree_builder(tree, strdup(curr->str), level);
						g_string_erase(curr,0,-1);
						level = 0;}
<FOLDER>^-*[ ]*		{level = track_level(yytext);}
<*>\{%{wd}%\}			{yytext[yyleng-2] = '\0';
					g_string_append_printf(curr, "%s", (char*)getValueDictionary(meta, yytext+2));}
<*>.|\n                {;}
%%

int track_level(char *str) {
	int i;

	for(i = 0; str[i] && str[i] == '-'; i++)
			;

	return i;
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

	yylex(); 

	printf("There are %d keys in the hash table\n", sizeDictionary(meta));

	foreachDictionary(meta, printMeta);

	printf("Dumping tree");
	dump_tree_builder(tree);

	destroy_tree_builder(tree);
	destroyDictionary(meta);

	return 0; 
}