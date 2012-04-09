#ifdef _MANAGED
#pragma managed(push, off)
#endif

#include <Windows.h>
#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::hex;

/*****************************************************************************
 * Main entry point.
*****************************************************************************/
BOOL APIENTRY DllMain(HMODULE hModule, DWORD callReason, LPVOID reserved)
{
  unsigned* bullets;

  switch (callReason)
  {
    case DLL_PROCESS_ATTACH:
      cout << "DLL_PROCESS_ATTACH\nThe module handle is: 0x"  
           << hex << hModule << "\nSetting bullets to 220" << endl;

      // Change the bullets.
      bullets  = reinterpret_cast<unsigned int*>(0x18FED0);
      *bullets = 220;
      break;
    case DLL_THREAD_ATTACH:
      cout << "DLL_THREAD_ATTACH" << endl;
      break;
    case DLL_PROCESS_DETACH:
      cout << "DLL_PROCESS_DETACH" << endl;
      break;
    case DLL_THREAD_DETACH:
      cout << "DLL_THREAD_DETACH" << endl;
      break;
  }
  return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif