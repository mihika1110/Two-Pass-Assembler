/*
   NAME- MIHIKA
   ROLL NUMBER-2301CS31
   ASSEMBLER

   Declaration of Authorship
*/

#include <bits/stdc++.h>
using namespace std;

// Structure to hold information about each assembly instruction
struct asm_table { 
    string label;  // Label for instruction
    string mnemonic;   // Instruction type or we can say Operation code (e.g., ADD, SUB, etc)
    string operand;    // Operand (e.g., a number or a label) associated with the instruction
    int operand_type;   // Type of operand (e.g., decimal, hexadecimal)
    bool label_present;   // Flag to indicate if a label is present
};

// Global variables and data structures
vector<asm_table> asm_data;                    // stores assembly code information
map<string, pair<string, int>> opcode_map;     // Maps mnemonics to their opcode and operand type
vector<pair<int, string>> error_list;
vector<string> cleaned_code;
map<string, int> label_map;
vector<pair<int, string>> machine_code_list;
vector<int> program_counters;
bool is_halt_present = false;
string asm_file_name;

bool is_octal(string s);
bool is_hexadecimal(string s);
bool is_decimal(string s);
bool is_valid_label(string label);
string decimal_to_hexadecimal_conversion(int number, int bits = 24);

void initialize_opcodes();
void function_to_generate_error(int line, string type);
string clean_line(string s, int line);
void push_set_instructions(vector<string> &temp, string token, string s, int j);
void implement_set_instruction();
void process_labels();
void fill_asm_data(int i, string label, string mnemonic, string operand, int type);
int get_operand_type(string s);
void function_to_create_asm_table();
void function_to_separate_data_segment();
void first_pass();
bool function_to_display_errors();
string pad_zeroes(string s, int length = 6);
void second_pass();
void write_output_files();

int main() {
    first_pass();
    if (function_to_display_errors()) {
        second_pass();
        write_output_files();
    }
    system("pause");
    return 0;
}

string decimal_to_hexadecimal_conversion(int number, int bits) {
    // Ensure the number fits within the specified number of bits
    unsigned int masked_number = static_cast<unsigned int>(number) & ((1 << bits) - 1);

    // Convert to hexadecimal string
    string hexadecimal_string = "";
    do {
        int digit = masked_number % 16;
        hexadecimal_string = "0123456789abcdef"[digit] + hexadecimal_string;
        masked_number = masked_number / 16;
    } while (masked_number > 0);

    // Ensure the result has the correct number of digits (padded with leading zeros if necessary)
    while (hexadecimal_string.length() < (bits / 4)) {
        hexadecimal_string = "0" + hexadecimal_string;
    }

    return hexadecimal_string;
}


void initialize_opcodes() {
    opcode_map["data"] = {"", 1};
    opcode_map["ldc"] = {"00", 1};
    opcode_map["adc"] = {"01", 1};
    opcode_map["ldl"] = {"02", 2};
    opcode_map["stl"] = {"03", 2};
    opcode_map["ldnl"] = {"04", 2};
    opcode_map["stnl"] = {"05", 2};
    opcode_map["add"] = {"06", 0};
    opcode_map["sub"] = {"07", 0};
    opcode_map["shl"] = {"08", 0};
    opcode_map["shr"] = {"09", 0};
    opcode_map["adj"] = {"0A", 1};
    opcode_map["a2sp"] = {"0B", 0};
    opcode_map["sp2a"] = {"0C", 0};
    opcode_map["call"] = {"0D", 2};
    opcode_map["return"] = {"0E", 0};
    opcode_map["brz"] = {"0F", 2};
    opcode_map["brlz"] = {"10", 2};
    opcode_map["br"] = {"11", 2};
    opcode_map["HALT"] = {"12", 0};
    opcode_map["SET"] = {"", 1};
}

void function_to_generate_error(int line, string type) {
    error_list.push_back({line + 1, "Error at line: " + to_string(line) + " -- Type: " + type});
}


string clean_line(string s, int line) {
    // Remove leading and trailing whitespace
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](char ch) { return !isspace(ch); }));
    s.erase(find_if(s.rbegin(), s.rend(), [](char ch) { return !isspace(ch); }).base(), s.end());

    string result;
    bool is_last_was_space = false;
    
    for (int i = 0; i < s.size(); i++) {
        if (s[i] == ';') break; // Stop at the first semicolon
        
        if (s[i] == ':') {
            result = result + ':'; // Add colon
            if (i + 1 < s.size() && !isspace(s[i + 1])) result = result + ' ';
            continue;
        }

        if (isspace(s[i])) {
            if (!is_last_was_space) result = result + ' ';
            is_last_was_space = true;
        } else {
            result = result + s[i];
            is_last_was_space = false;
        }
    }

    // Remove any trailing spaces from result
    if (!result.empty() && result.back() == ' ') result.pop_back();

    // Check for excessive spaces in final result
    int space_count = count(result.begin(), result.end(), ' ');
    if (space_count > 2) function_to_generate_error(line + 1, "Invalid syntax");

    return result;
}

void push_set_instructions(vector<string> &temp, string token, string s, int j) {
    if (s.size() <= j + 5) {
        return;
    }
    temp.push_back("adj 10000");
    temp.push_back("stl -1");
    temp.push_back("stl 0");
    temp.push_back("ldc " + s.substr(j + 6, s.size() - (j + 6)));
    temp.push_back("ldc " + token.substr(0, j));
    temp.push_back("stnl 0");
    temp.push_back("ldl 0");
    temp.push_back("ldl -1");
    temp.push_back("adj -10000");
}
void implement_set_instruction() {
    vector<string> temp;
    for (int i = 0; i < static_cast<int>(cleaned_code.size()); i++) {
        const string& line = cleaned_code[i];
        int colon_position = line.find(':');

        if (colon_position != string::npos && line.size() > colon_position + 5 && line.substr(colon_position + 2, 3) == "SET") {
            string label = line.substr(0, colon_position);
            bool is_existing_label = abs(label_map[label]) == i;

            if (is_existing_label) {
                label_map[label] = static_cast<int>(temp.size());
                temp.push_back(line.substr(0, colon_position + 1) + " data " + line.substr(colon_position + 6));
            } else {
                push_set_instructions(temp, label, line, colon_position);
            }
        } else if (!line.empty()) {
            temp.push_back(line);
        }
    }
    cleaned_code = move(temp);
}


void process_labels() {
    for (int i = 0; i < (int) cleaned_code.size(); i++) {
        string current_label;
        bool label_found = false;

        for (int j = 0; j < (int) cleaned_code[i].size(); j++) {
            if (cleaned_code[i][j] == ':') {
                label_found = true;

                // Validate label
                if (!is_valid_label(current_label)) {
                    function_to_generate_error(i + 1, "Invalid label name");
                    break;
                }

                // Check for multiple declarations or specific conditions
                bool is_set_statement = (cleaned_code[i].size() > j + 4 && cleaned_code[i].substr(j + 2, 3) == "SET");
                bool is_data_statement = (cleaned_code[i].size() > j + 5 && cleaned_code[i].substr(j + 2, 4) == "data");

                if (label_map.count(current_label)) {
                    if (is_set_statement) continue;
                    if (is_data_statement && label_map[current_label] < 0) {
                        label_map[current_label] = i;
                        continue;
                    }
                    function_to_generate_error(i + 1, "Multiple declaration of label: " + current_label);
                } else {
                    label_map[current_label] = is_set_statement ? -i : i;
                }
                break; // Exit after processing the label
            }

            // Accumulate characters for the label until the colon
            if (!label_found) current_label = current_label + cleaned_code[i][j];
        }
    }
}

void fill_asm_data(int i, string label, string mnemonic, string operand, int type) {
    asm_data[i].label = label;
    asm_data[i].mnemonic = mnemonic;
    asm_data[i].operand = operand;
    asm_data[i].operand_type = type;
}

int get_operand_type(string s) {
    if (s.empty()) return 0;

    // Skip leading sign
    if (s[0] == '+' || s[0] == '-') s = s.substr(1);

    // Return type based on operand format
    if (s.empty()) return -1;
    if (is_decimal(s)) return 10;
    if (is_octal(s)) return 8;
    if (is_hexadecimal(s)) return 16;
    return is_valid_label(s) ? 1 : -1;
}

// Helper function to split line by label and spaces, similar to original logic
void split_line(const string &line, string &label, string &mnemonic, string &operand) {
    label = mnemonic = operand = "";
    int part_index = 0;
    string current_part;
    bool label_detected = false;

    for (int j = 0; j < line.size(); j++) {
        char ch = line[j];
        
        if (ch == ':' && !label_detected) { // Label end detected
            label = current_part;
            current_part.clear();
            label_detected = true;
            continue;
        } else if (ch == ' ') { // Space separates parts
            if (!current_part.empty()) {
                if (part_index == 0) mnemonic = current_part;
                else if (part_index == 1) operand = current_part;
                current_part.clear();
                part_index++;
            }
        } else {
            current_part = current_part + ch;
        }
    }
    if (!current_part.empty()) {
        if (part_index == 0) mnemonic = current_part;
        else if (part_index == 1) operand = current_part;
    }
}

// Main function to parse cleaned_code and create asm_data
void function_to_create_asm_table() {
    int program_counter = 0;
    for (int i = 0; i < (int)cleaned_code.size(); i++) {
        string label, mnemonic, operand;
        split_line(cleaned_code[i], label, mnemonic, operand);

        // Assign label presence
        asm_data[i].label_present = !label.empty();

        // Add label to label_map
        if (!label.empty()) {
            label_map[label] = program_counter;
        }

        // Handle HALT instruction
        if (mnemonic == "HALT") {
            is_halt_present = true;
        }

        // Set program counter if there’s no mnemonic
        if (mnemonic.empty()) {
            fill_asm_data(i, label, "", "", 0);
            continue;
        }

        // Validate mnemonic
        if (!opcode_map.count(mnemonic)) {
            function_to_generate_error(i + 1, "Invalid Mnemonic");
            continue;
        }

        // Validate syntax
        if (min(opcode_map[mnemonic].second, 1) != (operand.empty() ? 0 : 1)) {
            function_to_generate_error(i + 1, "Invalid OPCode-Syntax combination");
            continue;
        }

        // Determine operand type and fill asm_data
        int operand_type = get_operand_type(operand);
        fill_asm_data(i, label, mnemonic, operand, operand_type);

        // Handle errors for label or invalid operand
        if (operand_type == 1 && !label_map.count(operand)) {
            function_to_generate_error(i + 1, "No such label / data variable");
        } else if (operand_type == -1) {
            function_to_generate_error(i + 1, "Invalid number");
        }

        // Assign program counter and increment
        program_counters[i] = program_counter;
        program_counter++;
    }
}


void function_to_separate_data_segment() {
    vector<string> instructions, data_segment;
    bool in_data_section = false;

    for (const string& line : cleaned_code) {
        // Detect start of data segment by presence of "data" keyword in line
        if (line.find("data") != string::npos) {
            in_data_section = true;
            data_segment.push_back(line);
        }
        // Check for label definition that precedes a data declaration
        else if (!line.empty() && line.back() == ':' && in_data_section) {
            data_segment.push_back(line);
        }
        // Otherwise, categorize as instruction
        else {
            in_data_section = false;
            instructions.push_back(line);
        }
    }

    // Append the data segment lines after the instructions
    instructions.insert(instructions.end(), data_segment.begin(), data_segment.end());

    // Update cleaned_code with the reordered lines
    cleaned_code = instructions;
}

void first_pass() {
    cout << "Enter ASM file name to assemble:" << endl;
    cin >> asm_file_name;

    ifstream infile(asm_file_name);
    if (!infile) {
        cerr << "Input file doesn't exist. Ensure the file is in the same directory as the code!" << endl;
        exit(EXIT_FAILURE);
    }

    // Read and clean each line, adding it to `cleaned_code`
    string line;
    while (getline(infile, line)) {
        string cleaned_line = clean_line(line, static_cast<int>(cleaned_code.size()));
        cleaned_code.push_back(cleaned_line);
    }

    infile.close();  // Explicitly close the file

    // Initialize and process assembler-specific components
    initialize_opcodes();
    process_labels();

    // Only implement `SET` instructions if there are no errors
    if (error_list.empty()) {
        implement_set_instruction();
    }

    // Prepare data structures for further assembly steps
    asm_data.resize(static_cast<int>(cleaned_code.size()));
    program_counters.resize(static_cast<int>(cleaned_code.size()));

    // Additional assembler functions to finalize setup
    function_to_separate_data_segment();
    function_to_create_asm_table();
}

bool open_log_file(ofstream& error_file) {
    const string log_filename = "logFile.log";
    error_file.open(log_filename);
    if (!error_file) {
        cerr << "Error: Could not open log file for writing." << endl;
        return false;
    }
    return true;
}

void write_success_log(ofstream& error_file) {
    error_file << "++++++ A S S E M B L Y   L O G ++++++" << endl;
    cout << "YAYY!! The assembly was successfully completed. No errors found!! Well Done!" << endl;
    
    if (!is_halt_present) {
    cout << "Warning: It appears that the program lacks a HALT instruction, which is essential for proper termination of the program. Please verify if the absence of this instruction is intentional or an oversight." << endl;
    error_file << "[WARNING] The program does not contain a HALT instruction, which may result in the program running indefinitely or terminating unexpectedly. Please ensure that this instruction is included in the program for proper execution." << endl;
}

error_file << "The following output files have been successfully generated as part of the assembly process. These files are crucial for further analysis and execution of the compiled program:" << endl;
error_file << " - The machine code file: 'machineCode.o' contains the binary instructions that the computer will execute." << endl;
error_file << " - The listing code file: 'listCode.l' provides a detailed listing of the assembly program, including the original source code along with the corresponding machine code." << endl;

}

void write_error_log(ofstream& error_file) {
    error_file << "++++++ A S S E M B L Y   L O G ++++++" << endl;
    error_file << "+*+*+* E R R O R S +*+*+*" << endl;
    
    sort(error_list.begin(), error_list.end());
    cout << "Assembly encountered " << error_list.size() << " error(s). Check log file for details." << endl;
    
    for (const auto& error : error_list) {
        error_file << error.second << endl;
    }
}

bool function_to_display_errors() {
    ofstream error_file;
    if (!open_log_file(error_file)) {
        return false;
    }

    if (error_list.empty()) {
        write_success_log(error_file);
    } else {
        write_error_log(error_file);
    }

    error_file.close();
    return true;
}

string pad_zeroes(string s, int length) {
    if (s.size() < length) {
        s.insert(s.begin(), length - s.size(), '0');
    }
    return s;
}


// Helper function for generating machine code when operand type is 1
string function_to_generate_relative_machine_code(int i) {
    int decimal_value = (opcode_map[asm_data[i].mnemonic].second == 2)
                            ? label_map[asm_data[i].operand] - (program_counters[i] + 1)
                            : label_map[asm_data[i].operand];
    return pad_zeroes(decimal_to_hexadecimal_conversion(decimal_value)) + opcode_map[asm_data[i].mnemonic].first;
}

// Helper function for generating machine code when operand type is 0 (no operand)
string generate_no_operand_machine_code(int i) {
    return "000000" + opcode_map[asm_data[i].mnemonic].first;
}

// Helper function for generating machine code for "data" or constant operands
string generate_data_or_constant_machine_code(int i) {
    int hex_length = (asm_data[i].mnemonic == "data") ? 8 : 6;
    int bit_length = (asm_data[i].mnemonic == "data") ? 32 : 24;
    int decimal_value = stoi(asm_data[i].operand, nullptr, asm_data[i].operand_type);
    return pad_zeroes(decimal_to_hexadecimal_conversion(decimal_value, bit_length), hex_length) + opcode_map[asm_data[i].mnemonic].first;
}

// The main function, `second_pass`, which now uses helper functions for clarity
void second_pass() {
    for (int i = 0; i < asm_data.size(); i++) {
        if (cleaned_code[i].empty()) {
            continue;  // Skip empty lines
        }

        string machine_code;

        // Check if the mnemonic is empty and handle accordingly
        if (asm_data[i].mnemonic.empty()) {
            machine_code = "        ";
        } else {
            // Generate machine code based on operand type
            if (asm_data[i].operand_type == 1) {
                machine_code = function_to_generate_relative_machine_code(i);
            } else if (asm_data[i].operand_type == 0) {
                machine_code = generate_no_operand_machine_code(i);
            } else {
                machine_code = generate_data_or_constant_machine_code(i);
            }
        }

        // Append the generated machine code to the list
        machine_code_list.emplace_back(i, machine_code);
    }
}


// Helper function to write the listing code file
void write_listing_file(const string& filename) {
    ofstream listing_file(filename);
    if (!listing_file) {
        cerr << "Error: Could not open " << filename << " for writing." << endl;
        return;
    }

    for (const auto& code : machine_code_list) {
        string address = pad_zeroes(decimal_to_hexadecimal_conversion(program_counters[code.first]));
        listing_file << address << " " << code.second << " " << cleaned_code[code.first] << endl;
    }
    listing_file.close();
}

// Helper function to write the machine code binary file
void write_machine_code_file(const string& filename) {
    ofstream machine_file(filename, ios::binary | ios::out);
    if (!machine_file) {
        cerr << "Error: Could not open " << filename << " for writing." << endl;
        return;
    }

    for (const auto& code : machine_code_list) {
        if (code.second.empty() || code.second == "        ") {
            continue;  // Skip empty or placeholder codes
        }

        unsigned int code_value;
        stringstream ss(code.second);
        ss >> hex >> code_value;
        
        if (ss.fail()) {
            cerr << "Error: Failed to convert machine code to hexadecimal for line " << code.first << "." << endl;
            continue;
        }

        machine_file.write(reinterpret_cast<const char*>(&code_value), sizeof(unsigned int));
    }
    machine_file.close();
}

// Main function to write output files
void write_output_files() {
    const string listing_filename = "listCode.l";
    const string machine_filename = "machineCode.o";

    write_listing_file(listing_filename);
    write_machine_code_file(machine_filename);

    cout << "Log code of the oeprations has been generated in: logFile.log" << endl;
    cout << "Machine code has been generated in: " << machine_filename << endl;
    cout << "Listing code has been generated in: " << listing_filename << endl;
}

bool is_octal(string s) {
    // Check if string is non-empty and starts with '0'
    if (s.size() < 2 || s[0] != '0') return false;
    
    // Ensure all characters are between '0' and '7'
    for (int i = 1; i < s.size(); i++) {
        if (s[i] < '0' || s[i] > '7') return false;
    }
    return true;
}

bool is_hexadecimal(string s) {
    // Check if the string has "0x" or "0X" prefix and enough length
    if (s.size() < 3 || s[0] != '0' || (s[1] != 'x' && s[1] != 'X')) return false;
    
    // Ensure all characters after "0x" are valid hex digits
    for (int i = 2; i < s.size(); i++) {
        if (!((s[i] >= '0' && s[i] <= '9') || 
              (s[i] >= 'a' && s[i] <= 'f') || 
              (s[i] >= 'A' && s[i] <= 'F'))) {
            return false;
        }
    }
    return true;
}

bool is_decimal(string s) {
    // Ensure all characters are digits
    if (s.empty()) return false;
    for (char ch : s) {
        if (!isdigit(ch)) return false;
    }
    return true;
}

bool is_valid_label(string label) {
    // Check that label is non-empty and starts with a letter or underscore
    if (label.empty() || !(isalpha(label[0]) || label[0] == '_')) return false;
    
    // Ensure all characters are alphanumeric or underscore
    for (char ch : label) {
        if (!(isalnum(ch) || ch == '_')) return false;
    }
    return true;
}
