# ProcessManipulator
ProcessManipulator is a library that helps with manipulating arbitrary 32-bit processes in Windows, and it's
particularly helpful for hacking video games. The library can be used to read and write memory from and to 
an arbitrary process, load and unload libraries, get handles to modules (DLLs and executables) in an
external process's address space, and call exported (extern) __stdcall and __cdecl functions.  A series of
examples are included to help developers get up and running quickly.  The examples should be compiled in
Visual Studio 2015.

##### Table of Contents
- [Constructor](#constructor)
- [WriteMemory](#writememory)

##### Constructor
The `ProcessManipulator` constructor takes a single argument, which is the title/name of a window.  For example:

```c++
ProcessManipulator pm("AssaultCube");
```

##### WriteMemory
This function allows the user to write memory to an arbitrary process.  Parameters are as follows:

- `void* address` A pointer to the base address in the specified process to which `data` is written.
  The method first sets this region of memory to `PAGE_EXECUTE_READWRITE`, so the memory is guaranteed
  to be writable.
- `void* data` A pointer to the buffer that contains `data` to be written in the address space of the specified process.
- `size_t dataSize` The number of bytes to be written to the specified process (i.e. the size of `data`).

The function returns the number of bytes written to the foreign process.  If a failure occurs, a `ProcessManipulatorException`
is raised with a description of the error.

For an example, refer to `GodMode.cpp`, which gives the player in `BensGame` unlimited life.
