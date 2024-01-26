///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Unity Process Passthrough (UPP)
//
//Q:  Why does this small library exist?
//A: https://forum.unity.com/threads/solved-il2cpp-and-process-start.533988/
//   The very short answer is that Unity has a broken StartProcess function which breaks the ability of Unity to start a .exe when the game is compiled as stand-alone.
//   In the past this seemed to work with starting a bare-bones Python processes, however this does not seem to apply well to .exes.
//   As seen by the long list of angry replies on that thread, it seems unlikely it will get fixed soon.
// 
//   Someone decided to monetize on this by making a paid "fix" to the problem but essentially I believe it is similar to what I am doing here.
//   Since I don't want to break the open-sourceness of the CLEVER project, I will just make a short library and make it MIT and solve this problem in the 
//   laziest way possible. 
//
//   As usual, this software has no guarantees and use your best intition to use and determine problems with this library. 
//
/// This is somwhat built using ChatGPT as some start but the main documentation is from MS.
/// https://learn.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output

#include <string>
#include <cstdlib>
#include <iostream>
#include "StartProcessUnity.h"
#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>
#include <chrono>
#include <thread>


#define _CRT_SECURE_NO_WARNINGS

void StoreTestValue(int value) 
{
    storedValue = value;
}

int RetrieveValue()
{
    return storedValue; 
}


void TestCall() 
{
    std::cout << "Testing the output.\n";
    std::cin; 
}

char* MergeChars(const char* str1, const char* str2) {
    char* TotalLine{ new char[strlen(str1) + strlen(str2) + 1] };
#pragma warning(suppress : 4996)
    strcpy(TotalLine, str1);
#pragma warning(suppress : 4996)
    strcat(TotalLine, str2);
    return TotalLine;
}

LPWSTR ConvertCharToOutput(const char* input) {
    wchar_t wtext[250];
#pragma warning(suppress : 4996)
    std::mbstowcs(wtext, input, strlen(input) + 1); 
    LPWSTR ptr = wtext;
    return ptr;
}


int SetupProcessProperties()
{
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT. 
    if (!CreatePipe(&outCMDPtr_Rd, &outCMDPtr_Wr, &saAttr, 0))
        return -2;

    // Ensure the read handle to the pipe for STDOUT is not inherited.

    if (!SetHandleInformation(outCMDPtr_Rd, HANDLE_FLAG_INHERIT, 0))
        return -3;

    // Create a pipe for the child process's STDIN. 

    if (!CreatePipe(&inCMDPtr_Rd, &inCMDPtr_Wr, &saAttr, 0))
        return -4;

    // Ensure the write handle to the pipe for STDIN is not inherited. 

    if (!SetHandleInformation(inCMDPtr_Wr, HANDLE_FLAG_INHERIT, 0))
        return -5;
    return 0; 
}

void KillAllProcessHandles()
{

    if (processInfo.hProcess != NULL)
    {
        TerminateProcess(processInfo.hProcess, 0);
        CloseHandle(processInfo.hProcess);
    }
    if(processInfo.hThread != NULL)
        CloseHandle(processInfo.hThread);
    if(outCMDPtr_Wr != NULL)
        CloseHandle(outCMDPtr_Wr);
    if(outCMDPtr_Rd != NULL)
        CloseHandle(outCMDPtr_Rd);
    if(inCMDPtr_Wr != NULL)
        CloseHandle(inCMDPtr_Wr);
    if(inCMDPtr_Rd != NULL)
        CloseHandle(inCMDPtr_Rd);

}

int StartProcess(const char* executablePath, const char* argument, const char* workingDirectory)
{
    // Set up the process information structure
    STARTUPINFO startupInfo;
    int status = SetupProcessProperties();
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    ZeroMemory(&processInfo, sizeof(processInfo));
    startupInfo.cb          = sizeof(STARTUPINFO);
    startupInfo.hStdError   = outCMDPtr_Wr;
    startupInfo.hStdOutput  = outCMDPtr_Wr;
    startupInfo.hStdInput   = inCMDPtr_Rd;
    startupInfo.dwFlags     |= STARTF_USESTDHANDLES;

    // Create the command line string
//#pragma warning(suppress : 4996)
//    char* comspecPath = std::getenv("COMSPEC");

    LPWSTR cmd;
    LPWSTR args = NULL;
    LPWSTR outDir = NULL; 


    if (argument == "" || argument == " " || argument == "\"\"" || argument ==  NULL)
    {
        cmd = ConvertCharToOutput(executablePath);
    }
    else
    {
        char* args_a = MergeChars(executablePath, " ");
        args_a = MergeChars(args_a, argument);
        cmd = ConvertCharToOutput(args_a);
    }

    std::cout << cmd << std::endl;
    if (workingDirectory == "" || workingDirectory == " " || workingDirectory == "\"\"" || workingDirectory == NULL)
    {
        outDir = NULL;
    }
    else 
    {
        outDir = ConvertCharToOutput(workingDirectory);
    }
    
    // Try to start the external process
    if (CreateProcess(
        NULL, //ConvertCharToOutput(executablePath),// lpApplicationName
        cmd, // lpCommandLine
        NULL,                   // lpProcessAttributes
        NULL,                   // lpThreadAttributes
        TRUE,                  // bInheritHandles
        0,                      // dwCreationFlags
        NULL,                   // lpEnvironment
        outDir,                   // lpCurrentDirectoryConvertCharToOutput(workingDirectory),//
        &startupInfo,           // lpStartupInfo
        &processInfo            // lpProcessInformation
    ))
    {
        // Optionally wait for the process to finish
        initializedProcess = TRUE;
        std::cout << "Starting" << std::endl;
        return status;
    }
    else
    {
        // Handle error
        //KillAllProcessHandles();
        DWORD error = GetLastError();
        std::cout << "Error" << std::endl;
        std::cout << error << std::endl;
        return (int)error;
    }
}

int WriteToActiveProcess(const char* inputString)
{
    int bufferSize = strlen(inputString);
    DWORD dwRead, dwWritten;

    BOOL bSuccess = FALSE;
    int timeout = 1000;
    int counter = 0;
    
    if (initializedProcess == FALSE)
    {
        return -1;
    }
    bSuccess = WriteFile(inCMDPtr_Wr, inputString, bufferSize, &dwWritten, NULL);
    std::cout << bSuccess << std::endl; 
    return bSuccess;
}

char* ReadFromActiveProcess()
{
    DWORD dwRead, dwWritten;
    const int BUFSIZE = 2000;
    CHAR chBuf[BUFSIZE];
    BOOL bSuccess = FALSE;
    HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    int timeout = 1000;
    int counter = 0;

    bSuccess = ReadFile(outCMDPtr_Wr, chBuf, BUFSIZE, &dwRead, NULL);
    return chBuf;
}

//
//void ma/*in()
//{
//
//}*/

void main()
{
    //F:\____CLEVER Testing\CLEVER V0.8.4\Assets\StreamingAssets\NAudioUtility\SystemRecorder
    //const char* executab/*le = "\"F:/____CLEVER Testing/CLEVER V0.8.4/Assets/StreamingAssets/NAudioUtility/SystemRecorder/NAudioWrapperUnity.exe\"";
    //const char* arg = "\"C:/CLEVER_Output/Audio_Recording_2023-11-28-19-40-09-978.wav\"";
    //const char* directory = ""*/;
    //const char* executable = "C:/Program Files/obs-studio/bin/64bit/obs64.exe";
    //const char* arg = ""; // "--startrecording --scene C : \\OBS_Settings\\VideoandMicrophoneVRCapture.json --profile C : \\OBS_Settings\\Tomatoes";
    //const char* directory ="C:/Program Files/obs-studio/bin/64bit";
    const char* executable = "C:\\Windows\\system32\\notepad.exe";
    const char* arg = "";  
    const char* directory = "";


    std::cout << "Starting" << std::endl;
    StartProcess(executable, arg, directory);
    WriteToActiveProcess("START\r");

    using namespace std::this_thread;     // sleep_for, sleep_until
    using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
    using std::chrono::system_clock;
    //sleep_for(10ns);
    sleep_until(system_clock::now() + 5s);

    WriteToActiveProcess("STOP\r");
    sleep_until(system_clock::now() + 3s);
    KillAllProcessHandles();
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
