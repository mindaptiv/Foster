//Cylon.h 
//Gives definition for cylonStruct and its supporting structures for use with Foster and other Final Five services.
//"By your command," - Cylon Centurion
//josh@mindaptiv.com

#pragma once

//includes
#include <string>
#include <list>
#include <stdint.h>

//definitions
#define cylon_username__max_Utf8_k 128
#define cylon_deviceName__max_Utf8_k 192

//Type mappings for reference
/*
UBYTE -> unsigned byte  -> uint8_t
WORD  -> unsigned short -> uint16_t
DWORD -> unsigned int   -> uint32_t
long unsigned int       -> uint64_t

BYTE      -> signed byte      -> int8_t
SHORT     -> short integer    -> int16_t
long      -> signed integer   -> int32_t
long long -> signed long long -> int64_t

void*                         -> int64_t
bool						  -> uint32_t
*/

//support structure for cylonStruct for holding the properties of a given device in a single struct
struct deviceStruct
{
	//NOTE: Values of 0 are errors for non-bools
	uint32_t		panelLocation;		//devices panel location on the physical computer
	uint32_t		inLid;				//if the device is located in the lid of the computer 
	uint32_t		inDock;				//if the device is in the docking station of the computer
	uint32_t		isDefault;			//if device is the default for its function
	uint32_t		isEnabled;			//if the device is enabled
	std::string     name;
	std::string		id;

	//type
	uint32_t				deviceType;	//0 is error, 1 is generic, 2 is portable storage, 
										//3 is audio capture, 4 is audio render, 5 is video capture, 
										//6 is image scanner, 7 is location aware, 8 is display
										//9 is mouse, 10 is keyboard, 11 is gamepad
	uint32_t				displayIndex; //device's index in the displayDevices list if type is 8
	uint32_t				controllerIndex; //device's index in the pointerDevices list if type is 9

};
//END deviceStruct

//for handling DisplayInformation class objects' metadata
struct displayStruct
{
	struct deviceStruct superDevice; //parent deviceStruct object

	uint32_t	rotationPreference;
	uint32_t	currentRotation;
	uint32_t	nativeRotation;
	uint32_t	resolutionScale;
	float32		logicalDPI;
	float32		rawDPIX;
	float32		rawDPIY;
	uint32_t	isStereoscopicEnabled;
	//unsigned char*  colorData;
	//unsigned int	colorLength;
};
//end displayStruct

//struct for a gamepad device
struct controllerStruct
{
	struct deviceStruct superDevice;

	uint32_t userIndex; //player number 0-3

	//xinput state
	uint32_t	packetNumber; //for detecting changes
	uint16_t	buttons; //bit mask for what buttons are pressed
	int8		leftTrigger;
	int8		rightTrigger;
	int16		thumbLeftY;
	int16		thumbLeftX;
	int16		thumbRightX;
	int16		thumbRightY;
};
//end controller struct

//for handling device-specific metadata for a mouse
struct mouseStruct
{
	struct deviceStruct superDevice; //parent deviceStruct object

	//properties of available mice
	uint32_t anyLeftRightSwapped;
	uint32_t anyVerticalWheelPresent;
	uint32_t anyHorizontalWheelPresent;
	uint32_t maxNumberOfButons; //most buttons available for all given mice attached (i.e. if 3 and 5 button mice are attached, return value should be 5)
};
//END mouseStruct

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
	std::string					timeZoneName;

	//names
	std::string					deviceName;
	std::string					username;

	//processor
	unsigned short				architecture; //0=error, 1=x64, 2=ARM, 3=Itanium, 4=x86
	unsigned short				processorLevel; //architecture-dependent processor level
	unsigned long				pageSize;  //size of page in bytes
	unsigned long				allocationGranularity; //granularity for starting address where virtual memory can be allocated (assuming in bits?)
	void*						minAppAddress; //lowest point in memory an application can access 
	void*						maxAppAddress; //highest point in memory an app can access
	float32						hertz; //speed of processor (or default lowest possible speed for current OS)
	UINT64						processorCount; //number of processors

	//memory
	UINT64			memoryBytes; //system memory measured in bytes
	unsigned int	osArchitecture; //operating system architecture, 16, 32, 64, 128, etc.

	//account picture
	//TODO add picture location from IStorageFile
	std::string					pictureType;

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
	std::list<struct controllerStruct> controllers;
	struct mouseStruct mice;

	//utf8
	//std::string	

	//error
	int32			error;
};
//END cylonStruct

