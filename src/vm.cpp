#include "../include/vm.h"

void VirtualMachine::fetch()
{
    currentAddress = memory[pc];
    pc++;
}

void VirtualMachine::decode()
{
    uint16_t opcode = currentAddress >> 12;    // 1111000110101011   , 7 = 0111
    uint16_t dr = (currentAddress >> 9 ) & 0x7;
    uint16_t sr= (currentAddress >> 6)  & 0x7;
    uint16_t immi = (currentAddress >> 5) & 0x1;

    switch (opcode)
    {
    case opcode::ADD:
        if(immi == 1)
        {
            uint16_t val = sign_extend(currentAddress & 0x1f, 5);           // 0000 0000 00001 1111
            Reg[dr] = Reg[sr] + val;
        }
        else{
            uint16_t sr2 = currentAddress & 0x7;
            Reg[dr] = Reg[sr] + Reg[sr2];
        }
        UpdateFlags(Reg[dr]);
        break;

    case opcode::LDI:
        
        break;
        
    case opcode::AND:

        break;

    case opcode::JMP:
        break;

    case opcode::NOT:
        break;
    
    case opcode::LD:
        break;
    
    case opcode::ST:
        break;

    case opcode::TRAP:
        break;

    case opcode::LEA:
        break;

    case opcode::LDR:
        break;
    
    default:
        break;
    }
}

void VirtualMachine :: execute()
{

}

uint16_t VirtualMachine ::read(uint16_t &address)
{
   uint16_t readAddress = memory[pc];
   pc++;
   return readAddress;
}

uint16_t VirtualMachine :: write(uint16_t &address, uint16_t data)
{
    
}


void VirtualMachine::UpdateFlags(uint16_t &r)
{
    if(Reg[r] == 0) {Z = true; return;}
    uint16_t msb = Reg[r] >> 15;
    if(msb == 1) {N = true;}
    else{
        P = true;
    }
}

uint16_t sign_extend(uint16_t x, int bit_count)
{
   if(x >> (bit_count-1) & 1){
    x |= 0xffff << bit_count; 
   }
   return x;
}

void VirtualMachine::run()
{
    running = true;
    while(running)
    {
        fetch();
        decode();
        execute();
    }
}
