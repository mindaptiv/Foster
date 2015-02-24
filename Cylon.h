//Cylon.h 
//Gives definition for cylonStruct and its supporting structures for use with Foster and other Final Five services.
//"By your command," - Cylon Centurion
//josh@mindaptiv.com

#pragma once

//includes
#include <string>
#include <list>

//struct definition for storing user and system data from a WinRT based machine for later use
struct cylonStruct
{
	//polymorphism
	unsigned int				type;	//0 is error, 1 is generic, 2 is portable storage, 
										//3 is audio capture, 4 is audio render, 5 is video capture, 
										//6 is image scanner, 7 is location aware, 8 is display

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

	//memory
	uint64			memoryBytes; //system memory measured in bytes
	unsigned int	osArchitecture; //operating system architecture, 16, 32, 64, 128, etc.

	//account picture
	Windows::Storage::IStorageFile^ picture;
	std::wstring				pictureType;

	//devices
	unsigned int installedDeviceCount;
	unsigned int detectedDeviceCount;
	unsigned int portableStorageCount;
	unsigned int videoCount;
	unsigned int micCount;
	unsigned int speakerCount;
	unsigned int locationCount;
	unsigned int scannerCount;

	//TODO refactor these into deviceStructs and linked lists
	/*Windows::Devices::Enumeration::DeviceInformationCollection^ installedDevices;
	Windows::Devices::Enumeration::DeviceInformationCollection^ portableStorageDevices;
	Windows::Devices::Enumeration::DeviceInformationCollection^ audioCaptureDevices;
	Windows::Devices::Enumeration::DeviceInformationCollection^ audioRenderDevices;
	Windows::Devices::Enumeration::DeviceInformationCollection^ videoCaptureDevices;
	Windows::Devices::Enumeration::DeviceInformationCollection^ imageScannerDevices;
	Windows::Devices::Enumeration::DeviceInformationCollection^ locationAwareDevices;*/

	//devices list
	std::list<struct deviceStruct> detectedDevices;

	//utf8
	//std::string	

	//error
	int32			error;
};
//END cylonStruct

//support structure for cylonStruct for holding the properties of a given device in a single struct
struct deviceStruct
{
	//TODO convert to UTF8
	//NOTE: Values of 0 are errors for non-bools
	unsigned int	panelLocation;		//devices panel location on the physical computer
	bool			inLid;				//if the device is located in the lid of the computer 
	bool			inDock;				//if the device is in the docking station of the computer
	bool			isDefault;			//if device is the default for its function
	bool			isEnabled;			//if the device is enabled
	std::wstring	wName, wID, wIcon;  //TODO convert to utf8

};
//END deviceStruct