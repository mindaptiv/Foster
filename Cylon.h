//Cylon.h 
//Gives definition for cylonStruct and its supporting structures for use with Foster and other Final Five services.
//"By your command," - Cylon Centurion
//josh@mindaptiv.com

#pragma once

//includes
#include <string>
#include <list>

//definitions
#define cylon_username__max_Utf8_k 128
#define cylon_deviceName__max_Utf8_k 192

//struct definition for storing user and system data from a WinRT based machine for later use
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
	uint64						hertz;

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
	std::list<struct deviceStruct> detectedDevices;
	std::list<struct displayStruct> displayDevices;

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
	std::wstring	wName, wID;			//TODO convert to utf8

	//type
	unsigned int			deviceType;	//0 is error, 1 is generic, 2 is portable storage, 
										//3 is audio capture, 4 is audio render, 5 is video capture, 
										//6 is image scanner, 7 is location aware, 8 is display
										//9 is pointer, 10 is keyboard, 11 is gamepad
	unsigned int			displayIndex; //device's index in the displayDevices list if type is 8
	unsigned int			inputIndex; //device's index in the pointerDevices list if type is 9

};
//END deviceStruct

//for handling DisplayInformation class objects' metadata
struct displayStruct
{
	struct deviceStruct superDevice; //parent deviceStruct object

	unsigned int	rotationPreference;
	unsigned int	currentRotation;
	unsigned int	nativeRotation;
	unsigned int	resolutionScale;
	float32			logicalDPI;
	float32			rawDPIX;
	float32			rawDPIY;
	bool			isStereoscopicEnabled;
	//unsigned char*  colorData;
	//unsigned int	colorLength;
};
//end displayStruct

//for handling device-specific metadata for pointer devices
struct pointerStruct
{
	struct deviceStruct superDevice; //parent deviceStruct object

	unsigned int type; //0 is error/unknown/invalid, 1 is mouse, 2 is stylus, 3 is touch pad

	bool isIntegrated;
	unsigned int maxContacts;

	//physical device rect
	float physicalX;
	float physicalY;
	float physicalWidth;
	float physicalHeight;

	//scren rect
	float screenX;
	float screenY;
	float screenWidth;
	float screenHeight;

	//usage
	int32 maxLogical;
	int32 maxPhysical;
	int32 minLogical;
	int32 minPhysical;
	float32 physicalMultiplier;
	uint32 unit;
	uint32 usage;
	uint32 usagePage;
};
//END pointerStruct

//struct for a gamepad device
struct controllerStruct
{
	unsigned int userIndex; //player number 0-3

	//xinput state
	DWORD packetNumber; //for detecting changes
	WORD  buttons; //bit mask for what buttons are pressed
	BYTE  leftTrigger; 
	BYTE  rightTrigger;
	SHORT thumbLeftY;
	SHORT thumbLeftX;
	SHORT thumbRightX;
	SHORT thumbRightY;
};
//end controller struct