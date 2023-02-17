# include <bits/stdc++.h>
# define all(v) (v).begin(), (v).end()
# define OPCODE_BITS 8
# define OPERAND_BITS 24

using namespace std;

map<string,int> opcodes;
map<string,int> labels;
map<string,int> values;
vector<string> instructions = {"ldc","adc","ldl","stl","ldnl","stnl","add","sub","shl","shr","adj",
								"a2sp","sp2a","call","return","brz","brlz","br","HALT"};

vector<string> need_operand = {"ldc","adc","ldl","stl","ldnl","stnl","adj","call","brz","brlz","br"};

void initilize(){
	for(int i=0;i<19;++i){
		opcodes[instructions[i]] = i;
	}
}
vector<string> split(const string& str) {
    vector<string> result;
    istringstream iss(str);
    for (string s; iss >> s; ) {
        result.push_back(s);
    }
    return result;
}
int string_to_int(string str) {
    char *endptr;
    long value;
    int base = 10;
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        base = 16;
        str += 2;
    }
    value = strtol(str.c_str(), &endptr, base);
    if (*endptr != '\0') {
        // Invalid character found in string, return 0
        return 0;
    }
    return (int)value;
}

int counter;
bool error_free;
string line;

int main(){

	initilize();
	ifstream in("test01.asm");
  	ofstream bin("out.bin");
  	ofstream listing("listing.txt");
  	ofstream err("test01.log");

  	error_free = true;
  	counter = 0;

	cout << "First pass for storing labels and error detection" << endl;
	while(getline(in,line)){
		string label = "";
		int j = 0,k=size(line);
		for(int i=0;i<size(line);++i){
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
		if(j<size(line)) workon = split(line.substr(j,k-j));
		for(auto val : workon){
			cout << val << endl;
		}
		if(label!=""){
			cout << "Label = " << label << endl;
			if(label[0]>='0'&&label[0]<='9'){
				err << "Bogous label name" << endl;
				error_free = false;
			}
			bool data = (line.find("data")!=string::npos);
			if(labels.find(label)!=labels.end()||values.find(label)!=values.end()){
				err << "Label already defined" << endl;
				error_free = false;
			}else{
				if(data){
					if(size(workon)>1){
						values[label] = stoi(workon[1]);
					}else{
						err << "A value must be provided to store" << endl;
						error_free = false;
					}
					continue;
				}
				else labels[label] = counter;
			}
			if(size(workon)<2) continue;
		}
		if(size(workon)<1) continue;
		string operation = workon[0];
		if(find(all(instructions),operation)==instructions.end()){
			err << "No such instruction is available " << operation << endl;
			error_free = false;
		}
		if(size(workon)<2&&find(all(instructions),operation)==need_operand.end()){
			err << "Expected an operand" << endl;
			error_free = false;
		}else if(size(workon)>2){
			err << "Extra operand detected" << endl;
			error_free = false;
		}
		counter++;
	}

	if(!error_free){
		return 1;
	}

	in.clear();
	in.seekg(0);
	cout << "Second Pass" << endl;
	counter = 0;
	while(getline(in,line)){

		stringstream ss;
    	ss << std::setfill('0') << std::setw(8) << counter;
    	string counter_width8 = ss.str();

		string label = "";
		int j = 0,k=size(line);
		for(int i=0;i<size(line);++i){
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
		if(j<size(line)) workon = split(line.substr(j,k-j));
		for(auto val : workon){
			cout << val << endl;
		}
		if(label!=""){
			bool data = (line.find("data")!=string::npos);
			if(data){
				listing << counter_width8 << "\t" << std::hex << std::uppercase << setw(8) << setfill('0') << values[label] << "\t" << label << ": " << workon[1] << endl;
			}else listing << counter_width8 << "\t\t\t\t" << label << ":" << endl;
			continue;
		}
		if(size(workon)<1) continue;
		string operation = workon[0];
		cout << "operation = " << operation << endl;
		int opcode = opcodes[operation];
		string operand_str = "";
		if(size(workon)>1) operand_str = workon[1];
		int operand = 0;
		if(operation[0]=='b'||operation=="call"){
			if(labels.find(operand_str)==labels.end()){
				err << "Label not yet defined " << operand_str << endl;
				return 1;
			}
			int offset = labels[workon[1]]-counter;
			operand = offset;
		}else{
			if(values.find(operand_str)!=values.end()){
				operand_str = values[operand_str] + '0';
			}
			operand = string_to_int(operand_str);
		}
		unsigned int binary = ((operand<<OPCODE_BITS)|(opcode));
		bitset<32> binary_bin(binary);
		listing << counter_width8 << "\t" << std::hex << std::uppercase << setw(8) << setfill('0')  << binary << "\t" << workon[0] << " " << operand_str << endl;
		bin << binary_bin.to_string() << endl;
		counter++;
	}

	in.close();
	bin.close();
	listing.close();
	err.close();
    return 0;
}

