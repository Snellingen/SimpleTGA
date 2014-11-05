#include "util.h"
void getUserInput(int toLower, char *input)
{	
	if(scanf("%s", input) != 1)
	{
		printf("\nInvalid input:");
		getUserInput(toLower, input);
	}
	if (!toLower) return;
	for(int i = 0; input[i]; i++)
	{
  		input[i] = tolower(input[i]);
	}
}



//Java implementation of string hash
//http://grepcode.com/file/repository.grepcode.com/java/root/jdk/openjdk/6-b14/java/lang/String.java#String.hashCode%28%29
int GetHash(const char *s)
{
	int hash = 0;
	while (*s)
	{
		hash = 31 * hash + (*s++);
	}
	return hash;
}


