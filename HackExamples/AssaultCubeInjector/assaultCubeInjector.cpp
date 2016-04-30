#include <string>
#include <iostream>
#include "..\..\ProcessManipulator\ProcessManipulator.h"
#include "..\..\ProcessManipulator\ProcessManipulatorException.h"
using std::string;
using std::cout;
using std::endl;
using std::cin;

int main()
{
  try
  {
    ProcessManipulator pm("AssaultCube");
    const unsigned     MAXLEN = 256;
    char               curDir[MAXLEN];
    string             dllName;
    HMODULE            hMod;
    void*              initAddr;
    void*              cleanupAddr;
    DWORD              args[] = { 110 };

    // Resolve the DLL name path.
    GetCurrentDirectory(MAXLEN, curDir);
    dllName = curDir;
    dllName += "\\..\\..\\AssaultCubeAimbot\\Release\\AssaultCubeAimbot.dll";

    cout << "\n\nInjecting the DLL: " << dllName << "\n\n";
    hMod = pm.LoadLibrary(dllName);
    cout << "Module handle: 0x" << std::hex << hMod << "\n\n";

    cout << "Getting the address of initialize: ";
    initAddr = pm.GetProcAddress(hMod, "initialize");
    cout << "0x" << initAddr << "\n\n";

    cout << "Calling initialize.\n\n";
    pm.CallFunction(initAddr, args, 1);

    cout << "Press enter to unload the DLL." << endl;
    cin.get();

    cout << "Getting the address of cleanup: ";
    cleanupAddr = pm.GetProcAddress(hMod, "cleanup");
    cout << "0x" << cleanupAddr << "\n\n";

    cout << "Calling cleanup.\n\n";
    pm.CallFunction(cleanupAddr, NULL, 0);

    cout << "Removing the DLL." << endl;
    pm.FreeLibrary(hMod);
  }
  catch (ProcessManipulatorException ex)
  {
    cout << ex.what() << endl;
  }
}
