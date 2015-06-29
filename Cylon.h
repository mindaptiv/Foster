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
unsigned long           -> uint32_t
long unsigned int       -> uint64_t

BYTE      -> signed byte      -> int8_t
SHORT     -> short integer    -> int16_t
long      -> signed integer   -> int32_t
long long -> signed long long -> int64_t

void*                         -> uintptr_t
bool						  -> uint32_t
float32                       -> float
*/

//Constants
//Days
static const int SUNDAY = 0;
static const int MONDAY = 1;
static const int TUESDAY = 2;
static const int WEDNESDAY = 3;
static const int THURSDAY = 4;
static const int FRIDAY = 5;
static const int SATURDAY = 6;

//Months
static const int JANUARY = 1;
static const int FEBRUARY = 2;
static const int MARCH = 3;
static const int APRIL = 4;
static const int MAY = 5;
static const int JUNE = 6;
static const int JULY = 7;
static const int AUGUST = 8;
static const int SEPTEMBER = 9;
static const int OCTOBER = 10;
static const int DECEMBER = 12;
static const int NOVEMBER = 11;

//DST
static const int STANDARD_TIME = 0;
static const int DAYLIGHT_TIME = 1;

//controllerStruct buttons
static const int A_BUTTON = 0x1000;
static const int B_BUTTON = 0x2000;
static const int X_BUTTON = 0x4000;
static const int Y_BUTTON = 0x8000;
static const int UP_DPAD = 0x0001;
static const int DOWN_DPAD = 0x0002;
static const int LEFT_DPAD = 0x0004;
static const int RIGHT_DPAD = 0x0008;
static const int START_BUTTON = 0x0010;
static const int SELECT_BUTTON = 0x0020;
static const int LEFT_THUMB = 0x0040;
static const int RIGHT_THUMB = 0x0080;
static const int LEFT_SHOULDER = 0x0100;
static const int RIGHT_SHOULDER = 0x0200;

//deviceStruct Types
static const int ERROR_TYPE = 0;
static const int GENERIC_TYPE = 1;
static const int AUDIO_CAPTURE_TYPE = 2;
static const int AUDIO_RENDER_TYPE = 3;
static const int STORAGE_TYPE = 4;
static const int VIDEO_CAPTURE_TYPE = 5;
static const int IMAGE_SCANNER_TYPE = 6;
static const int LOCATION_AWARE_TYPE = 7;
static const int DISPLAY_TYPE = 8;
static const int MOUSE_TYPE = 9;
static const int KEYBOARD_TYPE = 10;
static const int CONTROLLER_TYPE = 11;
static const int TOUCHSCREEN_TYPE = 12;
static const int TOUCH_PAD_TYPE = 13;
static const int TRACKBALL_TYPE = 14;
static const int STYLUS_TYPE = 15;
static const int POSITION_TYPE = 16;

static const int RUMBLE_TYPE = 18;
static const int SENSOR_TYPE = 19;
static const int BLUETOOTH_RADIO_TYPE = 20;

//deviceStruct location
static const int UNKNWON_PANEL_LOCATION = 0;
static const int FRONT_PANEL = 1;
static const int BACK_PANEL = 2;
static const int TOP_PANEL = 3;
static const int BOTTOM_PANEL = 4;
static const int LEFT_PANEL = 5;
static const int RIGHT_PANEL = 6;

//displayStruct rotations
static const int NO_ROTATION = 0;
static const int LANDSCAPE = 1;
static const int PORTRAIT = 2;
static const int FLIPPED_LANDSCAPE = 4;
static const int FLIPPED_PORTRAIT = 8;

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

	//additions from Saul Marrow
	uint32_t vendorID;
	uint32_t orientation;
	uint32_t storageIndex;
	uint32_t sensorIndex;

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
	float		logicalDPI;
	float		rawDPIX;
	float		rawDPIY;
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
	int8_t		leftTrigger;
	int8_t		rightTrigger;
	int16_t		thumbLeftY;
	int16_t		thumbLeftX;
	int16_t		thumbRightX;
	int16_t		thumbRightY;
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

//for handling device-specific metadata for a sensor in an Android context
struct sensorStruct
{
	struct deviceStruct superDevice; //parent deviceStruct object

	//data from Android 16+
	uint32_t minDelay;
	uint32_t type; //based on Android Sensor class type mappings
	uint32_t version;
	std::string name;
	std::string vendor;
	float power;
	float resolution;
	float maxRange;

	//data from Android 19+
	uint32_t fifoMaxEventCount;
	uint32_t fifoReservedEventCount;

	//data from Android 20+
	std::string stringType;

	//data from Android 21+
	uint32_t maxDelay;
	uint32_t reportingMode; //based on Android Sensor class type mappings
	uint32_t isWakeUpSensor;
};

//struct for representing a path used for storage (device, mounted network drive, etc.)
struct storageStruct
{
	struct deviceStruct superDevice; //parent deviceStruct object

	//Path to access the storage device in the file system
	std::string path;

	//Space
	uint64_t bytesAvails;
	uint64_t totalBytes;

	//Emulated Storage?
	uint32_t isEmulated;
};

//struct definition for storing user and system data from a WinRT based machine for later use
struct cylonStruct
{
	//time
	uint32_t					milliseconds;
	uint32_t					seconds;
	uint32_t					minutes;
	uint32_t					hours;

	//date
	uint32_t					day; //day of the week, 0-6
	uint32_t					date; //1-31
	uint32_t					month; //1-12
	uint32_t					year; //1601 until the cows come home

	//timezone
	uint32_t					dst;			//0 is standard time, 1 is daylight time, otherwise is invalid
	int32_t						timeZone;		//expressed in minutes +/- UTC
	std::string					timeZoneName;

	//names
	std::string					deviceName;
	std::string					username;

	//processor
	std::string					architecture;
	uint16_t					processorLevel; //architecture-dependent processor level
	uint32_t					pageSize;  //size of page in bytes
	uint32_t					allocationGranularity; //granularity for starting address where virtual memory can be allocated (assuming in bits?)
	uintptr_t					minAppAddress; //lowest point in memory an application can access
	uintptr_t					maxAppAddress; //highest point in memory an app can access
	float						hertz; //speed of processor (or default lowest possible speed for current OS)
	uint64_t					processorCount; //number of processors

	//memory
	uint64_t					memoryBytes; //system memory measured in bytes
	uint32_t					osArchitecture; //operating system architecture, 16, 32, 64, 128, etc.
	uint32_t					lowMemory;
	uint64_t					threshold;
	uint64_t					bytesAvails;

	//account picture
	//TODO add picture location from IStorageFile
	std::string					pictureType;
	std::string					picturePath;
	uintptr_t					pictureLocation;

	//devices
	uint32_t installedDeviceCount;
	uint32_t detectedDeviceCount;
	uint32_t portableStorageCount;
	uint32_t videoCount;
	uint32_t micCount;
	uint32_t speakerCount;
	uint32_t locationCount;
	uint32_t scannerCount;
	std::list<struct deviceStruct> detectedDevices;
	std::list<struct displayStruct> displayDevices;
	std::list<struct controllerStruct> controllers;
	std::list<struct sensorStruct> sensors;
	std::list<struct storageStruct> storages;
	struct mouseStruct mice;

	//error
	int32_t			error;
};
//END cylonStruct

