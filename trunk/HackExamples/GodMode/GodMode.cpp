#include <iostream>
#include <iomanip>
#include <Windows.h>
#include "..\..\ProcessManipulator\ProcessManipulator.h"
using std::cout;
using std::endl;
using std::hex;
using std::dec;

typedef unsigned char byte;
typedef unsigned int  uint;

__declspec(naked) void godMode()
{
  __asm
  {
    add edx,[ebp+0x8]
    nop
  }
}

int main()
{
  try
  {
    byte*  pGodMode = reinterpret_cast<byte*>(godMode);
    DWORD* pDecLife = reinterpret_cast<DWORD*>(0x401056);
    uint  godSize;
    ProcessManipulator pm("BensGame");

    // Calculate the size of the godMode function.
    cout << hex;
    for (godSize = 0; pGodMode[godSize] != 0x90; ++godSize)
      cout << " 0x" << (uint)pGodMode[godSize] << ' ';
    cout << endl;
    cout << dec << godSize << endl;

    // Overwrite the instruction.
    pm.WriteMemory(pDecLife, pGodMode, godSize);
    system("pause");
  }
  catch (exception ex)
  {
    cout << ex.what() << endl;
    system("pause");
  }
  return 0;
}