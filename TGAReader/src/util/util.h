#pragma once
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
	char *word;
	int wordCount;  
}Word; 

void getUserInput(int toLower, char *input);
int GetHash(const char *s);

