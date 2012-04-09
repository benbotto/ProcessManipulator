#ifdef _MANAGED
#pragma managed(push, off)
#endif

#include <Windows.h>
#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::hex;
using std::dec;

HANDLE    hndThread;
bool      flagExit;
unsigned* life;
HANDLE    hndMutex;

/*****************************************************************************
 * Regenerate one life per second.
*****************************************************************************/
DWORD WINAPI regenerate(void* unused)
{
  WaitForSingleObject(hndMutex, INFINITE);

  while (!flagExit)
  {
    ReleaseMutex(hndMutex);

    cout << "Tick" << endl;
    ++*life;
    Sleep(1000);

    WaitForSingleObject(hndMutex, INFINITE);
  }

  return 0;
}

/*****************************************************************************
 * Initialize the DLL, and start a thread.
*****************************************************************************/
extern "C" __declspec(dllexport) void __cdecl initialize(DWORD initLife)
{
  // Init.
  life     = reinterpret_cast<unsigned int*>(0x18FEB8);
  flagExit = false;
  hndMutex = CreateMutex(NULL, FALSE, NULL);

  cout << "Initialize called.  Setting life to: " << dec << initLife << endl;

  // Change the bullets.
  *life = initLife;
  
  // Start the regeneration thread.
  cout << "Starting the regeneration thread." << endl;
  hndThread = CreateThread(NULL, 0, regenerate, NULL, 0, NULL);
}

/*****************************************************************************
 * End the thread
*****************************************************************************/
extern "C" __declspec(dllexport) void __cdecl cleanup()
{
  cout << "Cleanup called - killing the thread." << endl;
  WaitForSingleObject(hndMutex, INFINITE);
  flagExit = true;
  ReleaseMutex(hndMutex);
  WaitForSingleObject(hndThread, INFINITE);
  CloseHandle(hndThread);
  CloseHandle(hndMutex);
}

/*****************************************************************************
 * Main entry point.
*****************************************************************************/
BOOL APIENTRY DllMain(HMODULE hModule, DWORD callReason, LPVOID reserved)
{  
  switch (callReason)
  {
    case DLL_PROCESS_ATTACH:
      cout << "DLL_PROCESS_ATTACH\n"
           << "\nAddress of initialize: 0x" << reinterpret_cast<void*>(initialize)
           << endl;
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