#include <iostream>
#include <sstream>
#include <bitset>
#include <map>
#include <vector>
#include <cstdlib>
#include <string>
#include <fstream>
#include <algorithm>

//1. the 1st step, a Assembler can transfer assembly code to binary machine code, and vice versa.
/*
only support the following instructions: 
add, sub, addi, mul, lw, sw, beq, lui,
and, andi, or, ori, sll, srl, slti, and sltiu
*/

std::string toLower(std::string str)
{
    std::string lowerCase;
    for (auto ch : str)
    {
        if (ch >= 'A' && ch <= 'Z')
        {
            ch += 32;
        }
    }
    return lowerCase;
}

std::vector<std::string> splitInstruction(std::string str)
{
    std::vector<std::string> result;
    std::string temp = "";
    // ignore the commments
    for (size_t i = 0; i < str.size() && str[i] != '/'; i++)
    {
        if (str[i] == ' ' || str[i] == ',')
        {
            if (temp != "")
            {
                result.push_back(temp);
                temp = "";
            }
        }
        else
        {
            temp += str[i];
        }
    }
    if (temp != "")
        result.push_back(temp);
    return result;
}

std::map<std::string, std::string> Register32 = {
    {"$zero", "00000"},
    {"$at", "00001"},
    {"$v0", "00010"},
    {"$v1", "00011"},
    {"$a0", "00100"},
    {"$a1", "00101"},
    {"$a2", "00110"},
    {"$a3", "00111"},
    {"$t0", "01000"},
    {"$t1", "01001"},
    {"$t2", "01010"},
    {"$t3", "01011"},
    {"$t4", "01100"},
    {"$t5", "01101"},
    {"$t6", "01110"},
    {"$t7", "01111"},
    {"$s0", "10000"},
    {"$s1", "10001"},
    {"$s2", "10010"},
    {"$s3", "10011"},
    {"$s4", "10100"},
    {"$s5", "10101"},
    {"$s6", "10110"},
    {"$s7", "10111"},
    {"$t8", "11000"},
    {"$t9", "11001"},
    {"$k0", "11010"},
    {"$k1", "11011"},
    {"$gp", "11100"},
    {"$sp", "11101"},
    {"$fp", "11110"},
    {"$ra", "11111"}};

// transfer assembly insrtuction to binary code
std::string instructionToBinaryCode(std::vector<std::string> instruction)
{
    /*
    Syntax: ArithLog  
    Encoding: Register  
    f $d, $s, $t
    
            31..26  25..21  20..16  15..11  10..6   5..0
            op      rs      rt      rd      shamt   func
    add :   000000  rs      rt      rd      00000   100000
    sub :   000000  rs      rt      rd      00000   100010 
    and :   000000  rs      rt      rd      00000   100100
    or  :   000000  rs      rt      rd      00000   100101
    mul(t): 000000  rs      rt      rd      00000   011000

    // mul(mult)
    // mul $a,$b,$c, 
    // meaning that we multiply the contents of $b and $c, the least significant
    // 32 bits of results are placed in register $a and the most significant 32-bits of the result will be
    // stored in register $(a+1).

    */

    // ArithLog's name to func
    // add, sub, and, or, mul(mult)
    std::map<std::string, std::string> ArithLog = {
        {"add", "100000"},
        {"sub", "100010"},
        {"and", "100100"},
        {"or", "100101"},
        {"mul", "011000"}};

    if (ArithLog.find(instruction[0]) != ArithLog.end())
    {
        std::string op, rs, rt, rd, shamt, func;
        op = "000000";

        rs = Register32[instruction[2]];
        rt = Register32[instruction[3]];
        rd = Register32[instruction[1]];
        shamt = "00000";
        func = ArithLog[instruction[0]];

        return op + rs + rt + rd + shamt + func;
    }

    /*
    Syntax: Shift  
    Encoding: Register  
    f $d, $t, a

            31..26  25..21  20..16  15..11  10..6   5..0
            op      rs      rt      rd      shamt   func
    sll :   000000  00000   rt      rd      shamt   000000
    srl :   000000  00000   rt      rd      shamt   000010 
    */

    // Shift's name to func
    // sll, srl
    std::map<std::string, std::string> Shift = {
        {"sll", "000000"},
        {"srl", "000010"}};

    if (Shift.find(instruction[0]) != Shift.end())
    {
        std::string op, rs, rt, rd, shamt, func;
        op = "000000";
        rs = "00000";
        rt = Register32[instruction[2]];
        rd = Register32[instruction[1]];

        // integer string to binary string
        // integer string -> int -> binary
        std::bitset<5> temp(std::stoi(instruction[3]));
        // binary -> binary string
        shamt = temp.to_string();
        func = Shift[instruction[0]];

        return op + rs + rt + rd + shamt + func;
    }

    /*
    Syntax: ArithLogI  
    Encoding: Immediate  
    o $t, $s, i

            31..26  25..21  20..16  15..0
            op      rs      rt      immediate
    addi:   001000  rs      rt      immediate
    andi:   001100  rs      rt      immediate
    ori :   001101  rs      rt      immediate
    slti:   001010  rs      rt      immediate
    sltiu:  001011  rs      rt      immediate
    */
    // ArithLogI's name to op
    // addi, andi, ori, slti, sltiu
    std::map<std::string, std::string> ArithLogI = {
        {"addi", "001000"},
        {"andi", "001100"},
        {"ori", "001101"},
        {"slti", "001010"},
        {"sltiu", "001011"}};

    if (ArithLogI.find(instruction[0]) != ArithLogI.end())
    {
        std::string op, rs, rt, immediate;
        op = ArithLogI[instruction[0]];
        rs = Register32[instruction[2]];
        rt = Register32[instruction[1]];

        std::stringstream ss;
        // 16bit
        ss << std::hex << instruction[3];
        int n;
        ss >> n;
        std::bitset<16> temp(n);
        immediate = temp.to_string();

        return op + rs + rt + immediate;
    }

    /*
    Syntax: LoadStore  
    Encoding: Immediate  
    op $t, i, $s
    (lui $t, i)

            31..26  25..21  20..16  15..0
            op      rs      rt      immediate
    lw  :   100011  rs      rt      immediate
    sw  :   101011  rs      rt      immediate
    lui :   001111  00000   rt      immediate    
    */

    // LoadStore's name to op
    // addi, andi, ori, slti, sltiu
    std::map<std::string, std::string> LoadStore = {
        {"lw", "100011"},
        {"sw", "101011"},
        {"lui", "001111"}};

    if (LoadStore.find(instruction[0]) != LoadStore.end())
    {
        std::string op, rs, rt, immediate;
        op = LoadStore[instruction[0]];
        if (instruction[0] == "lui")
        {
            rs = "00000";
        }
        else
        {
            rs = Register32[instruction[3]];
        }

        rt = Register32[instruction[1]];

        std::stringstream ss;
        // 16bit
        ss << std::hex << instruction[2];
        int n;
        ss >> n;
        std::bitset<16> temp(n);
        immediate = temp.to_string();

        return op + rs + rt + immediate;
    }

    /*
    Syntax: Branch  
    Encoding: Immediate  
    o $s, $t, label

            31..26  25..21  20..16  15..0
            op      rs      rt      immediate
    beq :   000100  rs      rt      immediate
    */
    if (instruction[0] == "beq")
    {
        std::string op, rs, rt, immediate;
        op = "000100";
        rs = Register32[instruction[1]];
        rt = Register32[instruction[2]];

        std::stringstream ss;
        // 16bit
        ss << std::hex << instruction[3];
        int n;
        ss >> n;
        std::bitset<16> temp(n);
        immediate = temp.to_string();

        return op + rs + rt + immediate;
    }

    // default, return error
    return "instructionToBinaryCodeError";
}

// get MIPS assembly instruction from fMIPSInstruction,
// convert to binary code,
// save in fBinaryCode
void getInputReady(std::string fMIPSInstruction, std::string fBinaryCode)
{
    std::ifstream ifs;
    ifs.open(fMIPSInstruction, std::ifstream::in);
    std::ofstream ofs(fBinaryCode);
    std::string line;

    while (getline(ifs, line))
    {
        // if it is not an empty line ,
        // then format the input and convert it to binary code
        if (line != "")
        {
            std::string str;
            // format, transfer to lowercase
            std::transform(line.begin(), line.end(), back_inserter(str), ::tolower);
            // split the insturction, and ignore the comments
            std::vector<std::string> instruction = splitInstruction(str);

            // for (auto str : instruction)
            // {
            //     ofs << str << " ";
            // }
            // ofs << std::endl;

            std::string BinaryCode = instructionToBinaryCode(instruction);
            std::bitset<32> code(BinaryCode);
            ofs << std::hex << code.to_ulong() << std::endl;
            //ofs << BinaryCode << std::endl;
        }
    }

    ifs.close();
}

int main()
{
    getInputReady("MIPSInstruction.txt", "BinaryCode.txt");

    return 0;
}
