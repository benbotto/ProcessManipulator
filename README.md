# ProcessManipulator
ProcessManipulator is a library that helps with manipulating arbitrary 32-bit processes in Windows, and it's
particularly helpful for hacking video games. The library can be used to read and write memory from and to 
an arbitrary process, load and unload libraries, get handles to modules (DLLs and executables) in an
external process's address space, and call exported (extern) __cdecl functions.

One of my driving reasons for writing this software is that, although there are many DLL injectors
available, most are deficient in my opinion.  There are very few operations that can be performed safely in
`DllMain` (refer to
[MSDN's DllMain documentation](https://msdn.microsoft.com/en-us/library/windows/desktop/ms682583(v=vs.85).aspx)).
For example, threads should not be created in `DllMain`, nor should functions form `user32` be called. Yet, most
DLL injectors provide examples that immediately spawn a thread or create a MessageBox.  This is unsafe, and can
lead to difficult-to-diagnose errors.  `ProcessManipulator` handles this by allowing the user to export functions
in their DLL, and then call those functions after the DLL has been loaded.  This can be used for setup and teardown
operations, allowing the user to safely create threads, load additional libraries, and call managed functions.
Likewise, most other DLL injectors ignore the return value of `LoadLibrary`, and this makes it hard to "uninject"
DLLs because the handle to the DLL module (`HMODULE`) is needed.  `ProcessManipulator` makes freeing DLLs trivial.

Documentation is light, but the code is thoroughly commented and a series of examples are included to help
developers get up and running quickly.  The examples should be compiled in Visual Studio 2015.  The AssaultCube
examples run against version 1.2.0.2.

**This solution must be built in release mode**; it absolutely will not work in debug mode.

##### Table of Contents
- [Constructor](#constructor)
- [WriteMemory](#writememory)
- [ReadMemory](#readmemory)
- [LoadLibrary](#loadlibrary)
- [FreeLibrary](#freelibrary)
- [CallFunction](#callfunction)
- [Malloc](#malloc)
- [Free](#free)
- [GetModuleHandle](#getmodulehandle)
- [GetProcAddress](#getprocaddress)

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

##### LoadLibrary
This method is used to inject a DLL into the foreign process.  The method takes a single argument:

- `const string& dllName` The name of the DLL to load.  It's best to use the absolute path to the DLL; otherwise, the DLL must be in the system %PATH%.  (The external process must be able to find the DLL in order to load it.)

A handle to the loaded module (`HMODULE`) is returned.  This handle should be kept so that the module can be freed using the [FreeLibrary](#freelibrary) method.

For a trivial example, see `injector.cpp` which injects the DLL created by the `ChangeBullets2.cpp` file (the DLL changes the players bullets in the `BensGame` process).  Often a setup function needs to be called after the DLL is loaded.  Refer to the [CallFunction](#callfunction) documentation below for an example of calling an initialization function.

##### FreeLibrary
Use this method to remove a library from the foreign process.  A single parameter is needed:

- `const HMODULE& hmod` The handle to the module.  This handle is returned from the [LoadLibrary](#loadlibrary) method, and can also be attained using the [GetModuleHandle](#getmodulehandle) method.

A boolean is returned indicating whether or not the library was successfully unloaded.

For a simple example, see `injector.cpp` and `ChangeBullets2.cpp`.

##### CallFunction
Use this method to call an exported function in the foreign process.  The function must have the following signature:

```c++
extern "C" __declspec(dllexport) void __cdecl initialize(DWORD arg1, DWORD argN)
```

This method is helpful for calling an initialization function after a DLL has been injected, or calling a
teardown function before a DLL is unloaded.  Parameters are as follows:

- `const void* procAddr` A pointer to the address of the function in the specified process from which to read.
  Use the [GetProcAddress](#getprocaddress) method to find the address of an exported function in the foreign process.
- `const DWORD* args` A pointer to an array of DWORD arguments (the arguments can really be cast to any type).  This
  method will automatically write the arguments to the foreign process.
- `const DWORD& numArgs` The number of arguments provided in `args`.

The method returns a `DWORD` that is the return value of the called function in the arbitrary process.

For an example, refer to `Regenerate.cpp` which exports an `initialize` method.  `initialize` is in turn called by `injector.cpp`
with a single argument.  The example demonstrates how to safely create a thread in an injected DLL, and then how to safely remove the DLL after calling a `cleanup` function.


##### Malloc
Use this function to allocate memory in the arbitrary process.  It is the user's responsobility to clean the memory up using the `Free` method.  This method takes a single parameter:

- `const unsigned& numBytes` The number of bytes to allocate in the foreign process.

The method returns the address of the allocated memory in the foreign process's address space.  The memory is given read and write priveleges (`PAGE_READWRITE`).  If a failure occurs, a `ProcessManipulatorException`
is raised with a description of the error.

##### Free
This method deallocates memory in the foreign process, and takes a single parameter:

- `void* addr` The address of the memory to free.

No size argument is needed; the memory is freed using a `freeType` of `MEM_RELEASE`.  If a failure occurs, a `ProcessManipulatorException` is raised with a description of the error.

##### GetModuleHandle
This method gets the handle of a module (`HMODULE`) _in the foreign process's address space_.  A single parameter is required:

-`const string& dllName` The name of the dll (or executable) for which the a handle shall be retrieved.

An `HMODULE` is returned.

##### GetProcAddress
Get the address of a function in the foreign process.  Parameters are as follows:

- `const HMODULE& hmod` A handle to the module that exports the function.  This can be obtained from the [LoadLibrary](#loadlibrary) method in the case of an injected DLL, or by using the [GetModuleHandle](#getmodulehandle) method.
- `const string& functionName` The name of the function.  Keep in mind that many WINAPI function have both a unicode and a multi-byte version (e.g. `MessageBoxW` and `MessageBoxA`).

A pointer to the address in the foreign process's address space is returned.
