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
#define cylon_username__max_Utf8_k 128
#define cylon_deviceName__max_Utf8_k 192

//Method Declaration:

//getters
unsigned int getFosterMilliseconds(struct cylonStruct tf);
unsigned int getFosterSeconds(struct cylonStruct tf);
unsigned int getFosterMinutes(struct cylonStruct tf);
unsigned int getFosterHours(struct cylonStruct tf);
unsigned int getFosterDay(struct cylonStruct tf);
unsigned int getFosterDate(struct cylonStruct tf);
unsigned int getFosterMonth(struct cylonStruct tf);
unsigned int getFosterYear(struct cylonStruct tf);
long getFosterTimeZone(struct cylonStruct tf);
unsigned int getFosterDST(struct cylonStruct tf);
std::wstring getFosterTimeZoneName(struct cylonStruct tf);
std::wstring getFosterUsername(struct cylonStruct tf);
std::wstring getFosterDeviceName(struct cylonStruct tf);
unsigned short getFosterArchitecture(struct cylonStruct tf); 
unsigned short getFosterProcessorLevel(struct cylonStruct tf); 
unsigned long	getFosterPageSize(struct cylonStruct tf);  
unsigned long	getFosterProcessorCount(struct cylonStruct tf); 
unsigned long	getFosterAllocationGranularity(struct cylonStruct tf); 
void*			getFosterMinAppAddress(struct cylonStruct tf); 
void*			getFosterMaxAppAddress(struct cylonStruct tf); 
std::wstring getFosterPictureType(struct cylonStruct tf);
Windows::Storage::IStorageFile^ getFosterPictureFile(struct cylonStruct tf);

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

//Builders
//NOTE: not situated in a cylon.cpp because the method of construction may vary on later platforms
//build Tory for the current machine
struct cylonStruct buildTory();

//build a deviceStruct for the current machine
struct deviceStruct buildDevice(Windows::Devices::Enumeration::DeviceInformation^ deviceInfo, unsigned int deviceType);
struct displayStruct buildDisplay(struct deviceStruct super);
//End Constructors
//End methods declaration