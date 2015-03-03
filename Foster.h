//Foster.h 
//Defines functionality for Foster service.  Retrieves system details from Windows 8.1 using a combination of Windows Store API calls and legacy Win32 API calls.
//"Bad becomes good," - Tory Foster
//josh@mindaptiv.com

#pragma once

//includes
#include "Cylon.h"

//includes for windows functionality
#include <WinSock2.h>

//definitions
//via Ted's Blog
typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

//Method Declaration:
//Producers
//for getting username
void		produceUsername(struct cylonStruct& tf);

//for getting time zone info
void		produceTimeZone(struct cylonStruct& tf);

//for getting time info
void		produceDateTime(struct cylonStruct& tf);

//for getting device name
void		produceDeviceName(struct cylonStruct& tf);

//for getting processor info
void		produceProcessorInfo(struct cylonStruct& tf);

//for getting memory info
HMODULE		GetKernelModule();
void		produceMemoryInfo(struct cylonStruct& tf);

//for getting account picture
void		produceAccountPicture(struct cylonStruct& tf);

//for getting attached devices
void		produceDeviceInformation(struct cylonStruct& tf);
void		produceDeviceTypeInformation(struct cylonStruct& tf, std::string type);
void		produceDeviceTypesInformation(struct cylonStruct& tf);
void		produceDisplayInformation(struct cylonStruct& tf);

//Builders
//NOTE: not situated in a cylon.cpp because the method of construction may vary on later platforms
//build Tory for the current machine
struct cylonStruct buildTory();

//build a deviceStruct for a given DeviceInformation object
struct deviceStruct buildDevice(Windows::Devices::Enumeration::DeviceInformation^ deviceInfo, unsigned int deviceType);

//build a displayStruct for a given DisplayInformation object
struct displayStruct buildDisplay(struct deviceStruct superDevice, Windows::Graphics::Display::DisplayInformation^ displayInformation);
//End Constructors
//End methods declaration