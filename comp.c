// With significant assistance from https://github.com/JakobCh
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shared.h"

int str_len(char* str)
{
    int i = 0;
    while(str[i] != 0)
        i++;
    return i;
}

int IsWhiteSpace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n');
}

int GetInst(char* line, char* inst, char* arg)
{
    int i = 0;

    while(!IsWhiteSpace(line[i]) && line[i] != 0) //read until we hit whitespace/end of string
    {
        inst[i] = line[i];
        //printf("Adding char: %c\n", inst[i]);
        i++;
    }
    inst[i] = 0; //add a 0 onto the end
    
    if (line[i] == 0) //if we stoped because the string ended
        return 0; //we can't pickup a arg so return

    while(IsWhiteSpace(line[i])) //skip all whitespace
        i++;

    int l = 0;
    while(!IsWhiteSpace(line[i]) && line[i] != 0)
    {
        arg[l] = line[i];
        i++;
        l++;
    }
    //printf("[GetInst] Found instruction %s with arg %s.\n", inst, arg);
    return 0;
}


int main(int argc, char* argv[])
{
    if (argc !=  3 && !strstr(argv[2], ".cdwn") && !strstr(argv[1], ".darwin"))
    {
        printf("Usage: %s inputfile.darwin outputfile.cdwn\n", argv[0]);
        return 1;
    }
    char* in_file = argv[1];
	char* out_file = argv[2];

    int sizeLine = 0;
    int sizeChar = 0;
    char lines[256][256];

    FILE* f = fopen(in_file, "rb");

    int currentChar;
    while((currentChar = fgetc(f)) != EOF)
    {
        if (currentChar == '\n')
        {
            lines[sizeLine][sizeChar] = 0;
            sizeLine++;
            sizeChar = 0;
        } else {
            if (currentChar >= 'a' && currentChar <= 'z') //if it's a lowercase character make it a uppercase one 
                currentChar -= 32;
            lines[sizeLine][sizeChar] = currentChar;
            sizeChar++;
        }
    }
    fclose(f);

    char outBuffer[256];
    int out_size = 0;

    for (int i = 0; i < sizeLine+1; i++)
    {
        char inst[256];
        char arg[256];

        GetInst(lines[i], inst, arg);
        for(int l = 0; l < INSTRUCTION_COUNT; l++)
        {
            if (strcmp(inst, strInstruction[l]) == 0) //match instruction
            {
                printf("Instruction %s (%d)", strInstruction[l], l);
                outBuffer[out_size] = (char)l;
                out_size++;

                if (l >= INFECT)
                {
                    char temp = atoi(arg);
                    printf(" with arg %d", temp);
                    outBuffer[out_size] = temp;
                    out_size++;
                }
                    
                printf("\n");
            }
        }
    }
    
    FILE* f2 = fopen(out_file, "wb");
    fwrite(outBuffer, 1, out_size, f2);
    fclose(f2);
    return 0;
}
