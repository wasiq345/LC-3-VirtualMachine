#include "../include/vm.h"

int main()
{
    VirtualMachine* lc3 = new VirtualMachine();
    //lc3->loadProgramFile("2048.obj");
//   std::vector<uint16_t> simple_moving = {
//     // Load VRAM address
//     0x5020,  // 0x3000: AND R0, R0, #0
//     0x200C,  // 0x3001: LD R0, VRAM
    
//     // Load white color
//     0x5260,  // 0x3002: AND R1, R1, #0
//     0x14BF,  // 0x3003: ADD R1, R1, #-1    (R1 = 0xFFFF white)
    
//     // Loop: just keep drawing
//     0x7200,  // 0x3004: STR R1, R0, #0     (Draw)
//     0x1021,  // 0x3005: ADD R0, R0, #1     (Move)
//     0x0FFE,  // 0x3006: BRnzp #-2          (Loop)
    
//     // Padding
//     0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    
//     // Data
//     0xC000   // 0x300D: VRAM
// };
//     lc3->loadProgramVector(simple_moving);
    //lc3->run();

    

     lc3->write(0xC000, 0x7C00);  // Red
    lc3->write(0xC001, 0x03E0);  // Green
    lc3->write(0xC002, 0x001F);  // Blue
    lc3->write(0xC003, 0x7FFF);  // white

    InitWindow(512, 512, "LC-3 VM");
    SetTargetFPS(60);


   while(!WindowShouldClose())
{
    for(int i = 0; i < 10 && lc3->getRunning(); i++)
    {
        lc3->runStep();
    }
    
    BeginDrawing();
    ClearBackground(BLACK);
    lc3->renderScreen();  // Only draws non-black pixels
    EndDrawing();
}

   delete lc3;
    return 0;
}
