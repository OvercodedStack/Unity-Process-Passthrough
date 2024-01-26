#pragma once

#ifdef StartProcessUnity_EXPORTS
#define StartProcessUnity_API __declspec(dllexport)
#else // StartProcessUnity_EXPORTS
#define StartProcessUnity_API __declspec(dllimport)
#endif
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>

//Testing functions
extern "C" StartProcessUnity_API void TestCall();					// Use this only if you want to ping the .DLL
extern "C" StartProcessUnity_API void StoreTestValue(int value);	// Use this to "store" a value in this .DLL. This I am only using to test the theory that .DLLs can exist alone and keep a variable (which matters in order to store pointers)
extern "C" StartProcessUnity_API int RetrieveValue();				// Same concept, only this shovels out a variable out to Unity. 
int storedValue;

//Unity External Processing API
extern "C" StartProcessUnity_API int StartProcess(
	 const char* executablePath, 
	 const char* argument,
	 const char* directory);	// Creates a new CMD process.
extern "C" StartProcessUnity_API int WriteToActiveProcess(const char* inputString);				// Talks to an active CMD process.
extern "C" StartProcessUnity_API char* ReadFromActiveProcess();								// Retrieves CMD process responses.
extern "C" StartProcessUnity_API void KillAllProcessHandles();								// Kills active process and pipes. 

//External process properties for I/O 
HANDLE outCMDPtr_Rd = NULL;
HANDLE outCMDPtr_Wr = NULL;
HANDLE inCMDPtr_Rd	= NULL;
HANDLE inCMDPtr_Wr	= NULL;
HANDLE processID	= NULL;



BOOL initializedProcess = FALSE;			//Is the process active?

PROCESS_INFORMATION processInfo;
