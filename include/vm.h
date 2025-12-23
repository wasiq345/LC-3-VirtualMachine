#include<iostream>
#include<cstdint>
const uint16_t MAX_MEMORY = 1 << 16;

class VirtualMachine{
    bool running;
    uint16_t memory[MAX_MEMORY];
    uint16_t Reg[8];
    bool flag[3];
    bool &N = flag[0]; bool &P = flag[1]; bool &Z = flag[2];
    uint16_t pc; 
    uint16_t currentAddress;

    enum opcode{
        ADD, 
        AND,
        LDI,
        LDR,
        JMP,
        ST,
        LD,
        LEA,
        TRAP,
        NOT,
    };

    void fetch();
    void decode();
    void execute();
    void UpdateFlags(uint16_t &r);

    public:
    VirtualMachine() : running(false), pc(0) {for(int i = 0; i<3;i++) flag[i] = false;}

    void run();
    void loadProgram();


    uint16_t read(uint16_t &address);
    uint16_t write(uint16_t &address, uint16_t data);
};