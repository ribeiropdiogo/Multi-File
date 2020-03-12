%{
#include <glib.h>

GHashTable *metadados;
char *metadado, *valor;
%}

%x META
%%

^===[ ]*(?i:meta)[^===] 				{BEGIN META;}
<META>^[a-zA-Z]+[ ]*					{ metadado = strdup(yytext);}
<META>:[ ]*[a-zA-Z|\@\.áéíãó]+[^\n]  	{ valor = strdup(yytext+1);
											g_hash_table_insert(metadados,metadado,valor);
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
    printf("%s -> %s\n",key,value);
}

int main(int argc, char **argv)
{ 
	metadados = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(metadados,"name",argv[1]);

	yylex(); 


	printf("There are %d keys in the hash table\n", g_hash_table_size(metadados));

	g_hash_table_foreach(metadados, printMeta, kill);

	return 0; 
}
