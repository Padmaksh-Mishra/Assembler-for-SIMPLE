#include <stdio.h>
#include <string.h>

struct Instruction {
    const char *name;
    char *address;
};

const int INSTRUCTION_COUNT = 20;
struct Instruction instructionMap[] = {
                    {"ldc",     "00000000"},
                    {"adc",     "00000001"},
                    {"ldl",     "00000010"},
                    {"stl",     "00000011"},
                    {"ldnl",    "00000100"},
                    {"stnl",    "00000101"},
                    {"add",     "00000110"},
                    {"sub",     "00000111"},
                    {"shl",     "00001000"},
                    {"shr",     "00001001"},
                    {"adj",     "00001010"},
                    {"a2sp",    "00001011"},
                    {"sp2a",    "00001100"},
                    {"call",    "00001101"},
                    {"return", "00001110"},
                    {"brz",     "00001111"},
                    {"brlz",    "00010000"},
                    {"br",      "00010001"},
                    {"HALT",    "00010010"}
                };

char* searchInstruction(const char *instructionName) {
    for (int i = 0; i < INSTRUCTION_COUNT; i++) {
        if (strcmp(instructionMap[i].name, instructionName) == 0) {
            return instructionMap[i].address;
        }
    }
    return NULL;
}

int main() {
    char *instructionAddress = searchInstruction("ldc");
    if (instructionAddress != NULL) {
        printf("Instruction found: %s\n", instructionAddress);
    } else {
        printf("Instruction not found\n");
    }
    return 0;
}
