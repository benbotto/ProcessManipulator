#ifdef _MANAGED
#pragma managed(push, off)
#endif

#include <Windows.h>
#include <sstream>
#include <cmath>
using std::ostringstream;
using std::hex;
using std::dec;

HANDLE  hndThread;
bool    flagExit;
HANDLE  hndMutex;

struct Player
{
  char unknown[0x34];
  float x;
  float y;
  float z;
  float yaw;
  float pitch;
};

/*****************************************************************************
* Aimbot loop.
*****************************************************************************/
DWORD WINAPI aim(void* unused)
{
  DWORD acEXEAddr = (DWORD)GetModuleHandle("ac_client.exe");

  // Init.
  DWORD     p1Offset = 0x109B74;
  Player**  p1       = reinterpret_cast<Player**>(acEXEAddr + p1Offset);
  DWORD     p2Offset = 0x10F4F8;
  Player*** p2       = reinterpret_cast<Player***>(acEXEAddr + p2Offset);
  const float PI     = 3.14159265f;

  // Debug info.
  /*ostringstream pInfo;

  pInfo << "ac_client.exe is at: " << hex << acEXEAddr << ' '
    << "Player1 is at: " << hex << acEXEAddr + p1Offset << ' ' << dec
    << "Yaw: " << (*p1)->yaw << ' '
    << "Pitch:" << (*p1)->pitch << ' '
    << "X: " << (*p1)->x << ' '
    << "Y: " << (*p1)->y << ' '
    << "Z: " << (*p1)->z << ' '

    << "Player2 is at: " << hex << acEXEAddr + p2Offset << ' ' << dec
    << "Yaw: " << (**p2)->yaw << ' '
    << "Pitch:" << (**p2)->pitch << ' '
    << "X: " << (**p2)->x << ' '
    << "Y: " << (**p2)->y << ' '
    << "Z: " << (**p2)->z << ' ';

  MessageBox(NULL, pInfo.str().c_str(), "", MB_OK);*/

  WaitForSingleObject(hndMutex, INFINITE);

  while (!flagExit)
  {
    ReleaseMutex(hndMutex);

    /**
     * AC uses a left-handed coordinate system.  Facing 180 degrees:
     * Left is positive x (thumb).
     * Forward is positive y (index finger).
     * Up is positive z (middle finger).
     * Yaw right is positive (clockwise about z).
     * Pitch up is positive (clockwise about x).
     */

    float xDist   = (**p2)->x - (*p1)->x;
    float yDist   = (**p2)->y - (*p1)->y;
    float zDist   = (**p2)->z - (*p1)->z;
    float horDist = std::sqrt(std::pow(xDist, 2) + std::pow(yDist, 2));

    // Because a left-hand system is used, the x and y are flipped, then the
    // angle is negated.  The -180 inside the parens gives an angle between
    // -360 and 0.  Pitch is trivial.
    (*p1)->yaw   = -(std::atan2(xDist, yDist)  * 180 / PI - 180);
    (*p1)->pitch = std::atan2(zDist,  horDist) * 180 / PI;

    WaitForSingleObject(hndMutex, INFINITE);
  }

  return 0;
}

/*****************************************************************************
* Initialize the DLL, and start a thread.
*****************************************************************************/
extern "C" __declspec(dllexport) void __cdecl initialize()
{
  // A mutex and flag combo is used for unloading the DLL.
  flagExit = false;
  hndMutex = CreateMutex(NULL, FALSE, NULL);

  // Start the thread.
  hndThread = CreateThread(NULL, 0, aim, NULL, 0, NULL);
}

/*****************************************************************************
* End the thread
*****************************************************************************/
extern "C" __declspec(dllexport) void __cdecl cleanup()
{
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
  return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif