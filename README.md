# LC-3 Virtual Machine

C++ implementation of the **LC-3 (Little Computer 3)** architecture. This project features a full 16-bit instruction set emulator and memory-mapped I/O.

---

## Understanding the LC-3 Architecture

The LC-3 is a symbolic computer architecture designed to teach the fundamentals of computer organization. It is a **Reduced Instruction Set Computer (RISC)** with a 16-bit word size.

### Core Specifications:
* **Word Size:** 16-bit (all registers and memory locations are 16 bits).
* **Registers:** 8 General Purpose Registers (**R0-R7**), one Program Counter (**PC**), and one Instruction Register (**IR**).
* **Memory:** 65,536 ($2^{16}$) addressable locations.
* **Condition Codes:** Three status flags—**N** (Negative), **Z** (Zero), and **P** (Positive)—which are updated whenever a result is written to a register.



---

## How the Virtual Machine Works

The VM simulates the hardware logic of a physical CPU through a process called the **Instruction Cycle**.

### 1. The Fetch-Decode-Execute Cycle
The VM runs a continuous loop that performs these three steps for every instruction:

1.  **Fetch:** The VM retrieves the instruction from `memory[pc]`, stores it in the **IR**, and increments the **PC**.
2.  **Decode:** The VM examines the first 4 bits (the **Opcode**) to determine which instruction to execute.
3.  **Execute:** The VM performs the specific operation (e.g., adding numbers, loading from memory, or branching to a new address).



### 2. Memory Mapping
This VM uses **Memory-Mapped I/O**, where hardware devices are treated as specific memory addresses rather than using separate instructions:

| Address Range | Component | Description |
| :--- | :--- | :--- |
| `0x3000 - 0xBFFF` | **User RAM** | Standard space for programs and data. |
| `0xFE00` | **KBSR** | Keyboard Status Register (checks if a key is pressed). |
| `0xFE02` | **KBDR** | Keyboard Data Register (holds the ASCII value of the key). |

---

## Implementation Features

### Bit Manipulation & Sign Extension
The LC-3 often uses small immediate values (e.g., 5 bits for an `ADD` instruction). The VM implements a `sign_extend` function to convert these small signed numbers into 16-bit integers while preserving their mathematical sign.

---


## 📜 Example Program: Name Input & Greeting

The repo contains two obj files named **name-out** and **calculator-out** to demo the VM.
In additiin to this you can write any lc3 program, assemble it and input the obj file to the main program.
**Example:** This assembly program demonstrates how the VM handles loops, keyboard input (`GETC`), and string output (`PUTS`):

```assembly
.ORIG x3000
    LD R1, NAME_PTR      ; Point R1 to storage buffer
LOOP:
    GETC                 ; Read character into R0
    OUT                  ; Echo character back to screen
    ADD R2, R0, #-10     ; Subtract 10 (Enter key ASCII)
    BRz DONE             ; If R2 is 0, user pressed Enter
    STR R0, R1, #0       ; Store character in buffer
    ADD R1, R1, #1       ; Move pointer to next slot
    BRnzp LOOP           ; Repeat
DONE:
    AND R0, R0, #0       ; Clear R0
    STR R0, R1, #0       ; Null-terminate the string (\0)
    LEA R0, GREET        ; Print "Hi "
    PUTS
    LD R0, NAME_PTR      ; Print the captured name
    PUTS
    HALT
GREET .STRINGZ "\nHi "
NAME_PTR .FILL x3015     ; Buffer starts here
.END
```

##  How To Run

Follow these steps to get the LC-3 VM running on your local machine.

### 1. Prerequisites
Ensure you have a C++ compiler (GCC or Clang)

### 2. Compilation
Navigate to your project root and run the following command. 

```bash
g++ -I include src/*.cpp -o vm
```

### 3. Loading a Program

The Virtual Machine supports two methods for loading instructions into memory:

#### **A. Vector Mode (Hardcoded)**
This is ideal for quick testing or small snippets. You can hardcode your hex instructions directly in `main.cpp` using the `loadProgramVector()` method:

```cpp
std::vector<uint16_t> my_program = {
    0x5020, // AND R0, R0, #0
    0x1025, // ADD R0, R0, #5
    0xF025  // HALT
};

lc3->loadProgramVector(my_program);
```

#### **B. OBJ File method**
For larger assembly projects, you can load a compiled LC-3 .obj binary file. The VM automatically handles the origin address (usually 0x3000) and converts Big-Endian LC-3 files to the host machine's format:
```cpp

    lc3->loadProgramFile("name-out.obj");
    lc3->run();
```
