/*****************************************************************************
TITLE: Assembler																																
AUTHOR:   2101CS56
Declaration of Authorship
This cpp file, assembler.cpp, is part of the miniproject of CS209/CS210 at the 
department of Computer Science and Engg, IIT Patna . 
*****************************************************************************/

# include <bits/stdc++.h>
# define all(v) (v).begin(), (v).end()
# define OPCODE_BITS 8
# define OPERAND_BITS 24

using namespace std;

map<string,int> opcodes;	//maps opcodes to respective numberical opcodes
map<string,int> labels;		//stores branch labels
map<string,int> values;		//stores the labels declared using data

vector<pair<int,string>> errors,warnings;
string operand_type[][3] = {
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
vector<string> instructions = {"ldc","adc","ldl","stl","ldnl","stnl","add","sub","shl","shr","adj",
								"a2sp","sp2a","call","return","brz","brlz","br","HALT"};

vector<string> need_operand = {"ldc","adc","ldl","stl","ldnl","stnl","adj","call","brz","brlz","br"};

void initilize(){ 
	for(int i=0;i<19;++i){
		opcodes[instructions[i]] = i;
	}
}
vector<string> split(const string& str) {		//function to split the sting on basis of whitespaces
    vector<string> result;
    istringstream iss(str);
    for (string s; iss >> s; ) {
        result.push_back(s);
    }
    return result;
}
int string_to_int(string str) {			//convert (hex or int) to int
    char *endptr;
    long value;
    int base = 10;
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        base = 16;
        str += 2;
    }
    value = strtol(str.c_str(), &endptr, base);
    return (int)value;
}

bool is_number(string s) {		//checks if a string is a number (hex/int) or not
    if (s.empty()) {
        return false;
    }
    int i = 0;
    if (s[i] == '-' || s[i] == '+') {
        ++i;
    }
    bool has_digits = false;
    bool has_hex_prefix = false;
    for (; i < s.size(); ++i) {
        if (std::isdigit(s[i])) {
            has_digits = true;
        } else if (s[i] == 'x' && i == 1 && s[0] == '0') {
            has_hex_prefix = true;
        } else {
            return false;
        }
    }
    return has_digits || has_hex_prefix;
}

int counter,line_number;	//counter acts like PC and line_number is to specify the loaction of errors
string line;

int main(int argc, char* argv[]){

	initilize();

	// file managemnt 
	if(argc<2){
		cout << "Usage [file name to assemble]" << endl;
		return 1;
	}
	string filename = argv[1];
	size_t pos = filename.find_last_of(".");
    string name = filename.substr(0, pos);
	
	ifstream in(filename);

  	ofstream bin(name+".o",ios:: binary);
  	ofstream listing(name + ".l");

  	counter = 0;
  	line_number = 0;

	//"First pass for storing labels and error detection"

	while(getline(in,line)){
		line_number++;

		string label = "";
		int j = 0;
		int k = line.size();

		//whitespace management and splitting the string
		for(int i=0;i<k;++i){
			if(line[i]==';'){
				k=i;
				break;
			}   
		}
		for(int i=0;i<k;++i){
			if(line[i]==':'){
				label = line.substr(0,i);
				j = i+1;
				break;
			}
		}
		vector<string> workon = {};
		if(j<k) workon = split(line.substr(j,k-j));
		for(auto val : workon){
			//cout<< val << endl;
		}
		if(label!=""){
			for(int i=0;i<label.size();++i){
				if((label[0]>='0'&&label[0]<='9')||(label[0]<'a'&&label[0]>'z')){
					errors.emplace_back(make_pair(line_number,"Bogous label name"));
					break;
				}
			}
			bool data = (line.find("data")!=string::npos);
			if(labels.find(label)!=labels.end()||values.find(label)!=values.end()){
				errors.emplace_back(make_pair(line_number,"Duplicate label definition"));
			}else{
				if(data){
					if(workon.size()>1){
						values[label] = counter;
						counter++;
					}else{
						errors.emplace_back(make_pair(line_number,"Expected a value to store"));
					}
					continue;
				}
				else labels[label] = counter;
			}
			if(workon.size()<2) continue;
		}
		if(workon.size()<1) continue;
		string operation = workon[0];
		if(find(all(instructions),operation)==instructions.end()){
			errors.emplace_back(make_pair(line_number,"Bogus mnemonic"));
		}else if(find(all(need_operand),operation)!=need_operand.end()){
			if(workon.size()<2){
				errors.emplace_back(make_pair(line_number,"Expected an operand"));
			}else if(workon.size()>2){ 
				errors.emplace_back(make_pair(line_number,"Extra operand "));
			}
		}
		counter++;
	}

	in.clear();		//clear the error state flags of a file stream
	in.seekg(0);	//moves the file read pointer to the start of file
	counter = 0;
	line_number = 0;

	//Second Pass further error detection and machine code generation

	while(getline(in,line)){
		line_number++;

		//making the 8 bit counter string
		stringstream ss;
    	ss << std::setfill('0') << std::setw(8)  << std::hex << std::uppercase << counter;
    	string counter_width8 = ss.str();

		string label = "";
		int j = 0;
		int k = line.size();

		//whitespace management and splitting the string
		for(int i=0;i<line.size();++i){
			if(line[i]==';'){
				k=i;
				break;
			}
		}
		for(int i=0;i<k;++i){
			if(line[i]==':'){
				label = line.substr(0,i);
				j = i+1;
				break;
			}
		}
		vector<string> workon = {};
		if(j<line.size()) workon = split(line.substr(j,k-j));

		//check if line has a label (label/data)
		if(label!=""){
			bool data = (line.find("data")!=string::npos);
			if(data){
				bitset<32> tmp(stoi(workon[1]));
				bin << tmp.to_string() << endl;
				listing << counter_width8 << "\t" << std::hex << std::uppercase << setw(8) << setfill('0') << workon[1] << "\t" << label << ": " << workon[1] << endl;
				counter++;
				continue;
			}else listing << counter_width8 << "\t\t\t\t" << label << ":" << endl;
		}
		if(workon.size()<1) continue;
		string operation = workon[0];
		int opcode = opcodes[operation];
		string operand_str = "";
		if(workon.size()>1) operand_str = workon[1];
		int operand = 0;

		//handling branch and calls seperatly 
		if(operation[0]=='b'||operation=="call"){
			if(labels.find(operand_str)==labels.end()){
				errors.emplace_back(make_pair(line_number,"\"" + operand_str+"\" is not yet defined"));
				continue;
			}
			int offset = labels[workon[1]]-counter-1;
			operand = offset;
			if(offset==0){
				warnings.emplace_back(make_pair(line_number,"Infinite Loop detected"));
			}
		}else{
			if(operand_str!=""&&!is_number(operand_str)){
				if(values.find(operand_str)!=values.end()){
					operand = values[operand_str];
				}else if(labels.find(operand_str)!=labels.end()){
					operand = values[operand_str];
				}else{
					errors.emplace_back(make_pair(line_number,"Badly formatted number - " + operand_str));
					continue;
				}	
			}else{
				operand = string_to_int(operand_str);
			}
		}

		//Print machine code to the name.o file
		unsigned int binary = ((operand<<OPCODE_BITS)|(opcode));
		bitset<32> binary_bin(binary);
		listing << counter_width8 << "\t" << std::hex << std::uppercase << setw(8) << setfill('0')  << binary << "\t" << workon[0] << " " << operand_str << endl;
		bin << binary_bin << endl;
		counter++;
	}

	in.close();
	bin.close();
	listing.close();

	//sorting errors on the basis of their line of occurance
	sort(all(errors));
	sort(all(warnings));
	ofstream err(name+".log");
	for(auto val : warnings){
		err << "Warning @Line: " << val.first << "\t" << val.second << endl;
	}
	err << "Total errors: " << errors.size() << endl;
	for(auto val : errors){
		err << "Error @Line:" << val.first << "\t" << val.second << endl;
	}
	err.close();

	//if there are errors program exits with error code 1
	if(errors.size()){
		return 1;
	}else return 0;
}

