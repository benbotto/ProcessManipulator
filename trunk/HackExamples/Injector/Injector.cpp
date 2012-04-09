#include <string>
#include <iostream>
#include "..\..\ProcessManipulator\ProcessManipulator.h"
#include "..\..\ProcessManipulator\ProcessManipulatorException.h"
using std::string;
using std::cout;
using std::endl;

int main()
{
  try
  {
    ProcessManipulator pm("BensGame");
    const unsigned     MAXLEN = 256;
    char               curDir[MAXLEN];
    string             dllName;// = "Z:\\Programming\\C++\\Security\\ProcessManip\\ProcessManipulator\\HackExamples\\ChangeBullets2\\Release\\ChangeBullets2.dll";
    HMODULE            hMod;
    void*              initAddr;
    void*              cleanupAddr;
    DWORD              args[] = {110};

    /*
     * ChangeBullets2.dll
    */

    // Resolve the DLL name path.
    GetCurrentDirectory(MAXLEN, curDir);
    dllName  = curDir;
    dllName += "\\..\\..\\ChangeBullets2\\Release\\ChangeBullets2.dll";

    cout << "\n\nInjecting the DLL: " << dllName << "\n\n";
    hMod = pm.LoadLibrary(dllName);
    cout << "Module handle: 0x" << std::hex << hMod << "\n\n";

    cout << "Removing the DLL.\n" << endl;
    pm.FreeLibrary(hMod);

    /*
     * Regenerate.dll
    */

    // Resolve the DLL name path.
    dllName  = curDir;
    dllName += "\\..\\..\\Regenerate\\Release\\Regenerate.dll";

    cout << "\n\nInjecting the DLL: " << dllName << "\n\n";
    hMod = pm.LoadLibrary(dllName);
    cout << "Module handle: 0x" << std::hex << hMod << "\n\n";

    cout << "Getting the address of initialize: ";
    initAddr = pm.GetProcAddress(hMod, "initialize");
    cout << "0x" << initAddr << "\n\n";

    cout << "Calling initialize.\n\n";
    pm.CallFunction(initAddr, args, 1);

    cout << "Regenerating for 10 seconds.\n" << endl;
    Sleep(10000);

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
