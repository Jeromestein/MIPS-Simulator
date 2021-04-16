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

std::string Register32Name[32] = {
    "$zero", "$at", "$v0", "$v1",
    "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3",
    "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3",
    "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9", "$k0", "$k1",
    "$gp", "$sp", "$fp", "$ra"};

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
        // hex, 2's complement!!!!
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
bool getBCode(std::string fMIPSInstruction, std::string fBinaryCode)
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
            // output hex code in .txt
            //ofs << std::hex << code.to_ulong() << std::endl;
            // output binary code in .txt
            ofs << BinaryCode << std::endl;
        }
    }

    ifs.close();

    return true;
}

class latchReg
{
    //private:
public:
    // 32bits
    std::bitset<32> IR, ALUOutput, LMD; // load memory data
    std::bitset<32> A, B, Imm;
    bool cond;
    unsigned long NPC;

public:
    unsigned long PC;
};

latchReg IF_ID, ID_EX, EX_MEM, MEM_WB;
// 2KB, 2048*8bits
std::bitset<8> IMem[2048], DMem[2048];
std::bitset<32> Regs[32], hi, lo;
unsigned long PC;
int instruction_cnt;

// combine 4 bytes, 4*8bits to 1 word, 32bits
std::bitset<32> byte2word(std::bitset<8> array_byte[], long address)
{
    long value = 0;
    for (size_t i = 0; i < 4; i++)
    {
        // value<<8, 2^8=256
        value *= 256;
        value += array_byte[address + i].to_ulong();
    }
    std::bitset<32> word(value);
    return word;
}

// split 1 word to 4 bytes
// i=0,1,2,3 means ith bytes of this word
std::bitset<8> word2byte(std::bitset<32> word, int i)
{
    std::string str_byte = word.to_string().substr(i * 8, 8);
    std::bitset<8> byte(str_byte);
    return byte;
}

// from 2's complement binary code to decimal
long _2sComplement(std::bitset<32> binary)
{
    long decimal;
    if (binary[31] == 1)
    {
        // negtive
        decimal = binary.flip().to_ulong() + 1;
        decimal *= -1;
    }
    else
    {
        decimal = binary.to_ulong();
    }
    return decimal;
}

unsigned long long clk_cnt, i_cnt, IF_cnt, ID_cnt, EX_cnt, MEM_cnt, WB_cnt;
bool isBranch, waitBranch;
int IF_stage()
{
    IF_ID.IR = byte2word(IMem, PC);

    //if ((EX_MEM.op == "branch") && EX_MEM.cond)
    {
        IF_ID.NPC = EX_MEM.ALUOutput.to_ulong();
        IF_ID.PC = EX_MEM.ALUOutput.to_ulong();
    }
    //else
    {
        IF_ID.NPC = PC + 4;
        IF_ID.PC = PC + 4;
    }
    IF_cnt++;
    return 0;
}

int ID_stage()
{
    ID_EX.NPC = IF_ID.NPC;
    ID_EX.IR = IF_ID.IR;

    std::bitset<5> rs(IF_ID.IR.to_string(), 25, 5);
    ID_EX.A = Regs[rs.to_ulong()];

    std::bitset<5> rt(IF_ID.IR.to_string(), 20, 5);
    ID_EX.B = Regs[rt.to_ulong()];

    std::bitset<16> imm(IF_ID.IR.to_string(), 15, 16);
    // 2's complement
    ID_EX.Imm = imm.to_ulong();

    /*data hazard detect*/
    // Lock the destination register until WB stage.
    // (different instruction has different destination register, rt, rs or rd)
    // check if the target or source registers are locked,
    // if true, wait until it is unlocked.

    /*control hazard detect*/
    // if the instruction is branch, in this case, beq op : 000100
    // then set isBranch as true
    std::string op = ID_EX.IR.to_string().substr(0, 6);
    if (op == "000100")
    {
        isBranch = true;
        waitBranch = true;
    }

    ID_cnt++;

    return 0;
}

int EX_stage()
{
    EX_MEM.IR = ID_EX.IR;
    std::string op = EX_MEM.IR.to_string().substr(0, 6);
    // ALU instruction

    // EX_MEM.IR = ID_EX.IR;
    // EX_MEM.ALUOutput = ID_EX.A func ID_EX.B;
    // // or
    // EX_MEM.ALUOutput = ID_EX.A op ID_EX.Imm;

    if (op == "000000")
    {
        /*
                31..26  25..21  20..16  15..11  10..6   5..0
                op      rs      rt      rd      shamt   func
        add :   000000  rs      rt      rd      00000   100000
        sub :   000000  rs      rt      rd      00000   100010 
        and :   000000  rs      rt      rd      00000   100100
        or  :   000000  rs      rt      rd      00000   100101

        mul(t): 000000  rs      rt      rd      00000   011000

        sll :   000000  00000   rt      rd      shamt   000000
        srl :   000000  00000   rt      rd      shamt   000010 
        */
        std::string func = EX_MEM.IR.to_string().substr(26, 6);
        if (func == "100000")
        {
            // add
            EX_MEM.ALUOutput = ID_EX.A.to_ulong() + ID_EX.B.to_ulong();
        }
        if (func == "100010")
        {
            // sub
            EX_MEM.ALUOutput = ID_EX.A.to_ulong() - ID_EX.B.to_ulong();
        }
        if (func == "100100")
        {
            // and
            EX_MEM.ALUOutput = ID_EX.A.to_ulong() & ID_EX.B.to_ulong();
        }
        if (func == "100101")
        {
            // or
            EX_MEM.ALUOutput = ID_EX.A.to_ulong() | ID_EX.B.to_ulong();
        }
        if (func == "011000")
        {
            // mul(t)
            std::bitset<64> x(ID_EX.A.to_ullong() * ID_EX.B.to_ullong());
            hi = x.to_ullong() >> 32;
            lo = x.to_ullong() % 4294967296; //2^32
        }
        if (func == "000000")
        {
            // sll
            std::bitset<5> shamt(EX_MEM.IR.to_string(), 10, 5);
            EX_MEM.ALUOutput = ID_EX.B.to_ulong() << shamt.to_ulong();
        }
        if (func == "000010")
        {
            // srl
            std::bitset<5> shamt(EX_MEM.IR.to_string(), 10, 5);
            EX_MEM.ALUOutput = ID_EX.B.to_ulong() >> shamt.to_ulong();
        }
    }
    /*
            31..26  25..21  20..16  15..0
            op      rs      rt      immediate
    addi:   001000  rs      rt      immediate
    andi:   001100  rs      rt      immediate
    ori :   001101  rs      rt      immediate
    slti:   001010  rs      rt      immediate
    sltiu:  001011  rs      rt      immediate
    */
    if (op == "001000")
    {
        // addi
        // imm is a signed 16-bit int, 2's complement
        EX_MEM.ALUOutput = ID_EX.A.to_ulong() + _2sComplement(ID_EX.Imm);
    }
    if (op == "001100")
    {
        // andi
        EX_MEM.ALUOutput = ID_EX.A.to_ulong() & _2sComplement(ID_EX.Imm);
    }
    if (op == "001101")
    {
        // ori
        EX_MEM.ALUOutput = ID_EX.A.to_ulong() | _2sComplement(ID_EX.Imm);
    }
    if (op == "001010")
    {
        // slti
        EX_MEM.ALUOutput = ID_EX.A.to_ulong() < _2sComplement(ID_EX.Imm);
    }
    if (op == "001011")
    {
        // sltiu
        // unsigned long
        EX_MEM.ALUOutput = ID_EX.A.to_ulong() < ID_EX.Imm.to_ulong();
    }

    /*
                31..26  25..21  20..16  15..0
            op      rs      rt      immediate
    lw  :   100011  rs      rt      immediate
    sw  :   101011  rs      rt      immediate
    lui :   001111  00000   rt      immediate   
    */
    /*
                31..26  25..21  20..16  15..0
            op      rs      rt      immediate
    beq :   000100  rs      rt      immediate
    */
    // //Load or store instruction
    // EX_MEM.IR = ID_EX.IR;
    // EX_MEM.ALUOutput = ID_EX.A + ID_EX.Imm;
    // EX_MEM.B = ID_EX.B; //(for store)
    if (op == "100011")
    {
        // lw
        EX_MEM.ALUOutput = ID_EX.A.to_ulong() + _2sComplement(ID_EX.Imm);
        EX_MEM.B = ID_EX.B;
    }
    if (op == "101011")
    {
        // sw
        EX_MEM.ALUOutput = ID_EX.A.to_ulong() + _2sComplement(ID_EX.Imm);
        EX_MEM.B = ID_EX.B;
    }
    if (op == "001111")
    {
        // lui
        EX_MEM.ALUOutput = ID_EX.Imm << 16;
        EX_MEM.B = ID_EX.B;
    }
    // //Branch instruction
    // EX_MEM.ALUOutput = ID_EX.NPC + (ID_EX.Imm << 2);
    // EX_MEM.cond = (ID_EX.A == 0);
    if (op == "000100")
    {
        // beq
        // if rs==rt, then pc=pc+imm<<2
        EX_MEM.ALUOutput = ID_EX.NPC + (_2sComplement(ID_EX.Imm) << 2);
        EX_MEM.cond = (ID_EX.A == ID_EX.B);
    }
    EX_cnt++;
    return 0;
}
int MEM_stage()
{
    MEM_WB.IR = EX_MEM.IR;
    std::string op = MEM_WB.IR.to_string().substr(0, 6);
    //         31..26  25..21  20..16  15..0
    //         op      rs      rt      immediate
    // addi:   001000  rs      rt      immediate
    // andi:   001100  rs      rt      immediate
    // ori :   001101  rs      rt      immediate
    // slti:   001010  rs      rt      immediate
    // sltiu:  001011  rs      rt      immediate

    // ALU instruction
    // MEM_WB.IR = EX_MEM.IR;
    // MEM_WB.ALUOutput = EX_MEM.ALUOutput;
    if (op == "000000" ||
        op == "001000" ||
        op == "001100" ||
        op == "001101" ||
        op == "001010" ||
        op == "001011")
    {
        MEM_WB.ALUOutput = EX_MEM.ALUOutput;
    }

    /*
                31..26  25..21  20..16  15..0
            op      rs      rt      immediate
    lw  :   100011  rs      rt      immediate
    sw  :   101011  rs      rt      immediate
    lui :   001111  00000   rt      immediate   
    */
    // //Load or store instruction
    // MEM_WB.IR = EX_MEM.IR;
    // MEM_WB.LMD = DMem[EX_MEM.ALUOutput];
    // //    or
    // DMem[EX_MEM.ALUOutput] = EX_MEM.B;
    if (op == "100011" ||
        op == "101011" ||
        op == "001111")
    {
        // lw or lui
        if (op == "100011" || op == "001111")
        {
            MEM_WB.LMD = byte2word(DMem, EX_MEM.ALUOutput.to_ulong());
        }
        // sw
        if (op == "101011")
        {
            // EX_MEM.ALUOutput is the address of first byte
            // big endian
            unsigned long address = EX_MEM.ALUOutput.to_ulong();
            DMem[address + 0] = word2byte(EX_MEM.B, 0);
            DMem[address + 1] = word2byte(EX_MEM.B, 1);
            DMem[address + 2] = word2byte(EX_MEM.B, 2);
            DMem[address + 3] = word2byte(EX_MEM.B, 3);
        }
    }
    MEM_cnt++;

    return 0;
}
int WB_stage()
{
    MEM_WB.IR = EX_MEM.IR;
    std::string op = MEM_WB.IR.to_string().substr(0, 6);
    // ALU instruction
    // Regs[MEM_WB.IR[rd]] = MEM_WB.ALUOutput;
    //    or
    // Regs[MEM_WB.IR[rt]] = MEM_WB.ALUOutput;

    /*
            31..26  25..21  20..16  15..11  10..6   5..0
            op      rs      rt      rd      shamt   func
    add :   000000  rs      rt      rd      00000   100000
    sub :   000000  rs      rt      rd      00000   100010 
    and :   000000  rs      rt      rd      00000   100100
    or  :   000000  rs      rt      rd      00000   100101

    mul(t): 000000  rs      rt      rd      00000   011000

    sll :   000000  00000   rt      rd      shamt   000000
    srl :   000000  00000   rt      rd      shamt   000010 
    */
    // Regs[MEM_WB.IR[rd]] = MEM_WB.ALUOutput;
    if (op == "000000")
    {
        // add, sub, and, or, sll, srl
        std::string func = MEM_WB.IR.to_string().substr(26, 6);
        if (func == "100000" ||
            func == "100010" ||
            func == "100100" ||
            func == "100101" ||
            func == "000000" ||
            func == "000010")
        {
            std::bitset<5> rd(MEM_WB.IR.to_string(), 15, 5);
            Regs[rd.to_ulong()] = MEM_WB.ALUOutput;
        }

        // mul(t)
        if (func == "011000")
        {

            // Assume the syntax for mul is mul $a,$b,$c,
            // meaning that we multiply the contents of $b and $c,
            // the least significant 32 bits of results are placed in register $a
            // and the most significant 32-bits of the result will be stored in register $(a+1)
            std::bitset<5> rd(MEM_WB.IR.to_string(), 15, 5);
            Regs[rd.to_ulong()] = lo;
            Regs[rd.to_ulong() + 1] = hi;
        }
    }
    /*
            31..26  25..21  20..16  15..0
            op      rs      rt      immediate
    addi:   001000  rs      rt      immediate
    andi:   001100  rs      rt      immediate
    ori :   001101  rs      rt      immediate
    slti:   001010  rs      rt      immediate
    sltiu:  001011  rs      rt      immediate
    */
    // Regs[MEM_WB.IR[rt]] = MEM_WB.ALUOutput;
    // addi, andi, ori, slti, sltiu
    if (op == "001000" ||
        op == "001100" ||
        op == "001101" ||
        op == "001010" ||
        op == "001011")
    {
        std::bitset<5> rt(IF_ID.IR.to_string(), 20, 5);
        Regs[rt.to_ulong()] = MEM_WB.ALUOutput;
    }

    // Load or store instruction
    // //For load only:
    // Regs[MEM_WB.IR[rt]] = MEM_WB.LMD;
    /*
                31..26  25..21  20..16  15..0
            op      rs      rt      immediate
    lw  :   100011  rs      rt      immediate
    sw  :   101011  rs      rt      immediate
    lui :   001111  00000   rt      immediate   
    */
    // lui
    if (op == "001111")
    {
        std::bitset<5> rt(IF_ID.IR.to_string(), 20, 5);
        Regs[rt.to_ulong()] = MEM_WB.LMD;
    }

    WB_cnt++;
    // complete one instruction
    i_cnt++;
    return 0;
}

// Store the BCode from fBinaryCode to IMem
bool getInstruction(std::string fBinaryCode, std::bitset<8> IMem[])
{
    std::ifstream ifs;
    ifs.open(fBinaryCode, std::ifstream::in);
    std::string line;
    instruction_cnt = 0;

    while (getline(ifs, line))
    {
        // if it is not an empty line ,
        // then store input to IMem
        if (line != "")
        {
            std::bitset<32> word(line);
            IMem[instruction_cnt * 4 + 0] = word2byte(word, 0);
            IMem[instruction_cnt * 4 + 1] = word2byte(word, 1);
            IMem[instruction_cnt * 4 + 2] = word2byte(word, 2);
            IMem[instruction_cnt * 4 + 3] = word2byte(word, 3);

            instruction_cnt++;
        }
    }
    ifs.close();

    return true;
}

void printAllInstructions(std::bitset<8> IMem[])
{
    for (int i = 0; i < instruction_cnt; i++)
    {
        std::cout << i << "\t";
        std::cout << IMem[instruction_cnt * 4 + 0]
                  << IMem[instruction_cnt * 4 + 1]
                  << IMem[instruction_cnt * 4 + 2]
                  << IMem[instruction_cnt * 4 + 3];

        if (i == PC)
        {
            std::cout << "  <<";
        }
        std::cout << std::endl;
    }
}

void printAllRegisters(std::bitset<32> Regs[])
{
    for (int i = 0; i < 32; i++)
    {
        std::cout << Register32Name[i] << ":\t" << Regs[i] << std::endl;
    }
}

int main()
{
    // like an assembler?
    if (!getBCode("MIPSInstruction.txt", "BinaryCode.txt"))
    {
        std::cout << "getBCode Wrong!" << std::endl;
    }
    if (!getInstruction("BinaryCode.txt", IMem))
    {
        std::cout << "getInstruction Wrong!" << std::endl;
    }

    // initialization
    PC = 0;
    clk_cnt = i_cnt = IF_cnt = ID_cnt = EX_cnt = MEM_cnt = WB_cnt = 0;

    while (true)
    {
        std::map<std::string, int> cmd = {
            {"exit", -1},
            {"ins ls all", 1},
            {"reg stat", 2},

        };
        std::string strcmd;

        std::cout << "cmd:" << std::endl;
        //std::cin >> strcmd;
        getline(std::cin, strcmd);

        switch (cmd[strcmd])
        {
        case -1:
            return 0;
            break;

        case 1:
            printAllInstructions(IMem);
            break;

        case 2:
            printAllRegisters(Regs);
            break;

        default:
            std::cout << "cmd is not valid." << std::endl;
            break;
        }
    }

    IF_stage();
    clk_cnt++;

    ID_stage();
    IF_stage();
    clk_cnt++;

    EX_stage();
    ID_stage();
    IF_stage();
    clk_cnt++;

    MEM_stage();
    EX_stage();
    ID_stage();
    IF_stage();
    clk_cnt++;

    WB_stage();
    MEM_stage();
    EX_stage();
    ID_stage();
    IF_stage();
    clk_cnt++;

    return 0;
}
