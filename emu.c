#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the number of bits in opcode and operand
#define OPCODE_BITS 8
#define OPERAND_BITS 24
#define MEMORY_SIZE 10000
#define BUF_MAX 33 //\0 will take 1 extra space

unsigned int memory[MEMORY_SIZE];
// Define A struct to store the instructions
struct Instruction {
    const char *name;
    unsigned int op_code;
};

struct Instruction instructionMap[] = {
  //add DATA AND SET
          {"data",  255},
          {"ldc",   0},
          {"adc",   1},
          {"ldl",   2},
          {"stl",   3},
          {"ldnl",  4},
          {"stnl",  5},
          {"add",   6},
          {"sub",   7},
          {"shl",   8},
          {"shr",   9},
          {"adj",   10},
          {"a2sp",  11},
          {"sp2a",  12},
          {"call",  13},
          {"return",14},
          {"brz",   15},
          {"brlz",  16},
          {"br",    17},
          {"HALT",  18}
          //{"set program counter to ",   "11111111"},  //takes the address to jump to 
          // {"SET",    ""}     //extra mks
        };

// Define A struct to store the register values
struct Registers {
    int A;
    int B;
    int SP;    //stack-pointer is for f***ing memory
    int PC;
}registers;

int main(){

    //initilizing memory to 0
    for(int i=0;i<MEMORY_SIZE;++i){
        memory[i] = 0;
    }

    //read the binary file

    FILE *Bin;
    Bin = fopen("out.bin", "r");
    if (Bin == NULL) {
        printf("Input file could not be opened.");
        exit(1);
    }

    char line[BUF_MAX];
    int offset = 0;
    while (fgets(line, BUF_MAX, Bin)) {

        int inst = strtol(line,NULL,2);
        unsigned int operand = (inst >> OPCODE_BITS);
        unsigned int opcode = (inst & ((1 << OPCODE_BITS) - 1));
        printf("%s\n",line);
        printf("%d\n",inst);
        printf("opcode = %d\t operand = %d\n",opcode,operand);
        switch (opcode) {
            case 0: // ldc
                registers.B = registers.A;
                registers.A = operand;
                break;
            case 1: // adc
                registers.A += operand;
                break;
            case 2: // ldl
                registers.B = registers.A;
                registers.A = memory[registers.SP+operand];
                break;
            case 3: // stl
                memory[registers.SP+operand] = registers.A;
                registers.A = registers.B;
                break;
            case 4: // ldnl
                registers.A = memory[registers.A + operand];
                break;
            case 5: // stnl
                memory[registers.A + operand] = registers.B;
                break;
            case 6: // add
                registers.A = registers.B + registers.A;
                break;
            case 7: // sub
                registers.A = registers.B - registers.A;
                break;
            case 8: // shl
                registers.A = (registers.B << registers.A);
                break;
            case 9: // shr
                registers.A = (registers.B >> registers.A);
                break;
            case 10: // adj
                registers.SP = registers.SP + operand;
                break;
            case 11: // a2sp
                registers.SP = registers.A;
                registers.A = registers.B;
                break;
            case 12: // sp2a
                registers.B = registers.A;
                registers.A = registers.SP;
                break;
            case 13: // call
                registers.B = registers.A;
                registers.A = registers.PC;
                offset = (operand<<3);
                registers.PC = registers.PC + offset;
                fseek(Bin,offset,SEEK_CUR);
                offset = 0;
                break;
            case 14: // return
                offset = registers.A - registers.PC;
                registers.PC = registers.A;
                registers.A = registers.B;
                fseek(Bin,offset,SEEK_CUR);
                offset = 0;
                break;
            case 15: // brz
                if(registers.A==0){
                    offset = (operand<<3);
                    registers.PC = registers.PC + offset;
                    fseek(Bin,offset,SEEK_CUR);
                    offset = 0;
                }
                break;
            case 16: // brlz
                if(registers.A<0){
                    offset = (operand<<3);
                    registers.PC = registers.PC + offset;
                    fseek(Bin,offset,SEEK_CUR);
                    offset = 0;
                }
                break;
            case 17: // br
                offset = (operand<<3);
                registers.PC = registers.PC + offset;
                fseek(Bin,offset,SEEK_CUR);
                offset = 0;
                break;
            case 18: // sp2a
                return 1;
                break;
            }
        }
    return 0;
}

