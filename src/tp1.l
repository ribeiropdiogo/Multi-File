%{
#include <glib.h>
#include "Dictionary.h"

Dictionary meta;
char *metadado, *valor;
%}

%x META
%%

^===[ ]*(?i:meta)[^===] 				{BEGIN META;}
<META>^[a-zA-Z]+[ ]*					{ metadado = strdup(yytext);}
<META>:[ ]*[a-zA-Z|\@\.áéíãó]+[^\n]  	{ valor = strdup(yytext+1);
											insertDictionary(meta,metadado,valor);
										}
<META>#.*								{}
<META>.*/===							{BEGIN INITIAL;}

^===[ ]*(?i:tree)[^===]+ 			{printf("tree\n");}
^===[ ]*(?i:makefile)[^===]+ 		{printf("makefile\n");}
^===[ ]*\{\%name\%\}\.md[^===]+ 	{printf("markdown\n");}
^===[ ]*\{\%name\%\}\.fl[^===]+		{printf("flex\n");}
^===[ ]*(?i:readme)[^===]+ 			{printf("readme\n\n");}
%%

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

	insertDictionary(meta, g_strdup("name"), g_strdup(argv[1]));

	yylex(); 

	printf("There are %d keys in the hash table\n", sizeDictionary(meta));

	foreachDictionary(meta, printMeta);

	destroyDictionary(meta);

	return 0; 
}