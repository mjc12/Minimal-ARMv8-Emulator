#include <stdio.h>   // for fgets, fopen
#include <stdlib.h>  // for atio, stio
#include <string.h>  // for strtok and strcpy
#include <stdbool.h> // for boolean flags

#define DELIM_LINE_END "\t\r\n\v\f" // delimiteers to parse lines of instructions
#define DELIM " #X,\t\r\n\v\f[]"    // used to get int values out of register and constant notation
#define MEM_SIZE 2000               // num of memory addresses
#define MEM_LENGTH 30               // max length of 1 mem address
#define SP 28                       // stack-pointer
#define PC 29                       // program counter
#define LR 30                       // link register
#define XZR 31                      // zero register
#define STACK_SIZE 100              // given minimum size for the stack
#define STACK_START 4000            // starting stack pointer

int main(int argc, char **argv)
{
    // make sure file containing instrucitons is given
    if (argc != 2)
    {
        printf("Usage: ./ARM.c instructions.txt\n");
        return 0;
    }

    char mem[MEM_SIZE][MEM_LENGTH];   // first index address second contents add address
    int reg[32];                      // array of registers
    reg[XZR] = 0;                     // set XZR
    reg[PC] = 100;                    // set PC
    reg[SP] = STACK_START;            // set SP, SP/4 = index for mem array

    // fill all memory locations with empty string
    for(int i = 0; i < MEM_SIZE; i++)
    {
        for(int j = 0; j < MEM_LENGTH; j++)
        {
            mem[i][j] = '\0';
        }
    }

    FILE *f = fopen(argv[1], "r");  // read file given as argument to parse

    char line[MEM_LENGTH];           // temporarily holds current line

    // loop to fill memory array with file contents
    while (fgets(line, sizeof line, f))
    {
        // copy of line so strtok doesn't modify original string yet
        char lineCopy[30];
        strcpy(lineCopy, line);

        // first token of each line is the address where it should be stored in memory
        reg[PC] = atoi(strtok(lineCopy,DELIM));

        // set mem location refrenced by pc above to full line
        // memory addresses are accessed by pc/4 to reduced unused array elements
        strcpy(mem[reg[PC]/4], strtok(line, DELIM_LINE_END));
    }

    // print memory array, both data and instructions
    printf("\nDATA MEMORY:\n");
    int i = 25;
    while (strcmp(mem[i], "") != 0) 
    {
        printf("%s\n", mem[i]);
        i++;
    }

    printf("\nPROGRAM MEMORY:\n");
    i = 50;
    while (strcmp(mem[i], "") != 0) 
    {
        printf("%s\n", mem[i]);
        i++;
    }

    printf("\n");

    // instructions start at 200
    reg[PC] = 200;

    // MAIN PROCESSING LOOP:
    // loop to iterate over memory performing instructions
    // runs until the line is empty of instructions
    while (strcmp(mem[reg[PC]/4], "") != 0) // make sure line of instruction isn't blank
    {
        // make 2 copies of current line of instructions
        // 1 to process and run, and the other to print out after to display what has run
        char instruction[30];
        char instructionCopy[30];
        strcpy(instruction, mem[reg[PC]/4]);
        strcpy(instructionCopy, instruction);

        // skip past token containing pc information
        strtok(instruction, DELIM);

        // get instruction
        char inst[5];
        strcpy(inst,strtok(NULL, DELIM));

        // get location of destination
        char dest[4];
        strcpy(dest, strtok(NULL, DELIM));
        int destI = 0;

        // boolean to keep track of if the stack pointer is being modified in order to check
        // if the stack pointer was iterated outside the stack -- stack overflow and exit
        bool isSPmodified = false;

        // boolean to keep track if instruction is a branch and therefore the program counter
        // should not be iterated and instead should just branch to given value
        bool isBranch = false;

        // handle SP and LR destinations
        if (strcmp(dest, "SP") == 0)
        {
            destI = SP;
            isSPmodified = true;
        }

        else if (strcmp(dest, "LR") == 0)
        {
            destI = LR;
        }

        // only SP and LR registers are referred by alias in code, everything else
        // can be handled normally by accessing the register index
        else
        {
            destI = atoi(dest);
        }

        // process instruction
        // ARM ADDI
        if (strcmp(inst, "ADDI") == 0)
        {
            // get first operand
            char oper[4];
            strcpy(oper,strtok(NULL, DELIM));
            int operI = 0;

            // set operand to value in register
            // check if operand is SP
            if (strcmp(oper, "SP") == 0)
            {
                // alias for 13 which is ARM stack pointer
                operI = reg[SP];
            } 

            else if (strcmp(oper, "LR") == 0)
            {
               operI = reg[LR];
            }
            
            else
            {
                operI = reg[atoi(oper)];
            }

            // get constant
            char con[4];
            strcpy(con, strtok(NULL, DELIM));
            int conI = atoi(con);

            // put solution in destination register
            reg[destI] = operI + conI;

        } 
        
        // ARM ADD
        else if (strcmp(inst, "ADD\0") == 0)
        {
            // get first operand
            char oper[4];
            strcpy(oper,strtok(NULL, DELIM));
            int operI = 0;

            // set operand to value in register
            // check if operand is SP
            if (strcmp(oper, "SP") == 0)
            {
                operI = reg[SP]; // alias for ARM stack pointer
            } 

            else if (strcmp(oper, "LR") == 0)
            {
               operI = reg[LR];
            }
            
            // get integer value for register as long as its not SP
            else
            {
                operI = reg[atoi(oper)];
            }

            // get second operand
            char oper2[4];
            strcpy(oper2,strtok(NULL, DELIM));
            int oper2I = 0;

            // set operand to value in register
            // check if operand is SP
            if (strcmp(oper2, "SP") == 0)
            {
                oper2I = reg[SP]; // alias for ARM stack pointer
            } 

            else if (strcmp(oper2, "LR") == 0)
            {
               oper2I = reg[LR];
            }
            
            // get integer value for register as long as its not SP
            else
            {
                oper2I = reg[atoi(oper)];
            }

            // put solution in destination register
            reg[destI] = operI + oper2I;

        } 

        // ARM SUBI
        else if (strcmp(inst, "SUBI") == 0)
        {
            // get first operand
            char oper[4];
            strcpy(oper,strtok(NULL, DELIM));
            int operI = 0;

            // set operand to value in register
            // check if operand is SP
            if (strcmp(oper, "SP") == 0)
            {
                operI = reg[SP]; // alias for ARM stack pointer
            } 

            else if (strcmp(oper, "LR") == 0)
            {
               operI = reg[LR];
            }
            
            // get integer value for register as long as its not SP
            else
            {
                operI = reg[atoi(oper)];
            }

            // get constant
            char con[4];
            strcpy(con, strtok(NULL, DELIM));
            int conI = atoi(con);

            // put solution in destination register
            reg[destI] = operI - conI;

        } 
        
        // branch to link register
        else if (inst[0] == 'B' && inst[1] == 'L')
        {
            // branch PC to value in LR + destI value (destI represents constant)
            reg[PC] = reg[LR] + destI;
                
            // if branching to LR the stack needs to be cleared
            for (int j = reg[SP]; j <= STACK_START; j = j + 8)
            {
                strcpy(mem[j/8], "");
            }

            // reset stack pointer to the top
            reg[SP] = STACK_START;

            isBranch = true;
        } 
        
        // branch to register
        else if (inst[0] == 'B' && inst[1] == 'R')
        {
            // make sure register being branched to isn't XZR
            if (dest[0] == 'Z' && dest[1] == 'R')
            {
                reg[PC] = 0;
            } 
            
            // only use given register if register is not XZR
            else
            {
                reg[PC] = reg[destI];
            }
            isBranch = true;
        } 

        // ARM branch
        else if (inst[0] == 'B')
        {
            // branch amount same as destI above
            reg[PC] = reg[PC] + destI;
            isBranch = true;
        } 
        
        // conditional branch if zero
        else if (strcmp(inst, "CBZ\0") == 0)
        {
            // get distance to branch from instruction
            char branch[5];
            strcpy(branch, strtok(NULL, DELIM));
            int branchI = atoi(branch);

            // destI from above already holds value that needs to be compared
            // if reg[destI] is zero, add branchI to pc
            if (reg[destI] == 0) reg[PC]+= branchI;

            isBranch = true;
        } 
        
        // load mem data into reg
        else if (strcmp(inst, "LDUR") == 0)
        {
            // get memory location
            char oper[4];
            strcpy(oper,strtok(NULL, DELIM));
            int operI = 0;

            // set operand to value in register
            // check if operand is SP
            if (strcmp(oper, "SP") == 0)
            {
                // alias for ARM stack pointer, contents of SP point to mem
                // location to load data from
                operI = reg[SP]/8;
            } 
            
            // get integer value for register as long as its not SP
            else
            {
                operI = reg[atoi(oper)]/4;
            }

            // get constant mem offset
            char con[4];
            strcpy(con, strtok(NULL, DELIM));
            int conI = 0;

            // divide by doubleword if accessing SP
            if (operI == reg[SP]/8)
            {
                conI = atoi(con)/8;
            }

            // else acessing normal memory only need to divide by word length
            else
            {
                conI = atoi(con)/4;
            }

            // get full contents of memory at this location
            char memcopy[MEM_LENGTH];
            strcpy(memcopy, mem[operI + conI]);

            // skip over token containing mem address
            strtok(memcopy, DELIM);
            
            // load memory data into register
            reg[destI] = atoi(strtok(NULL, DELIM));

        } 
        
        // store reg data into mem
        else if (strcmp(inst, "STUR") == 0)
        {
            // get memory destination
            char oper[4];
            strcpy(oper,strtok(NULL, DELIM));
            int operI = 0;

            // set operand to value in register
            // check if operand is SP
            if (strcmp(oper, "SP") == 0)
            {
                // alias for ARM stack pointer, contents of SP point to mem
                // location to load data from
                operI = reg[SP]/8;
            } 
            
            // get integer value for register as long as its not SP
            else
            {
                operI = reg[atoi(oper)]/4;
            }

            // get constant mem offset
            char con[4];
            strcpy(con, strtok(NULL, DELIM));
            int conI = atoi(con);

            // create string to be stored in memory
            char data[MEM_LENGTH];
            if (strcmp(oper, "SP") != 0) sprintf(data, "%d %d 0 0", operI*4, reg[destI]);
            else sprintf(data, "%d %d 0 0", operI*8, reg[destI]);

            // copy formatted string to location in mem
            strcpy(mem[operI], data);
        }

        printf("%s\n\nRESULT:\n", instructionCopy);
        for(int i = 0; i < 28; i++)
        {
            if (reg[i] != 0) printf("X%d = %d\n", i, reg[i]);
        }
        printf("\n");

        // print prog counter, stack pointer, and link register
        printf("PC = %d\n", reg[PC]);
        printf("SP = %d\n", reg[SP]);
        printf("LR = %d\n", reg[LR]);

        // print data stored in modifiable memory
        printf("\nDATA MEMORY:\n");
        i = 25;
        while (strcmp(mem[i], "") != 0) 
        {
            printf("%s\n", mem[i]);
            i++;
        }

        // print contents of stack, if any
        printf("\nSTACK:\n");
        i = STACK_START / 8;
        if (reg[SP] == STACK_START && mem[STACK_START/8][0] == '\0')
        {
            printf("EMPTY\n");
        }

        else
        {
            while (strcmp(mem[i], "") != 0) 
            {
                printf("%s\n", mem[i]);
                i = i - 1;
            }
        }
        printf("\n");

        // check if the stack pointer has been modified, and if so, check if
        // the SP has been iterated out of the stack, causing a STACK OVERFLOW
        if (isSPmodified)
        {
            // check if SP is outside specified size
            if ( (reg[SP] > STACK_START) || (reg[SP] < (STACK_START - STACK_SIZE)) )
            {
                printf("%d %d\n", STACK_START, reg[SP]);
                printf("Stack overflow detected, terminating...\n");
                return 0;
            }
        } 

        // if PC is ever zero (like BR XZR) halt execution
        if (reg[PC] == 0)
        {
            printf("PC set to 0, terminating...\n");
            break;
        }
        
        // only increment PC if PC is not zero and instruction was not a branch
        else if (!isBranch)
        {
            reg[PC] = reg[PC] + 4;
        }

        // wait for user to hit zero to move to next instruction
        printf("Press enter for next instruction...\n");
        while (getchar() != '\n')
        {
            ;
        }
    }

    printf("End of %s reached\n", argv[1]);

    // print memory array, both data and instructions
    printf("\nDATA MEMORY:\n");
    i = 25;
    while (strcmp(mem[i], "") != 0) 
    {
        printf("%s\n", mem[i]);
        i++;
    }

    printf("\nPROGRAM MEMORY:\n");
    i = 50;
    while (strcmp(mem[i], "") != 0) 
    {
        printf("%s\n", mem[i]);
        i++;
    }

    printf("\nSTACK:\n");
    i = STACK_START / 8;
    if (reg[SP] == STACK_START && mem[STACK_START/8][0] == '\0')
    {
        printf("EMPTY\n");
    }

    else
    {
        while (strcmp(mem[i], "") != 0) 
        {
            printf("%s\n", mem[i]);
            i = i - 1;
        }
    }
    printf("\n");
}