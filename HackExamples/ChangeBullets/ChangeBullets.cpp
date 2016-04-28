#include <iostream>
#include <iomanip>
#include <Windows.h>
using std::cout;
using std::endl;
using std::hex;
using std::dec;

typedef unsigned int uint;

int main()
{
  HWND             hwnd;
  DWORD            pid;
  DWORD            threadID;
  HANDLE           hndThisProc;
  HANDLE           hndToken;
  HANDLE           hndProc;
  LUID             luid;
  DWORD*           bulletsAddr = reinterpret_cast<DWORD*>(0x0018FEF4);
  uint             bullets;
  uint             newBullets = 100;
  TOKEN_PRIVILEGES tp;

  // Find the window.
  if (!(hwnd = FindWindow(NULL, "BensGame")))
  {
    cout << "Failed to find the window." << endl;
    return 1;
  }
  cout << hex << "Window handle: " << hwnd << endl;

  // Get the PID (threadID is unused).
  threadID = GetWindowThreadProcessId(hwnd, &pid);
  if (!pid)
  {
    cout << "Failed to get the PID." << endl;
    return 0;
  }
  cout << "ProcessID: " << pid << " ThreadID: " << threadID << endl;

  /*
   * This process (the ChangeBullets process) needs permissions to "debug" the
   * BensGame process.
  */

  // Get the current process.
  hndThisProc = GetCurrentProcess();
  cout << "This process's handle is: " << hndThisProc << endl;

  // Get this process's current privileges.
  OpenProcessToken(hndThisProc, TOKEN_ALL_ACCESS, &hndToken);
  cout << "Current privileges handle: " << hndToken << endl;

  // We want debug privileges on the BensGame process.  Find the debug id.
  LookupPrivilegeValue(NULL, "SeDebugPrivilege", &luid);
  cout << "SeDebugPrivilege locally unique identifier.  Low: "
       << luid.LowPart << " High: " << luid.HighPart << endl;

  // Change this process's privilege (note: this must be run as Administrator).
  tp.PrivilegeCount           = 1;
  tp.Privileges[0].Luid       = luid;
  tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  AdjustTokenPrivileges(hndToken, false, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
  if (GetLastError() != ERROR_SUCCESS)
  {
    cout << "Failed to adjust privileges." << endl;
    return 1;
  }

  // Open the process.
  if (!(hndProc = OpenProcess(PROCESS_ALL_ACCESS, false, pid)))
  {
    cout << "Failed to open the process." << endl;
    return 1;
  }
  cout << "Process handle: " << hndProc << endl;

  // Get the number of bullets.
  if (!ReadProcessMemory(hndProc, bulletsAddr, &bullets, sizeof(uint), NULL))
  {
    cout << GetLastError() << endl;
    cout << "Failed to read memory." << endl;
    return 1;
  }
  cout << "\nNumber of bullets: " << dec << bullets << endl;

  // Change the number of bullets.
  if (!WriteProcessMemory(hndProc, bulletsAddr, &newBullets, sizeof(uint), NULL))
  {
    cout << "Failed to write memory." << endl;
    return 1;
  }

  system("pause");
  return 0;
}
