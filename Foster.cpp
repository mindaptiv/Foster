//Foster.cpp
//Implements definitions of Foster functionality.
//"Bad becomes good" - Tory Foster
//josh@mindaptiv.com

//includes
#include "pch.h"  //TODO remove this if necessary for later deployments of Foster
#include "Foster.h"

using namespace std;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace Windows::Storage::Streams;
using namespace concurrency;
using namespace Windows::Gaming::Input;

//If Visual Studio freaks out about this code someday, add this line back in OR modify your project settings
#pragma comment(lib, "Ws2_32.lib")

//Variables
enum gamepadUpdatingState
{
	STARTED,
	REMOVED,
	ALL_REMOVED
};
bool controllersPurged;

uint32_t presentGamepadsAdded;
gamepadUpdatingState gamepadState;
//END Variables

//Method definitions:
//build a message to be logged to debug
void debug(wstring str)
{
	//Credit to Community & anon @ StackOverflow for the og macro code
	std::wostringstream os_;
	os_ << str << "\n";
	OutputDebugStringW(os_.str().c_str());
}//END debug print

//encoding:
//via tfinniga @ stackoverflow
std::string utf8_encode(const std::wstring &wstr)
{
	if (wstr.empty())
	{
		return std::string();
	}

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

	return strTo;
}//end utf8 encoding

std::wstring utf8_decode(const std::string &str)
{
	if (str.empty())
	{
		return std::wstring();
	}

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}//end utf8 decoding

 //Producers:
//Fills cylonStruct with timezone name, UTC offset bias, and dst flag
void produceTimeZone(struct cylonStruct& tory)
{
	//Variable Declaration
	DWORD					tzResult;
	TIME_ZONE_INFORMATION	tzinfo;
	std::wstring			timezoneName;

	//grab and convert bias
	tzResult = GetTimeZoneInformation(&tzinfo);

	//set bias
	tory.timeZone = tzinfo.Bias;

	//Check DWORD value
	if (tzResult == TIME_ZONE_ID_STANDARD)
	{
		//standard time
		tory.dst = STANDARD_TIME;
	}
	else if (tzResult == TIME_ZONE_ID_DAYLIGHT)
	{
		//daylight time
		tory.dst = DAYLIGHT_TIME;
	}
	else
	{
		//otherwise or invalid ==> shenanigans
		//"Oh hell! I have to run home and grab my broom!"
		tory.dst = STANDARD_TIME;
	}
	//end if

	//grab time zone name
	std::wstring standardName;

	//grab name from TimeZoneInformation
	standardName = tzinfo.StandardName;

	//convert to utf8
	tory.timeZoneName = utf8_encode(standardName);
}
//end produceBias

//Grabs time information and stores it in cylonStruct
void produceDateTime(struct cylonStruct& tory)
{
	//Variable declaration
	SYSTEMTIME st;

	//init st
	GetLocalTime(&st);

	//grab values from SYSTEMTIME
	tory.milliseconds = st.wMilliseconds;
	tory.seconds = st.wSecond;
	tory.minutes = st.wMinute;
	tory.hours = st.wHour;

	if (SUNDAY <= st.wDayOfWeek && st.wDayOfWeek <= SATURDAY)
	{
		//0 = Sun, ..., 6 = Sat
		tory.day = st.wDayOfWeek;
	}
	else
	{
		//error case
		tory.day = SUNDAY;
	}//end if

	if (1 <= st.wDay && st.wDay <= 31)
	{
		tory.date = st.wDay;
	}
	else
	{
		//error
		tory.date = 0;
	}//end if

	if (1 <= st.wMonth && st.wMonth <= 12)
	{
		tory.month = st.wMonth;
	}
	else
	{
		//error
		tory.month = 0;
	}//end if

	if (st.wYear < 0)
	{
		//error
		tory.year = 0;
	}
	else
	{
		tory.year = st.wYear;
	}
}
//end produceDateTime

//populates tory's device name
void produceDeviceName(struct cylonStruct& tory)
{
	//Variable declaration
	int				result;
	char			hostBuffer[MAX_PATH];
	std::string		deviceName;
	std::wstring	wDeviceName;
	WSAData			wsa_data;

	//start WSA
	WSAStartup(MAKEWORD(1, 1), &wsa_data);

	//grab result
	result = gethostname(hostBuffer, MAX_PATH);
	deviceName = hostBuffer;

	//check socket errors
	int error;
	error = WSAGetLastError();

	//cleanup WSA
	WSACleanup();

	//convert string to wstring (this is done for debugging with the separate debugging test app that wants wstrings)
	wDeviceName = utf8_decode(deviceName);

	//Check for empty name
	if (wDeviceName.length() <= 0)
	{
		wDeviceName = L"0";
	}//end if

	 //set device name for tory
	tory.deviceName = utf8_encode(wDeviceName);
}
//end produceDeviceName

//for getting processor info
void produceProcessorInfo(struct cylonStruct& tf)
{
	//Variable Declaration
	SYSTEM_INFO sysinfo;
	std::string architecture_s;
	float32 minHertzz = 1000000000;

	//Grab system info
	GetNativeSystemInfo(&sysinfo);

	//Convert results into local values
	//Convert architecture
	if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
	{
		//x64 (AMD or Intel)
		tf.architecture = "x64";
	}
	else if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM)
	{
		//ARM
		tf.architecture = "ARM";
	}
	else if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
		//Intel Itanium-based
		tf.architecture = "Itanium";
	}
	else if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
	{
		//x86
		tf.architecture = "x86";
	}
	else
	{
		//unknown error
		tf.architecture = "0";
	}
	//end if

	//set tory page size
	tf.pageSize = (uint32_t)sysinfo.dwPageSize;

	//set the min and max pointers for apps
	tf.minAppAddress = (uintptr_t)sysinfo.lpMinimumApplicationAddress;
	tf.maxAppAddress = (uintptr_t)sysinfo.lpMaximumApplicationAddress;

	//set the number of processors
	tf.processorCount = (UINT64)sysinfo.dwNumberOfProcessors;

	//set allocation granularity
	tf.allocationGranularity = (uint32_t)sysinfo.dwAllocationGranularity;

	//grab default minimum CPU hertz
	tf.hertz = minHertzz;
}
//end produce processor info

//via Ted's Blog
HMODULE GetKernelModule()
{
	//NOTE: may not be permissable in Windows Store - hack to get into kernel32
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	VirtualQuery(VirtualQuery, &mbi, sizeof(mbi));
	return reinterpret_cast<HMODULE>(mbi.AllocationBase);
}//end GetKernelModule

 //for getting memory info
void produceMemoryInfo(struct cylonStruct& tf)
{
	//variable declaration
	BOOL bIsWow64 = FALSE;
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	HMODULE kernelModule = GetKernelModule();

	//set unavailable fields
	tf.lowMemory = 0;
	tf.threshold = 0;
	tf.bytesAvails = 0;

	//determine OS architecture
	//use get process address to get a pointer to function if it exists
	//use virtual query of virtual query in place of GetModuleHandle()
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(kernelModule, "IsWow64Process");

	//if isWoW64Process is found
	if (NULL != fnIsWow64Process)
	{
		//current process is not found to be Wow64
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			//error case, assume 32-bit
			tf.memoryBytes = 1000000000;
			tf.osArchitecture = 32;
		}
		//current process is found to be Wow64
		else
		{
			//Process is running under WOW64, assume 64-bit
			tf.memoryBytes = 2000000000;
			tf.osArchitecture = 64;
		}
	}
	//if isWow64 is not found
	else
	{
		//not 64-bit, so assume 32-bit
		tf.memoryBytes = 1000000000;
		tf.osArchitecture = 32;
	}

	//New for Win10/Sinew Memory Manager get AppMemoryReport
	AppMemoryReport^ report = MemoryManager::GetAppMemoryReport();
	
	//Grab memory info for cylonStruct
	tf.memoryBytes = (uint64_t) report->TotalCommitLimit;
	float lowMemory = (float)0.99;
	tf.threshold = (uint64_t) (lowMemory * tf.memoryBytes);
	tf.bytesAvails = (uint64_t)(report->TotalCommitLimit - report->TotalCommitUsage);
	
	//Calculate low memory
	if (tf.bytesAvails / tf.memoryBytes >= lowMemory)
	{
		tf.lowMemory = 1;
	}
	else
	{
		tf.lowMemory = 0;
	}//END if lowMemory
}
//end produceMemoryInfo

//for getting account picture info
void produceAccountPicture(struct cylonStruct& tf)
{
	//Set type
	tf.pictureType = ".png";
}
//end produceAccountPicture


void produceDeviceTypeInformation(struct cylonStruct& tf, std::string type)
{
	//Variable Declaration
	Windows::Foundation::IAsyncOperation<Windows::Devices::Enumeration::DeviceInformationCollection^>^ operation;
	Windows::Devices::Enumeration::DeviceInformationCollection^ devices;
	Windows::Devices::Enumeration::DeviceClass deviceType;
	unsigned int deviceStructType; //type variable in deviceStruct(s) to be built

								   //set deviceType operation filter based on type string
	if (type == "all" || type == "All")
	{
		deviceType = Windows::Devices::Enumeration::DeviceClass::All;
		deviceStructType = GENERIC_TYPE;
	}
	else if (type == "AudioCapture" || type == "audioCapture")
	{
		deviceType = Windows::Devices::Enumeration::DeviceClass::AudioCapture;
		deviceStructType = AUDIO_CAPTURE_TYPE;
	}
	else if (type == "AudioRender" || type == "audioRender")
	{
		deviceType = Windows::Devices::Enumeration::DeviceClass::AudioRender;
		deviceStructType = AUDIO_RENDER_TYPE;
	}
	else if (type == "PortableStorageDevice" || type == "portableStorageDevice")
	{
		deviceType = Windows::Devices::Enumeration::DeviceClass::PortableStorageDevice;
		deviceStructType = STORAGE_TYPE;
	}
	else if (type == "VideoCapture" || type == "videoCapture")
	{
		deviceType = Windows::Devices::Enumeration::DeviceClass::VideoCapture;
		deviceStructType = VIDEO_CAPTURE_TYPE;
	}
	else if (type == "ImageScanner" || type == "imageScanner")
	{
		deviceType = Windows::Devices::Enumeration::DeviceClass::ImageScanner;
		deviceStructType = IMAGE_SCANNER_TYPE;
	}
	else if (type == "Location" || type == "location")
	{
		deviceType = Windows::Devices::Enumeration::DeviceClass::Location;
		deviceStructType = LOCATION_AWARE_TYPE;
	}
	else
	{
		//"Hey... You ever wonder why we're here?" - Simmons
		//ERROR case, default to all
		deviceType = Windows::Devices::Enumeration::DeviceClass::All;
		deviceStructType = GENERIC_TYPE;
	}

	//Grab devices collection for audio rendering
	operation = Windows::Devices::Enumeration::DeviceInformation::FindAllAsync(deviceType);

	while (operation->Status == Windows::Foundation::AsyncStatus::Started)
	{
		//WAIT, YO
	}

	//get the results and close the operation
	devices = operation->GetResults();
	operation->Close();

	//store results in tory based on type string
	//have to repeat if logic here due to necessary waiting on operation, either repeat if-logic or repeat the operation code above
	if (type == "all" || type == "All")
	{
		tf.installedDeviceCount = devices->Size;
	}
	else if (type == "AudioCapture" || type == "audioCapture")
	{
		//Store Size
		tf.micCount = devices->Size;
	}//END IF
	else if (type == "AudioRender" || type == "audioRender")
	{
		tf.speakerCount = devices->Size;
	}
	else if (type == "PortableStorageDevice" || type == "portableStorageDevice")
	{
		tf.portableStorageCount = devices->Size;
	}
	else if (type == "VideoCapture" || type == "videoCapture")
	{
		tf.videoCount = devices->Size;
	}
	else if (type == "ImageScanner" || type == "imageScanner")
	{
		tf.scannerCount = devices->Size;
	}
	else if (type == "Location" || type == "location")
	{
		tf.locationCount = devices->Size;
	}
	else
	{
		//"Hey... You ever wonder why we're here?" - Simmons
		//ERROR case, default to all
		tf.installedDeviceCount = devices->Size;
	}

	//toss all detected devices into the list
	for (unsigned int i = 0; i < devices->Size; i++)
	{
		//Variable Declaration
		struct deviceStruct device;

		//Create a device
		device = buildDevice(devices->GetAt(i), deviceStructType);

		//if necessary, build a storage device
		if (deviceStructType == STORAGE_TYPE)
		{
			//build storage device
			struct storageStruct storage = buildStorage(devices->GetAt(i), device);

			//insert into storages
			tf.storages.push_back(storage);

			//set storage index of super
			device.storageIndex = tf.storages.size() - 1;
		}

		//put device in detectedDevices
		tf.detectedDevices.push_back(device);

		if (deviceStructType == STORAGE_TYPE)
		{
			//synchronize the list nodes
			tf.storages.back().superDevice = tf.detectedDevices.back();
		}
	}//END FOR
}//END produce device information


 //produces device information for all types except the "all" filter
void produceDeviceTypesInformation(struct cylonStruct& tf)
{
	//Grab collections and counts
	produceDeviceTypeInformation(tf, "AudioCapture");
	produceDeviceTypeInformation(tf, "AudioRender");
	produceDeviceTypeInformation(tf, "Location");
	produceDeviceTypeInformation(tf, "ImageScanner");
	produceDeviceTypeInformation(tf, "VideoCapture");
	produceDeviceTypeInformation(tf, "PortableStorageDevice");


	//Grab primary display device
	produceDisplayInformation(tf);

	//Grab Keyboard, Mouse, Controllers
	produceKeyboardInformation(tf);
	produceMouseInformation(tf);
	produceGamepadInformation(tf);

	//Grab total count
	tf.detectedDeviceCount = tf.detectedDevices.size();
}//END produce device types information

 

//produces the device and display structs for the primary monitor
void produceDisplayInformation(struct cylonStruct& tf)
{
	//Variable Declaration
	Windows::Graphics::Display::DisplayInformation^ displayInformation;
	Windows::Devices::Enumeration::DeviceInformation^ deviceInfo;
	Windows::Foundation::IAsyncOperation<Windows::Storage::Streams::IRandomAccessStream^>^ operation;
	Windows::Storage::Streams::IRandomAccessStream^ resultStream;
	Windows::Storage::Streams::DataReader^ reader;
	Platform::Array<byte>^ bufferBytes;
	struct displayStruct displayDevice;
	struct deviceStruct  superDevice;

	//Build super
	superDevice = buildDevice(deviceInfo, DISPLAY_TYPE);

	//Grab display info
	displayInformation = Windows::Graphics::Display::DisplayInformation::GetForCurrentView();

	//Build display device
	displayDevice = buildDisplay(superDevice, displayInformation);

	//TODO readd this later and test more extensively
	/*
	//Get Color Profile
	operation = displayInformation->GetColorProfileAsync();

	while (operation->Status == Windows::Foundation::AsyncStatus::Started)
	{
	//WAIT, YO
	}
	resultStream = operation->GetResults();
	operation->Close();

	reader = ref new Windows::Storage::Streams::DataReader(resultStream);
	Windows::Storage::Streams::DataReaderLoadOperation^ readOperation = reader->LoadAsync(static_cast<unsigned int>(resultStream->Size));
	while (readOperation->Status == Windows::Foundation::AsyncStatus::Started)
	{
	//WAIT, YO
	}

	if (reader != nullptr)
	{
	//read bytes
	bufferBytes = ref new Platform::Array<byte>(reader->UnconsumedBufferLength);
	reader->ReadBytes(bufferBytes);
	displayDevice.colorData		= bufferBytes->Data;
	displayDevice.colorLength	= bufferBytes->Length;
	}
	else
	{
	displayDevice.colorData		= (unsigned char*) "0";
	displayDevice.colorLength	= 0;
	}
	*/

	//Insert super/parent into devices lists
	tf.displayDevices.push_back(displayDevice);
	displayDevice.superDevice.displayIndex = tf.displayDevices.size() - 1;
	tf.detectedDevices.push_back(displayDevice.superDevice);
	tf.displayDevices.back().superDevice = tf.detectedDevices.back();
}//END produceDisplayInformation

 //produces information about pointer devices
void produceMouseInformation(struct cylonStruct& tf)
{
	//Variable Declaration
	Windows::Devices::Input::MouseCapabilities mouseStats;
	Windows::Devices::Enumeration::DeviceInformation^ deviceInfo;
	struct deviceStruct mouse;
	struct mouseStruct mice;

	//check if mouse exists
	if (mouseStats.MousePresent == 1)
	{
		//build device
		mouse = buildDevice(deviceInfo, MOUSE_TYPE);

		//insert mouse device into detectedDevices
		tf.detectedDevices.push_back(mouse);

		//Populate mice variables
		if (mouseStats.HorizontalWheelPresent == 1)
		{
			tf.mice.anyHorizontalWheelPresent = true;
		}
		else
		{
			//error/invalid/unknown/not present
			tf.mice.anyHorizontalWheelPresent = false;
		}

		if (mouseStats.VerticalWheelPresent == 1)
		{
			tf.mice.anyVerticalWheelPresent = true;
		}
		else
		{
			tf.mice.anyHorizontalWheelPresent = false;
		}

		if (mouseStats.SwapButtons == 1)
		{
			tf.mice.anyLeftRightSwapped = true;
		}
		else
		{
			tf.mice.anyLeftRightSwapped = false;
		}

		tf.mice.maxNumberOfButons = mouseStats.NumberOfButtons;
	}
}

//create deviceStruct for keyboard
void produceKeyboardInformation(struct cylonStruct& tf)
{
	//Variable Declaration
	struct deviceStruct keyboard;
	Windows::Devices::Enumeration::DeviceInformation^ deviceInfo;
	Windows::Devices::Input::KeyboardCapabilities keyboardInfo;

	//check if keyboard exists

	//If keyboard exists
	if (keyboardInfo.KeyboardPresent == 1)
	{
		//build device
		keyboard = buildDevice(deviceInfo, KEYBOARD_TYPE);

		//insert keyboard device into detectedDevices
		tf.detectedDevices.push_back(keyboard);
	}//END if
}
//end produce Keyboard information

//grabs information for (up to) 4 XInput controllers
//NOTE: can throw app-crashing exceptions when multiple gamepads are in use and disconnect in certain orders, use at your own risk!
void produceControllerInformation(struct cylonStruct& tf)
{
	//Variable Declaration
	DWORD result;
	XINPUT_STATE state;

	//for plays 0-maxPlayerCount
	for (DWORD userIndex = 0; userIndex < XUSER_MAX_COUNT; userIndex++)
	{
		//zero memory
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		//Get state of controller
		result = XInputGetState(userIndex, &state);

		if (result == ERROR_SUCCESS)
		{
			//build device struct
			struct deviceStruct device = buildDevice(userIndex);

			//build controller struct
			struct controllerStruct controller = buildController(device, state, userIndex);

			//insert into controllers
			tf.controllers.push_back(controller);

			//set controller index
			controller.superDevice.controllerIndex = tf.controllers.size() - 1;

			//insert into devices
			tf.detectedDevices.push_back(controller.superDevice);

			//sync lists
			tf.controllers.back().superDevice = tf.detectedDevices.back();
		}//END If controller connected

	}//END FOR
}//END produceControllerInfo

void produceGamepadInformation(struct cylonStruct& tf)
{
	//Set State Trackers
	gamepadState = STARTED;
	presentGamepadsAdded = 0;
	controllersPurged = false;

	//Event handlers
	Gamepad::GamepadAdded += ref new EventHandler<Gamepad^>(OnGamepadAdded);
	Gamepad::GamepadRemoved += ref new EventHandler<Gamepad^>(OnGamepadRemoved);

	for (uint32_t i = 0; i < Gamepad::Gamepads->Size; i++)
	{
		//GamepadReading
		GamepadReading reading = Gamepad::Gamepads->GetAt(i)->GetCurrentReading();

		//Build Device and Controller Structs
		struct deviceStruct device = buildDevice(i);
		struct controllerStruct controller = buildController(device, i, reading);
		
		//insert into lists an dsync
		tf.controllers.push_back(controller);
		controller.superDevice.controllerIndex = tf.controllers.size() - 1;
		tf.detectedDevices.push_back(controller.superDevice);
		tf.controllers.back().superDevice = tf.detectedDevices.back(); 
	}//END for all gamepads 
}//END produceGamepadInfo

//for logging
void produceLog(struct cylonStruct& tf)
{
	std::wostringstream os_;
	os_ << "Cylon @: " << &tf << endl
		<< "Username: " << utf8_decode(tf.username) << endl
		<< "Device Name: " << utf8_decode(tf.deviceName) << endl
		<< "Timestamp: " << tf.day << ", " << tf.month << "/" << tf.day << "/" << tf.year << " " << tf.hours << ":" << tf.minutes << ":" << tf.seconds << ":" << tf.milliseconds << endl
		<< "Profile Picture Location: " << hex<< tf.pictureLocation <<dec<< " Type: " << utf8_decode(tf.pictureType) << " Path: " << utf8_decode(tf.picturePath) << endl
		<< "Processor Architecture: " << utf8_decode(tf.architecture) << endl
		<< "Processor Count: " << tf.processorCount << endl
		<< "Processor Level: " << tf.processorLevel << endl
		<< "Processor Clock Speed: "<< tf.hertz<< "Hz" << endl
		<< "OS Architecture: " << tf.osArchitecture << endl
		<< "Total Memory: " << tf.memoryBytes << endl
		<< "Available Memory: " << tf.bytesAvails << endl
		<< "Low Memory Threshold: " << tf.threshold << endl
		<< "Low Memory? " << tf.lowMemory << endl
		<< "Page Size: " << tf.pageSize << endl
		<< "Allocation Granularity: " << tf.allocationGranularity << endl
		<< "Min/Max App Address: "<< tf.minAppAddress << "/" << tf.maxAppAddress << endl
		<< "Detected Device Count: "<< tf.detectedDeviceCount << endl
		<< "Error: " << tf.error <<endl<<endl
		<< "Devices: "<<endl
		;

	for (list<deviceStruct>::const_iterator iterator = tf.detectedDevices.begin(), end = tf.detectedDevices.end(); iterator != end; ++iterator)
	{
		os_
			<< "\t" << "Name: " << utf8_decode(iterator->name) << endl
			<< "\t" << "Type: " << iterator->deviceType << endl
			<< "\t" << "Vendor ID: " << endl
			<< "\t" << "ID: " << utf8_decode(iterator->id) << endl
			<< "\t" << "Orientation: " << iterator->orientation << endl
			<< "\t" << "USB Bus: " << endl
			<< "\t" << "UDev Device #: " << endl
			<< "\t" << "Panel Location: " << iterator->panelLocation << endl
			<< "\t" << "In Lid: " << iterator->inLid << endl
			<< "\t" << "In Dock: " << iterator->inDock << endl
			<< "\t" << "Is Default: " << iterator->isDefault <<endl
			<< "\t" << "Is Enabled: " <<iterator->isEnabled <<endl
			<< "\t" << "Controller Index: " << iterator->controllerIndex << endl
			<< "\t" << "Storage Index: " << iterator->storageIndex << endl
			<< "\t" << "Display Index: "<<iterator->displayIndex << endl
			<<endl
			;
	}	

	os_ << endl << "Controllers: " << endl;

	for (list<controllerStruct>::const_iterator iterator = tf.controllers.begin(), end = tf.controllers.end(); iterator != end; ++iterator)
	{
		os_
			<< "\t" << "User Index: " << iterator->userIndex << endl
			<< "\t" << "Packet Number: " << iterator->packetNumber<<endl
			<< "\t" << "Left Trigger: " <<iterator->leftTrigger <<endl
			<< "\t" << "Right Trigger: " << iterator->rightTrigger <<endl
			<< "\t" << "Left Thumb X: " << iterator->thumbLeftX <<endl
			<< "\t" << "Left Thumb Y: " << iterator->thumbLeftY <<endl
			<< "\t" << "Right Thumb X: " << iterator->thumbRightX <<endl
			<< "\t" << "Right Thumb Y: " << iterator->thumbRightY << endl
			<< "\t" << "Buttons: " << hex<< iterator->buttons << dec<<endl
			<< endl
			;
	}

	os_ << endl << "Displays: " << endl;
	for (list<displayStruct>::const_iterator iterator = tf.displayDevices.begin(), end = tf.displayDevices.end(); iterator != end; ++iterator)
	{
		os_
			<< "\t" << "Rotation Preference: " << iterator->rotationPreference << endl
			<< "\t" << "Current Rotation: " << iterator->currentRotation << endl
			<< "\t" << "Native Rotation: " << iterator->nativeRotation << endl
			<< "\t" << "Stereoscopic Enabled? " << iterator->isStereoscopicEnabled << endl
			<< "\t" << "Resolution Scale: " << iterator->resolutionScale << endl
			<< "\t" << "Logical DPI: " << iterator->logicalDPI << endl
			<< "\t" << "Raw DPI X: " << iterator->rawDPIX << endl
			<< "\t" << "Raw DPI Y: " << iterator->rawDPIY << endl
			<< endl
			;
	}

	os_ << "Mouse Stats: " << endl
		<< "\t" << "Left/Right Swapped: " << tf.mice.anyLeftRightSwapped << endl
		<< "\t" << "Vertical Wheel: " << tf.mice.anyVerticalWheelPresent << endl
		<< "\t" << "Horizontal Wheel: " << tf.mice.anyHorizontalWheelPresent << endl
		<< "\t" << "Button Count: " << tf.mice.maxNumberOfButons << endl;
	
	OutputDebugStringW(os_.str().c_str());
}//END produceLog

 //produce Tory
void produceTory(struct cylonStruct& tory)
{
	//Clear pre-existing lists
	tory.detectedDevices.clear();
	tory.displayDevices.clear();
	tory.controllers.clear();

	//device name
	produceDeviceName(tory);

	//time zone
	produceTimeZone(tory);

	//date and timef
	produceDateTime(tory);

	//processor
	produceProcessorInfo(tory);

	//picture
	produceAccountPicture(tory);

	//devices
	produceDeviceTypesInformation(tory);

	//memory
	produceMemoryInfo(tory);

	//log
	produceLog(tory);
}
//end produce tory 
//END producers

//Builders
//build Tory
struct cylonStruct buildTory()
{
	//Variable Declartion
	struct cylonStruct tory;

	//device name
	produceDeviceName(tory);

	//time zone
	produceTimeZone(tory);

	//date and time
	produceDateTime(tory);

	//processor
	produceProcessorInfo(tory);

	//picture
	produceAccountPicture(tory);

	//devices
	produceDeviceTypesInformation(tory);

	//memory
	produceMemoryInfo(tory);

	//log
	produceLog(tory);

	//return
	return tory;
}
//end build tory

//build a storageStruct with given data
struct storageStruct buildStorage(Windows::Devices::Enumeration::DeviceInformation^ deviceInfo, struct deviceStruct superDevice)
{
	//Variable declaration
	struct storageStruct storage;
	std::wstring error = L"0";

	//Set parent paired deviceStruct
	storage.superDevice = superDevice;

	//TODO: remove this when restore proper path retrieval
	storage.path = "0";

	//get path
	//TODO: try Ellen code as solution
	//TOOD: Restore this somehow?  Clearly have file in c:\Program Files (x86)\Windows Kits\10\Include\10.0.10240.0\winrt\windows.devices.portable.h but compiler cannot resolve namespace
	//NOTE: if not already noted in documentation, your package manifest requires access to Removable Storage for these next two lines to function!
	//Windows::Storage::StorageFolder^ folder = Windows::Devices::Portable::StorageDevice::FromId(deviceInfo->Id);
	//storage.path = utf8_encode(folder->Path->Data());
	
	//set unavailable fields
	storage.bytesAvails = 0;
	storage.totalBytes = 0;
	storage.isEmulated = 0;

	//return struct
	return storage;
}

//build a device struct with given data
struct deviceStruct buildDevice(Windows::Devices::Enumeration::DeviceInformation^ deviceInfo, unsigned int deviceType)
{
	struct deviceStruct device;
	std::wstring error = L"0";

	//set device type
	device.deviceType = deviceType;

	//set unused fields
	device.vendorID = 0;

	//set to zero for now, modify later if necessary
	device.displayIndex = 0;
	device.controllerIndex = 0;
	device.sensorsIndex = 0;
	device.storageIndex = 0;
	device.orientation = 0;

	//get out for display/keyboard/mouse/controller devices, as they have different metadata than the regular kind we retrieve
	if (device.deviceType == DISPLAY_TYPE || device.deviceType == KEYBOARD_TYPE || device.deviceType == MOUSE_TYPE || device.deviceType == CONTROLLER_TYPE)
	{
		//set errors/unknown values
		device.name = utf8_encode(error);
		device.id = utf8_encode(error);
		device.inDock = false;
		device.inLid = false;
		device.panelLocation = 0;

		//default these to true 
		device.isDefault = true;
		device.isEnabled = true;

		//return
		return device;
	}
	//END if device is a special type

	//Set device variables from DeviceInformation data
	if (deviceInfo->Name->IsEmpty())
	{
		device.name = utf8_encode(error);
	}
	else
	{
		device.name = utf8_encode(deviceInfo->Name->Data());
	}//END if Name Empty

	if (deviceInfo->Id->IsEmpty())
	{
		device.id = utf8_encode(error);
	}
	else
	{
		device.id = utf8_encode(deviceInfo->Id->Data());
	}//END if ID Empty

	if (deviceInfo->EnclosureLocation != nullptr)
	{
		if (deviceInfo->EnclosureLocation->InDock == true)
		{
			//if device is in docking station of computer
			device.inDock = true;
		}
		else
		{
			//false or error
			device.inDock = false;
		}//END if inDock

		if (deviceInfo->EnclosureLocation->InLid == true)
		{
			//if device is in the lid of the computer
			device.inLid = true;
		}
		else
		{
			//false or error
			device.inLid = false;
		}//END if inLid

		 //set the panel location of the device (if available)
		if (deviceInfo->EnclosureLocation->Panel.Equals(Windows::Devices::Enumeration::Panel::Top))
		{
			device.panelLocation = TOP_PANEL;
		}
		else if (deviceInfo->EnclosureLocation->Panel.Equals(Windows::Devices::Enumeration::Panel::Bottom))
		{
			device.panelLocation = BOTTOM_PANEL;
		}
		else if (deviceInfo->EnclosureLocation->Panel.Equals(Windows::Devices::Enumeration::Panel::Front))
		{
			device.panelLocation = FRONT_PANEL;
		}
		else if (deviceInfo->EnclosureLocation->Panel.Equals(Windows::Devices::Enumeration::Panel::Back))
		{
			device.panelLocation = BACK_PANEL;
		}
		else if (deviceInfo->EnclosureLocation->Panel.Equals(Windows::Devices::Enumeration::Panel::Left))
		{
			device.panelLocation = LEFT_PANEL;
		}
		else if (deviceInfo->EnclosureLocation->Panel.Equals(Windows::Devices::Enumeration::Panel::Right))
		{
			device.panelLocation = RIGHT_PANEL;
		}
		else
		{
			//unknown or error
			device.panelLocation = UNKNOWN_PANEL_LOCATION;
		}//END if panelLocation
	}//end if enclosurelocation is null
	else
	{
		//if enclosure is null
		//errors for all because unknown/invalid/missing/empty
		device.panelLocation = 0;
		device.inDock = false;
		device.inLid = false;
	}
	if (deviceInfo->IsDefault == true)
	{
		deviceInfo->Name->Data();
		device.isDefault = true;
	}
	else
	{
		//includes empty, false, etc.
		device.isDefault = false;
	}//END if IsDefault

	if (deviceInfo->IsEnabled == true)
	{
		device.isEnabled = true;
	}
	else
	{
		//includes empty, false, etc.
		device.isEnabled = false;
	}//END if IsEnabled

	 //return
	return device;
}
//END build device

//build a deviceStruct for a given XINPUT controller
struct deviceStruct buildDevice(uint32_t userIndex)
{
	//Variable Declaration
	struct deviceStruct device;

	//Set default/unavailable fields
	device.panelLocation = UNKNOWN_PANEL_LOCATION;
	device.inLid = 0;
	device.inDock = 0;
	device.isEnabled = 1;
	device.orientation = NO_ROTATION;
	device.vendorID = 0;
	device.controllerIndex = 0;
	device.displayIndex = 0;
	device.sensorsIndex = 0;
	device.storageIndex = 0;
	
	//Build custom name, allows for easier lookup later when devices disconnect
	device.name = "XINPUT Controller #";
	device.name += userIndex;
	device.id = userIndex;
	
	//determine if default
	if (userIndex == 0)
	{
		device.isDefault = 1;
	}
	else
	{
		device.isDefault = 0;
	}//END if user is 0

	//return
	return device;
}//END build controller device


//builds a displayStruct from a given deviceStruct
struct displayStruct buildDisplay(struct deviceStruct superDevice, Windows::Graphics::Display::DisplayInformation^ displayInformation)
{
	//Variable Declaration
	struct displayStruct displayDevice;

	//Set parent struct
	displayDevice.superDevice = superDevice;

	//Set member variables
	//preferred app orientation
	if (displayInformation->AutoRotationPreferences == Windows::Graphics::Display::DisplayOrientations::None)
	{
		displayDevice.rotationPreference = NO_ROTATION;

	}
	else if (displayInformation->AutoRotationPreferences == Windows::Graphics::Display::DisplayOrientations::Landscape)
	{
		displayDevice.rotationPreference = LANDSCAPE;
	}
	else if (displayInformation->AutoRotationPreferences == Windows::Graphics::Display::DisplayOrientations::Portrait)
	{
		displayDevice.rotationPreference = PORTRAIT;

	}
	else if (displayInformation->AutoRotationPreferences == Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped)
	{
		displayDevice.rotationPreference = FLIPPED_LANDSCAPE;

	}
	else if (displayInformation->AutoRotationPreferences == Windows::Graphics::Display::DisplayOrientations::PortraitFlipped)
	{
		displayDevice.rotationPreference = FLIPPED_PORTRAIT;
	}
	else
	{
		//error case
		displayDevice.rotationPreference = NO_ROTATION;
	}//END if orientation

	 //current monitor orientation
	if (displayInformation->CurrentOrientation == Windows::Graphics::Display::DisplayOrientations::None)
	{
		displayDevice.currentRotation = NO_ROTATION;
		displayDevice.superDevice.orientation = NO_ROTATION;
	}
	else if (displayInformation->CurrentOrientation == Windows::Graphics::Display::DisplayOrientations::Landscape)
	{
		displayDevice.currentRotation = LANDSCAPE;
		displayDevice.superDevice.orientation = LANDSCAPE;
	}
	else if (displayInformation->CurrentOrientation == Windows::Graphics::Display::DisplayOrientations::Portrait)
	{
		displayDevice.currentRotation = PORTRAIT;
		displayDevice.superDevice.orientation = PORTRAIT;
	}
	else if (displayInformation->CurrentOrientation == Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped)
	{
		displayDevice.currentRotation = FLIPPED_LANDSCAPE;
		displayDevice.superDevice.orientation = FLIPPED_LANDSCAPE;
	}
	else if (displayInformation->CurrentOrientation == Windows::Graphics::Display::DisplayOrientations::PortraitFlipped)
	{
		displayDevice.currentRotation = FLIPPED_PORTRAIT;
		displayDevice.superDevice.orientation = FLIPPED_PORTRAIT;
	}
	else
	{
		//error case
		displayDevice.currentRotation = NO_ROTATION;
		displayDevice.superDevice.orientation = NO_ROTATION;
	}//END current monitor orientation

	 //native monitor orientation
	if (displayInformation->NativeOrientation == Windows::Graphics::Display::DisplayOrientations::None)
	{
		displayDevice.nativeRotation = NO_ROTATION;
	}
	else if (displayInformation->NativeOrientation == Windows::Graphics::Display::DisplayOrientations::Landscape)
	{
		displayDevice.nativeRotation = LANDSCAPE;
	}
	else if (displayInformation->NativeOrientation == Windows::Graphics::Display::DisplayOrientations::Portrait)
	{
		displayDevice.nativeRotation = PORTRAIT;
	}
	else if (displayInformation->NativeOrientation == Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped)
	{
		displayDevice.nativeRotation = FLIPPED_LANDSCAPE;
	}
	else if (displayInformation->NativeOrientation == Windows::Graphics::Display::DisplayOrientations::PortraitFlipped)
	{
		displayDevice.nativeRotation = FLIPPED_PORTRAIT;
	}
	else
	{
		//error case
		displayDevice.nativeRotation = NO_ROTATION;
	}//END if native monitor orientation

	 //DPI
	displayDevice.logicalDPI = displayInformation->LogicalDpi;
	displayDevice.rawDPIX = displayInformation->RawDpiX;
	displayDevice.rawDPIY = displayInformation->RawDpiY;

	//Stereoscopic 3D
	displayDevice.isStereoscopicEnabled = displayInformation->StereoEnabled;

	//Resolution Scale
	if (displayInformation->ResolutionScale == Windows::Graphics::Display::ResolutionScale::Invalid)
	{
		displayDevice.resolutionScale = (float)0.0;
	}
	else if (displayInformation->ResolutionScale == Windows::Graphics::Display::ResolutionScale::Scale100Percent)
	{
		displayDevice.resolutionScale = (float)1.0;
	}
	else if (displayInformation->ResolutionScale == Windows::Graphics::Display::ResolutionScale::Scale120Percent)
	{
		displayDevice.resolutionScale = (float)1.2;
	}
	else if (displayInformation->ResolutionScale == Windows::Graphics::Display::ResolutionScale::Scale140Percent)
	{
		displayDevice.resolutionScale = (float)1.4;
	}
	else if (displayInformation->ResolutionScale == Windows::Graphics::Display::ResolutionScale::Scale150Percent)
	{
		displayDevice.resolutionScale = (float)1.5;
	}
	else if (displayInformation->ResolutionScale == Windows::Graphics::Display::ResolutionScale::Scale160Percent)
	{
		displayDevice.resolutionScale = (float)1.6;
	}
	else if (displayInformation->ResolutionScale == Windows::Graphics::Display::ResolutionScale::Scale180Percent)
	{
		displayDevice.resolutionScale = (float)1.8;
	}
	else if (displayInformation->ResolutionScale == Windows::Graphics::Display::ResolutionScale::Scale225Percent)
	{
		displayDevice.resolutionScale = (float)2.25;
	}
	else
	{
		//error case
		displayDevice.resolutionScale = (float)0.0;
	}

	//return
	return displayDevice;
}
//END build display

//build a controllerStruct for a given player number, deviceStruct, and GamepadReading
struct controllerStruct buildController(struct deviceStruct superDevice, uint32_t userIndex, GamepadReading buttons)
{
	//Variable Declaration
	struct controllerStruct controller;

	//Get/set new trigger values
	//TODO: change to float64?
	controller.leftTrigger = (float)buttons.LeftTrigger;
	controller.rightTrigger = (float)buttons.RightTrigger;

	//Get/set new thumbstick values
	controller.thumbLeftX = (float)buttons.LeftThumbstickX;
	controller.thumbLeftY = (float)buttons.LeftThumbstickY;
	controller.thumbRightX = (float)buttons.RightThumbstickX;
	controller.thumbRightY = (float)buttons.RightThumbstickY;

	//Set Buttons TODO add windows gaming constants
	controller.buttons = 0;
	updateControllerState(controller, buttons);

	//Set Properties
	controller.superDevice = superDevice;
	controller.userIndex = userIndex;

	//return controller
	return controller;
}//END builder

//build a controllerStruct for a given player number and XINPUT_STATE object
struct controllerStruct buildController(struct deviceStruct superDevice, XINPUT_STATE state, DWORD userIndex)
{
	//Variable Declaration
	struct controllerStruct controller;

	//Normalize Trigger values
	float oldTriggerMin = (float)0.0;
	float oldTriggerMax = (float)255.0;

	float newTriggerMin = (float)0.0;
	float newTriggerMax = (float)1.0;

	float newTriggerRange = (float)1.0;
	float oldTriggerRange = (float)255.0;

	float oldTriggerLeft = (float)state.Gamepad.bLeftTrigger;
	float oldTriggerRight = (float)state.Gamepad.bRightTrigger;

	//Get/set new trigger values
	controller.leftTrigger = (float)((((oldTriggerLeft - oldTriggerMin) * newTriggerRange) / oldTriggerRange) + newTriggerMin);
	controller.rightTrigger = (float)((((oldTriggerRight - oldTriggerMin) * newTriggerRange) / oldTriggerRange) + newTriggerMin);

	//Normalize thumbstick values
	float oldThumbMin = (float)-32768.0;
	float oldThumbMax = (float)32767.0;

	float newThumbMin = (float)-1.0;
	float newThumbMax = (float)1.0;

	float oldThumbRange = (float)(32767 + 32768);
	float newThumbRange = (float)(2.0);

	float oldThumbLeftX = (float)state.Gamepad.sThumbLX;
	float oldThumbLeftY = (float)state.Gamepad.sThumbLY;
	float oldThumbRightX = (float)state.Gamepad.sThumbRX;
	float oldThumbRightY = (float)state.Gamepad.sThumbRY;

	//Get/set new thumbstick values
	controller.thumbLeftX = (float)((((oldThumbLeftX - oldThumbMin) * newThumbRange) / oldThumbRange) + newThumbMin);
	controller.thumbLeftY = (float)((((oldThumbLeftY - oldThumbMin) * newThumbRange) / oldThumbRange) + newThumbMin);
	controller.thumbRightX = (float)((((oldThumbRightX - oldThumbMin) * newThumbRange) / oldThumbRange) + newThumbMin);
	controller.thumbRightY = (float)((((oldThumbRightY - oldThumbMin) * newThumbRange) / oldThumbRange) + newThumbMin);

	//Set properties
	controller.superDevice = superDevice; //set parent
	controller.userIndex = (unsigned int)userIndex;
	controller.buttons = state.Gamepad.wButtons;
	controller.packetNumber = state.dwPacketNumber;

	return controller;
}//END build controller
 //END Builders

void Centurion::Tory::grabUserInfo()
{
	//Sample code from Microsoft UWP Samples @ GitHub
	// Populate the list of users.
	create_task(User::FindAllAsync()).then([this](IVectorView<User^>^ users)
	{
	
		// Get the display name of each user.
		std::vector<task<Object^>> tasks(users->Size);
		std::transform(begin(users), end(users), begin(tasks), [](User^ user)
		{
			return create_task(user->GetPropertyAsync(KnownUserProperties::DisplayName));
		});

		return when_all(begin(tasks), end(tasks)).then([this, users](std::vector<Object^> results)
		{
				auto displayName = safe_cast<String^>(results[0]);
				// Choose a generic name if we do not have access to the actual name.
				if (displayName->IsEmpty())
				{
					//set cylon device name to default error case
					this->CylonName = "0";
					this->NonRoamableId = "0";
					PictureStream = nullptr;
					PictureLocation = 0;

					//Announce that stuff is ready for copying
					InfoReady = true;

				}
				else
				{
					//set the name field for consumption by cylonStruct
					this->CylonName = displayName;
					this->NonRoamableId = users->GetAt(0)->NonRoamableId;

					User^ user = nullptr;
					try
					{
						user = User::GetFromId(NonRoamableId);
					}
					catch (Exception^ ex)
					{
						debug(L"caught an exception creating user");
					}

					if (user != nullptr)
					{
						//Grab Avatar
						auto task = create_task(user->GetPictureAsync(UserPictureSize::Size64x64));
						task.then([this](IRandomAccessStreamReference^ streamReference)
						{
							if (streamReference != nullptr)
							{
								return create_task(streamReference->OpenReadAsync());
							}
							else
							{
								return create_task([]() -> IRandomAccessStreamWithContentType^ {return nullptr; });
							}
						}).then([this](IRandomAccessStreamWithContentType^ stream)
							{
								if (stream != nullptr)
								{
									PictureStream = stream;
									IRandomAccessStreamWithContentType^* tempPic = &stream;
									PictureLocation = (uintptr_t)tempPic;

									//Announce that data is ready for copying
									InfoReady = true;
								}
								else
								{
									//set defaults for avatar
									PictureLocation = 0;
									PictureStream = nullptr;

									//Announce that data is ready for copying
									InfoReady = true;
								}
							}
						);
					}//END if user is null
					else
					{
						//set defaults for avatar
						PictureLocation = 0;
						PictureStream = nullptr;

						//Announce that data is ready for copying
						InfoReady = true;
					}
				}//END else	if display name not empty
		});
	});
}//END grabUsername

//Call this method in your code to see if the Tory helper class has finished its asynchronous calls and has data ready to be written to the given cylonStruct
void syncTory(struct cylonStruct& tf, Centurion::Tory^ tory)
{
	if (tory->InfoReady == true && tory->InfoCopied == false)
	{
		//copy
		tf.username = utf8_encode(tory->CylonName->Data());
		tf.pictureLocation = tory->PictureLocation;

		//Set path where Win10 holds profile pictures
		//Credit to AnandK@TWC on @ The Windows Club for path info
		tf.picturePath = "C:/Users/" + tf.username + "/AppData/Roaming/Microsoft/Windows/AccountPictures/";

		//prevent further copying
		tory->InfoCopied = true;
	}//END if info ready and not already copied
}

//For updating a cylonStruct
void updateFoster(struct cylonStruct& tf, Centurion::Tory^ tory)
{
	//Synchronize with data grabbed by helper reference class
	syncTory(tf, tory);

	//Grab new gamepad info
	updateGamepadInformation(tf);

	//update machine state data
	produceTimeZone(tf);
	produceDateTime(tf);
	produceMemoryInfo(tf);
}

//Update the buttons and axes of a given controller struct for a new reading
void updateControllerState(struct controllerStruct& controller, GamepadReading buttons)
{
	//Get/set new trigger values
	//TODO: change to float64?
	controller.leftTrigger = (float)buttons.LeftTrigger;
	controller.rightTrigger = (float)buttons.RightTrigger;

	//Get/set new thumbstick values
	controller.thumbLeftX = (float)buttons.LeftThumbstickX;
	controller.thumbLeftY = (float)buttons.LeftThumbstickY;
	controller.thumbRightX = (float)buttons.RightThumbstickX;
	controller.thumbRightY = (float)buttons.RightThumbstickY;

	//Set Buttons TODO add windows gaming constants
	controller.buttons = 0;
	if ((buttons.Buttons & GamepadButtons::A) == GamepadButtons::A)
	{
		controller.buttons |= A_BUTTON;
	}
	else if ((controller.buttons & A_BUTTON) == A_BUTTON)
	{
		controller.buttons -= A_BUTTON;
	}

	if ((buttons.Buttons & GamepadButtons::B) == GamepadButtons::B)
	{
		controller.buttons |= B_BUTTON;
	}
	else if ((controller.buttons & B_BUTTON) == B_BUTTON)
	{
		controller.buttons -= B_BUTTON;
	}

	if ((buttons.Buttons & GamepadButtons::X) == GamepadButtons::X)
	{
		controller.buttons |= X_BUTTON;
	}
	else if ((controller.buttons & X_BUTTON) == X_BUTTON)
	{
		controller.buttons -= X_BUTTON;
	}

	if ((buttons.Buttons & GamepadButtons::Y) == GamepadButtons::Y)
	{
		controller.buttons |= Y_BUTTON;
	}
	else if ((controller.buttons & Y_BUTTON) == Y_BUTTON)
	{
		controller.buttons -= Y_BUTTON;
	}

	if ((buttons.Buttons & GamepadButtons::Menu) == GamepadButtons::Menu)
	{
		controller.buttons |= START_BUTTON;
	}
	else if ((controller.buttons & START_BUTTON) == START_BUTTON)
	{
		controller.buttons -= START_BUTTON;
	}

	if ((buttons.Buttons & GamepadButtons::View) == GamepadButtons::View)
	{
		controller.buttons |= SELECT_BUTTON;
	}
	else if ((controller.buttons & SELECT_BUTTON) == SELECT_BUTTON)
	{
		controller.buttons -= SELECT_BUTTON;
	}

	if ((buttons.Buttons & GamepadButtons::LeftThumbstick) == GamepadButtons::LeftThumbstick)
	{
		controller.buttons |= LEFT_THUMB;
	}
	else if ((controller.buttons & LEFT_THUMB) == LEFT_THUMB)
	{
		controller.buttons -= LEFT_THUMB;
	}

	if ((buttons.Buttons & GamepadButtons::RightThumbstick) == GamepadButtons::RightThumbstick)
	{
		controller.buttons |= RIGHT_THUMB;
	}
	else if ((controller.buttons & RIGHT_THUMB) == RIGHT_THUMB)
	{
		controller.buttons -= RIGHT_THUMB;
	}

	if ((buttons.Buttons & GamepadButtons::LeftShoulder) == GamepadButtons::LeftShoulder)
	{
		controller.buttons |= LEFT_SHOULDER;
	}
	else if ((controller.buttons & LEFT_SHOULDER) == LEFT_SHOULDER)
	{
		controller.buttons -= LEFT_SHOULDER;
	}

	if ((buttons.Buttons & GamepadButtons::RightShoulder) == GamepadButtons::RightShoulder)
	{
		controller.buttons |= RIGHT_SHOULDER;
	}
	else if ((controller.buttons & RIGHT_SHOULDER) == RIGHT_SHOULDER)
	{
		controller.buttons -= RIGHT_SHOULDER;
	}

	if ((buttons.Buttons & GamepadButtons::DPadUp) == GamepadButtons::DPadUp)
	{
		controller.buttons |= UP_DPAD;
	}
	else if ((controller.buttons & UP_DPAD) == UP_DPAD)
	{
		controller.buttons -= UP_DPAD;
	}

	if ((buttons.Buttons & GamepadButtons::DPadDown) == GamepadButtons::DPadDown)
	{
		controller.buttons |= DOWN_DPAD;
	}
	else if ((controller.buttons & DOWN_DPAD) == DOWN_DPAD)
	{
		controller.buttons -= DOWN_DPAD;
	}

	if ((buttons.Buttons & GamepadButtons::DPadLeft) == GamepadButtons::DPadLeft)
	{
		controller.buttons |= LEFT_DPAD;
	}
	else if ((controller.buttons & LEFT_DPAD) == LEFT_DPAD)
	{
		controller.buttons -= LEFT_DPAD;
	}

	if ((buttons.Buttons & GamepadButtons::DPadRight) == GamepadButtons::DPadRight)
	{
		controller.buttons |= RIGHT_DPAD;
	}
	else if ((controller.buttons & RIGHT_DPAD) == RIGHT_DPAD)
	{
		controller.buttons -= RIGHT_DPAD;
	}
}

//For updating controllers via xinput
//NOTE: can throw app-crashing exceptions when multiple gamepads are in use and disconnect in certain orders, use at your own risk!
void updateControllers(struct cylonStruct& tf)
{
	//Variable Declaration
	DWORD result;
	XINPUT_STATE state;
	Windows::Devices::Enumeration::DeviceInformation^ deviceInfo;

	//for plays 0-maxPlayerCount
	for (DWORD userIndex = 0; userIndex < XUSER_MAX_COUNT; userIndex++)
	{
		//zero memory
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		//Get state of controller
		result = XInputGetState(userIndex, &state);

		if (result == ERROR_SUCCESS)
		{
			debug(L"Got state");

			//keep track of if we found the controller in the list
			bool found = false;

			//iterate over 
			for (list<controllerStruct>::iterator iterator = tf.controllers.begin(), end = tf.controllers.end(); iterator != end; ++iterator)
			{
				//if the controller matches
				if (iterator->userIndex == userIndex)
				{
					//controller match found in list
					found = true;

					//update the fields of the controller
					//Normalize Trigger values
					float oldTriggerMin = (float)0.0;
					float oldTriggerMax = (float)255.0;

					float newTriggerMin = (float)0.0;
					float newTriggerMax = (float)1.0;

					float newTriggerRange = (float)1.0;
					float oldTriggerRange = (float)255.0;

					float oldTriggerLeft = (float)state.Gamepad.bLeftTrigger;
					float oldTriggerRight = (float)state.Gamepad.bRightTrigger;

					//Get/set new trigger values
					iterator->leftTrigger = (float)((((oldTriggerLeft - oldTriggerMin) * newTriggerRange) / oldTriggerRange) + newTriggerMin);
					iterator->rightTrigger = (float)((((oldTriggerRight - oldTriggerMin) * newTriggerRange) / oldTriggerRange) + newTriggerMin);

					//Normalize thumbstick values
					float oldThumbMin = (float)-32768.0;
					float oldThumbMax = (float)32767.0;

					float newThumbMin = (float)-1.0;
					float newThumbMax = (float)1.0;

					float oldThumbRange = (float)(32767 + 32768);
					float newThumbRange = (float)(2.0);

					float oldThumbLeftX = (float)state.Gamepad.sThumbLX;
					float oldThumbLeftY = (float)state.Gamepad.sThumbLY;
					float oldThumbRightX = (float)state.Gamepad.sThumbRX;
					float oldThumbRightY = (float)state.Gamepad.sThumbRY;

					//Get/set new thumbstick values
					iterator->thumbLeftX = (float)((((oldThumbLeftX - oldThumbMin) * newThumbRange) / oldThumbRange) + newThumbMin);
					iterator->thumbLeftY = (float)((((oldThumbLeftY - oldThumbMin) * newThumbRange) / oldThumbRange) + newThumbMin);
					iterator->thumbRightX = (float)((((oldThumbRightX - oldThumbMin) * newThumbRange) / oldThumbRange) + newThumbMin);
					iterator->thumbRightY = (float)((((oldThumbRightY - oldThumbMin) * newThumbRange) / oldThumbRange) + newThumbMin);

					//Set properties
					iterator->buttons = state.Gamepad.wButtons;
					iterator->packetNumber = state.dwPacketNumber;
					//user index values should not repeat, so break
					break;
				}//END if match
			}//END for

			//if we still haven't found a controller with the appropriate user index
			if (!found)
			{
				//build device struct
				struct deviceStruct device = buildDevice(deviceInfo, CONTROLLER_TYPE);

				//build controller struct
				struct controllerStruct controller = buildController(device, state, userIndex);

				//insert into controllers
				tf.controllers.push_back(controller);
				debug(L"Controller Added");

				//set controller index
				controller.superDevice.controllerIndex = tf.controllers.size() - 1;

				//insert into devices
				tf.detectedDevices.push_back(controller.superDevice);
				debug(L"Device Added");

				//sync lists
				tf.controllers.back().superDevice = tf.detectedDevices.back();
			}//END if not found
		}//END If controller connected
		
		else
		{
			debug(L"No state");
			//TODO: remove the controller and device from the lists
			//iterate over 
			for (list<controllerStruct>::iterator iterator = tf.controllers.begin(), end = tf.controllers.end(); iterator != end; ++iterator)
			{
				//pick the right controller struct
				if (iterator->userIndex == userIndex)
				{
					//build name of deviceStruct to look for
					std::string deviceName = "XINPUT Controller #";
					deviceName += (int)userIndex;

					//iterate over devices list
					for (list<deviceStruct>::iterator iteratorDevices = tf.detectedDevices.begin(), end = tf.detectedDevices.end(); iteratorDevices != end; ++iteratorDevices)
					{
						//check to make sure the superDevice matches the deviceStruct in the detectedDevices list
						if
							(
								(iterator->superDevice.name.compare(iteratorDevices->name) == 0) &&
								(iteratorDevices->deviceType == CONTROLLER_TYPE)
								//TODO: add id_int check later too
								)
						{
							//remove the node and break
							tf.detectedDevices.erase(iteratorDevices);
							debug(L"Device Removed");
							break;
						}//END if devices match
					}//END for all detectedDevices

					//remove the controllerStruct from controllers and get out
					tf.controllers.erase(iterator);
					debug(L"Controller Removed");
					break;
				}//END if userIndex matches
			}//END for all controllers

		}//END if no controller detected for a given user index
	}//END FOR all user index values
}//END update controllers

void updateGamepadInformation(struct cylonStruct& tf)
{
	//If a gamepad was removed, clear out the controllers 
	if (((gamepadState == REMOVED) || (gamepadState == ALL_REMOVED)) && controllersPurged == false)
	{
		//prevent unnecssary multiple list traversals for each update call 
		controllersPurged = true;

		//purge the controllers from collected device lists
		tf.controllers.clear();
		for (list<deviceStruct>::iterator iterator = tf.detectedDevices.begin(), end = tf.detectedDevices.end(); iterator != end; ++iterator)
		{
			if (iterator->deviceType == CONTROLLER_TYPE)
			{
				tf.detectedDevices.erase(iterator);
			}//END if controller
		}//end detected devices iterator
	}//end if gamepad removed
	else if (gamepadState == STARTED)
	{
		//Grab controller info
		for (uint32_t i = 0; i < Gamepad::Gamepads->Size; i++)
		{
			//Prepare flag
			bool found = false;

			//GamepadReading
			GamepadReading reading = Gamepad::Gamepads->GetAt(i)->GetCurrentReading();

			//iterate over controllers to determine if a gamepad already exists for this user index or if a new one was recently added
			for (list<controllerStruct>::iterator iterator = tf.controllers.begin(), end = tf.controllers.end(); iterator != end; ++iterator)
			{
				if (iterator->userIndex == i)
				{
					//Set the flag
					found = true;
					
					//Update the state of the controller
					struct controllerStruct gamePad = (*iterator);
					updateControllerState(gamePad, reading);
					(*iterator) = gamePad;

					break;
				}//end if match
			}//END for controllers

			 //if no match found
			if (!found)
			{
				//Build a new controller and device struct and insert them into the lists
				//Build Device and Controller Structs
				struct deviceStruct device = buildDevice(i);
				struct controllerStruct controller = buildController(device, i, reading);

				//insert into lists an dsync
				tf.controllers.push_back(controller);
				controller.superDevice.controllerIndex = tf.controllers.size() - 1;
				tf.detectedDevices.push_back(controller.superDevice);
				tf.controllers.back().superDevice = tf.detectedDevices.back();
			}
		}//END for all Gamepads
	}//END else if
}//END updater

//Event listener for new gamepads
void OnGamepadAdded(_In_ Object^ sender, _In_ Gamepad^ gamepad)
{
	presentGamepadsAdded++;

	if ((presentGamepadsAdded == 1) && (gamepadState == ALL_REMOVED))
	{
		gamepadState = STARTED;
		controllersPurged = false; //we now have controllers
	}
}//END OnGamepadAdded

//Event listener for removed gamepads
void OnGamepadRemoved(_In_ Object^ sender, _In_ Gamepad^ gamepad)
{
	presentGamepadsAdded--;

	if (presentGamepadsAdded > 0)
	{
		gamepadState = REMOVED;
	}
	else if (presentGamepadsAdded == 0)
	{
		gamepadState = ALL_REMOVED;
	}
}//END OnGamepadRemoved