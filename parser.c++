#include <iostream>
#include "lexer.h"
#include <vector>
#include <map>
#include <string>
#include <array>
using namespace std;

// --- AST Node Definitions ---
struct IfNode {
    vector<Token> condition; 
    vector<Token> body;      
};

struct WhileNode{
    vector<Token> condition;
    vector<Token> body;
};

struct FunctionNode{
    string name;
    vector<string> params;
    vector <Token> body;
};

// ONLY keep these at the top:
void compile_if(IfNode& node, vector<unsigned char>& bytecode, map<string, int>& name_to_id, int& next_var_id, map<string, int>& func_to_address);

void compile_block(const vector<Token>& tokens, vector<unsigned char>& bytecode, map<string, int>& name_to_id, int& next_var_id, map<string, int>& func_to_address);

void parse_function(const vector<Token>& tokens, size_t& i, vector<unsigned char>& bytecode, map<string, int>& func_to_address, map<string, int>& name_to_id, int& next_var_id);

void parse_function(const vector<Token>& tokens, size_t& i, vector<unsigned char>& bytecode, 
                    map<string, int>& func_to_address, map<string, int>& name_to_id, 
                    int& next_var_id);
// --- VM Execution ---
void run_vm(const vector<unsigned char>& bytecode) {
    vector<array<int, 256>> frames;
    frames.push_back({});
    vector<int> stack;      
    size_t ip = 0;    
    vector<int> call_stack; // For future function calls
    map<int, vector<int>> heap;
    int next_ptr = 1;

    while (ip < bytecode.size()) {
        unsigned char opcode = bytecode[ip++];
        switch (opcode) {
            case 0x00: // HALT
                cout << "VM: Halted normally.\n";
                return;

            case 0x01: { // PUSH_INT
                int val = 0;
                for (int i = 0; i < 4; i++) {
                    val |= (static_cast<int>(bytecode[ip++]) << (i * 8));
                }
                stack.push_back(val);
                break;
            }

            case 0x02: { // STORE_VAR
                int var_id = bytecode[ip++];
                frames.back()[var_id] = stack.back();
                stack.pop_back();
                break;
            }
            case 0x03: { // PRINT
                if (stack.empty()){
                    cout << "VM Error: Nothing to print" << endl;
                    return;
                }
                int val = stack.back();
                stack.pop_back();
                cout << ">> " << val << endl;
                break;
            }

            case 0x04: { // LOAD_VAR
                int var_id = bytecode[ip++];
                stack.push_back(frames.back()[var_id]);
                break;
            }

            case 0x10: { // ADD
                int b = stack.back(); stack.pop_back();
                int a = stack.back(); stack.pop_back();
                stack.push_back(a + b);
                break;
            }

            case 0x11: { // SUB
                int b = stack.back(); stack.pop_back();
                int a = stack.back(); stack.pop_back();
                stack.push_back(a - b);
                break;
            }

            case 0x12: { // MUL
                int b = stack.back(); stack.pop_back();
                int a = stack.back(); stack.pop_back();
                stack.push_back(a * b);
                break;
            }

            case 0x14: { // OP_EQ
                int b = stack.back(); stack.pop_back();
                int a = stack.back(); stack.pop_back();
                stack.push_back(a == b ? 1 : 0);
                break;
            }

            case 0x15: { // OP_JZ
                int target = bytecode[ip++];
                int condition = stack.back(); stack.pop_back();
                if (condition == 0) ip = target; 
                break;
            } 
            case 0x016: {
                int target = bytecode[ip++];
                ip = target;
                break;
            }

            case 0x17: { // OP_LT
                int b = stack.back(); stack.pop_back();
                int a = stack.back(); stack.pop_back();
                stack.push_back(a < b ? 1 : 0);
                break;
            }
            case 0x18: { // OP_GT
                int b = stack.back(); stack.pop_back();
                int a = stack.back(); stack.pop_back();
                stack.push_back(a > b ? 1 : 0);
                break;
            }

            case 0x19:{ //OP_CALL
                int func_address = bytecode[ip++ ];
                call_stack.push_back(ip); //save the return address
                frames.push_back({});  // new frame for this call
                ip = func_address;
                break;

            }
            case 0x20:{ // OP_RET

                if (call_stack.empty()){
                    cout << "VM Error: Return without Call" << endl;                    return;
                }
                ip = call_stack.back();
                call_stack.pop_back();
                frames.pop_back();  // destroy this call's frame
                break;

            }
            case 0x30: { // ALLOC
                if (stack.empty()) {
                    cout << "VM Error: Stack empty during ALLOC" << endl;
                    return;
                }
                int size = stack.back(); stack.pop_back();
                if (size < 0 || size > 1000000) { // Safety limit: 1 million elements
                    cout << "VM Error: Invalid allocation size: " << size << endl;
                    return;
                }
                heap[next_ptr] = vector<int>(size, 0);
                stack.push_back(next_ptr++);
                break;
            }

            case 0x31:{ //store index (ptr, idx, val)
                int val = stack.back(); stack.pop_back();
                int idx = stack.back(); stack.pop_back();
                int ptr = stack.back(); stack.pop_back();
                heap[ptr][idx] = val;
                break;
            }

            case 0x32:{ //load value (ptr, index);

                int idx = stack.back(); stack.pop_back();
                int ptr = stack.back(); stack.pop_back();
                stack.push_back(heap[ptr][idx]);
                break;

            }

            default:
                cout << "VM Error: Unknown OpCode " << (int)opcode << endl;
                return;
        }
    }
}


void compile_arithmetic_statement(const vector<Token>& stmt, vector<unsigned char>& bytecode, map<string, int>& name_to_id) {
    string var_name = stmt[0].value;
    int var_id = name_to_id[var_name];

    // Push first operand
    if (stmt[2].type == NUMBER) {
        int first_num = stoi(stmt[2].value);
        bytecode.push_back(0x01);
        for (int i = 0; i < 4; i++) bytecode.push_back((first_num >> (i * 8)) & 0xFF);
    } else {
        bytecode.push_back(0x04);
        bytecode.push_back((unsigned char)name_to_id[stmt[2].value]);
    }

    // Process operations
    for (size_t i = 3; i < stmt.size(); i += 2) {
        Token op = stmt[i];
        Token next = stmt[i + 1];

        if (next.type == NUMBER) {
            int val = stoi(next.value);
            bytecode.push_back(0x01);
            for (int j = 0; j < 4; j++) bytecode.push_back((val >> (j * 8)) & 0xFF);
        } else {
            bytecode.push_back(0x04);
            bytecode.push_back((unsigned char)name_to_id[next.value]);
        }

        if (op.type == PLUS) bytecode.push_back(0x10);
        else if (op.type == MINUS) bytecode.push_back(0x11);
        else if (op.type == MULTIPLY) bytecode.push_back(0x12);
    }

    bytecode.push_back(0x02); // STORE
    bytecode.push_back((unsigned char)var_id);
}



void dump_bytecode(const vector<unsigned char>& bytecode, map<string, int>& func_to_address) {
    cout << "--- BYTECODE DUMP ---" << endl;
    for (size_t i = 0; i < bytecode.size(); ++i) {
        printf("%04zu: 0x%02X ", i, bytecode[i]);
        
        // Match addresses back to function names for clarity
        for (auto const& [name, addr] : func_to_address) {
            if (addr == (int)i) cout << "  <-- Function: " << name;
        }
        
        // Add a little hint for common opcodes
        if (bytecode[i] == 0x12) cout << " (OP_CALL)";
        if (bytecode[i] == 0x20) cout << " (OP_RET)";
        if (bytecode[i] == 0x16) cout << " (OP_JMP/SKIP)";
        
        cout << endl;
    }
    cout << "---------------------" << endl;
}

void compile_condition(const vector<Token>& cond_tokens, vector<unsigned char>& bytecode, map<string, int>& name_to_id) {
    for (int i : {0, 2}) { // Left and Right side
        if (cond_tokens[i].type == INDENT) {
            bytecode.push_back(0x04);//load the variable
            bytecode.push_back((unsigned char)name_to_id[cond_tokens[i].value]);
        } else {
            int val = stoi(cond_tokens[i].value);
            bytecode.push_back(0x01);
            for (int j = 0; j < 4; j++) bytecode.push_back((val >> (j * 8)) & 0xFF);
        }
    }
    if (cond_tokens[1].value == "==") bytecode.push_back(0x14);
    else if (cond_tokens[1].value == "<"){
        bytecode.push_back(0x17);
        
    }
    else if (cond_tokens[1].value == ">"){
        bytecode.push_back(0x18);
    }
}




void compile_while(WhileNode& node, vector<unsigned char>& bytecode, map<string, int>& name_to_id, int& next_var_id, map<string, int>& func_to_address){

    int start = bytecode.size(); // Start of the loop
    compile_condition(node.condition, bytecode, name_to_id); //put either 0 or 1 on the stack 

    bytecode.push_back(0x015);
    int exit = bytecode.size();
    bytecode.push_back(0);
    compile_block(node.body, bytecode, name_to_id, next_var_id, func_to_address);
    bytecode.push_back(0x016);
    bytecode.push_back((unsigned char)start);
    bytecode[exit] = (unsigned char) bytecode.size();



}

void compile_if(IfNode& node, vector<unsigned char>& bytecode, map<string, int>& name_to_id, int& next_var_id, map<string, int>& func_to_address) {
    compile_condition(node.condition, bytecode, name_to_id); //put either 0 or 1 on the stack 

    bytecode.push_back(0x15); // JZ
    int jump_address_index = bytecode.size(); 
    bytecode.push_back(0); 

    //Compile the body
    compile_block(node.body, bytecode, name_to_id, next_var_id, func_to_address);

    bytecode[jump_address_index] = (unsigned char)bytecode.size();
}


void parse_function(const vector<Token>& tokens, size_t& i, vector<unsigned char>& bytecode, map<string, int>& func_to_address, map<string, int>& name_to_id, int& next_var_id) {
    
    bytecode.push_back(0x016);
    int skip_placeholder = bytecode.size();
    bytecode.push_back(0);
    string func_name = tokens[i++].value;
    func_to_address[func_name] = bytecode.size();
    //collecting params
    map<string, int> local_vars;
    int local_id = 0;
    vector<string> params;
    i ++ ; //skip the "("

    while (tokens[i].value != ")") {
        if (tokens[i].type == INDENT) {
            params.push_back(tokens[i].value);
        }
        i++;
    }

    // reverse order so first param gets first arg
    for (int p = params.size() - 1; p >= 0; p--) {
        local_vars[params[p]] = local_id++;
        bytecode.push_back(0x02);  // STORE_VAR
        bytecode.push_back((unsigned char)local_vars[params[p]]);
    }
    //collect body tokens

    vector<Token> body;
    i ++;
    i ++;
    int brace_count = 1;
    while (brace_count > 0 && i < tokens.size()){
        if (tokens[i].value == "{") brace_count ++;
        if (tokens[i].value == "}") brace_count --;
        if (brace_count > 0 ) body.push_back(tokens[i]);
        i ++;
    }

    //compile body
    compile_block(body, bytecode, local_vars, local_id, func_to_address);
    bytecode.push_back(0x20); // RET
    bytecode[skip_placeholder] = bytecode.size();
}


void compile_call(const vector<Token>& tokens, size_t& i, vector<unsigned char>& bytecode, map<string, int>& func_to_address, map<string, int>& name_to_id) {

    string func_name = tokens[i++].value;
    i++; // skip '('

    // Loop until we hit the closing parenthesis
    while (tokens[i].value != ")") {
        if (tokens[i].type == NUMBER) {
            int val = stoi(tokens[i].value);
            bytecode.push_back(0x01);
            for (int j = 0; j < 4; j++) bytecode.push_back((val >> (j * 8)) & 0xFF);
        } else if (tokens[i].type == INDENT) {
            bytecode.push_back(0x04);
            bytecode.push_back((unsigned char)name_to_id[tokens[i].value]);
        }
        i++; // Move to next token (like a comma or the closing parenthesis)
    }
    
    i++; // skip ')'
    if (i < tokens.size() && tokens[i].value == ";") {
        i++; // skip ';' safely so the main loop starts fresh on the next line
    }

    bytecode.push_back(0x19); // OP_CALL
    bytecode.push_back((unsigned char)func_to_address[func_name]);
    i -- ;
}


void compile_block(const vector<Token>& tokens, vector<unsigned char>& bytecode, map<string, int>& name_to_id, int& next_var_id, map<string, int>& func_to_address) {
    vector<Token> stmt;

    for (size_t i = 0; i < tokens.size(); i++) {
        const Token& t = tokens[i];
        cout << "token: " << t.value << " i=" << i << endl;

        if (stmt.empty() && t.value == "while"){
            WhileNode wh_node;
            i ++;
            if (tokens[i].value == "("){
                i ++;
            while(i < tokens.size() && tokens[i].value != ")") {
                wh_node.condition.push_back(tokens[i++]);
            }               
             i ++; 

            }

            if (tokens[i].value == "{"){
                i ++;
                int brace_count = 1;
                while (i < tokens.size() && brace_count > 0 ){
                    if (tokens[i].value == "{") brace_count ++;
                    if (tokens[i].value == "}") brace_count --;
                    if (brace_count > 0){
                        wh_node.body.push_back(tokens[i]);
                    } 
                    i ++;
                }
                compile_while(wh_node, bytecode, name_to_id, next_var_id,  func_to_address);
                continue;
            }

        }

        // 1. Handle IF Blocks (Brace Counting)
        else if (stmt.empty() && t.value == "if") {
            IfNode if_node;
            i++; // skip 'if'
            if (tokens[i].value == "(") {
                i++;
                while (tokens[i].value != ")") if_node.condition.push_back(tokens[i++]);
                i++; // skip ')'
            }
            if (tokens[i].value == "{") {
                i++; // skip '{'
                int brace_count = 1;
                while (i < tokens.size() && brace_count > 0) {
                    if (tokens[i].value == "{") brace_count++;
                    if (tokens[i].value == "}") brace_count--;
                    
                    if (brace_count > 0) if_node.body.push_back(tokens[i]);
                    i++;
                }
                compile_if(if_node, bytecode, name_to_id, next_var_id, func_to_address);
                i--; // Adjust for loop increment
            }
            continue;
        }
        


        if (stmt.empty() && t.value == "define"){
            i ++;
            parse_function(tokens, i , bytecode, func_to_address, name_to_id, next_var_id);
            i --;
            continue;
        }

        if (stmt.empty() && t.value == "call"){
            i ++;
            compile_call(tokens, i, bytecode, func_to_address, name_to_id);
            i--;
            continue;
        }


        // 2. Build Statements
        if (stmt.empty()) {
            if (t.type == KEYWORD || t.type == INDENT) stmt.push_back(t);
            continue;
        }

        if (t.type == SEMICOLON) {

    // 3. Check for ALLOC (Matches 'int x = alloc(5)')
    if (stmt.size() >= 4 && stmt[3].value == "alloc") {
        string var_name = stmt[1].value;
        if (name_to_id.find(var_name) == name_to_id.end()) name_to_id[var_name] = next_var_id++;

        int size = stoi(stmt[5].value);
        bytecode.push_back(0x01); // PUSH_INT
        for (int j = 0; j < 4; j++) bytecode.push_back((size >> (j * 8)) & 0xFF);
        
        bytecode.push_back(0x30); // OP_ALLOC
        bytecode.push_back(0x02); // STORE_VAR
        bytecode.push_back((unsigned char)name_to_id[var_name]);
    }
            // Declaration: int x = 5;
    else if (stmt.size() == 4 && stmt[0].type == KEYWORD) {
                string var_name = stmt[1].value;
                if (name_to_id.find(var_name) == name_to_id.end()) name_to_id[var_name] = next_var_id++;
                int val = stoi(stmt[3].value);
                bytecode.push_back(0x01);
                for (int j = 0; j < 4; j++) bytecode.push_back((val >> (j * 8)) & 0xFF);
                bytecode.push_back(0x02);
                bytecode.push_back((unsigned char)name_to_id[var_name]);
            }
            // Print: print x;
            else if (stmt.size() == 2 && stmt[0].value == "print") {
                bytecode.push_back(0x04);
                bytecode.push_back((unsigned char)name_to_id[stmt[1].value]);
                bytecode.push_back(0x03);
            }
            else if (stmt.size() >= 3) {
                compile_arithmetic_statement(stmt, bytecode, name_to_id);
            }

        
            stmt.clear();
        } else {
            stmt.push_back(t);
        }
    }
}

int main() {
    cout << "--- Le3ba Language Compiler ---" << endl;
    vector<Token> tokens = run_lexer("le3ba.txt");
    vector<unsigned char> bytecode;
    map<string, int> name_to_id;
    map<string, int> func_to_address;
    int next_var_id = 0;

    compile_block(tokens, bytecode, name_to_id, next_var_id, func_to_address);
    bytecode.push_back(0x00); // HALT

    run_vm(bytecode);
    return 0;
}