#include<iostream>
#include<cstdint>
#include<vector>
const uint16_t MAX_MEMORY = 1 << 16;

class VirtualMachine{
    bool running;
    uint16_t memory[MAX_MEMORY];
    uint16_t Reg[8];
    bool flag[3];
    bool &N = flag[0]; bool &P = flag[1]; bool &Z = flag[2];
    uint16_t pc; 
    uint16_t IR;

    enum opcode{
        BR = 0,
        ADD,    
        LD,     
        ST,    
        JSR,    
        AND,    
        LDR,    
        STR,    
        RTI,    
        NOT,  
        LDI,    
        STI,    
        JMP,    
        RES,    
        LEA,    
        TRAP    
    };

    enum TrapCode{
        TRAP_GETC = 0x20,
        TRAP_OUT = 0x21,
        TRAP_PUTS = 0x22,
        TRAP_IN = 0x23,
        TRAP_PUTSP = 0x24,
        TRAP_HALT = 0x25
    };

    void fetch();
    void decodeAndExecute();
    void UpdateFlags(uint16_t &r);

    public:
    VirtualMachine() : running(false) {for(int i = 0; i<3;i++) flag[i] = false;}

    void run();
    void loadProgram(std::vector<uint16_t> prog);


    uint16_t read(uint16_t &address);
    void write(uint16_t &address, uint16_t data);
};