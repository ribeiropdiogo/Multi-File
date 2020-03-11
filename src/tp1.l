%{
char *name, *email, *author;
%}


%x META
%%
^===[ ]*(?i:meta)[^===]+ 			{printf("Meta:\n%s \n\n----------\n",yytext);}
^===[ ]*(?i:tree)[^===]+ 			{printf("Tree:\n%s \n\n----------\n",yytext);}
^===[ ]*(?i:makefile)[^===]+ 		{printf("Makefile:\n%s \n\n----------\n",yytext);}
^===[ ]*\{\%name\%\}\.md[^===]+ 	{printf("Markdown:\n%s \n\n----------\n",yytext);}
^===[ ]*\{\%name\%\}\.fl[^===]+		{printf("Flex:\n%s \n\n----------\n",yytext);}
%%

int yywrap()
{ 
	return(1); 
}

int main(int argc, char **argv)
{ 
	name = strdup(argv[1]);
	printf("Para efeitos oficiais o name = %s\n\n\n\n",name);
	yylex(); 
	return 0; 
}
