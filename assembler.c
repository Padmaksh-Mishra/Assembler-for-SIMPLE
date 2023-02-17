#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // for isspace

#define HASH_TABLE_SIZE 256
#define BUF_MAX 1024
#define MAX_TOKENS 20
#define MAX_LABELS 100
#define INSTRUCTION_COUNT 20
#define INVALID_VALUE -999999

int program_counter;
int program_counter;

struct Label {
    char *name;
    int address;
};
struct Data {
    char *name;
    int value;
};

struct Label labelMap[MAX_LABELS];
struct Data valueMap[MAX_LABELS];

int labelCount = 0;
int valueCount = 0;

void addLabel(const char *name, int address){
    labelMap[labelCount].name = strdup(name);
    labelMap[labelCount].address = address;
    labelCount++;
}
void addValue(const char *name, int value){
    valueMap[valueCount].name = strdup(name);
    valueMap[valueCount].value = value;
    valueCount++;
}
int findLabel(const char *name){
    for (int i = 0; i < labelCount; i++)
    {
        if (strcmp(labelMap[i].name, name) == 0)
            return labelMap[i].address;
    }

    return INVALID_VALUE;
}
int findValue(const char *name){
    for (int i = 0; i < valueCount; i++)
    {
        if (strcmp(valueMap[i].name, name) == 0)
            return valueMap[i].value;
    }

    return INVALID_VALUE;
}
// void updateLabelAddress(const char* label, char* address) {
//     for (int i = 0; i < MAX_LABELS; i++) {
//         if (strcmp(labelMap, labelMap[i].name) == 0) {
//             labelMap[i].address = address;
//             return;
//         }
//     }
// }

struct Instruction {
	const char *name;
	char *op_code;
};

struct Instruction instructionMap[] = {
	//add DATA AND SET
					{"data",	"11111111"},
					{"ldc",  	"00000000"},
					{"adc",  	"00000001"},
					{"ldl",  	"00000010"},
					{"stl",  	"00000011"},
					{"ldnl", 	"00000100"},
					{"stnl", 	"00000101"},
					{"add",  	"00000110"},
					{"sub",  	"00000111"},
					{"shl",  	"00001000"},
					{"shr",  	"00001001"},
					{"adj",  	"00001010"},
					{"a2sp",  	"00001011"},
					{"sp2a",  	"00001100"},
					{"call",  	"00001101"},
					{"return",  "00001110"},
					{"brz",  	"00001111"},
					{"brlz",  	"00010000"},
					{"br",  	"00010001"},
					{"HALT",  	"00010010"}
					//{"set program counter to ",		"11111111"},	//takes the address to jump to 
					// {"SET",		""}			//extra mks
				};

char **split(char *str) {
    int i = 0;
    char *p = strtok(str, " :");
    static char *tokens[MAX_TOKENS];
    while (p != NULL) {
        tokens[i++] = p;
        p = strtok(NULL, " :");
    }
    tokens[i] = NULL;
    return tokens;
}

char* int_to_binary_str(int num) {
    static char binary_str[25];  
    binary_str[24] = '\0';  // null terminator
    
    for (int i = 23; i >= 0; --i) {
        binary_str[i] = (num & 1) + '0';
        num >>= 1;
    }
    
    return binary_str;
}
int string_to_int(const char *str) {
    char *endptr;
    long value;
    int base = 10;
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        base = 16;
        str += 2;
    } 
    value = strtol(str, &endptr, base);
    if (*endptr != '\0') {
        // Invalid character found in string, return 0
        return 0;
    }
    return (int)value;
}
int main() {

	// Open I/O files
	// Check that files opened properly
	FILE *In;
	In = fopen("test01.asm", "r");
	if (In == NULL) {
		printf("Input file could not be opened.");
		exit(1);
	}

	FILE *Out;
	Out = fopen("out.bin", "w");
	if (Out == NULL) {
		printf("Output file could not be opened.");
		exit(1);
	}

	FILE *Listing;
	Listing = fopen("Listing.txt", "w");
	if (Listing == NULL) {
		printf("Listing file could not be opened.");
		exit(1);
	}	

	FILE *Err;
	Err = fopen("test01.log", "w");
	if (Listing == NULL) {
		printf("Error file could not be opened.");
		exit(1);
	}	

// First pass: parse labels and build symbol table + reserve values for DATA
	printf("***********************FIRST PASS***********************\n");
	program_counter = 0;
    char line[BUF_MAX];
    while (fgets(line, sizeof(line), In)) {

    	char **tokens = split(line);
        int token_count = 0;
        while (tokens[token_count]){
            token_count++;
        }

	  	char* token = tokens[0];		//say it is a label else move
	  	if(!token) continue;
	  	printf("%s\n",token);
	  	if(token[0]==';') continue;
	  	char* op_code = NULL;
	  	for (i = 0; i < INSTRUCTION_COUNT; i++) {
	        if (strcmp(instructionMap[i].name,instruction)==0) {
	            op_code = instructionMap[i].op_code;
	        }
	    }
	    if(op_code) continue;
	    
	  	if (line[strlen(token) - 1] == ':') {

	  		token[strlen(token) - 1] = '\0';
	  		
	  		if(token_count>2&&strcmp(tokens[1],"data")==0){		
	  			//Labels for data and set statements

				int value = string_to_int(tokens[2]);
				addValue(token,value);
				printf("%s is now %d\n",token,value);
			}else{
	  			//Labels for branch and call statements 
				addLabel(token, program_counter);
				printf("%d\n",program_counter);
			}
			if(token_count>1) program_counter++;
	  	}else if(token[0]==';') continue;
	  	else{
	  		program_counter++;
	  	}

	  	for (int i = 0; i < token_count; i++)
            free(tokens[i]);
	}

// Second pass: generate machine code and replace labels with addresses
	printf("***********************SECOND PASS***********************\n");
	rewind(In);
	program_counter = 0;
	// while (fgets(line, sizeof(line), In)) {

	// 	char **tokens = split_string(line);
    //     int token_count = 0;
    //     while (tokens[token_count])
    //         token_count++;
    //     int instruction_part = 0;
	//   	char* instruction = tokens[instruction_part];


	//   	//changing program_counter to 8 ditits
	//   	char program_counter_eightdgts[9];
  	// 	sprintf(program_counter_eightdgts, "%08d", program_counter);


	//   	if (instruction && instruction[strlen(instruction) - 1] == ':') {
	//   		if(token_count>2&&strcmp(tokens[1],"data")==0){		
	//   			// Labels for data and set statements
	// 			continue;
	// 		}else{
	//   			//Labels for branch and call statements 
	//   			fprintf(Listing,"%s\t%s\t%s\n",program_counter_eightdgts,"\t\t",instruction);
	//   			program_counter++;
	// 		}
	//   		instruction_part++;
	//     	instruction = tokens[instruction_part];
	//   	}
	//   	if(instruction==NULL) continue;
	//   	if(instruction[0]==';') continue;
	//   	char* op_code = NULL;
	//   	int i;
	    // for (i = 0; i < INSTRUCTION_COUNT; i++) {
	    //     if (strcmp(instructionMap[i].name,instruction)==0) {
	    //         op_code = instructionMap[i].op_code;
	    //     }
	    // }

	//   	if(op_code){
	  		
	//   		if(instruction[0]=='b'||strcmp(instruction,"call")==0){
	//   			char *label_pass2 = tokens[1];
	//   			printf("%s\n",label_pass2);
	// 	      	int address = findLabel(label_pass2);
	// 	      	if (address!=INVALID_VALUE) {
	// 	      		int offset = (address-program_counter);
	// 	        	char hex[32];
	// 	        	char* bin = int_to_binary_str(offset);
	// 	        	strcat(bin,op_code);
    // 				sprintf(hex, "%08X", strtol(bin,NULL,8));
	// 	        	fprintf(Out,"%s\n",bin);
	// 	        	fprintf(Listing,"%s\t%s\t%s %s\n",program_counter_eightdgts,hex,instruction,label_pass2);
	// 	      	}else {
	// 	        	fprintf(Err,"Error: Unknown label %s\n", label_pass2);
	// 	        	return 1;
	// 	      	}
	//   		}
	//   		else{
	//   			char *operand = "";
	//   			++instruction_part;
	// 	  		if(token_count>instruction_part) operand = tokens[instruction_part];
	// 	  		int oprand_asint = findValue(operand);
	// 	  		if(oprand_asint==INVALID_VALUE) oprand_asint = string_to_int(operand);
	// 	  		printf("%d\n",oprand_asint);
	// 	  		char hex[32];
	//         	char* bin = int_to_binary_str(oprand_asint);
	//         	strcat(bin,op_code);
	// 			sprintf(hex, "%08X", strtol(bin,NULL,8));
	//         	fprintf(Out,"%s\n",bin);
	//     		fprintf(Listing,"%s\t%s\t%s %s\n", program_counter_eightdgts,hex,instruction,operand);
	//   		}
	//   		program_counter++;
	//   	}else{
	//   		fprintf(Err,"Error: unknown instruction '%s'\n", instruction);
	//         return 1;
	//   	}
	//   	for (int i = 0; i < token_count; i++)
    //         free(tokens[i]);
    // }

  return 0;
}
