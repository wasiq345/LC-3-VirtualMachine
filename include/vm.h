#pragma once
#include <iostream>
#include <stdint.h>
#include <signal.h>
#include<fstream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>
#include <vector>
const size_t MAX_MEMORY = 1 << 16;

class VirtualMachine{
    bool running;
    uint16_t* memory;
    uint16_t Reg[8];
    bool flag[3];
    bool *N; bool *P; bool *Z;
    uint16_t pc; 
    uint16_t IR;

    public:
    bool getRunning() {return running;}

    private:

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

    enum MemoryMappedRegister{
        MR_KBSR = 0xFE00,
        MR_KBDR = 0xFE02
    };

    void fetch();
    void decodeAndExecute();
    void UpdateFlags(uint16_t r);

    public:
    VirtualMachine() : running(false), N(&flag[0]), P(&flag[1]), Z(&flag[2]) {
        pc = 0x3000;
        memory = new uint16_t[MAX_MEMORY];
        for(int i = 0; i<3;i++) flag[i] = false;
        for(int i = 0; i < MAX_MEMORY; i++)
        {
            memory[i] = 0;
        }
        for(int i = 0; i < 8; i++)
        {
            Reg[i] =0;
        }
    }


    ~VirtualMachine()
    {
        delete[] memory;
    }

    void run();
    void runStep();
    void loadProgramFile(const std :: string& filename);
    void loadProgramVector(const std :: vector<uint16_t> &prog);


    uint16_t read(uint16_t address);
    public: void write(uint16_t address, uint16_t data);
    void renderScreen();                        // not fully implemented yet
    void drawDebugger();                        // NOt fully implemented yet

    uint16_t getReg(int i) { return Reg[i]; }
    uint16_t getPC() { return pc; }
    uint16_t getIR() { return IR; }
    bool getFlagN() { return *N; }
    bool getFlagZ() { return *Z; }
    bool getFlagP() { return *P; }
};