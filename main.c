#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shared.h"
#define NUM_WILDS 0

typedef struct 
{
	char instructions[256]; //Stores instructions and arguments
	int size; //how much we're using of "instructions"
    int lines[100]; // lines[line-1] is equal to the index in "instructions" where that line starts
    int num_lines; //how many lines we have
    char letter;
} CreatureProgram;

typedef struct 
{
	CreatureProgram* program;
	int current_line, facing, x, y;
} Creature;

typedef struct 
{
	int width, height;
	CreatureProgram** programs;
	Creature*** field;
} Board;

CreatureProgram* ProgramFromFile(const char* filename);
void CalculateLines(CreatureProgram* prog);
void DisplayBoard(Board* board);
void ExecuteProgram(Creature* prog, Board* board);
void PrintProgram(Creature* creature);
void ProgramToFile(const char* filename, CreatureProgram* prog);
void usage(char* name);

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		usage(argv[0]);
	}

    Board* board = malloc(sizeof(Board));
    if (!board)
    {
		printf("Could not allocate memory!\n");
		usage(argv[0]);
	}
    board->width = 10;
    board->height = 10;
    board->programs = calloc(argc + NUM_WILDS , sizeof(CreatureProgram*));
    if (!board->programs)
    {
		printf("Could not allocate memory!\n");
		usage(argv[0]);
	}
    char* arg;
    int num_species = 0, x = 0, y = 0, per_species = 0, max_cycles = 1000, snapshots = 10;
	for (int i = 1; i < argc; i++)
	{
		arg = argv[i];
		if (!strcmp(arg, "-c")) printf("ffug\n");
		if (!strcmp(arg, "-w") && ++i < argc) 
		{
			x = atoi(argv[i]);
			board->width = (x != 0) ? x : board->height;
		}
		else if (!strcmp(arg, "-h") && ++i < argc)
		{
			x = atoi(argv[i]);
			board->height = (x != 0) ? x : board->height;
		}
		else if (!strcmp(arg, "-n") && ++i < argc) 
		{
			x = atoi(argv[i]);
			per_species = (x != 0) ? x : per_species;
		}
		else if ((!strcmp(arg, "-c") || !strcmp(arg, "-cycles")) && ++i < argc) 
		{
			x = atoi(argv[i]);
			max_cycles = (x <= 0) ? max_cycles : x;
		}
		else if (!strcmp(arg, "-s") && ++i < argc)
		{
			x = atoi(argv[i]);
			snapshots = (x <= 0) ? snapshots : x;
		}
		else if (strstr(arg, ".cdwn"))
		{
			if (num_species < 100) 
			{
				CreatureProgram* program = ProgramFromFile(argv[i]);
				if (program)
				{
					CalculateLines(program);
					program->letter = num_species + 'A';
					printf("The species from '%s' is the the symbol '%c'!\n", argv[i], program->letter);
					board->programs[num_species] = program;
					num_species++;
				}
			}
		}
		else
		{
			free(board->programs);
			free(board);
			usage(argv[0]);
		}
	}
	// Final steps before the simulation begins
	if (!per_species)
	{
		per_species = (board->width * board->height) / (4 * num_species);
	}
	board->field = calloc(board->height, sizeof(Creature**));
	if (!board->field)
    {
		printf("Could not allocate memory!\n");
		return -1;
	}
	for (int i = 0; i < board->height; i++)
	{
		board->field[i] = calloc(board->width, sizeof(Creature*));
		if (!board->field[i])
		{
			printf("Could not allocate memory!\n");
			return -1;
		}
	}
	// Only references
	Creature** creatures = calloc(num_species * per_species, sizeof(Creature*));
	if (!creatures)
    {
		printf("Could not allocate memory!\n");
		return -1;
	}
	int species_completed = 0;
	per_species = 5;
	for (int species = 0; species < num_species; species++) 
	{
		for (int current = 0; current < per_species; current++) 
		{
			x = rand() % board->width;
			y = rand() % board->height;
			if (board->field[y][x])
			{
				current--;
				continue;
			}
			board->field[y][x] = malloc(sizeof(Creature));
			if (board->field[y][x]) 
			{
				board->field[y][x]->program = board->programs[species];
				board->field[y][x]->facing = rand() % 4;
				board->field[y][x]->current_line = 0;
				board->field[y][x]->x = x;
				board->field[y][x]->y = y;
				creatures[species_completed++] = board->field[y][x];
			}
			else 
			{
				board->field[y][x] = NULL;
				current--;
			}
		}
	}
	// Run the game!
	printf("Initial State\n");
	DisplayBoard(board);
	int iterations = 1;
	while (iterations <= max_cycles)
	{
		for (int i = 0; i < species_completed; i++)
		{
			ExecuteProgram(creatures[i], board);
		}
		if (iterations % (max_cycles / snapshots) == 0)
		{
			printf("After %i cycles(out of %i)\n", iterations, max_cycles);
			DisplayBoard(board);
		}
		iterations++;
	}

	// Deallocate all allocated memory
	free(creatures);
	for (int i = 0; i < num_species; i++)
	{
		free(board->programs[i]);
	}
	free(board->programs);

	for (int y = 0; y < board->height; y++)
	{
		for (int x = 0; x < board->width; x++) 
		{
			if (board->field[y][x])
			{
				free(board->field[y][x]);
			}
		}
		free(board->field[y]);
	}
	free(board->field);
	free(board);
	return 0;
}

void DisplayBoard(Board* board)
{
	for (int x = 0; x < board->width; x++)
		printf("===");
	printf("==\n");
	
	for (int y = 0; y < board->height; y++)
	{
		printf("|");
		for (int x = 0; x < board->width; x++) 
		{
			if (board->field[y][x])
			{
				printf(" %c ", (char)board->field[y][x]->program->letter);
			}
			else
			{
				printf("   ");
			}
		}
		printf("|\n");
	}
	for (int x = 0; x < board->width; x++)
		printf("===");
	printf("==\n");
}

void PrintProgram(Creature* creature)
{
    printf("Creature %p: %i %i\n", creature, creature->x, creature->y);
    CreatureProgram* prog = creature->program;
    int validPc = FALSE;
    printf("\tInstructions:\n");
    for (int l = 0; l < prog->size; l++)
    {
        int inst = prog->instructions[l];
        //if we line up with creature->current_line we've found the currently executing instruction
        if (l == creature->current_line)
        {
            printf(">");
            validPc = TRUE; 
        }

        if (inst >= INFECT)
        {
            printf("\t\t%d %s %d %i\n", l, strInstruction[inst], prog->instructions[l + 1], prog->instructions[l]);
            l += 1;
        }
        else
        {
            printf("\t\t%d %s\n", l, strInstruction[inst]);
        }
    }
    if (validPc == FALSE) //we never found the current instruction, that means it's pointing to an argument BAD
        printf("INVALID PC %d\n", creature->current_line);

    printf("\tLine table:\n");
    for (int l = 0; l < prog->num_lines; l++)
    {
        printf("\t\t%d=>%d\n", l, prog->lines[l]);
    }


    printf("\n");
}

void ExecuteProgram(Creature* creature, Board* board)
{
	if (!creature || !board)
	{
		return;
	}
	unsigned char inst = 0;
	unsigned char arg = 0;
	int executed_action = FALSE;
	CreatureProgram* prog = creature->program;
	int x = creature->x, y = creature->y, old_x = x, old_y = y;
	while (!executed_action && creature->current_line < prog->num_lines)
	{
		inst = prog->instructions[creature->current_line];
        if (inst >= INFECT) //if the instruction is INFECT or higher we need the argument, else we can ignore it
        {
            creature->current_line++;
            arg = prog->instructions[creature->current_line];
        }
		switch (creature->facing)
		{
			case (A_RIGHT):
				x++;
				break;
			case (A_UP):
				y--;
				break;
			case (A_LEFT):
				x--;
				break;
			case (A_DOWN):
				y--;
				break;
		}
        switch (inst)
        {
            case (HOP):
                executed_action = TRUE;
				if ((x >= 0 && x < board->width) && (y >= 0 && y < board->height))
				{
					if (!board->field[y][x])
					{
						board->field[y][x] = creature;
						board->field[old_y][old_x] = NULL;
						creature->x = x;
						creature->y = y;
					}
				}
                break;
            case (LEFT):
                creature->facing++;
                creature->facing %= 4;
                executed_action = TRUE;
                break;
            case (RIGHT):
                creature->facing--;
                creature->facing %= 4;
                executed_action = TRUE;
                break;
            case (GO):
                creature->current_line = prog->lines[arg - 1] - 1;
                break;
			case (IFRANDOM):
				if (rand() % 2)
				{
					creature->current_line = prog->lines[arg - 1] - 1;
				}
				break;
			case (IFEMPTY):
				if ((x >= 0 && x < board->width) && (y >= 0 && y < board->height))
				{
					if (!board->field[y][x])
					{
						creature->current_line = prog->lines[arg - 1] - 1;
					}
				}
				break;
			case (IFSAME):
				if ((x >= 0 && x < board->width) && (y >= 0 && y < board->height))
				{
					if (board->field[y][x] && board->field[y][x]->program == creature->program)
					{
						creature->current_line = prog->lines[arg - 1] - 1;
					}
				}
				break;
			case (IFENEMY):
				if ((x >= 0 && x < board->width) && (y >= 0 && y < board->height))
				{
					if (board->field[y][x] && board->field[y][x]->program != creature->program)
					{
						creature->current_line = prog->lines[arg - 1] - 1;
					}
				}
				break;
			case (IFWALL):
				if ((x < 0 || x >= board->width) || (y < 0 || y >= board->height))
				{
					creature->current_line = prog->lines[arg - 1] - 1;
				}
				break;
			case (INFECT):
				executed_action = TRUE;
				if ((x >= 0 && x < board->width) && (y >= 0 && y < board->height))
				{
					if (board->field[y][x] && board->field[y][x]->program != prog)
					{
						board->field[y][x]->program = prog;
						board->field[y][x]->current_line = arg;
					}
				}
            case (NOP):
            default:
				executed_action = TRUE;
				break;
        }
        x = old_x;
        y = old_y;
        creature->current_line++; // increment what instruction we're on
	}
}

CreatureProgram* ProgramFromFile(const char* filename)
{
	int currentChar;
	FILE* f = fopen(filename, "rb");
	if (f == NULL)
	{
		printf("Couldn't open file %s\n", filename);
		return NULL;
	}
	
    //wow a malloc
	CreatureProgram* p = malloc(sizeof(CreatureProgram));
	p->size = 0;
    //while the current char isn't an end of file
	while((currentChar = fgetc(f)) != EOF)
	{
		p->instructions[p->size++] = currentChar;
	}
	fclose(f);
	return p;
}

void CalculateLines(CreatureProgram* prog)
{
    prog->num_lines = 0;
    for (int i = 0; i < prog->size; i++)
    {
        prog->lines[prog->num_lines] = i;
        prog->num_lines++;
        if (prog->instructions[i] >= INFECT) // skip arg
		{
			i++;
		}
    }
}

void ProgramToFile(const char* filename, CreatureProgram* prog)
{
    FILE* f = fopen(filename, "wb");
    if (f == NULL)
	{
		printf("Couldn't open file %s", filename);
		return;
	}

    for(int i=0; i < prog->size; i++)
    {
        fputc(prog->instructions[i], f);
    }

    fclose(f);
}

void usage(char* name) {
	printf("usage: %s c1 [c2 ...] [-w width] [-h height] [-n num] [{-c | -cycles} cycles] [-s num]\n", name);
	printf("\tc1, c2, etc.: *.cdwn files for compiled creatures to simulate\n\t\t(maximum amount of species is 100)\n");
	printf("\t-w width, -h height: width and height of the playing field respectively.\n\t\t(default is 10 for both)\n");
	printf("\t-n num: number of each species\n\t\t (defualt is floor((w * h) / 4 * (nspecies)) )\n");
	printf("\t-c cycles: max amount of times the code for each animal will be executed\n\t\t (defualt is 1000)\n");
	printf("\t-s num: the number of times the screen will be drawn\n\t\t(default is 10)\n");
	exit(-1);
}
