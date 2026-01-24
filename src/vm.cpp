#include "../include/vm.h"


struct termios original_tio;

void disable_input_buffering()
{
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

uint16_t checkKey()
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}



uint16_t sign_extend(uint16_t x, int bit_count);

void VirtualMachine::fetch()
{
    IR = memory[pc];
    pc++;
}

void VirtualMachine::decodeAndExecute()
{
    uint16_t opcode = IR >> 12;    // 1111000110101011   , 7 = 0111

    switch (opcode)
    {
    case opcode::ADD:
    {
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
        UpdateFlags(dr);
        //std :: cout << "Add called\n";
        break;
    }

    case opcode::LDI:
    {
        uint16_t dr = (IR >> 9 ) & 0x7;
        uint16_t offset = sign_extend(IR  & 0x1FF, 9);

        uint16_t address = offset + pc;
        uint16_t finalAddress = memory[address];
        Reg[dr] = memory[finalAddress];
        UpdateFlags(dr);
        break;
    }

    case opcode::AND:
    {
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

        UpdateFlags(dr);
        break;
    }

     case opcode::NOT:
     {
        uint16_t dr = (IR >> 9) & 0x7;
        uint16_t sr = (IR >> 6) & 0x7;
        Reg[dr] = ~Reg[sr];
        UpdateFlags(dr);
        break;
     }

    case opcode::BR:
    {
        uint16_t flags = (IR >> 9) & 0x7;    
        uint16_t offset = sign_extend(IR & 0x1ff, 9);
        bool should_branch = (flags & 0x4 && *N) || (flags & 0x2 && *Z) || (flags & 0x1 && *P);
        if(should_branch)
        {
            pc += offset;
        } 
        break;
    }

    case opcode::JMP:
    {
        uint16_t baseR = (IR >> 6) & 0x7;
        pc = Reg[baseR];
        break;
    }

    case opcode::JSR:
    {
        uint16_t notLabel = (IR >> 11) & 0x1;
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
    }
    case opcode::LD:
    {
        uint16_t offset = sign_extend(IR & 0x1ff, 9);
        uint16_t dr = (IR >> 9) & 0x7;
        Reg[dr] = memory[pc + offset];
        UpdateFlags(dr);
        break; 
    }
    
    case opcode::LEA:
    {
        uint16_t offset = sign_extend(IR & 0x1ff, 9);
        uint16_t dr = (IR >> 9) & 0x7;
        Reg[dr] = pc + offset;
        UpdateFlags(dr); 
        break;
    }
    case opcode::ST:
    {
        uint16_t offset = sign_extend(IR & 0x1ff, 9);
        uint16_t sr = (IR >> 9) & 0x7;
        uint16_t address = offset + pc;
        write(address, Reg[sr]);
        break;
    }
    case opcode::STI:
    {   
        uint16_t offset = sign_extend(IR & 0x1ff, 9);
        uint16_t sr = (IR >> 9) & 0x7;
        uint16_t address = offset + pc;  // go to that address
        uint16_t finalAddress = memory[address];    // read the address present at that address
        write(finalAddress, Reg[sr]);   
        break;
    }

    case opcode::STR:
    {
        uint16_t offset = sign_extend(IR & 0x3f, 6);  // 0000 1111 10 10 1111
        uint16_t baseR = (IR >> 6) & 0x7;
        uint16_t sr = (IR >> 9) & 0x7;
        uint16_t address = Reg[baseR] + offset; 
        write(address, Reg[sr]);
        break;
    }

    case opcode::TRAP:
    {
            Reg[7] = pc;
        switch(IR & 0xff)
        {
            case TrapCode::TRAP_GETC:
            {
                Reg[0] = (uint16_t)getchar();      // by default reg 0 stores the character input
                std :: cin.ignore();
                uint16_t x = 0;
                UpdateFlags(x);
            break;
            }

            case TrapCode::TRAP_OUT:
            {
                char c = (char)Reg[0];
                std :: cout << c;
            break;
            }

            case TrapCode::TRAP_IN:
            {
               std :: cout << "Enter a character: ";
               Reg[0] = (uint16_t)getchar();
               std :: cin.ignore();
               uint16_t  x = 0;
               UpdateFlags(x);
            break;
            }

            case TrapCode::TRAP_HALT:
            {
                std :: cout << "\nProgram exited";
                running = false;
            break;
            }

            case TrapCode::TRAP_PUTS:
            {
                uint16_t* firstChar = memory + Reg[0];
                while(*firstChar != 0x0000)
                {
                    std :: cout << char(*firstChar);
                    firstChar++;
                }
                break;
            }

            case TrapCode::TRAP_PUTSP:
            {
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

        }
        break;
    }


    case opcode::LDR:
    {
        uint16_t dr = (IR >> 9) & 0x7;
        uint16_t baseR = (IR >> 6) & 0x7;
        uint16_t offset = sign_extend(IR & 0x3f, 6);
        uint16_t address = Reg[baseR] + offset;
        Reg[dr] = read(address);
        UpdateFlags(dr);
        break;
    }
    
    default:
        break;
    }
}

uint16_t VirtualMachine ::read(uint16_t address)
{
    if(address >= MAX_MEMORY) {
        std :: cout << "Memory Out of bounds\n";
        return 0;
    }

    if(address == MemoryMappedRegister::MR_KBSR)
    {
        if(checkKey())
        {
            memory[MR_KBSR] = (1 << 15);
            memory[MR_KBDR] = getchar();
            std::cin.ignore();
        }
        else{
            memory[MR_KBSR] = 0;
        }
    }
   return memory[address];
}

void VirtualMachine :: write(uint16_t address, uint16_t data)
{
     if(address >= MAX_MEMORY) {
        std :: cout << "Memory Out of bounds\n";
        return;
    }
    memory[address] = data;
}


void VirtualMachine::UpdateFlags(uint16_t r)
{
    *Z = false; *N = false; *P = false;
    if(Reg[r] == 0) {*Z = true; return;}
    uint16_t msb = Reg[r] >> 15;
    if(msb == 1) {*N = true;}
    else{
        *P = true;
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
        decodeAndExecute();
    }
}

void VirtualMachine::runStep()
{
    if(running)
    {
        fetch();
        decodeAndExecute();
    }
}

uint16_t convertToLittleEndian(uint16_t x)
{
    return (x << 8) | (x >> 8);
}

void VirtualMachine::loadProgramFile(const std :: string& filename)
{
    std :: ifstream file(filename, std :: ios :: binary);
    if(!file)
    {
        std :: cout << "Error opening file\n";
        return;
    }

    uint16_t origin;
    file.read(reinterpret_cast<char*>(&origin), sizeof(uint16_t));

    origin = convertToLittleEndian(origin);
    pc = origin;
    uint16_t instruction;
    uint16_t address = origin;
    while(file.read(reinterpret_cast<char*>(&instruction), sizeof(uint16_t)))
    {
        instruction = convertToLittleEndian(instruction);
        memory[address] = instruction;
        address++;
        if(address >= MAX_MEMORY) break;
    }

    file.close();
}

void VirtualMachine :: loadProgramVector(const std :: vector<uint16_t> &prog)
{
    uint16_t origin = 0x3000;
    pc = origin;
    running = true;
    for(uint16_t inst : prog)
    {
        memory[origin] = inst;
        origin++;
        if(origin >= MAX_MEMORY) break;
    }
}

void VirtualMachine :: renderScreen()
{
     const int scale = 5;
    for(int y = 0; y < 128; y++)
    {
        for(int x = 0; x < 128; x++)
        {
            uint16_t index = 0xC000 + (y*128 + x);
            if(index >= 65536) continue;
            uint16_t color16 = memory[index];
            if(color16 == 0) continue;
            //if (x == 0 && y == 0) printf("Looking at VRAM Start: 0x%04X\n", address);

            unsigned char r = (((color16 >> 10) & 0x1F) * 8);
            unsigned char g = (((color16 >> 5) & 0x1F) * 8);
            unsigned char b = (((color16 >> 0) & 0x1F) * 8);

            DrawRectangle(x*scale, y*scale, scale, scale, {r, g, b, 255});
        }
    }
}
    