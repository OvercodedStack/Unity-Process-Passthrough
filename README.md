# Unity-Process-Passthrough


Unity Process Passthrough (UPP) is part of Collecting and Logging OpenVR Data from SteamVR Applications ([CLOVR](https://github.com/xrtlab/clovr)), which offers a way of calling a process and using it as if it were run by a command line application. The official way of passing a process to start through either the Unity Editor or a compliled Unity application is to use Process.Start() from System.Diagnostics pacakge in Unity. However, this does not guarantee a started application will startup/shutdown or command line IO. This led to many [angry forum posts](https://forum.unity.com/threads/solved-il2cpp-and-process-start.533988/) where Unity admits this is not a resolved issue yet. 

UPP aims to resolve this issue and remain a level of freeware for other Unity packages. This package originally was intended to run Open Broadcasting Software (OBS) for [CLOVR](https://github.com/xrtlab/clovr) which meant passing a variable from Unity to a console application and starting or closing the application. 

This project was compiled using Visual Studio 2019 and was written in C++. 

## How does it work? 

By using Unity's Native Plugins feature, we are able to write a Unity plugin wrapper that allows us to use C++'s CreateProcess() function. Functionally this should work the same as StartProcess() minus the inconveniences of the Unity version. UPP for now mantains the process handle of the requested process. As this functionally operates as a library, you can call a process and then UPP will retain the handle for that process until it is destroyed. Functionally Unity does not handle the process and as such, does not "bind" the runtime of your target application with your Unity application. A major downside of StartProcess() was its tendency of starting and binding the process started with Unity's runtime and cause the closure of your application if the started process was stopped.

## Requirements

I provide a Unity "drag and install" option where you can just manually install the compiled .dll and the C# script for Unity. This is found under the /Unity_Installation directory.

If requiring to compile the application, simply have Visual Studio 2019 installed along with the supporting C++ redistribuitables for compiling C++ applications.

## Installation

Simply copy the files from your /Assets folder to your /Assets folder. Unity reccomends placing plugins into a /Plugins folder and Unity scripts can go anywhere. 

The following are the files you will need for any Unity project

- StartProcessUnity.dll
- UnityProcessPassthrough.cs

Optionally, the following script is given as example code for using UPP.

- OBSManager.cs

## Todos/Future work

- Incorporate a Unity package for 1-click installation.
- Improve this library to support dynamic (multiple) process handling.
- Discover bugs with this library approach.
- The project is currently named "StartProcessUnity.dll", will be fixed in the future.
- See how much more extensible this needs to be.

## Credits and License

- Esteban Segarra M. 

- MIT License 
