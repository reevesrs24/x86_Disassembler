#include "x86Parser.h"
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <bits/stdc++.h>


 

x86Parser::x86Parser()
: registers { "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi" }
{
    labelCount = 1;
    byteCount = 0;
}

void x86Parser::parseFile(char* binFile)
{
    file.open(binFile, std::ios::in | std::ios::binary);
    
    
    while(file.get(ch)){
        currInstr.str(std::string());
        opCodes.clear();

        unsigned char c = (ch & 0xff);
        lookupInstruction(c);

        Instructions_t instr{currInstr.str(), opCodes};
        instrs.push_back(instr);
    }

    file.close();
    createASMFile();
}

void x86Parser::createASMFile()
{
    std::ofstream asmFile;
    asmFile.open("out.s");

    int count = 0;
    int bytes = 0;
    for (auto& i : instrs) {
        if (labels.count(count  + 1)) asmFile << labels[count + 1] << std::endl;

        bytes = 0;
        asmFile << std::right << std::hex << std::setw(8) << std::setfill('0') << count << ": ";
        for(auto& b : i.instructionOpCode) {
            asmFile  << std::hex << std::setw(2) << std::setfill('0') << b << " " ;
            count++;
            bytes++;
        }
        for (int i = 0; i < 40-(bytes * 3); i++)
            asmFile <<  " ";

        asmFile << i.instructionSeq;// << std::endl;
    }
    asmFile.close();
}   

std::string x86Parser::get8BitImmediate()
{
    std::string bits8ImmediateHex;
    std::stringstream ss;
    file.get(ch);
    unsigned char b1 = (ch & 0xff);;
    opCodes.push_back(b1);

    ss << std::hex << std::setw(2) << std::setfill('0') << (int)b1;
    bits8ImmediateHex = ss.str();
    return "0x" + bits8ImmediateHex;
}

std::string x86Parser::get16BitImmediate()
{
    std::string bits16ImmediateHex;
    std::stringstream ss;
    file.get(ch);
    unsigned char b1 = (ch & 0xff);
    opCodes.push_back(b1);
    file.get(ch);
    unsigned char b2 = (ch & 0xff);
    opCodes.push_back(b2);

    ss << std::hex << (int)b2 << (int)b1;
    bits16ImmediateHex = ss.str();
    return (bits16ImmediateHex.length() != 3) ? "0x" + bits16ImmediateHex : "0x" + bits16ImmediateHex.substr(1, 2);
}

std::string x86Parser::get32BitImmediate()
{
    std::string bits32ImmediateHex;
    std::stringstream ss;
    file.get(ch);
    unsigned char b1 = (ch & 0xff);
    opCodes.push_back(b1);
    file.get(ch);
    unsigned char b2 = (ch & 0xff);
    opCodes.push_back(b2);
    file.get(ch);
    unsigned char b3 = (ch & 0xff);
    opCodes.push_back(b3);
    file.get(ch);
    unsigned char b4 = (ch & 0xff);
    opCodes.push_back(b4);

    ss << "0x" << std::hex << (int)b4 << (int)b3 << (int)b2 << (int)b1;
    bits32ImmediateHex = ss.str();
    return bits32ImmediateHex;
}


void x86Parser::createLabelRel32()
{
    std::string labelRel32;
    std::stringstream ss;
    std::stringstream offset;
    int callLocation;

    file.get(ch);
    unsigned char b1 = (ch & 0xff);
    opCodes.push_back(b1);
    file.get(ch);
    unsigned char b2 = (ch & 0xff);
    opCodes.push_back(b1);
    file.get(ch);
    unsigned char b3 = (ch & 0xff);
    opCodes.push_back(b1);
    file.get(ch);
    unsigned char b4 = (ch & 0xff);
    opCodes.push_back(b1);

    offset << std::hex << (int)b4  << (int)b3 << (int)b2 << (int)b1;

    callLocation = std::strtol(offset.str().c_str(), NULL, 16);;

    ss << std::hex << std::setw(8) << std::setfill('0') << (callLocation +  file.tellg());
    labelRel32 = "label_" + ss.str() + ":";
    labels[ callLocation + file.tellg() + 1 ] = labelRel32;

    currInstr << labelRel32 << std::endl;
}

void x86Parser::createLabelRel8()
{
    std::string labelRel8;
    std::stringstream ss;
     std::stringstream offset;
    int8_t callLocation;

    file.get(ch);
    unsigned char b1 = (ch & 0xff);
    opCodes.push_back(b1);

    offset << std::hex << (int)b1;
    callLocation = std::strtol(offset.str().c_str(), NULL, 16);
    //std::cout << offset.str() << " " << std::to_string(callLocation) << std::endl;

    ss << std::hex << std::setw(8) << std::setfill('0') << std::right << (callLocation  + file.tellg());

    labelRel8 = "label_" + ss.str() + ":";
    labels[ callLocation + file.tellg() + 1 ] = labelRel8;

    currInstr << labelRel8 << std::endl;
}


void x86Parser::lookupInstruction(unsigned int hex)
{

    switch(hex){

        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
            addOpcode(hex);
            break;
        case 0x81:
            opCode81Fork(hex);
            break;
        case 0x21:
        case 0x20:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
            andOpcode(hex);
            break;
        case 0xE8:
            callOpcode(hex);
            break;
        case 0xAE:
            clflushOpcode(hex);
            break;
        case 0x38:
        case 0x39:
        case 0x3A:
        case 0x3B:
        case 0x3C:
        case 0x3D:
            cmpOpcode(hex);
            break;
        case 0xF7:
            opCodeF7Fork(hex);
            break;
        case 0xFF:
            opCodeFFFork(hex);
            break;
        case 0xFE:
            decOpcode(hex);
            break;
        case 0xF6:
            idivOpcode(hex);
            break;
        case 0x69:
        case 0x6B:
        case 0xAF:
            imulOpcode(hex);
            break;
        case 0xE9:
        case 0xEB:
            jmpOpcode(hex);
            break;
        case 0x74:     
        case 0x84:
            jzOpcode(hex);
            break;
        case 0x75:
            jnzOpcode(hex);
            break;
        case 0x8B:
        case 0x88:
        case 0x89:
        case 0x8A:
        case 0xB8:
        case 0xB9:
        case 0xBA:
        case 0xBB:
        case 0xBC:
        case 0xBD:
        case 0xBE:
        case 0xBF:
        case 0xA0:
        case 0xA1:
        case 0xA2:
        case 0xA3:
        case 0xC7:
            movOpcode(hex);
            break;
        case 0xA5:
            movsdOpcode(hex);
            break;
        case 0x90:
            nopOpcode(hex);
            break;
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
            xorOpcode(hex);
            break;
        case 0x1F:
        case 0x17:
        case 0x07:
        case 0x8F:
            popOpcode(hex);
            break;
        case 0x06:
        case 0x0E:
        case 0x16:
        case 0x1E:
        case 0x68:
        case 0x6A:
            pushOpcode(hex);
            break;
        case 0xF2:
            repneOpcode(hex);
            break;
        case 0xCA:
        case 0xCB:
            retfOpcode(hex);
            break;
        case 0xC2:
        case 0xC3:
            retnOpcode(hex);
            break;
        case 0x18:
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
            sbbOpcode(hex);
            break;
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4E:
        case 0x4F:
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
        case 0x58:
        case 0x59:
        case 0x5A:
        case 0x5B:
        case 0x5C:
        case 0x5D:
        case 0x5E:
        case 0x5F:
        case 0x60:
            decodeMODRM_O(hex);
            break;
        case 0x0F:
            opCode0FFork();
            break;
        case 0x8D:
            leaOpcode(hex);
            break;
        case 0x09:
        case 0x0B:
        case 0x0D:
            orOpcode(hex);
            break;
        case 0xA7:
            cmpsdOpcode(hex);
            break;
        case 0xD1:
            opcodeD1Fork(hex);
            break;
        case 0x85:
        case 0xA9:
            testOpcode(hex);
            break;
        case 0xE7:
            outOpcode(hex);
            break;
        default:
            currInstr << "Instruction Not Found! " << std::hex << hex << std::endl;

    }
    
}

void x86Parser::decodeMODRM_RMI()
{
    file.get(ch);
    // MODR/M (2 bits), Reg (3 bits), R/M (3 bits) 
    unsigned char modrmByte = (ch & 0xff);
    opCodes.push_back(modrmByte);
    std::string specialCase;

    unsigned int mod  = (modrmByte & 0xC0);         // Bitmask 11000000
    unsigned int reg  = (modrmByte & 0x38) >> 3;    // Bitmask 00111000
    unsigned int rm   = (modrmByte & 0x07);         // Bitmask 00000111

    switch(mod){
        case 0b00000000:
            specialCase = (rm == 0x05) ? get32BitImmediate() : registers[rm];
            currInstr << registers[reg] << ", dword [" << specialCase << "], ";
            currInstr << get32BitImmediate() << std::endl;
            break;
        case 0b01000000:
            currInstr << registers[reg] << ", dword [byte " << registers[rm] << " + " << get8BitImmediate() <<"], ";
            currInstr << get32BitImmediate() << std::endl;
            break;
        case 0b10000000:
            currInstr << registers[reg] << ", dword [dword " << registers[rm] << " + " << get32BitImmediate() <<"], ";
            currInstr << get32BitImmediate() << std::endl;
            break;
        case 0b11000000:
            currInstr << registers[rm] << ", " << registers[reg] << std::endl;
            break;
        default:
            currInstr << "MODRM Error" << std::endl;
            break;
    }
}

void x86Parser::decodeMODRM_RM()
{
    file.get(ch);
    // MODR/M (2 bits), Reg (3 bits), R/M (3 bits)
    unsigned char modrmByte = (ch & 0xff);
    opCodes.push_back(modrmByte);
    std::string specialCase;

    unsigned int mod  = (modrmByte & 0xC0);         // Bitmask 11000000
    unsigned int reg  = (modrmByte & 0x38) >> 3;    // Bitmask 00111000
    unsigned int rm   = (modrmByte & 0x07);         // Bitmask 00000111

    switch(mod){
        case 0b00000000:
            specialCase = (rm == 0x05) ? get32BitImmediate() : registers[rm];
            currInstr << registers[reg] << ", dword [" << specialCase << "]" << std::endl;
            break;
        case 0b01000000:
            currInstr << registers[reg] << ", dword [byte " << registers[rm] << " + " << get8BitImmediate() <<"]" << std::endl;
            break;
        case 0b10000000:
            currInstr << registers[reg] << ", dword [dword " << registers[rm] << " + " << get32BitImmediate() <<"]" << std::endl;
            break;
        case 0b11000000:
            currInstr << registers[rm] << ", " << registers[reg] << std::endl;
            break;
        default:
            currInstr << "MODRM Error" << std::endl;
            break;
    }
}

void x86Parser::decodeMODRM_MR()
{
    file.get(ch);
    // MODR/M (2 bits), Reg (3 bits), R/M (3 bits) 
    unsigned char modrmByte = (ch & 0xff);
    opCodes.push_back(modrmByte);
    std::string specialCase;
    std::stringstream ss;
    unsigned int mod  = (modrmByte & 0xC0);         // Bitmask 11000000
    unsigned int reg  = (modrmByte & 0x38) >> 3;    // Bitmask 00111000
    unsigned int rm   = (modrmByte & 0x07);         // Bitmask 00000111

    switch(mod){
        case 0b00000000:
            specialCase = (rm == 0x05) ? get32BitImmediate() : registers[rm];
            currInstr << "dword [" << specialCase << "], " << registers[reg] << std::endl;
            break;
        case 0b01000000:
            currInstr << "dword [byte " << registers[rm] << " + " << get8BitImmediate() <<"], " <<  registers[reg] << std::endl;
            break;
        case 0b10000000:
            currInstr << "dword [dword " << registers[rm] << " + " << get32BitImmediate() <<"], " << registers[reg] << std::endl;
            break;
        case 0b11000000:
            currInstr << registers[rm] << ", " << registers[reg] << std::endl;
            break;
        default:
            currInstr << "MODRM Error" << std::endl;
            break;
    }
}

void x86Parser::decodeMODRM_MI()
{
    file.get(ch);
    // MODR/M (2 bits), Reg (3 bits), R/M (3 bits)
    unsigned char modrmByte = (ch & 0xff);
    opCodes.push_back(modrmByte);
    std::string specialCase;

    unsigned int mod  = (modrmByte & 0xC0);         // Bitmask 11000000
    unsigned int reg  = (modrmByte & 0x38) >> 3;    // Bitmask 00111000
    unsigned int rm   = (modrmByte & 0x07);         // Bitmask 00000111

    switch(mod){
        case 0b00000000:
            specialCase = (rm == 0x05) ? get32BitImmediate() : registers[rm];
            currInstr << "dword [" << specialCase << "], " << get32BitImmediate() << std::endl;
            break;
        case 0b01000000:
            currInstr << "dword [byte " << registers[rm] << " + " << get8BitImmediate();
            currInstr << "], " << get32BitImmediate() << std::endl;
            break;
        case 0b10000000:
            currInstr << "dword [dword " << registers[rm] << " + " << get32BitImmediate() << "], ";
            currInstr << get32BitImmediate() << std::endl;
            break;
        case 0b11000000:
            currInstr << registers[rm] << ", " << get32BitImmediate() << std::endl;
            break;
        default:
            currInstr << "MODRM_MI Error" << std::endl;
            break;
    };
}

void x86Parser::decodeMODRM_D_Rel32()
{
    currInstr << get32BitImmediate() << std::endl;
}

void x86Parser::decodeMODRM_D_Rel8()
{
    currInstr << get8BitImmediate() << std::endl;
}

void x86Parser::decodeMODRM_M()
{
    file.get(ch);
    // MODR/M (2 bits), Reg (3 bits), R/M (3 bits)
    unsigned char modrmByte = (ch & 0xff);
    opCodes.push_back(modrmByte);
    std::string specialCase;

    unsigned int mod  = (modrmByte & 0xC0);         // Bitmask 11000000
    unsigned int reg  = (modrmByte & 0x38) >> 3;    // Bitmask 00111000
    unsigned int rm   = (modrmByte & 0x07);         // Bitmask 00000111

    switch(mod){
        case 0b00000000:
            specialCase = (rm == 0x05) ? get32BitImmediate() : registers[rm];
            currInstr << "dword [" << specialCase << "]" << std::endl;
            break;
        case 0b01000000:
            currInstr << "dword [byte " << registers[rm] << " + " << get8BitImmediate();
            currInstr << "]" << std::endl;
            break;
        case 0b10000000:
            currInstr << "dword [dword " << registers[rm] << " + " << get32BitImmediate() << "]";
            currInstr << std::endl;
            break;
        case 0b11000000:
            currInstr << registers[rm] << std::endl;
            break;
        default:
            currInstr << "MODRM_MI Error" << std::endl;
            break;
    }
}

void x86Parser::decodeMODRM_M1()
{
     file.get(ch);
    // MODR/M (2 bits), Reg (3 bits), R/M (3 bits)
    unsigned char modrmByte = (ch & 0xff);
    opCodes.push_back(modrmByte);
    std::string specialCase;

    unsigned int mod  = (modrmByte & 0xC0);         // Bitmask 11000000
    unsigned int reg  = (modrmByte & 0x38) >> 3;    // Bitmask 00111000
    unsigned int rm   = (modrmByte & 0x07);         // Bitmask 00000111

    switch(mod){
        case 0b00000000:
            specialCase = (rm == 0x05) ? get32BitImmediate() : registers[rm];
            currInstr << "dword [" << specialCase << "], 1" << std::endl;
            break;
        case 0b01000000:
            currInstr << "dword [byte " << registers[rm] << " + " << get8BitImmediate();
            currInstr << "], 1" << std::endl;
            break;
        case 0b10000000:
            currInstr << "dword [dword " << registers[rm] << " + " << get32BitImmediate() << "], 1";
            currInstr << std::endl;
            break;
        case 0b11000000:
            currInstr << registers[rm] << ", 1" << std::endl;
            break;
        default:
            currInstr << "MODRM_M1 Error" << std::endl;
            break;
    }
}

void x86Parser::decodeMODRM_O(unsigned int hex)
{   
    opCodes.push_back(hex);

    switch(hex){
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
            currInstr << "inc " << registers[hex % 0x40] << std::endl;
            break;
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4E:
            currInstr << "dec " << registers[hex % 0x48] << std::endl;
            break;
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
            currInstr << "push " << registers[hex % 0x50] << std::endl;
            break;
        case 0x58:
        case 0x59:
        case 0x5A:
        case 0x5B:
        case 0x5C:
        case 0x5D:
        case 0x5E:
        case 0x5F:
            currInstr << "pop " << registers[hex % 0x58] << std::endl;
            break;
        default:
            currInstr << "MODRM_0 Error" << std::endl;
    }
}


void x86Parser::opCode81Fork(unsigned int hex)
{
    unsigned int ch = file.peek();
    unsigned char modrmByte = (ch & 0xff);

    unsigned int mod  = (modrmByte & 0xC0);         // Bitmask 11000000
    unsigned int reg  = (modrmByte & 0x38) >> 3;    // Bitmask 00111000
    unsigned int rm   = (modrmByte & 0x07);         // Bitmask 00000111


    switch(reg) {
        case 0x00:
            addOpcode(hex);
            break;
        case 0x01:
            orOpcode(hex);
            break;
        case 0x02:
            break;
        case 0x03:
            sbbOpcode(hex);
            break;
        case 0x04:
            andOpcode(hex);
            break;
        case 0x05:
            break;
        case 0x06:
            xorOpcode(hex);
            break;
        case 0x07:
            cmpOpcode(hex);
            break;
        default:
            break;
        
    }
}

void x86Parser::opCodeFFFork(unsigned int hex)
{
    unsigned int ch = file.peek();
    unsigned char modrmByte = (ch & 0xff);

    unsigned int mod  = (modrmByte & 0xC0);         // Bitmask 11000000
    unsigned int reg  = (modrmByte & 0x38) >> 3;    // Bitmask 00111000
    unsigned int rm   = (modrmByte & 0x07);         // Bitmask 00000111

    //std::cout << "81 Fork " << std::to_string(reg) << std::endl;
    switch(reg) {
        case 0x00:
            incOpcode(hex);
            break;
        case 0x01:
            decOpcode(hex);
            break;
        case 0x02:
            callOpcode(hex);
            break;
        case 0x03:
            break;
        case 0x04:
            jmpOpcode(hex);
            break;
        case 0x05:
            break;
        case 0x06:
            pushOpcode(hex);
            break;
        case 0x07:
            break;
        default:
            break;
        
    }
}

void x86Parser::opCodeF7Fork(unsigned int hex)
{
    unsigned int ch = file.peek();
    unsigned char modrmByte = (ch & 0xff);

    unsigned int mod  = (modrmByte & 0xC0);         // Bitmask 11000000
    unsigned int reg  = (modrmByte & 0x38) >> 3;    // Bitmask 00111000
    unsigned int rm   = (modrmByte & 0x07);         // Bitmask 00000111

    //std::cout << "81 Fork " << std::to_string(reg) << std::endl;
    switch(reg) {
        case 0x00:
            testOpcode(hex);
            break;
        case 0x01:
            testOpcode(hex);
            break;
        case 0x02:
            notOpcode(hex);
            break;
        case 0x03:
            negOpcode(hex);
            break;
        case 0x04:
            mulOpcode(hex);
            break;
        case 0x05:
            imulOpcode(hex);
            break;
        case 0x06:
            //divOpcode(hex);
            break;
        case 0x07:
            idivOpcode(hex);
            break;
        default:
            break;
        
    }
}

void x86Parser::opcodeD1Fork(unsigned int hex)
{
    unsigned int ch = file.peek();
    unsigned char modrmByte = (ch & 0xff);

    unsigned int mod  = (modrmByte & 0xC0);         // Bitmask 11000000
    unsigned int reg  = (modrmByte & 0x38) >> 3;    // Bitmask 00111000
    unsigned int rm   = (modrmByte & 0x07);         // Bitmask 00000111

    switch(reg) {
        case 0x00:
            break;
        case 0x01:
            break;
        case 0x02:
            break;
        case 0x03:
            break;
        case 0x04:
            salOpcode(hex);
            break;
        case 0x05:
            shrOpcode(hex);
            break;
        case 0x06:
            break;
        case 0x07:
            sarOpcode(hex);
            break;
        default:
            break;
        
    }
}
void x86Parser::opCode0FFork()
{
    file.get(ch);
    unsigned char twoByteOpcode = (ch & 0xff);
    opCodes.push_back(twoByteOpcode);
    
    switch(twoByteOpcode){
        case 0xAF:
            imulOpcode(twoByteOpcode);
            break;
        case 0x84:
            jzOpcode(twoByteOpcode);
            break;
         case 0x85:
            jnzOpcode(twoByteOpcode);
            break;
        case 0xAE:
            clflushOpcode(twoByteOpcode);
            break;
        default:
            currInstr << "Error 0F " << std::hex << (int)twoByteOpcode << std::endl;
            break;
    }

}

void x86Parser::xorOpcode(unsigned int hex)
{   
    currInstr << "xor ";

    opCodes.push_back(hex);
    switch(hex){
        case 0x31:
            decodeMODRM_MR();
            break;
        case 0x33:
            decodeMODRM_RM();
            break;
        case 0x35:
            currInstr << "eax, " << get32BitImmediate() << std::endl;
            break;
        case 0x81:
            decodeMODRM_MI();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }
}

void x86Parser::addOpcode(unsigned int hex)
{
    currInstr << "add ";
    
    opCodes.push_back(hex);
    switch(hex){
        case 0x01:
            decodeMODRM_MR();
            break;
        case 0x81:
            decodeMODRM_MI();
            break;
        case 0x03:
            decodeMODRM_RM();
            break;
        case 0x05:
            currInstr << "eax, " << get32BitImmediate() << std::endl;;
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }
}

void x86Parser::pushOpcode(unsigned int hex)
{
    currInstr << "push ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xFF:
            decodeMODRM_M();
            break;
        case 0x68:
            currInstr << get32BitImmediate() << std::endl;
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }
}

void x86Parser::popOpcode(unsigned int hex)
{
    currInstr << "pop ";

    opCodes.push_back(hex);
    switch(hex){
        case 0x8F:
            decodeMODRM_M();
            break;
        
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }


}

void x86Parser::movOpcode(unsigned int hex)
{
    currInstr << "mov ";

    opCodes.push_back(hex);
    switch(hex){
        case 0x89:
            decodeMODRM_MR();
            break;
        case 0x8B:
            decodeMODRM_RM();
            break;
        case 0xB8:
        case 0xB9:
        case 0xBA:
        case 0xBB:
        case 0xBC:
        case 0xBD:
        case 0xBE:
        case 0xBF:
            currInstr << registers[hex % 0xB8] << ", " << get32BitImmediate() << std::endl;
            break;
        case 0xC7:
            decodeMODRM_MI();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }

};

void x86Parser::retnOpcode(unsigned int hex)
{
    currInstr << "retn ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xC2:
            currInstr << get16BitImmediate() << std::endl;  
            break;
        case 0xC3:
             currInstr << std::endl;  
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }


}

void x86Parser::cmpOpcode(unsigned int hex)
{
    currInstr << "cmp ";

    opCodes.push_back(hex);
    switch(hex){
        case 0x39:
            decodeMODRM_MR();
            break;
        case 0x81:
            decodeMODRM_MI();
            break;
        case 0x3B:
            decodeMODRM_RM();
            break;
        case 0x3D:
            currInstr << "eax, " << get32BitImmediate() << std::endl;
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }
}


void x86Parser::andOpcode(unsigned int hex)
{
    currInstr << "and ";

    opCodes.push_back(hex);
    switch(hex){
        case 0x21:
            decodeMODRM_MR();
            break;
        case 0x81:
            decodeMODRM_MI();
            break;
        case 0x23:
            decodeMODRM_RM();
            break;
        case 0x25:
            currInstr << "eax, " << get32BitImmediate() << std::endl;
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }
}

void x86Parser::orOpcode(unsigned int hex)
{
    currInstr << "or ";

    opCodes.push_back(hex);
    switch(hex){
        case 0x09:
            decodeMODRM_MR();
            break;
        case 0x81:
            decodeMODRM_MI();
            break;
        case 0x0B:
            decodeMODRM_RM();
            break;
        case 0x0D:
            currInstr << "eax, " << get32BitImmediate() << std::endl;
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }

}

void x86Parser::db(unsigned int byte)
{
    currInstr << "db 0x" << std::hex << std::setfill('0') << std::setw(2) << byte << std::endl;
    opCodes.push_back(byte);
}

void x86Parser::retfOpcode(unsigned int hex)
{
    currInstr << "retf ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xCB:
            currInstr << std::endl;
            break;
        case 0xCA:
            currInstr << get16BitImmediate() << std::endl;
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }
}

void x86Parser::callOpcode(unsigned int hex)
{
    currInstr << "call ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xE8:
            createLabelRel32();
            break;
        case 0xFF:
            decodeMODRM_M();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }

}

void x86Parser::jmpOpcode(unsigned int hex)
{
    currInstr << "jmp ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xE9:
            createLabelRel32();
            break;
        case 0xEB:
            createLabelRel8();
            break;
        case 0xFF:
            decodeMODRM_M();
            break;
        default:
            currInstr << "Error "<< std::hex << hex << std::endl;
            break;
    }

}

void x86Parser::jnzOpcode(unsigned int hex)
{
    currInstr << "jnz ";

    opCodes.push_back(hex);
    switch(hex){
        case 0x85:
            createLabelRel32();
            break;
        case 0x75:
            createLabelRel8();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }
}

void x86Parser::jzOpcode(unsigned int hex)
{
    currInstr << "jz ";

    opCodes.push_back(hex);
    switch(hex){
        case 0x74:
            createLabelRel8();
            break;
        case 0x84:
            createLabelRel32();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }
}

void x86Parser::decOpcode(unsigned int hex)
{
    currInstr << "dec ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xFF:
            decodeMODRM_M();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }

}

void x86Parser::idivOpcode(unsigned int hex)
{
    currInstr << "idiv ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xF7:
            decodeMODRM_M();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }

}


void x86Parser::mulOpcode(unsigned int hex)
{
    currInstr << "mul ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xF7:
            decodeMODRM_M();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }
    
}

void x86Parser::imulOpcode(unsigned int hex)
{
    currInstr << "imul ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xF7:
            decodeMODRM_M();
            break;
        case 0xAF:
            decodeMODRM_RM();
            break;
        case 0x69:
            decodeMODRM_RMI();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }

}


void x86Parser::incOpcode(unsigned int hex)
{
    currInstr << "inc ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xFF:
            decodeMODRM_M();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }

}


void x86Parser::leaOpcode(unsigned int hex)
{
    currInstr << "lea ";

    opCodes.push_back(hex);
    switch(hex){
        case 0x8D:
            decodeMODRM_RM();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }

}

void x86Parser::movsdOpcode(unsigned int hex)
{
    currInstr << "movsd" << std::endl;
    opCodes.push_back(hex);
}

void x86Parser::nopOpcode(unsigned int hex)
{
    currInstr << "nop" << std::endl;
    opCodes.push_back(hex);
}

void x86Parser::notOpcode(unsigned int hex)
{
    currInstr << "not ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xF7:
            decodeMODRM_M();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }

}

void x86Parser::negOpcode(unsigned int hex)
{
    currInstr << "neg ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xF7:
            decodeMODRM_M();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }
    
}

void x86Parser::repneOpcode(unsigned int hex)
{
    file.get(ch);
    unsigned char repInstr = (ch & 0xff);

    currInstr << "repne ";

    opCodes.push_back(hex);

    switch(repInstr){
        case 0xA7:
            lookupInstruction(repInstr);
            break;
        default:
            currInstr<< "Error " << std::hex << hex << std::endl;
            break;
    }

}

void x86Parser::cmpsdOpcode(unsigned int hex)
{
    currInstr << "cmpsd " << std::endl;
    opCodes.push_back(hex);
}

void x86Parser::sbbOpcode(unsigned int hex)
{
    currInstr << "sbb ";

    opCodes.push_back(hex);
    switch(hex){
        case 0x81:
            decodeMODRM_MI();
            break;
        case 0x19:
            decodeMODRM_MR();
            break;
        case 0x1B:
            decodeMODRM_RM();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }

}

void x86Parser::testOpcode(unsigned int hex)
{
    currInstr << "test ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xF7:
            decodeMODRM_MI();
            break;
        case 0x85:
            decodeMODRM_MR();
            break;
        case 0xA9:
            currInstr << "eax, " << get32BitImmediate() << std::endl;
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }

}

void  x86Parser::salOpcode(unsigned int hex)
{
    currInstr << "sal ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xD1:
            decodeMODRM_M1();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }
}

void  x86Parser::sarOpcode(unsigned int hex)
{
    currInstr << "sar ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xD1:
            decodeMODRM_M1();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }
}

void  x86Parser::shrOpcode(unsigned int hex)
{
    currInstr << "shr ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xD1:
            decodeMODRM_M1();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }
}

void x86Parser::clflushOpcode(unsigned int hex)
{
    currInstr << "clflush ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xAE:
            decodeMODRM_M();
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }

}

void x86Parser::outOpcode(unsigned int hex)
{
    currInstr << "out ";

    opCodes.push_back(hex);
    switch(hex){
        case 0xE7:
            currInstr << get8BitImmediate() << ", eax" << std::endl; 
            break;
        default:
            currInstr << "Error " << std::hex << hex << std::endl;
            break;
    }

}