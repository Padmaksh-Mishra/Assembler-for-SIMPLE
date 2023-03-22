/*****************************************************************************
TITLE: Emulator                                                                                                                               
AUTHOR:   2101CS56
Declaration of Authorship
This txt file, emulator.cpp, is part of the miniproject of CS209/CS210 at the 
department of Computer Science and Engg, IIT Patna . 
*****************************************************************************/
#include<bits/stdc++.h>
using namespace std;

#define OPCODE_BITS 8
#define OPERAND_BITS 24
#define MEMORY_SIZE (1<<24)     //maxium possible memory size

int SP, PC, A, B;

int memory[MEMORY_SIZE]; 


void print_instruction_set() {
    // Define the instruction set as a 2D array
    string instructions[][3] = {
        {"ldc", "0", "value"},
        {"adc", "1", "value"},
        {"ldl", "2", "offset"},
        {"stl", "3", "offset"},
        {"ldnl", "4", "offset"},
        {"stnl", "5", "offset"},
        {"add", "6", ""},
        {"sub", "7", ""},
        {"shl", "8", ""},
        {"shr", "9", ""},
        {"adj", "10", "value"},
        {"a2sp", "11", ""},
        {"sp2a", "12", ""},
        {"call", "13", "offset"},
        {"return", "14", ""},
        {"brz", "15", "offset"},
        {"brlz", "16", "offset"},
        {"br", "17", "offset"},
        {"HALT", "18", ""}
    };

    // Define the widths of each column in the table
    int col_widths[] = {8, 6, 7};

    // Print the top border
    cout << setfill('-') << setw(31) << "-" << endl;

    // Print the header row
    cout << setfill(' ') << "| " << setw(col_widths[0]) << left << "Mnemonic"
         << " | " << setw(col_widths[1]) << left << "Opcode"
         << " | " << setw(col_widths[2]) << left << "Operand"
         << " |" << endl;

    // Print the middle border
    cout << setfill('-') << setw(31) << "-" << endl;

    // Print each instruction row
    for (auto& instruction : instructions) {
        cout << setfill(' ') << "| " << setw(col_widths[0]) << left << instruction[0]
             << " | " << setw(col_widths[1]) << left << instruction[1]
             << " | " << setw(col_widths[2]) << left << instruction[2]
             << " |" << endl;
    }

    // Print the bottom border
    cout << setfill('-') << setw(31) << "-" << endl;
}

void dump(int line_num, ofstream& file,bool before) {
    if(before){
        file << "Memory Dump before exectution" << endl;
    }else file << "Memory Dump after execution" << endl;
    for (int i = 0; i < line_num; i++) {
        if (i % 4 == 0) file << std::endl << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << i << "\t";
        file << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << memory[i] << "\t";
    }
    file << endl;
    return;
}

int getOpcode(int instruction) {
    return ((1 << OPCODE_BITS) - 1) & instruction;
}

int getOperand(int instruction) {
    return (instruction >> OPCODE_BITS);
}

void printOptions() {
    cout << "usage: emu [options] [binary file to emulate] " << endl;
    cout << "-trace\t-> to show trace" << endl;
    cout << "-before\t-> to show memory dump before execution" << endl;
    cout << "-after\t-> to show memory dump after execution" << endl;
    cout << "-disc\t-> to display the details of the instruction-set" << endl;
    cout << "You can also give multiple options eg. -trace -before" << endl;
}

int executeInstruction(int opcode, int operand) {
    switch (opcode) {
        case 0: // ldc
            B = A;
            A = operand;
            break;
        case 1: // adc
            A += operand;
            break;
        case 2: // ldl
            B = A;
            A = memory[SP+operand];
            break;
        case 3: // stl
            memory[SP+operand] = A;
            A = B;
            break;
        case 4: // ldnl
            A = memory[A + operand];
            break;
        case 5: // stnl
            memory[A + operand] = B;
            break;
        case 6: // add
            A = B + A;
            break;
        case 7: // sub
            A = B - A;
            break;
        case 8: // shl
            A = (B << A);
            break;
        case 9: // shr
            A = (B >> A);
            break;
        case 10: // adj
            SP = SP + operand;
            break;
        case 11: // a2sp
            SP = A;
            A = B;
            break;
        case 12: // sp2a
            B = A;
            A = SP;
            break;
        case 13: // call
            B = A;
            A = PC;
            PC = PC + operand;
            break;
        case 14: // return
            PC = A;
            A = B;
            break;
        case 15: // brz
            if(A==0){
                PC = PC + operand;
            }
            break;
        case 16: // brlz
            if(A<0){
                PC = PC + operand;
            }
            break;
        case 17: // br
            PC = PC + operand;
            break;
        case 18: // HALT
            return 0;
    }
    return 1;
}

int execute(bool trace_on) {
    SP = (1 << 23) -1;
    int operand = 0, opcode = 0;
    int instruction_num = 0;
    while ((opcode = getOpcode(memory[PC])) <= 18) {
        
        operand = getOperand(memory[PC]);

        if(trace_on){
            fprintf(stderr, "PC=%08X, SP=%08X, A=%08X, B=%08X\n", PC, SP, A, B);
        }
        if (opcode == 18) break;
        if (opcode > 18) return instruction_num;
        PC++;
        executeInstruction(opcode, operand);
        instruction_num += 1;
    }
    return instruction_num;
}


int main(int argc, char* argv[]) {
    if (argc < 3) {
        printOptions();
        return 1;
    }
    string binary_file = argv[argc-1];
    ifstream bin(binary_file,ios::binary);
    ofstream Dump("dump.txt");

    bool trace,before,disc,after;
    trace=before=disc=after=false;
    for(int i=1;i<argc-1;++i){
        trace |= !strcmp(argv[i], "-trace");        //show the trace
        before |= !strcmp(argv[i],"-before");       //give the memory dump before the execution of the program
        disc |= !strcmp(argv[i], "-disc");          //show the instrution set 
        after |= !(strcmp(argv[i], "-after"));      //give the memory dump after the execution of the program
    }

    if(disc) print_instruction_set();
    string line;
    int line_number = 0;
    while (getline(bin,line)) {
        line_number++; 
    }
    bin.clear();    //clear the error state flags of a file stream
    bin.seekg(0);   //move the file read pointer to file.begin
    if(before) dump(line_number,Dump,true);
    line_number = 0;
    while (getline(bin,line)) {
        bitset<32> bitset(line.substr(0,32));
        int value = bitset.to_ulong();
        memory[line_number] = value;
        line_number++; 
    }
    bin.close();
    int totalExecuted = execute(trace);
    cout << std::dec << "Total "<< totalExecuted << " instructions have been exectued" << endl; //swithed the cout back to decimals
    if(after) dump(line_number,Dump,false);
    Dump.close();
    return 0;
}