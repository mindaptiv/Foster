//Foster.h 
//Defines functionality for Foster service.  Retrieves system details from Windows 8.1 using a combination of Windows Store API calls and legacy Win32 API calls.
//"Bad becomes good" - Tory Foster
//josh@mindaptiv.com

#pragma once

//includes
#include <string>

//includes for windows functionality
#include <Windows.h>
#include <WinSock2.h>
#include <windows.system.userprofile.h>

#define cylon_username__max_Utf8_k 128
#define cylon_deviceName__max_Utf8_k 192

//struct definition for storing data for later use
struct cylonStruct
{
	//time
	unsigned int				milliseconds;
	unsigned int				seconds;
	unsigned int				minutes;
	unsigned int				hours;

	//date
	unsigned int				day; //day of the week, 0-6
	unsigned int				date; //1-31
	unsigned int				month; //1-12
	unsigned int				year; //1601 until the cows come home
	
	//timezone
	unsigned int				dst;			//0 is standard time, 1 is daylight time, otherwise is invalid
	long						timeZone;		//expressed in minutes +/- UTC
	std::wstring				wTimeZoneName;	

	//names
	std::wstring				wUsername;		//TODO convert to UTF8?
	std::wstring				wDeviceName;		//TODO convert to UTF8?

	//processor
	unsigned short				architecture; //0=error, 1=x64, 2=ARM, 3=Itanium, 4=x86
	unsigned short				processorLevel; //architecture-dependent processor level
	unsigned long				pageSize;  //size of page in bytes
	unsigned long				processorCount; //number of processors
	unsigned long				allocationGranularity; //granularity for starting address where virtual memory can be allocated (assuming in bits?)
	void*						minAppAddress; //lowest point in memory an application can access 
	void*						maxAppAddress; //highest point in memory an app can access
	//TODO get hz?

	//TODO get memory information

	//utf8
	//std::string	

	//error
	int32			error;
};
//end fosterStruct


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
void		produceMemoryInfo(struct cylonStruct& tf);

//Constructor
//build tory
struct cylonStruct buildTory();
//End methods declaration

