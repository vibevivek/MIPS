#include<bits/stdc++.h>
using namespace std;

bool flag = false;
int row;
int col;
int update = 0;
vector<string> rev_register_map = {"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3","$t4"
                                    ,"$t5","$t6","$t7","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9"
                                    ,"$k0","$k1","$gp","$sp","$fp","$ra"};
unordered_map<string,int> register_map;
unordered_map<string,int> label_map;
unordered_map<string,int> ins_map;
unordered_set<string> labels_used;
int INSTR_NUM = 0;
int INSTR_MEM = 0;

void initializeInsMap(){
    ins_map["add"] = 0;
    ins_map["sub"] = 1;
    ins_map["mul"] = 2;
    ins_map["beq"] = 3;
    ins_map["bne"] = 4;
    ins_map["slt"] = 5;
    ins_map["j"] = 6;
    ins_map["lw"] = 7;
    ins_map["sw"] = 8;
    ins_map["addi"] = 9;

}

void initializeRegisterMap(){
    register_map["$zero"]=0;
    register_map["$at"]=1;
    register_map["$v0"]=2;
    register_map["$v1"]=3;
    register_map["$a0"]=4;
    register_map["$a1"]=5;
    register_map["$a2"]=6;
    register_map["$a3"]=7;
    register_map["$t0"]=8;
    register_map["$t1"]=9;
    register_map["$t2"]=10;
    register_map["$t3"]=11;
    register_map["$t4"]=12;
    register_map["$t5"]=13;
    register_map["$t6"]=14;
    register_map["$t7"]=15;
    register_map["$s0"]=16;
    register_map["$s1"]=17;
    register_map["$s2"]=18;
    register_map["$s3"]=19;
    register_map["$s4"]=20;
    register_map["$s5"]=21;
    register_map["$s6"]=22;
    register_map["$s7"]=23;
    register_map["$t8"]=24;
    register_map["$t9"]=25;
    register_map["$k0"]=26;
    register_map["$k1"]=27;
    register_map["$gp"]=28;
    register_map["$sp"]=29;
    register_map["$fp"]=30;
    register_map["$ra"]=31;
}

string intToBits5(int v){
    return bitset<5>(v).to_string();
}
string intToBits4(int v){
    return bitset<4>(v).to_string();
}
string intToBits18(int v){
    return bitset<18>(v).to_string();
}
string intToBits28(int v){
    return bitset<28>(v).to_string();
}
string intToBits23(int v){
    return bitset<23>(v).to_string();
}
string intToBits32(int v){
    return bitset<32>(v).to_string();
}

vector<int> register_File(32,0);
//vector<int> memory(1048576,0);
//char memory[1048576] = {'a'};
bitset<8388608> memory;

void copyToMem(string s,int m){
    int i=0;
    for(char ch:s){
        if(ch=='0'){
            memory[m+i]=0;
        }
        else{
            memory[m+i]=1;
        }
        i++;
    }
}


struct Instruction {
    string command;
    vector<string> param;
    int lineNumber;
    int execution_count;
    string str;
    int mem_address;
};

vector<Instruction> program;
vector<string> temp_paramList;
string temp_command;


bool validReg(string s){
    if(s=="$at" || register_map.find(s)==register_map.end()){
        return false;
    }
    return true;
}

bool validLabel(string s){
    if(s==""){
        return false;
    }
    if(! (s[0]=='_' || (s[0]>='a'&&s[0]<='z') || (s[0]>='A'&&s[0]<='Z') )){
        return false;
    }
    for(char ch:s){
        if(! (ch=='_' || (ch>='a'&&ch<='z') || (ch>='A'&&ch<='Z') || (ch>='0'&&ch<='9') )){
            return false;
        }
    }

    return true;
}

bool isNum(string s){
    if(s==""){
        return false;
    }

    int n=s.size();
    if(n==1 && s=="-"){
        return false;
    }
    for(int i=0;i<n;i++){
        if(i==0){
            if(s[i]=='-'){
                continue;
            }
        }
        if(! (s[i]<='9'&&s[i]>='0') ){
            return false;
        }
    }


    return true;
}

bool AddSubMulSlt_check(vector<string>& tokens){
    int n=tokens.size();
    if(n!=4){
        return false;
    }

    if(!validReg(tokens[1])){
        return false;
    }
    if(!validReg(tokens[2])){
        return false;
    }
    if(!validReg(tokens[3])){
        return false;
    }
    temp_paramList = {tokens[1],tokens[2],tokens[3]};
    temp_command = tokens[0];
    return true;
}

bool Branch_check(vector<string>& tokens){
    int n = tokens.size();
    if(n!=4){
        return false;
    }

    if(!validReg(tokens[1])){
        return false;
    }
    if(!validReg(tokens[2])){
        return false;
    }

    if(!validLabel(tokens[3])){
        return false;
    }
    labels_used.insert(tokens[3]);
    temp_paramList = {tokens[1],tokens[2],tokens[3]};
    temp_command = tokens[0];

    return true;
}

bool J_check(vector<string>& tokens){
    int n=tokens.size();
    if(n!=2){
        return false;
    }

    if(!validLabel(tokens[1])){
        return false;
    }
    labels_used.insert(tokens[1]);
    temp_paramList = {tokens[1]};
    temp_command = tokens[0];

    return true;
}

bool LwSw_check(vector<string> tokens){
    int n=tokens.size();
    if(n!=3){
        return false;
    }

    if(!validReg(tokens[1])){
        return false;
    }

    // address should be numeric number.
    if(!isNum(tokens[2])){
        int sz=tokens[2].size();
        if(sz<5){
            return false;
        }
        string reg_part = tokens[2].substr(sz-5+1,5-2);
        //if(reg_part != "($sp)"){
        //    return false;
        //}
        if(!validReg(reg_part)){
            return false;
        }

        if(sz==5){
            temp_paramList = {tokens[1],reg_part,"0"};
            temp_command = tokens[0];
            return true;
        }

        string num_part = tokens[2].substr(0,sz-5);
        if(!isNum(num_part)){
            return false;
        }
        int number = stoi(num_part);
        if(number>131071 || number<-131072){
            return false;
        }

        temp_paramList = {tokens[1],reg_part,num_part};
        temp_command = tokens[0];
        return true;
    }

    temp_paramList = {tokens[1],"$zero",tokens[2]};
    temp_command = tokens[0];

    return true;
}

bool Addi_check(vector<string>& tokens){
    int n=tokens.size();
    if(n!=4){
        return false;
    }

    if(!validReg(tokens[1])){
        return false;
    }
    if(!validReg(tokens[2])){
        return false;
    }

    if(!isNum(tokens[3])){
        return false;
    }
    int sz=tokens[3].size();
    if(sz>8){
        return false;
    }
    int number = stoi(tokens[3]);
    if(number>131071 || number<-131072){
        return false;
    }

    temp_paramList = {tokens[1],tokens[2],tokens[3]};
    temp_command = tokens[0];

    return true;
}

bool Label_check(vector<string>& tokens){
    int n=tokens.size();
    if(n>2){
        return false;
    }
    string lab;
    if(n==2){
        if(! (tokens[1]==":") ){
            return false;
        }
        else{
            if(!validLabel(tokens[0])){
                return false;
            }
        }
        lab = tokens[0];
    }
    else if(n==1){
        int sz=tokens[0].size();
        if(tokens[0][sz-1]!=':'){
            return false;
        }
        if(!validLabel(tokens[0].substr(0,sz-1))){
            return false;
        }
        lab = tokens[0].substr(0,sz-1);
    }

    // same label occurring twice in the program.
    if(label_map.find(lab) != label_map.end()){
        return false;
    }
    label_map[lab] = INSTR_MEM;
    temp_paramList = {};
    temp_command = lab;
    return true;
}


bool validate(string& s){
    int n=s.size();
    char str[n+1];
    strcpy(str,s.c_str());
    char* temp = strtok(str," \t,");
    vector<string> tokens;
    while(temp != NULL){
        tokens.push_back(temp);
        temp = strtok(NULL," \t,");
    }
    n = tokens.size();
    if(n==0){
        return false;
    }

    if(tokens[0] == "add"){
        return AddSubMulSlt_check(tokens);
    }
    else if(tokens[0] == "sub"){
        return AddSubMulSlt_check(tokens);
    }
    else if(tokens[0] == "mul"){
        return AddSubMulSlt_check(tokens);
    }
    else if(tokens[0] == "beq"){
        return Branch_check(tokens);
    }
    else if(tokens[0] == "bne"){
        return Branch_check(tokens);
    }
    else if(tokens[0] == "slt"){
        return AddSubMulSlt_check(tokens);
    }
    else if(tokens[0] == "j"){
        return J_check(tokens);
    }
    else if(tokens[0] == "lw"){
        return LwSw_check(tokens);
    }
    else if(tokens[0] == "sw"){
        return LwSw_check(tokens);
    }
    else if(tokens[0] == "addi"){
        return Addi_check(tokens);
    }
    else{
        // case of label
        return Label_check(tokens);
    }

    return false;
}

void tokenise(int lineNum,string str){
    INSTR_NUM++;
    Instruction temp;
    temp.command = temp_command;
    temp.param = temp_paramList;
    temp.lineNumber = lineNum;
    temp.execution_count = 0;
    temp.str = str;
    temp.mem_address = INSTR_MEM;
    program.push_back(temp);

    if(temp_paramList.size()!=0){
        INSTR_MEM+=32;
    }
}

int reg(string s){
    return register_map[s];
}

int register_file(int p){
    return register_File[p];
}

int goto_lab(string s){
    return label_map[s];
}

bool outOfBounds(int address){
    if(address > 1048576*8 -1){
        return true;
    }
    if(address < INSTR_MEM){
        return true;
    }
    if(address%32 !=0 ){
        return true;
    }
    return false;
}

int add_overflow(int v3,int v1,int v2){
    if(v1>0 && v2>0 && v3<0){
        return true;
    }
    else if(v1<0 && v2<0 && v3>0){
        return true;
    }
    return false;
}

int sub_overflow(int v3,int v1,int v2){
    if(v1>0 && v2<0 && v3<0){
        return true;
    }
    else if(v1<0 && v2>0 && v3>0){
        return true;
    }
    return false;
}

int mul_overflow(int v3,int v1,int v2){
    if(v1==0 || v2==0){
        return false;
    }
    if(v2 == v3/v1){
        return false;
    }
    return true;
}

bool isEmpty(string s){
    for(char ch:s){
        if(ch!=' '&&ch!='\t'){
            return false;
        }
    }
    return true;
}

void putRegInMem(string r,int m){
    int re = reg(r);
    string res = intToBits5(re);
    copyToMem(res,m);
}

int getFromMem(int m,int sz){
    int num=0;
    for(int i=m;i<m+sz;i++){
        num*=2;
        if(memory[i]==1){
            num+=1;
        }
    }
    if(sz>=18 && memory[m]==1){
        num = (num+1)*-1;
    }
    return num;
}

int main(){
    int r_delay;
    int c_delay;
    cout<<"Enter ROW_ACCESS_DELAY: ";
    cin>>r_delay;
    cout<<"Enter COL_ACCESS_DELAY: ";
    cin>>c_delay;
    initializeRegisterMap();
    initializeInsMap();
    register_File[29] = 1048576;

    ifstream fin;
    fin.open("input.txt");
    int lineNum=1;
    INSTR_NUM = 0;
    string str;
    while(getline(fin,str)){
        if(isEmpty(str)){
            lineNum++;
            continue;
        }

        bool correct_syntax = validate(str);
        if(!correct_syntax){
            cout<<"Syntax error: At line number: "<<lineNum<<" : "<<str<<endl;
            return -1;
        }

        tokenise(lineNum,str);

        lineNum++;
    }
    for(auto str:labels_used){
        if(label_map.find(str) == label_map.end()){
            cout<<"Syntax Error: Undefined label:"<<str<<"\n";
            return -1;
        }
    }
    fin.close();

    int i=0;
    int n=program.size();
    int m=0;
    while(i<n){
        string cmd = program[i].command;
        vector<string> arg = program[i].param;
        if(arg.size() == 0){
            i++;
            continue;
        }
        int in = ins_map[cmd];
        string ins = intToBits4(in);
        copyToMem(ins,m);
        if(in==0 || in==1 || in==2 || in==5){
            putRegInMem(arg[0],m+4);
            putRegInMem(arg[1],m+9);
            putRegInMem(arg[2],m+14);
        }
        else if(in==3 || in==4){
            putRegInMem(arg[0],m+4);
            putRegInMem(arg[1],m+9);
            int v=label_map[arg[2]]/8;
            if(v>131071){
                cout<<"Too Large Program!! Possibility of errors in storing labels\n";
                return -1;
            }
            string ad = intToBits18(v);
            copyToMem(ad,m+14);
        }
        else if(in==6){
            int v=label_map[arg[0]]/8;
            if(v>131071){
                cout<<"Too Large Program!! Possibility of errors in storing labels\n";
                return -1;
            }
            string ad = intToBits28(v);
            copyToMem(ad,m+4);
        }
        else if(in==7 || in==8){
            putRegInMem(arg[0],m+4);
            putRegInMem(arg[1],m+9);
            int v=stoi(arg[2]);
            string ad = intToBits18(v);
            copyToMem(ad,m+14);
        }
        else{
            putRegInMem(arg[0],m+4);
            putRegInMem(arg[1],m+9);
            int v=stoi(arg[2]);
            string ad = intToBits18(v);
            copyToMem(ad,m+14);
        }

        m+=32;
        i++;
    }

    int TOTAL_INSTR_EXECUTED = 0;
    int TOTAL_CYCLE = 0;
    int pc=0;
    while(pc < INSTR_MEM){
        int cmd = getFromMem(pc,4);
        if(cmd == 0){
            int dst = getFromMem(pc+4,5);
            int p1 = getFromMem(pc+9,5);
            int p2 = getFromMem(pc+14,5);
            if(dst!=-1){
                int val1 = register_file(p1);
                int val2 = register_file(p2);
                int val3 = val1 + val2;
                if(add_overflow(val3,val1,val2)){
                    cout<<"Memory Address: "<<pc<<" : Calculation overflow detected!!\n";
                    return -1;
                }
                register_File[dst] = val3;
                TOTAL_CYCLE++;
                cout<<"cycle "<<TOTAL_CYCLE<<": "<<rev_register_map[dst]<<"="<<val3<<endl;
            }
            
        }
        else if(cmd == 1){
            int dst = getFromMem(pc+4,5);
            int p1 = getFromMem(pc+9,5);
            int p2 = getFromMem(pc+14,5);
            if(dst!=0){
                int val1 = register_file(p1);
                int val2 = register_file(p2);
                int val3 = val1 - val2;
                if(sub_overflow(val3,val1,val2)){
                    cout<<"Memory Address: "<<pc<<" : Calculation overflow detected!!\n";
                    return -1;
                }
                register_File[dst] = val3;
                TOTAL_CYCLE++;
                cout<<"cycle "<<TOTAL_CYCLE<<": "<<rev_register_map[dst]<<"="<<val3<<endl;
            }
        }
        else if(cmd == 2){
            TOTAL_CYCLE++;
            int dst = getFromMem(pc+4,5);
            int p1 = getFromMem(pc+9,5);
            int p2 = getFromMem(pc+14,5);
            if(dst!=0){
                int val1 = register_file(p1);
                int val2 = register_file(p2);
                int val3 = val1 * val2;
                if(mul_overflow(val3,val1,val2)){
                    cout<<"Memory Address: "<<pc<<" : Calculation overflow detected!!\n";
                    return -1;
                }
                register_File[dst] = val3;
                TOTAL_CYCLE++;
                cout<<"cycle "<<TOTAL_CYCLE<<": "<<rev_register_map[dst]<<"="<<val3<<endl;
            }
        }
        else if(cmd == 3){
            TOTAL_CYCLE++;
            int p1 = getFromMem(pc+4,5);
            int p2 = getFromMem(pc+9,5);
            if(register_file(p1) == register_file(p2)){
                // change value of i to the instruction of label:arg[2]
                pc = getFromMem(pc+14,18)*8 - 32;
            }
        }
        else if(cmd == 4){
            TOTAL_CYCLE++;
            int p1 = getFromMem(pc+4,5);
            int p2 = getFromMem(pc+9,5);
            if(register_file(p1) != register_file(p2)){
                // store value of $ra register to this statement.
                // change value of i to the instruction of label:arg[2]
                pc = getFromMem(pc+14,18)*8 - 32;
            }
        }
        else if(cmd == 5){
            int dst = getFromMem(pc+4,5);
            int p1 = getFromMem(pc+9,5);
            int p2 = getFromMem(pc+14,5);
            if(dst!=0 && register_file(p1) < register_file(p2)){
                register_File[dst] = 1;
                TOTAL_CYCLE++;
                cout<<"cycle "<<TOTAL_CYCLE<<": "<<rev_register_map[dst]<<"="<<1<<endl;
            }
            else if(dst!=0){
                register_File[dst] = 0;
                TOTAL_CYCLE++;
                cout<<"cycle "<<TOTAL_CYCLE<<": "<<rev_register_map[dst]<<"="<<0<<endl;
            }
            
        }
        else if(cmd == 6){
            TOTAL_CYCLE++;
            // change value of pc to the label:arg[0]
             pc = getFromMem(pc+4,28)*8 - 32;
        }
        else if(cmd == 7){
            int r = getFromMem(pc+4,5);
            int a = getFromMem(pc+9,5);
            int p = getFromMem(pc+14,18);
            // load value from memory[p] to register_file[r];
            if(r!=0){
                int val = INSTR_MEM + p*8 +register_file(a)*8;
                if(outOfBounds(val)){
                    cout<<"Memory Address:"<<pc<<":Attempt to access memory failed!! Terminating Execution!\n";
                    return -1;
                }
                //register_File[r] = memory[val];
                int value = getFromMem(val,32);
                register_File[r]=value;
                TOTAL_CYCLE++;
                cout<<"cycle "<<TOTAL_CYCLE<<": DRAM request issued"<<endl;
                if (flag==false){
                    row = ((val-INSTR_MEM)/8)/1024;
                    col = ((val-INSTR_MEM)/8)%1024;
                    TOTAL_CYCLE++;
                    cout<<"cycle "<<TOTAL_CYCLE<<"-"<<TOTAL_CYCLE+r_delay+c_delay-1<<": "<<rev_register_map[r]<<"="<<value<<endl;
                    TOTAL_CYCLE=TOTAL_CYCLE+r_delay+1;
                    update++;
                    flag=true;
                }
                else{
                    if (((val-INSTR_MEM)/8)/1024 != row){
                        row = ((val-INSTR_MEM)/8)/1024;
                        col = ((val-INSTR_MEM)/8)%1024;
                        TOTAL_CYCLE++;
                        cout<<"cycle "<<TOTAL_CYCLE<<"-"<<TOTAL_CYCLE+2*r_delay+c_delay-1<<": "<<rev_register_map[r]<<"= "<<value<<endl;
                        TOTAL_CYCLE=TOTAL_CYCLE+2*r_delay+c_delay-1;
                        update++;
                    }
                    else{
                        col = ((val-INSTR_MEM)/8)%1024;
                        TOTAL_CYCLE++;
                        cout<<"cycle "<<TOTAL_CYCLE<<"-"<<TOTAL_CYCLE+c_delay-1<<": "<<rev_register_map[r]<<"= "<<value<<endl;
                        TOTAL_CYCLE=TOTAL_CYCLE+c_delay-1;
                    }
                }
            }
        }
        else if(cmd == 8){
            int r = getFromMem(pc+4,5);
            int a = getFromMem(pc+9,5);
            int p = getFromMem(pc+14,18);
            // store value in memory[p] from register_file[r];
            // memory form 0 to INSTR_MEM -1 is used for instructions, after that for data.
            int val = INSTR_MEM + p*8 +register_file(a)*8;
            if(outOfBounds(val)){
                cout<<"Memory Address:"<<pc<<":Attempt to access memory out of bounds failed!! Terminating Execution!\n";
                return -1;
            }
            //memory[val] = register_file(r);
            string value = intToBits32(register_file(r));
            copyToMem(value,val);
            TOTAL_CYCLE++;
            cout<<"cycle "<<TOTAL_CYCLE<<": DRAM request issued"<<endl;
            if (flag==false){
                row = ((val-INSTR_MEM)/8)/1024;
                col = ((val-INSTR_MEM)/8)%1024;
                TOTAL_CYCLE++;
                cout<<"cycle "<<TOTAL_CYCLE<<"-"<<TOTAL_CYCLE+r_delay+c_delay-1<<": memory address "<<(val-INSTR_MEM)/8<<"-"<<((val-INSTR_MEM)/8)+3<<"= "<<register_file(r)<<endl;
                TOTAL_CYCLE=TOTAL_CYCLE+r_delay+c_delay-1;
                update++;
                update++;
                flag=true;
            }
            else{
                if (((val-INSTR_MEM)/8)/1024 != row){
                    row = ((val-INSTR_MEM)/8)/1024;
                    col = ((val-INSTR_MEM)/8)%1024;
                    TOTAL_CYCLE++;
                    cout<<"cycle "<<TOTAL_CYCLE<<"-"<<TOTAL_CYCLE+2*r_delay+c_delay-1<<": memory address "<<(val-INSTR_MEM)/8<<"-"<<((val-INSTR_MEM)/8)+3<<"= "<<register_file(r)<<endl;
                    TOTAL_CYCLE=TOTAL_CYCLE+2*r_delay+c_delay-1;
                    update++;
                    update++;
                }
                else{
                    col = ((val-INSTR_MEM)/8)%1024;
                    TOTAL_CYCLE++;
                    cout<<"cycle "<<TOTAL_CYCLE<<"-"<<TOTAL_CYCLE+c_delay-1<<": memory address "<<(val-INSTR_MEM)/8<<"-"<<((val-INSTR_MEM)/8)+3<<"= "<<register_file(r)<<endl;
                    TOTAL_CYCLE=TOTAL_CYCLE+c_delay-1;
                    update++;
                }
            }
        }
        else if(cmd == 9){
            int dst = getFromMem(pc+4,5);
            int r = getFromMem(pc+9,5);
            int c = getFromMem(pc+14,18);
            if(dst!=0){
                int val1 = register_file(r);
                int val2 = c;
                int val3 = val1 + val2;
                if(add_overflow(val3,val1,val2)){
                    cout<<"Memory Address: "<<pc<<" : Calculation overflow detected!!\n";
                    return -1;
                }
                register_File[dst] = register_file(r) + c;
                TOTAL_CYCLE++;
                cout<<"cycle "<<TOTAL_CYCLE<<": "<<rev_register_map[dst]<<"="<<val3<<endl;
            }
        }
        pc+=32;
        TOTAL_INSTR_EXECUTED++;

    }
    cout<<"Total number of cycles: "<<TOTAL_CYCLE<<endl;
    cout<<"Number of Buffer updates: "<<update<<endl;
    cout<<"space taken by in struction: "<<INSTR_MEM<<endl;

    return 0;
}
