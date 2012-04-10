#include "ProcessManipulator.h"

/*****************************************************************************
 * DWORD WINAPI ThreadProc(void* pArgs) function that calls a WINAPI with an
 * arbitrary number of arguments.  pArgs points to a struct that looks like:
 * DWORD numArgs
 * DWORD arg1
 * DWORD arg2
 * DWORD argN
 * DWORD pFuncToCall
*****************************************************************************/
__declspec(naked) void stdProc()
{ 
  __asm
  {
    push ebp
    mov ebp, esp
    push ecx

    // Address of arguments[0].
    mov eax, [ebp + 0x8]

    // for (int i = numArgs; i > 0; --i)
    mov ecx, [eax]
    for_each_arg:
    cmp ecx, 0
    je for_each_arg_end

    // push arguments[i];
    push [eax + 4 * ecx]
    dec ecx
    jmp for_each_arg
    for_each_arg_end:

    // ex: GetProcAddress(HMODULE hmod, LPCSTRING procName);
    mov ecx, [eax]
    inc ecx
    mov eax, [eax + 4 * ecx]
    call eax

    // Restore.
    pop ecx
    mov esp, ebp
    pop ebp

    // This is a WINAPI (__stdcall).  Callee does the stack cleanup.
    ret 4

    // End-of-function signature for calculating the length.
    nop
    nop
    nop
    nop
  }
}

/*****************************************************************************
 * DWORD WINAPI ThreadProc(void* pArgs) function that calls a __cdecl with an
 * arbitrary number of arguments.  pArgs points to a struct that looks like:
 * DWORD numArgs
 * DWORD arg1
 * DWORD arg2
 * DWORD argN
 * DWORD pFuncToCall (must use a __cdecl calling convention).
*****************************************************************************/
__declspec(naked) void cProc()
{ 
  __asm
  {
    push ebp
    push ecx
    mov ebp, esp

    // Address of arguments[0].
    mov eax, [ebp + 0x0C]

    // for (int i = numArgs; i > 0; --i)
    mov ecx, [eax]
    for_each_arg:
    cmp ecx, 0
    je for_each_arg_end

    // push arguments[i];
    push [eax + 4 * ecx]
    dec ecx
    jmp for_each_arg
    for_each_arg_end:

    // ex: GetProcAddress(HMODULE hmod, LPCSTRING procName);
    mov ecx, [eax]
    inc ecx
    mov eax, [eax + 4 * ecx]
    call eax

    // Caller cleans up the stack (__cdecl).
    mov esp, ebp
    pop ecx
    pop ebp

    // This is a WINAPI (__stdcall).  Callee does the stack cleanup.
    ret 4

    // End-of-function signature for calculating the length.
    nop
    nop
    nop
    nop
  }
}

/*****************************************************************************
 * Initialze the process.
*****************************************************************************/
ProcessManipulator::ProcessManipulator(const string& windowName)
{
  HANDLE           hndThisProc;
  HANDLE           hndToken;
  LUID             luid;
  TOKEN_PRIVILEGES tp;
  BYTE*            pStdProc = reinterpret_cast<BYTE*>(stdProc);
  BYTE*            pCProc   = reinterpret_cast<BYTE*>(cProc);

  // Find the window.
  if (!(_hwnd = FindWindow(NULL, windowName.c_str())))
    throw ProcessManipulatorException("Failed to find window.");

  // Get the PID (threadID is unused).
  _threadID = GetWindowThreadProcessId(_hwnd, &_pid);

  if (!_pid)
    throw ProcessManipulatorException("Failed to get the PID");

  // Get the current process.
  hndThisProc = GetCurrentProcess();

  // Get this process's current privileges.
  OpenProcessToken(hndThisProc, TOKEN_ALL_ACCESS, &hndToken);

  // Find the debug privilege id.
  LookupPrivilegeValue(NULL, "SeDebugPrivilege", &luid);

  // Change this process's privilege (note: this must be run as Administrator).
  tp.PrivilegeCount            = 1;
  tp.Privileges[0].Luid       = luid;
  tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  AdjustTokenPrivileges(hndToken, false, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
  if (GetLastError() != ERROR_SUCCESS)
    throw ProcessManipulatorException("Failed to adjust privileges.");

  // Open the process.
  if (!(_hndProc = OpenProcess(PROCESS_ALL_ACCESS, false, _pid)))
    throw ProcessManipulatorException("Failed to open the process.");

  // Get the size of the threadProcs.
  for (_stdProcLen = 0; *reinterpret_cast<DWORD*>(pStdProc + _stdProcLen) != eof; ++_stdProcLen);
  for (_cProcLen   = 0; *reinterpret_cast<DWORD*>(pCProc   + _cProcLen)   != eof; ++_cProcLen);
}

/*****************************************************************************
 * Write memory to the process.
*****************************************************************************/
SIZE_T ProcessManipulator::WriteMemory(void* address, void* data, size_t dataSize) const
{
  DWORD  oldProt;
  SIZE_T bytesWritten;

  // Make sure the address is writable.
  if (!VirtualProtectEx(_hndProc, reinterpret_cast<void*>(address), 
    dataSize, PAGE_EXECUTE_READWRITE, &oldProt))
    throw ProcessManipulatorException("Failed to change permissions.");

  // Write the memory.
  if (!WriteProcessMemory(_hndProc, address, data, dataSize, &bytesWritten))
    throw ProcessManipulatorException("Failed to write memory.");

  return bytesWritten;
}

/*****************************************************************************
 * Read memory from the process.
*****************************************************************************/
SIZE_T ProcessManipulator::ReadMemory(void* address, void* data, size_t dataSize) const
{
  SIZE_T bytesRead;

  // Read the memory.
  if (!ReadProcessMemory(_hndProc, address, data, dataSize, &bytesRead))
    throw ProcessManipulatorException("Failed to write memory.");

  return bytesRead;
}

/*****************************************************************************
 * Allocate numBytes of memory.
*****************************************************************************/
void* ProcessManipulator::Malloc(const unsigned& numBytes) const
{
  void* addr;
  
  addr = VirtualAllocEx(_hndProc, NULL, numBytes, MEM_COMMIT, PAGE_READWRITE);

  if (!addr)
    throw ProcessManipulatorException("Failed to allocate memory.");

  return addr;
}

/*****************************************************************************
 * Free numBytes of memory at address addr.
*****************************************************************************/
void ProcessManipulator::Free(void* addr) const
{
  if (!VirtualFreeEx(_hndProc, addr, 0, MEM_RELEASE))
    throw ProcessManipulatorException("Failed to free memory.");
}

/*****************************************************************************
 * Call an exported function in a DLL.
*****************************************************************************/
DWORD ProcessManipulator::CallFunction(const string& dllName,
  const string& functionName, const DWORD* args, const DWORD& numArgs) const
{
  // Get the address of the function that the thread will call.
  void* procAddr = ::GetProcAddress(::GetModuleHandle(dllName.c_str()), functionName.c_str());
  
  return CallFunction(procAddr, args, numArgs, STD);
}

/*****************************************************************************
 * Call a __cdecl function in a DLL.
*****************************************************************************/
DWORD ProcessManipulator::CallFunction(const void* procAddr, const DWORD* args,
  const DWORD& numArgs) const
{
  return CallFunction(procAddr, args, numArgs, C);
}

/*****************************************************************************
 * Call an exported function in a DLL.
*****************************************************************************/
DWORD ProcessManipulator::CallFunction(const void* procAddr, const DWORD* args,
    const DWORD& numArgs, const unsigned& convention) const
{
  unsigned argSize = numArgs * sizeof(DWORD);
  unsigned asmLen;
  BYTE*    pASM;
  void*    baseAddr;
  void*    numArgsAddr;
  void*    argsAddr;
  void*    asmAddr;
  void*    pProcAddr;
  HANDLE   hndThread;
  DWORD    waitRet;
  DWORD    exitCode;

  // Get the right procedure.
  asmLen = (convention == STD) ? _stdProcLen : _cProcLen;
  pASM   = (convention == STD) ? 
    reinterpret_cast<BYTE*>(stdProc) : reinterpret_cast<BYTE*>(cProc);

  // Allocate space for the arguments, and for the ASM function.
  baseAddr    = Malloc(sizeof(DWORD) + numArgs * sizeof(DWORD) + asmLen);
  numArgsAddr = baseAddr;
  argsAddr    = reinterpret_cast<BYTE*>(baseAddr) + sizeof(DWORD);
  pProcAddr   = reinterpret_cast<BYTE*>(baseAddr) + sizeof(DWORD) + argSize;
  asmAddr     = reinterpret_cast<BYTE*>(baseAddr) + sizeof(DWORD) + argSize + sizeof(DWORD);

  // Write the number of arguments.
  WriteMemory(numArgsAddr, const_cast<DWORD*>(&numArgs), sizeof(DWORD));

  // Write the arguments if there are any.
  if (numArgs)
    WriteMemory(argsAddr, const_cast<DWORD*>(args), argSize);

  // Write the threadProc function.
  WriteMemory(asmAddr, pASM, asmLen);

  // Write the address of the function to call (i.e. LoadLibrary, etc.).
  WriteMemory(pProcAddr, &procAddr, sizeof(DWORD));

  // Create a thread that starts at the dynamic function.
  hndThread = CreateRemoteThread(_hndProc, NULL, 0,
    reinterpret_cast<LPTHREAD_START_ROUTINE>(asmAddr), numArgsAddr, 0, NULL);

  // Wait up to 5 seconds for the thread to finish.
  waitRet = WaitForSingleObject(hndThread, INFINITE);
  //waitRet = WaitForSingleObject(hndThread, 5000);

  if (waitRet != WAIT_OBJECT_0)
    throw ProcessManipulatorException("Failed to wait for the thread.");

  // Get the response from the thread (whatever the called function returned).
  GetExitCodeThread(hndThread, &exitCode);

  // Cleanup the allocated memory.
  CloseHandle(hndThread);
  Free(baseAddr);

  return exitCode;
}

/*****************************************************************************
 * Inject a DLL into the process.
*****************************************************************************/
HMODULE ProcessManipulator::LoadLibrary(const string& dllName) const
{
  void*   dllNameAddr;
  DWORD   arguments[1];
  HMODULE hMod;

  // Write the DLL name to memory.
  dllNameAddr = Malloc(dllName.length() + 1);
  WriteMemory(dllNameAddr, const_cast<char*>(dllName.c_str()), dllName.length() + 1);
  arguments[0] = (DWORD)dllNameAddr;

  hMod = (HMODULE)CallFunction("kernel32.dll", "LoadLibraryA", arguments, 1);

  Free(dllNameAddr);
 
  return hMod;
}

/*****************************************************************************
 * Remove a DLL from the process.
*****************************************************************************/
BOOL ProcessManipulator::FreeLibrary(const HMODULE& hmod) const
{
  DWORD arguments[1];

  // Write the DLL name to memory.
  arguments[0] = (DWORD)hmod;

  return (BOOL)CallFunction("kernel32.dll", "FreeLibrary", arguments, 1);
}

/*****************************************************************************
 * Get a handle to a module that the process has loaded.
*****************************************************************************/
HMODULE ProcessManipulator::GetModuleHandle(const string& dllName) const
{
  void*   dllNameAddr;
  DWORD   arguments[1];
  HMODULE hMod;

  // Write the DLL name to memory.
  dllNameAddr = Malloc(dllName.length() + 1);
  WriteMemory(dllNameAddr, const_cast<char*>(dllName.c_str()), dllName.length() + 1);
  arguments[0] = (DWORD)dllNameAddr;

  hMod = (HMODULE)CallFunction("kernel32.dll", "GetModuleHandleA", arguments, 1);

  Free(dllNameAddr);
 
  return hMod;
}

/*****************************************************************************
 * Get the address of an exported function in a DLL that the process as loaded.
*****************************************************************************/
void* ProcessManipulator::GetProcAddress(const HMODULE& hmod, 
  const string& functionName) const
{
  void*   hmodAddr;
  void*   funcNameAddr;
  void*   funcAddr;
  DWORD   arguments[2];

  // Create some space for the dll name and the function name.
  hmodAddr     = Malloc(sizeof(HMODULE) + functionName.length() + 1);
  funcNameAddr = reinterpret_cast<BYTE*>(hmodAddr) + sizeof(HMODULE);

  // Write the DLL name and the function name to memory.
  WriteMemory(hmodAddr, const_cast<HMODULE*>(&hmod), sizeof(HMODULE));
  WriteMemory(funcNameAddr, const_cast<char*>(functionName.c_str()), functionName.length() + 1);

  arguments[0] = (DWORD)hmod;
  arguments[1] = (DWORD)funcNameAddr;

  funcAddr = (void*)CallFunction("kernel32.dll", "GetProcAddress", arguments, 2);

  Free(hmodAddr);
 
  return funcAddr;
}

/*****************************************************************************
 * Cleanup.
*****************************************************************************/
 ProcessManipulator::~ProcessManipulator()
 {
   CloseHandle(_hndProc);
 }
