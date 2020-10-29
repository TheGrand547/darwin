#define TRUE 1
#define FALSE 0

#define INSTRUCTION_COUNT 11 //how many instructions we have

char strInstruction[][10] = 
{
    "NOP", "HOP", "LEFT", "RIGHT", "INFECT", "IFEMPTY", "IFWALL", "IFSAME", "IFENEMY", "IFRANDOM", "GO"
};

enum Instruction {
    NOP, HOP, LEFT, RIGHT, INFECT, IFEMPTY, IFWALL, IFSAME, IFENEMY, IFRANDOM, GO
};

enum Facing {
	A_RIGHT = 0, A_UP = 1, A_LEFT = 2, A_DOWN = 3
};
