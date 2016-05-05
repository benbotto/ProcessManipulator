# ProcessManipulator
ProcessManipulator is a library that helps with manipulating arbitrary 32-bit processes in Windows, and it's
particularly helpful for hacking video games. The library can be used to read and write memory from and to 
an arbitrary process, load and unload libraries, get handles to modules (DLLs and executables) in an
external process's address space, and call exported (extern) __stdcall and __cdecl functions.

One of my driving reasons for writing this software is that, although there are many DLL injectors
available, most are deficient in my opinion.  There are very few operations that can be performed safely in
`DllMain` (refer to
[MSDN's DllMain documentation](https://msdn.microsoft.com/en-us/library/windows/desktop/ms682583(v=vs.85).aspx)).
For example, threads should not be created in `DllMain`, nor should functions form `user32` be called. Yet, most
DLL injectors provide examples that immediately spawn a thread or create a MessageBox.  This is unsafe, and can
lead to difficult-to-diagnose errors.  `ProcessManipulator` handles this by allowing the user to export functions
in their DLL, and then call those function after the DLL has been loaded.  This can be used for setup and teardown
operations, allowing the user to safely create threads, load additional libraries, and call managed functions.
Likewise, most other DLL injectors ignore the return value of `LoadLibrary`, and this makes it hard to "uninject"
DLLs because the `HANDLE` to the DLL module is needed.  `ProcessManipulator` makes freeing DLLs trivial.

Documentation is light, but the code is thoroughly commented and a series of examples are included to help
developers get up and running quickly.  The examples should be compiled in Visual Studio 2015.  The AssaultCube
examples run against version 1.2.0.2.

##### Table of Contents
- [Constructor](#constructor)
- [WriteMemory](#writememory)
- [ReadMemory](#readmemory)
- [Malloc](#malloc)
- [Free](#free)

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

##### ReadMemory
This function allows the user to read memory from an arbitrary process.  Parameters are as follows:

- `void* address` A pointer to the base address in the specified process from which to read.  The memory region must
  be readable or a `ProcessManipulatorException` will be raised.
- `void* data` A pointer to a buffer that receives the contents from the address space of the specified process.
- `size_t dataSize` The number of bytes to be read from the specified process.

The function returns the number of bytes read from the foreign process.  If a failure occurs, a `ProcessManipulatorException`
is raised with a description of the error.

##### Malloc
Use this function to allocate memory in the arbitrary process.  It is the user's responsobility to clean the memory up using the `Free` method.  This method takes a single parameter:

- `const unsigned& numBytes` The number of bytes to allocate in the foreign process.

The method returns the address of the allocated memory in the foreign process's address space.  The memory is given read and write priveleges (`PAGE_READWRITE`).  If a failure occurs, a `ProcessManipulatorException`
is raised with a description of the error.

##### Free
This method deallocates memory in the foreign process, and takes a single parameter:

- `void* addr` The address of the memory to free.

No size argument is needed; the memory is freed using a `freeType` of `MEM_RELEASE`.  If a failure occurs, a `ProcessManipulatorException` is raised with a description of the error.
