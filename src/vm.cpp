#include "../include/vm.h"
uint16_t sign_extend(uint16_t x, int bit_count);

void VirtualMachine::fetch()
{
    IR = memory[pc];
    pc++;
}

void VirtualMachine::decode()
{
    uint16_t opcode = IR >> 12;    // 1111000110101011   , 7 = 0111

    switch (opcode)
    {
    case opcode::ADD:
        uint16_t dr = (IR >> 9 ) & 0x7;
        uint16_t sr= (IR >> 6)  & 0x7;
        uint16_t immi = (IR >> 5) & 0x1;
        if(immi == 1)
        {
            uint16_t val = sign_extend(IR & 0x1f, 5);           // 0000 0000 0001 1111
            Reg[dr] = Reg[sr] + val;
        }
        else{
            uint16_t sr2 = IR & 0x7;
            Reg[dr] = Reg[sr] + Reg[sr2];
        }
        UpdateFlags(Reg[dr]);
        break;

    case opcode::LDI:
        uint16_t dr = (IR >> 9 ) & 0x7;
        uint16_t offset = sign_extend(IR  & 0x1FF, 9);

        Reg[dr] = memory[memory[offset] + pc];
        UpdateFlags(Reg[dr]);
        break;

    case opcode::AND:
        uint16_t dr = (IR >> 9) & 0x7;
        uint16_t sr1 = (IR >> 6) & 0x7;
        uint16_t immi = (IR >> 5) & 0x1;
        if(immi == 1)
        {
            Reg[dr] = Reg[sr1] & sign_extend((IR & 0x1f), 5); 
        }
        else{
            uint16_t sr2 = IR & 0x7;           // 1011 0000 1111 1010 = 0000  0000 0000 0010
            Reg[dr] = Reg[sr1] & Reg[sr2];
        }

        UpdateFlags(Reg[dr]);
        break;

     case opcode::NOT:
        uint16_t dr = (IR >> 9) & 0x7;
        uint16_t sr = (IR >> 6) & 0x7;
        Reg[dr] = ~Reg[sr];
        UpdateFlags(Reg[dr]);
        break;

    case opcode::BR:
        uint16_t flags = (IR >> 9) & 0x7;
        bool Nflag = flags & 0x4 == 4 ? true : false;   // 0000 0000 0000 0100
        bool Zflag = flags & 0x2 == 2 ? true : false;
        bool Pflag = flags & 0x1 == 1 ? true : false;     
        uint16_t offset = sign_extend(IR & 0x1ff, 9);
        if(Nflag == N || Pflag == P || Zflag == Z)
        {
            pc += offset;
        } 
        break;

    case opcode::JMP:
        uint16_t baseR = (IR >> 6) & 0x7;
        pc = Reg[baseR];
        break;

    case opcode::JSR:
        uint16_t notLabel = IR >> 11 & 0x1;
        Reg[7] = pc;
        if(notLabel == 1){
            uint16_t offset = sign_extend(IR & 0x7ff, 11);
            pc += offset;
        }  
        else{
            uint16_t baseR = (IR >> 6) & 0x7;
            pc = Reg[baseR];
        }
    break;

    case opcode::LD:
        uint16_t offset = sign_extend(IR & 0x1ff, 9);
        uint16_t dr = (IR >> 9) & 0x7;
        Reg[dr] = memory[pc + offset];
        UpdateFlags(Reg[dr]);
        break; 
    
    
    case opcode::LEA:
        uint16_t offset = sign_extend(IR & 0x1ff, 9);
        uint16_t dr = (IR >> 9) & 0x7;
        Reg[dr] = pc + offset;
        UpdateFlags(Reg[dr]); 
        break;

    case opcode::ST:
        uint16_t offset = sign_extend(IR & 0x1ff, 9);
        uint16_t sr = (IR >> 9) & 0x7;
        uint16_t address = offset + pc;
        write(address, Reg[sr]);
        break;

    case opcode::STI:
        uint16_t offset = sign_extend(IR & 0x1ff, 9);
        uint16_t sr = (IR >> 9) & 0x7;
        uint16_t address = offset + sr;
        write(address, Reg[sr]);
        break;

    case opcode::STR:
        uint16_t offset = sign_extend(IR & 0x3f, 6);  // 0000 1111 10 10 1111
        uint16_t baseR = (IR >> 6) & 0x7;
        uint16_t sr = (IR >> 9) & 0x7;
        uint16_t address = Reg[baseR] + offset; 
        write(address, Reg[sr]);
        break;

    case opcode::TRAP:
            Reg[7] = pc;
        switch(IR & 0xff)
        {
            case TrapCode::TRAP_GETC:
                Reg[0] = (uint16_t)getchar();      // by default reg 0 stores the character input
                UpdateFlags(Reg[0]);
            break;

            case TrapCode::TRAP_OUT:
                char c = (char)Reg[0];
                std :: cout << c;
            break;

            case TrapCode::TRAP_IN:
               std :: cout << "Enter a character: ";
               Reg[0] = (uint16_t)getchar();
               UpdateFlags(Reg[0]);
            break;

            case TrapCode::TRAP_HALT:
                std :: cout << "Program exited";
                running = false;
            break;

            case TrapCode::TRAP_PUTS:
                uint16_t* firstChar = memory + Reg[0];
                while(*firstChar != 0x0000)
                {
                    std :: cout << char(*firstChar);
                    firstChar++;
                }
                break;

            case TrapCode::TRAP_PUTSP:
                uint16_t* firstChar = memory + Reg[0];
                while(*firstChar != 0x0000)
                {
                    char a = (char)(*firstChar & 0xff);
                    std :: cout << a;
                    char b = (char)(*firstChar >> 8);         
                    if(b) std :: cout << b;            /*if string odd e.g wasiq = wa will go to one memory loc,
                                                        si will go to second memory loc, q will go to third memory loc
                                                        but bcs one memory location(16 bits) contain two characters 
                                                        (0-7) first character, (8-15) second character. (bits 8-15)
                                                        will be empty*/
                    firstChar++;
                }
            break;

        }
        break;

    case opcode::RET:
        pc = Reg[7];
        break;

    case opcode::LDR:
        uint16_t dr = (IR >> 9) & 0x7;
        uint16_t baseR = (IR >> 6) & 0x7;
        uint16_t offset = sign_extend(IR & 0x3f, 6);
        uint16_t address = Reg[baseR] + offset;
        Reg[dr] = read(address);
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
   uint16_t readAddress = memory[address];
   return readAddress;
}

void VirtualMachine :: write(uint16_t &address, uint16_t data)
{
    memory[address] = data;
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
