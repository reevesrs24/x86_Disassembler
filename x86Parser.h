#ifndef X86PARSER_H
#define X86PARSER_H

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

        

class x86Parser
{


    public:
        x86Parser();
        void parseFile(char*);
        void lookupInstruction(unsigned int);
        void addOpcode(unsigned int);
        void andOpcode(unsigned int);
        void callOpcode(unsigned int);
        void clflushOpcode(unsigned int);
        void cmpOpcode(unsigned int);
        void decOpcode(unsigned int);
        void idivOpcode(unsigned int);
        void imulOpcode(unsigned int);
        void incOpcode(unsigned int);
        void jmpOpcode(unsigned int);
        void jzOpcode(unsigned int);
        void jnzOpcode(unsigned int);
        void leaOpcode(unsigned int);
        void movOpcode(unsigned int);
        void movsdOpcode(unsigned int);
        void mulOpcode(unsigned int);
        void negOpcode(unsigned int);
        void nopOpcode(unsigned int);
        void notOpcode(unsigned int);
        void orOpcode(unsigned int);
        void outOpcode(unsigned int);
        void popOpcode(unsigned int);
        void pushOpcode(unsigned int);
        void repneOpcode(unsigned int);
        void cmpsdOpcode(unsigned int);
        void retfOpcode(unsigned int);
        void retnOpcode(unsigned int);
        void salOpcode(unsigned int);
        void sarOpcode(unsigned int); 
        void shrOpcode(unsigned int); 
        void sbbOpcode(unsigned int);
        void testOpcode(unsigned int);
        void xorOpcode(unsigned int);

        void decodeMODRM_RMI();
        void decodeMODRM_RM();
        void decodeMODRM_MR();
        void decodeMODRM_MI();
        void decodeMODRM_M1();
        void decodeMODRM_M();
        void decodeMODRM_D_Rel32();
        void decodeMODRM_D_Rel8();
        void decodeMODRM_O(unsigned int);
        
        void db(unsigned int);

        void opCode81Fork(unsigned int);
        void opCodeFFFork(unsigned int);
        void opCodeF7Fork(unsigned int);
        void opCode0FFork();
        void opcodeD1Fork(unsigned int);

        std::string get8BitImmediate();
        std::string get16BitImmediate();
        std::string get32BitImmediate();
        
        void createLabelRel8();
        void createLabelRel32();

        void createASMFile();

    private:
        struct Instructions_t {
            std::string instructionSeq;
            std::vector<unsigned int>instructionOpCode;
        };

        std::ifstream file;
        char ch;
        int labelCount;
        unsigned int byteCount;
        std::unordered_map<unsigned int, std::string> labels;
        std::string registers[8];
        std::vector<Instructions_t> instrs;
        std::stringstream currInstr;
        std::vector<unsigned int> opCodes;
};




#endif