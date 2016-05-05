#ifndef _PROCESS_MANIPULATOR_H_
#define _PROCESS_MANIPULATOR_H_

#include <Windows.h>
#include <string>
#include <queue>
#include "ProcessManipulatorException.h"
using std::string;
using std::queue;

class ProcessManipulator
{
  string    _windowName;
  HWND      _hwnd;
  DWORD     _pid;
  DWORD     _threadID;
  HANDLE    _hndProc;
  unsigned  _stdProcLen;
  unsigned  _cProcLen;
  enum      {C, STD};
  static const DWORD eof = 0x90909090;

  // Calls an exported function in a dll, passing args.  Only for functions
  // that are in DLLs loaded by this process (i.e. kernel32.dll).
  DWORD CallFunction(const string& dllName, const string& functionName,
    const DWORD* args, DWORD numArgs) const;

  // Same as above but without using GetProcAddress, and with a calling 
  // convention (__cdecl or __stdcall).
  DWORD CallFunction(const void* procAddr, const DWORD* args,
    DWORD numArgs, unsigned convention) const;
public:
  ProcessManipulator(const string& windowName);
  
  // Write/read memory from the arbitrary process.
  SIZE_T WriteMemory(void* address, void* data, size_t dataSize) const;
  SIZE_T ReadMemory(void* address, void* data, size_t dataSize) const;
  
  // Allocate or free memory in the process.
  void* Malloc(unsigned numBytes) const;
  void Free(void* addr) const;

  // Calls an exported function in a dll, passing args.  The function must
  // have the following signature:
  // extern "C" __declspec(dllexport) void __cdecl initialize(DWORD arg1, DWORD argN)
  DWORD CallFunction(const void* procAddr, const DWORD* args, DWORD numArgs) const;

  // Returns the the handle to the module in the arbitrary process for
  // later freeing.  The module _should_ return TRUE from DllMain so that the
  // calling thread can be freed, and the module handle can be retrieved.
  // DLL initialization should be done in a separate thread.
  HMODULE LoadLibrary(const string& dllName) const;
  // Extract a DLL from the process.
  BOOL FreeLibrary(const HMODULE& hmod) const;

  // Get a handle to a module in the arbitrary process.
  HMODULE GetModuleHandle(const string& dllName) const;

  // Get the address of an exported function in a DLL that the process has loaded.
  void* GetProcAddress(const HMODULE& hmod, const string& functionName) const;

  ~ProcessManipulator();
};

#endif
