
/*
   NAME- MIHIKA
   ROLL NUMBER-2301CS31
   EMULATOR

   Declaration of Authorship
*/


#include <bits/stdc++.h>
using namespace std;

int accumulator_A, accumulator_B, program_counter, stack_pointer, index, total_instructions, execution_status;

// Struct to group emulator registers
struct Registers {
    int A = 0;   // register A
    int B = 0;   // register B
    int PC = 0;  // Program Counter
    int SP = 0;  // Stack Pointer
};

// Struct to group execution-related variables
struct ExecutionStatus {
    int index = 0; //index for instruction tracking 
    int total_instructions = 0;  //count of total instructions executed
    int status = 0; // execution status flag
};

// Global variables for emulator
int main_memory[1 << 24] = {0};  // 16 MB (2^24 bytes) memory
vector<string> machine_code_lines; //stores machine code as hexadecimal strings for display
Registers registers;  // Register object to hold A, B, PC, SP
ExecutionStatus execution;  // Execution status object
array<int, 2> memory_change = {0};  // To track memory changes, if any

// Mnemonics for the instruction set
const vector<string> mnemonics = {
    "ldc", "adc", "ldl", "stl", "ldnl", "stnl", 
    "add", "sub", "shl", "shr", "adj", "a2sp", 
    "sp2a", "call", "return", "brz", "brlz", 
    "br", "HALT"
};

//emulator instruction implementation

//load constant value to accumulator A
void load_constant(int value) { 
    accumulator_B = accumulator_A ; 
    accumulator_A = value; 
}

//add constant value to accumulator A
void add_constant(int value) { 
    accumulator_A = accumulator_A + value; 
}


//load a value from the local memory offset
void load_local(int offset) {
    accumulator_B = accumulator_A;
    accumulator_A = main_memory[stack_pointer + offset];
    memory_change = {stack_pointer + offset, 0};
    execution_status = 1;          // 1 for read
}

//store accumulator A's value to the local memory offset
void store_local(int offset) {
    memory_change = {main_memory[stack_pointer + offset], accumulator_A};
    main_memory[stack_pointer + offset] = accumulator_A;
    execution_status = 2;        // 2 for write
    accumulator_A = accumulator_B;
}

//load a non local memory value using offset
void load_non_local(int offset) {
    accumulator_A = main_memory[accumulator_A + offset];
    memory_change = {stack_pointer + offset, 0};
    execution_status = 1;
}

//store accumulator B's value to the non local memory offset
void store_non_local(int offset) {
    memory_change = {main_memory[accumulator_A + offset], accumulator_B};
    main_memory[accumulator_A + offset] = accumulator_B;
    execution_status = 2;
}

//defining arithmetic and control operations

//a = a + b
void add(int unused = 0) { 
    accumulator_A = accumulator_A + accumulator_B; 
}

// a = b - a
void subtract(int unused = 0) { 
    accumulator_A = accumulator_B - accumulator_A; 
}

// Function to perform left bitwise shift of accumulator_B by the number of bits in accumulator_A
// Result is stored in accumulator_A
void shift_left(int unused = 0) { 
    accumulator_A = accumulator_B << accumulator_A; 
}

// Function to perform right bitwise shift of accumulator_B by the number of bits in accumulator_A
// Result is stored in accumulator_A
void shift_right(int unused = 0) { 
    accumulator_A = accumulator_B >> accumulator_A; 
}

// Function to adjust the stack pointer (SP) by a given value
// The value is added to stack_pointer, allowing stack manipulation
void adjust_stack_pointer(int value) { 
    stack_pointer = stack_pointer + value; 
}

// Function to transfer the value in accumulator_A to the stack pointer (SP)
// Then, accumulator_A is set to the value in accumulator_B
// Useful for setting up stack frame or switching contexts
void accumulator_to_stack_pointer(int unused = 0) { 
    stack_pointer = accumulator_A; accumulator_A = accumulator_B; 
}

// Function to transfer the stack pointer (SP) value to accumulator_A
// accumulator_B is set to the previous value in accumulator_A
// Useful for retrieving stack frame or pointer values
void stack_pointer_to_accumulator(int unused = 0) { 
    accumulator_B = accumulator_A; accumulator_A = stack_pointer; 
}

// procedure call and return instructions

void call_procedure(int offset) {
    accumulator_B = accumulator_A;
    accumulator_A = program_counter;
    program_counter = program_counter + offset;
}

void return_procedure(int unused = 0) { 
    program_counter = accumulator_A; 
    accumulator_A = accumulator_B; 
}

//branch instructions

void branch_if_zero(int offset) { 
    if (accumulator_A == 0) 
    program_counter = program_counter + offset; 
}

void branch_if_less_than_zero(int offset) { 
    if (accumulator_A < 0) 
    program_counter = program_counter + offset; 
}

void branch(int offset) { 
    program_counter = program_counter + offset; 
}

// array of function pointers to execute corresponding opcode function
void (*instruction_functions[])(int) = {load_constant, add_constant, load_local, store_local, load_non_local,
                                       store_non_local, add, subtract, shift_left, shift_right,
                                       adjust_stack_pointer, accumulator_to_stack_pointer, stack_pointer_to_accumulator, call_procedure,
                                       return_procedure, branch_if_zero, branch_if_less_than_zero, branch};

//function to convert decimal numbers to hexadecimal (8-character string)
string decimal_to_hexadecimal_conversion(unsigned int number) {
    const char hexadecimal_digits[] = "0123456789abcdef";
    string hexadecimal_representation(8, '0');  // Start with a string of 8 '0's

    for (int i = 7; i >= 0; i--) {           // Fill the string from right to left
        hexadecimal_representation[i] = hexadecimal_digits[number % 16];  // Get the last hex digit
        number = number / 16;                          // Move to the next hex digit
    }

    return hexadecimal_representation;
}

//function to load machine code from a file into main memory
void load_machine_code() {
    cout << "Enter file name (e.g., machineCode.o): ";
    string file_name;
    getline(cin, file_name);  // Use getline to handle spaces

    ifstream input_file(file_name, ios::binary);  //ifstream - read data from file && ios:: - ensures the file is read as binary data, not text.
    if (!input_file) {
        cerr << "Error: Unable to open file " << file_name << endl;
        return;
    }

    unsigned int instruction;
    int position = 0;      //machine code instructions are often represented as integers

    while (input_file.read(reinterpret_cast<char*>(&instruction), sizeof(instruction))) { //allows the reading of binary data into the memory space of the instruction variable
        if (position < sizeof(main_memory) / sizeof(main_memory[0])) {
            main_memory[position++] = instruction;
            machine_code_lines.emplace_back(decimal_to_hexadecimal_conversion(instruction));
        } else {
            cerr << "Error: Exceeded memory limits." << endl;
            break;
        }
    }

    if (input_file.eof()) {
        cout << "Machine code loaded successfully from " << file_name << endl;
    } else {
        cerr << "Error: Reading interrupted before reaching end of file." << endl;
    }
}

void function_to_display_welcome_message() {
    cout << "Welcome to the Emulator!" << endl;
    cout << "\nAvailable Commands:\n" << endl;

    cout << left << setw(7) << "Sr.No." << setw(10) << "Command" << setw(40) << "Description" << endl;
    cout << "---------------------------------------------------------------" << endl;
    cout << left << setw(7) << "1." << setw(10) << "-dump"    << setw(40) << "Display a memory dump" << endl;
    cout << left << setw(7) << "2." << setw(10) << "-t"       << setw(40) << "Step through code one line at a time" << endl;
    cout << left << setw(7) << "3." << setw(10) << "-run"     << setw(40) << "Run the code until completion" << endl;
    cout << left << setw(7) << "4." << setw(10) << "-reg"     << setw(40) << "Display register and stack pointer (SP) values" << endl;
    cout << left << setw(7) << "5." << setw(10) << "-isa"     << setw(40) << "Show the instruction set" << endl;
    cout << left << setw(7) << "6." << setw(10) << "-read"    << setw(40) << "Read from memory" << endl;
    cout << left << setw(7) << "7." << setw(10) << "-write"   << setw(40) << "Write to memory" << endl;

    cout << "\nEnter a command to proceed." << endl;
}

//display the contents of machine_code_lines in a memory dump-like format
void function_to_display_memory_dump() {
    int memory_size = static_cast<int>(machine_code_lines.size());

    for (int i = 0; i < memory_size; i = i + 4) {
        cout << decimal_to_hexadecimal_conversion(i) << ": ";  // Print memory address in hex

        // Display up to 4 consecutive memory contents at this address
        for (int j = i; j < i + 4 && j < memory_size; j++) {
            cout << machine_code_lines[j] << " ";
        }
        
        cout << endl;
    }
}

void function_to_display_registers() {
    cout << "\n======================== Emulator Registers ========================\n";
    cout << "|   Register   |   Value (Hex)   |" << endl;
    cout << "--------------------------------------------------------------------" << endl;
    cout << "|     A        |   " << setw(10) << decimal_to_hexadecimal_conversion(accumulator_A) << "   |" << endl;
    cout << "|     B        |   " << setw(10) << decimal_to_hexadecimal_conversion(accumulator_B) << "   |" << endl;
    cout << "|     SP       |   " << setw(10) << decimal_to_hexadecimal_conversion(stack_pointer) << "   |" << endl;
    cout << "|     PC       |   " << setw(10) << decimal_to_hexadecimal_conversion(program_counter + 1) << "   |" << endl;
    cout << "--------------------------------------------------------------------" << endl;
    cout << "|   Mnemonic   |   " << mnemonics[program_counter] << endl;
    cout << "====================================================================\n";
}


void function_to_display_read_operations() {
    cout << "\n============= Memory Read Operation =============\n";
    cout << "|     Address | " << setw(13) << decimal_to_hexadecimal_conversion(program_counter) << " |" << endl;
    cout << "|      Value  | " << setw(13) << decimal_to_hexadecimal_conversion(memory_change[0]) << " |" << endl;
    cout << "====================================================================\n";
}

void function_to_display_write_operations() {
    cout << "\n====================== Memory Write Operation ======================\n";
    cout << "| Address       | " << setw(20) << decimal_to_hexadecimal_conversion(program_counter) << " |" << endl;
    cout << "| Previous Value| " << setw(20) << decimal_to_hexadecimal_conversion(memory_change[0]) << " |" << endl;
    cout << "| New Value     | " << setw(20) << decimal_to_hexadecimal_conversion(memory_change[1]) << " |" << endl;
    cout << "====================================================================\n";
}


void displayInstructionSet() {
    cout << "Instruction Set:\n" << endl;

    cout << left << setw(7) << "Sr. No."
         << setw(18) << "| OpMachineCode"
         << setw(20) << "| Mnemonic Operand" << endl;

    cout << "----------------------------------------------------------" << endl;

    cout << left << setw(7) << "     0." << setw(18) << "|    ldc"   << setw(20) << "| value" << endl;
    cout << left << setw(7) << "     1." << setw(18) << "|    adc"   << setw(20) << "| value" << endl;
    cout << left << setw(7) << "     2." << setw(18) << "|    ldl"   << setw(20) << "| value" << endl;
    cout << left << setw(7) << "     3." << setw(18) << "|    stl"   << setw(20) << "| value" << endl;
    cout << left << setw(7) << "     4." << setw(18) << "|    ldnl"  << setw(20) << "| value" << endl;
    cout << left << setw(7) << "     5." << setw(18) << "|    stnl"  << setw(20) << "| value" << endl;
    cout << left << setw(7) << "     6." << setw(18) << "|    add"   << setw(20) << "|" << endl;
    cout << left << setw(7) << "     7." << setw(18) << "|    sub"   << setw(20) << "|" << endl;
    cout << left << setw(7) << "     9." << setw(18) << "|     shr"   << setw(20) << "|" << endl;
    cout << left << setw(7) << "    10." << setw(18) << "|   adj"   << setw(20) << "| value" << endl;
    cout << left << setw(7) << "    11." << setw(18) << "|    a2sp"  << setw(20) << "|" << endl;
    cout << left << setw(7) << "    12." << setw(18) << "|   sp2a"  << setw(20) << "|" << endl;
    cout << left << setw(7) << "    13." << setw(18) << "|   call"  << setw(20) << "| offset" << endl;
    cout << left << setw(7) << "    14." << setw(18) << "|   return" << setw(20) << "|" << endl;
    cout << left << setw(7) << "    15." << setw(18) << "|   brz"   << setw(20) << "| offset" << endl;
    cout << left << setw(7) << "    16." << setw(18) << "|    brlz"  << setw(20) << "| offset" << endl;
    cout << left << setw(7) << "    17." << setw(18) << "|    br"    << setw(20) << "| offset" << endl;
    cout << left << setw(7) << "    18." << setw(18) << "|    HALT"  << setw(20) << "|" << endl;
}

bool function_to_execute_instructions(int operation, int maximum_executions = (1 << 25)) {
    // Begin a loop to execute instructions, with a limit on the number of executions .. the loop will run till the time max executions > 0
    while (maximum_executions-- && program_counter < machine_code_lines.size()) {
        total_instructions++;
        // Check if we've exceeded memory bounds or if we've executed too many instructions
        // If so, print "Segmentation Fault" and return false to stop execution
        if (program_counter >= machine_code_lines.size() || total_instructions > (int)3e7) {
            cout << "Segmentation Fault" << endl;
            return false;
        }
        // Retrieve the current instruction from the machine code lines using the program counter
        string current_instruction = machine_code_lines[program_counter];
        // Extract the opcode (bits 6-7 in the instruction, represented as hexadecimal)
        int opcode = stoi(current_instruction.substr(6, 2), 0, 16);
        if (opcode == 18) {
            cout << "HALT found" << endl;
            cout << total_instructions << " statements were executed in total" << endl;
            return true;
        }
        int operand = stoi(current_instruction.substr(0, 6), 0, 16);   //substr(6,2) extracts the substring at position 6-7(2 char) representing opcode in hex & stoi(.. 0,16) converts hex string to int
        if (operand >= (1 << 23)) { //checking if operand >= 2^23 (interpreting it as a 24 bit signed int)
            operand -= (1 << 24); // if it is true subtract 2^24 to convert it from unsigned to signed representation (for -ve numbers)
        }
        // If maximum_executions is zero, call a function to display the current register values
        if (maximum_executions == 0)
            function_to_display_registers();
        // Call the appropriate function for the current instruction based on its opcode
        // The operand is passed to the instruction function for execution
        (instruction_functions[opcode])(operand);
        if (operation == 1 && execution_status == 1) {
            function_to_display_read_operations();  // Display information about memory reads
            execution_status = 0;  // Reset execution status after displaying read operations
        }
        else if (operation == 2 && execution_status == 2) {
            function_to_display_write_operations();
            execution_status = 0;
        }
        program_counter++;
        index++;
    }
    return true;
}

// The main function to start the emulator
bool function_to_start_emulator() {
    cout << "Enter command or 0 to exit:" << endl;
    string command;
    cin >> command;

    // Map of commands to their respective functions
    map<string, function<void()>> command_map = {
        {"-dump", function_to_display_memory_dump},
        {"-reg", function_to_display_registers},
        {"-isa", displayInstructionSet}
    };

    map<string, function<bool()>> command_map_with_return = {
        {"-t", []() { return function_to_execute_instructions(0, 1); }},
        {"-run", []() { return function_to_execute_instructions(0); }},
        {"-read", []() { return function_to_execute_instructions(1); }},
        {"-write", []() { return function_to_execute_instructions(2); }}
    };

    // Exit condition
    if (command == "0") {
        exit(0);
    }

    // Execute commands that don’t return a boolean
    if (command_map.count(command)) {
        command_map[command]();
        return true;
    }
    // Execute commands that return a boolean
    else if (command_map_with_return.count(command)) {
        return command_map_with_return[command]();
    } else {
        cout << "Enter a valid command" << endl;
    }

    return true;
}

int main() {
    load_machine_code(); //load machine code into memory
    function_to_display_welcome_message(); //display the welcome message
    // Main loop to receive user commands and execute corresponding functions
    while (true) {
       function_to_start_emulator(); 
    }
    return 0;
}
