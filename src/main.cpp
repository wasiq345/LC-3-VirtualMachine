#include "../include/vm.h"
#include <string>
#include <iostream>
#include <thread>
#include <chrono>

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

void handle_interrupt(int signal)
{
    restore_input_buffering();
    printf("\n");
    exit(-2);
}

int main()
{
    signal(SIGINT, handle_interrupt);

    std::cout << "\033[1;36m" 
              << "┌────────────────────────────────────────────┐\n"
              << "│          LC-3 Virtual Machine              │\n"
              << "└────────────────────────────────────────────┘\033[0m\n\n";

    std::cout << "Loading program... ";
    const char* spinner = "|/-\\";
    for (int i = 0; i < 12; ++i) {
        std::cout << "\b" << spinner[i % 4] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }
    std::cout << "\b \n\n";

    disable_input_buffering();

    VirtualMachine* lc3 = new VirtualMachine();

    std::cout << "\033[1;32mProgram loaded successfully\033[0m\n"
              << "Starting execution...\n\n";

    lc3->loadProgramFile("name-out.obj");
    lc3->run();

    std::cout << "\n\033[1;33m────────────────────────────────────────────\033[0m\n"
              << "\033[1;32mExecution completed\033[0m\n";

    restore_input_buffering();
    delete lc3;

    return 0;
}
