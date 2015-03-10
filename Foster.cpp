//Foster.cpp
//Implements definitions of Foster functionality.
//"Bad becomes good" - Tory Foster
//josh@mindaptiv.com

//includes
#include "pch.h"  //TODO remove this if necessary for later deployments of Foster
#include "Foster.h"

//If Visual Studio freaks out about this code someday, add this line back in OR modify your project settings
#pragma comment(lib, "Ws2_32.lib")



//Method definitions:
//Producers:
//for getting username
void produceUsername(struct cylonStruct& tory)
{
	//Variable declaration
	Platform::String^ managedUsername;
	Windows::Foundation::IAsyncOperation<Platform::String^>^ operation;
	const wchar_t* operationDataPointer;
	std::wstring wideUsername;
	std::string username;

	//Retrieve username
	operation			= Windows::System::UserProfile::UserInformation::GetDisplayNameAsync();
	while(operation->Status == Windows::Foundation::AsyncStatus::Started)
	{
		//WAIT, YO
	}
	managedUsername = operation->GetResults();
	operation->Close();

	//Convert username to std::wstring
	operationDataPointer	= managedUsername->Data();
	wideUsername			= std::wstring(operationDataPointer);

	//TODO convert from wstring to string
	

	//check if retrieved username is empty string? (therefore UserInformation::NameAccessAllowed property would most likely be set to false)
	if (wideUsername.length() <= 0)
	{
		wideUsername = L"0";
	}

	//Set username
	tory.wUsername = wideUsername;
}
//end getDisplayNameAsync

//Fills cylonStruct with timezone name, UTC offset bias, and dst flag
void produceTimeZone(struct cylonStruct& tory)
{
	//Variable Declaration
	DWORD					tzResult;
	TIME_ZONE_INFORMATION	tzinfo;
	std::wstring			timezoneName;

	//grab and convert bias
	tzResult	= GetTimeZoneInformation(&tzinfo);

	//set bias
	tory.timeZone = tzinfo.Bias;
	
	//Check DWORD value
	if (tzResult == TIME_ZONE_ID_STANDARD)
	{
		//standard time
		tory.dst = 1;
	}
	else if (tzResult == TIME_ZONE_ID_DAYLIGHT)
	{
		//daylight time
		tory.dst = 2;
	}
	else
	{
		//otherwise or invalid ==> shenanigans
		//"Oh hell! I have to run home and grab my broom!"
		tory.dst = 0;
	}
	//end if

	//grab time zone name
	std::wstring standardName;

	//grab name from TimeZoneInformation
	standardName = tzinfo.StandardName;

	//place string name into tory
	tory.wTimeZoneName = standardName;
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
	tory.milliseconds	= st.wMilliseconds;
	tory.seconds		= st.wSecond;
	tory.minutes		= st.wMinute;
	tory.hours			= st.wHour;
	
	if (0 <= st.wDayOfWeek && st.wDayOfWeek <= 6)
	{
		//1 = Sun, ..., 7 = Sat
		tory.day = st.wDayOfWeek + 1;
	}
	else
	{
		//error case
		tory.day = 0;
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

	tory.year			= st.wYear;
}
//end produceDateTime

//populates tory's device name
void produceDeviceName(struct cylonStruct& tory) 
{
	//Variable declaration
	int				result;
	int				size_needed;
	char			hostBuffer[MAX_PATH];
	std::string		deviceName;
	WSAData			wsa_data;
	
	//start WSA
	WSAStartup(MAKEWORD(1, 1), &wsa_data);

	//grab result
	result		= gethostname(hostBuffer, MAX_PATH);
	deviceName	= hostBuffer;

	//check socket errors
	int error;
	error = WSAGetLastError();

	//cleanup WSA
	WSACleanup();

	//convert string to wstring
	size_needed = MultiByteToWideChar(CP_UTF8, 0, &deviceName[0], (int)deviceName.size(), NULL, 0);
	std::wstring wDeviceName(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &deviceName[0], (int)deviceName.size(), &wDeviceName[0], size_needed);

	//TODO convert to utf8


	//Check for empty name
	if (wDeviceName.length() <= 0)
	{
		wDeviceName = L"0";
	}//end if

	//set device name for tory
	tory.wDeviceName = wDeviceName;
}
//end produceDeviceName

//for getting processor info
void produceProcessorInfo(struct cylonStruct& tf)
{
	//Variable Declaration
	SYSTEM_INFO sysinfo;
	unsigned int architecture;
	float32 minHertzz = 1000000000;

	//Grab system info
	GetNativeSystemInfo(&sysinfo);
	
	//Convert results into local values
	//Convert architecture
	if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
	{
		//x64 (AMD or Intel)
		architecture = 1;
	}
	else if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM)
	{
		//ARM
		architecture = 2;
	}
	else if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
		//Intel Itanium-based
		architecture = 3;
	}
	else if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
	{
		//x86
		architecture = 4;
	}
	else
	{
		//unknown error
		architecture = 0;
	}
	//end if

	//set tory architecture
	tf.architecture = architecture;

	//set tory page size
	tf.pageSize = (unsigned long)sysinfo.dwPageSize;

	//set the min and max pointers for apps
	tf.minAppAddress = (void*)sysinfo.lpMinimumApplicationAddress;
	tf.maxAppAddress = (void*)sysinfo.lpMaximumApplicationAddress;

	//set the number of processors
	tf.processorCount = (UINT64)sysinfo.dwNumberOfProcessors;

	//set allocation granularity
	tf.allocationGranularity = (unsigned long)sysinfo.dwAllocationGranularity;

	//grab default minimum CPU hertz
	tf.hertz = minHertzz;
}
//end produce processor info

//via Ted's Blog
HMODULE GetKernelModule()
{
	//hack to get into kernel32
	MEMORY_BASIC_INFORMATION mbi = {0};
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
}
//end produceMemoryInfo

//for getting account picture
void produceAccountPicture(struct cylonStruct& tf)
{
	//variable declaration
	Windows::System::UserProfile::AccountPictureKind kind = Windows::System::UserProfile::AccountPictureKind::SmallImage;
	Windows::Storage::IStorageFile^ picture;
	std::wstring pictureType;
	const wchar_t* typeDataPointer;

	//retrieve picture
	picture = Windows::System::UserProfile::UserInformation::GetAccountPicture(kind);

	//set picture
	//TODO get picture location for platform agnosticity
	//tf.picture = picture;

	//convert picture type to wstring
	typeDataPointer = picture->FileType->Data();
	pictureType = std::wstring(typeDataPointer);

	//set picture type
	tf.pictureType = pictureType;
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
		deviceStructType = 1;
	}
	else if (type == "AudioCapture" || type == "audioCapture")
	{
		deviceType = Windows::Devices::Enumeration::DeviceClass::AudioCapture;
		deviceStructType = 2;
	}
	else if (type == "AudioRender" || type == "audioRender")
	{
		deviceType = Windows::Devices::Enumeration::DeviceClass::AudioRender;
		deviceStructType = 3;
	}
	else if (type == "PortableStorageDevice" || type == "portableStorageDevice")
	{
		deviceType = Windows::Devices::Enumeration::DeviceClass::PortableStorageDevice;
		deviceStructType = 4;
	}
	else if (type == "VideoCapture" || type == "videoCapture")
	{
		deviceType = Windows::Devices::Enumeration::DeviceClass::VideoCapture;
		deviceStructType = 5;
	}
	else if (type == "ImageScanner" || type == "imageScanner")
	{
		deviceType = Windows::Devices::Enumeration::DeviceClass::ImageScanner;
		deviceStructType = 6;
	}
	else if (type == "Location" || type == "location")
	{
		deviceType = Windows::Devices::Enumeration::DeviceClass::Location;
		deviceStructType = 7;
	}
	else
	{
		//"Hey... You ever wonder why we're here?" - Simmons
		//ERROR case, default to all
		deviceType = Windows::Devices::Enumeration::DeviceClass::All;
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
		tf.installedDeviceCount	= devices->Size;
	}
	else if (type == "AudioCapture" || type == "audioCapture")
	{
		//Store Size
		tf.micCount				= devices->Size;
	}//END IF
	else if (type == "AudioRender" || type == "audioRender")
	{
		tf.speakerCount			= devices->Size;
	}
	else if (type == "PortableStorageDevice" || type == "portableStorageDevice")
	{
		tf.portableStorageCount		= devices->Size;
	}
	else if (type == "VideoCapture" || type == "videoCapture")
	{
		tf.videoCount			= devices->Size;
	}
	else if (type == "ImageScanner" || type == "imageScanner")
	{
		tf.scannerCount			= devices->Size;
	}
	else if (type == "Location" || type == "location")
	{
		tf.locationCount		= devices->Size;
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

		//put device in detectedDevices
		tf.detectedDevices.insert(tf.detectedDevices.end(), device);

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
	produceControllerInformation(tf);

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
	unsigned int	deviceType = 8;

	//Build super
	superDevice = buildDevice(deviceInfo, 8);

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
	displayDevice.superDevice.displayIndex = tf.displayDevices.size();
	tf.displayDevices.insert(tf.displayDevices.end(), displayDevice); 
	tf.detectedDevices.insert(tf.detectedDevices.end(), displayDevice.superDevice);
}//END produceDisplayInformation

//produces information about pointer devices
void produceMouseInformation(struct cylonStruct& tf)
{
	//Variable Declaration
	Windows::Devices::Input::MouseCapabilities mouseStats;
	Windows::Devices::Enumeration::DeviceInformation^ deviceInfo;
	unsigned int mouseType = 9;
	struct deviceStruct mouse;
	struct mouseStruct mice;

	//check if mouse exists
	if (mouseStats.MousePresent == 1)
	{
		//build device
		mouse = buildDevice(deviceInfo, mouseType);

		//insert mouse device into detectedDevices
		tf.detectedDevices.insert(tf.detectedDevices.end(), mouse);

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

		tf.mice.maxNumberOfButons			= mouseStats.NumberOfButtons;
	}
}

//create deviceStruct for keyboard
void produceKeyboardInformation(struct cylonStruct& tf)
{
	//Variable Declaration
	struct deviceStruct keyboard;
	unsigned int keyboardType = 10;
	Windows::Devices::Enumeration::DeviceInformation^ deviceInfo;
	Windows::Devices::Input::KeyboardCapabilities keyboardInfo;

	//check if keyboard exists

	//If keyboard exists
	if (keyboardInfo.KeyboardPresent == 1)
	{
		//build device
		keyboard = buildDevice(deviceInfo, keyboardType);

		//insert keyboard device into detectedDevices
		tf.detectedDevices.insert(tf.detectedDevices.end(), keyboard);
	}//END if
}
//end produce Keyboard information

//grabs information for (up to) 4 XInput controllers
void produceControllerInformation(struct cylonStruct& tf)
{
	//Variable Declaration
	DWORD result;
	XINPUT_STATE state;
	Windows::Devices::Enumeration::DeviceInformation^ deviceInfo;
	unsigned int controllerType = 11;

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
			struct deviceStruct device = buildDevice(deviceInfo, controllerType);

			//build controller struct
			struct controllerStruct controller = buildController(device, state, userIndex);

			//set controller index
			controller.superDevice.controllerIndex = tf.controllers.size();

			//insert into controllers
			tf.controllers.insert(tf.controllers.end(), controller);

			//insert into devices
			tf.detectedDevices.insert(tf.detectedDevices.end(), controller.superDevice);
		}//END If controller connected

	}//END FOR
}//END produceControllerInfo

//produce Tory
void produceTory(struct cylonStruct& tory)
{
	//Clear pre-existing lists
	tory.detectedDevices.clear();
	tory.displayDevices.clear();
	tory.controllers.clear();

	//username
	produceUsername(tory);

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
}
//end produce tory 
//END producers

//Builders
//build Tory
struct cylonStruct buildTory()
{
	//Variable Declartion
	struct cylonStruct tory;

	//username
	produceUsername(tory);

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

	//return
	return tory;
}
//end build tory

//build a device struct with given data
struct deviceStruct buildDevice(Windows::Devices::Enumeration::DeviceInformation^ deviceInfo, unsigned int deviceType)
{
	//Variable Decalaration
	struct deviceStruct device;
	std::wstring error;
	error = L"0";
	
	//set device type
	device.deviceType = deviceType;

	//set to zero for now, modify later if necessary
	device.displayIndex		= 0;
	device.controllerIndex	= 0;

	//get out for display/keyboard/mouse/controller devices, as they have different metadata than the regular kind we retrieve
	if (device.deviceType == 8 || device.deviceType == 10 || device.deviceType == 9 || device.deviceType == 11)
	{
		//set errors/unknown values
		device.wName = error;
		device.wID	 = error;
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
		device.wName = error;
	}
	else
	{	
		device.wName = deviceInfo->Name->Data();
	}//END if Name Empty
	
	if (deviceInfo->Id->IsEmpty())
	{
		device.wID = error;
	}
	else
	{
		device.wID = deviceInfo->Id->Data();
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
			device.panelLocation = 3;
		}
		else if (deviceInfo->EnclosureLocation->Panel.Equals(Windows::Devices::Enumeration::Panel::Bottom))
		{
			device.panelLocation = 4;
		}
		else if (deviceInfo->EnclosureLocation->Panel.Equals(Windows::Devices::Enumeration::Panel::Front))
		{
			device.panelLocation = 1;
		}
		else if (deviceInfo->EnclosureLocation->Panel.Equals(Windows::Devices::Enumeration::Panel::Back))
		{
			device.panelLocation = 2;
		}
		else if (deviceInfo->EnclosureLocation->Panel.Equals(Windows::Devices::Enumeration::Panel::Left))
		{
			device.panelLocation = 5;
		}
		else if (deviceInfo->EnclosureLocation->Panel.Equals(Windows::Devices::Enumeration::Panel::Right))
		{
			device.panelLocation = 6;
		}
		else
		{
			//unknown or error
			device.panelLocation = 0;
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
		displayDevice.rotationPreference = 0;
	}
	else if (displayInformation->AutoRotationPreferences == Windows::Graphics::Display::DisplayOrientations::Landscape)
	{
		displayDevice.rotationPreference = 1;
	}
	else if (displayInformation->AutoRotationPreferences == Windows::Graphics::Display::DisplayOrientations::Portrait)
	{
		displayDevice.rotationPreference = 2;
	}
	else if (displayInformation->AutoRotationPreferences == Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped)
	{
		displayDevice.rotationPreference = 4;
	}
	else if (displayInformation->AutoRotationPreferences == Windows::Graphics::Display::DisplayOrientations::PortraitFlipped)
	{
		displayDevice.rotationPreference = 8;
	}
	else
	{
		//error case
		displayDevice.rotationPreference = 0;
	}//END if orientation

	//current monitor orientation
	if (displayInformation->CurrentOrientation == Windows::Graphics::Display::DisplayOrientations::None)
	{
		displayDevice.currentRotation = 0;
	}
	else if (displayInformation->CurrentOrientation == Windows::Graphics::Display::DisplayOrientations::Landscape)
	{
		displayDevice.currentRotation = 1;
	}
	else if (displayInformation->CurrentOrientation == Windows::Graphics::Display::DisplayOrientations::Portrait)
	{
		displayDevice.currentRotation = 2;
	}
	else if (displayInformation->CurrentOrientation == Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped)
	{
		displayDevice.currentRotation = 4;
	}
	else if (displayInformation->CurrentOrientation == Windows::Graphics::Display::DisplayOrientations::PortraitFlipped)
	{
		displayDevice.currentRotation = 8;
	}
	else
	{
		//error case
		displayDevice.currentRotation = 0;
	}//END current monitor orientation

	//native monitor orientation
	if (displayInformation->NativeOrientation == Windows::Graphics::Display::DisplayOrientations::None)
	{
		displayDevice.nativeRotation = 0;
	}
	else if (displayInformation->NativeOrientation == Windows::Graphics::Display::DisplayOrientations::Landscape)
	{
		displayDevice.nativeRotation = 1;
	}
	else if (displayInformation->NativeOrientation == Windows::Graphics::Display::DisplayOrientations::Portrait)
	{
		displayDevice.nativeRotation = 2;
	}
	else if (displayInformation->NativeOrientation == Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped)
	{
		displayDevice.nativeRotation = 4;
	}
	else if (displayInformation->NativeOrientation == Windows::Graphics::Display::DisplayOrientations::PortraitFlipped)
	{
		displayDevice.nativeRotation = 8;
	}
	else
	{
		//error case
		displayDevice.nativeRotation = 0;
	}//END if native monitor orientation

	//DPI
	displayDevice.logicalDPI	= displayInformation->LogicalDpi;
	displayDevice.rawDPIX		= displayInformation->RawDpiX;
	displayDevice.rawDPIY		= displayInformation->RawDpiY;

	//Stereoscopic 3D
	displayDevice.isStereoscopicEnabled = displayInformation->StereoEnabled;

	//Resolution Scale
	if (displayInformation->ResolutionScale == Windows::Graphics::Display::ResolutionScale::Invalid)
	{
		displayDevice.resolutionScale = 0;
	}
	else if (displayInformation->ResolutionScale == Windows::Graphics::Display::ResolutionScale::Scale100Percent)
	{
		displayDevice.resolutionScale = 100;
	}
	else if (displayInformation->ResolutionScale == Windows::Graphics::Display::ResolutionScale::Scale120Percent)
	{
		displayDevice.resolutionScale = 120;
	}
	else if (displayInformation->ResolutionScale == Windows::Graphics::Display::ResolutionScale::Scale140Percent)
	{
		displayDevice.resolutionScale = 140;
	}
	else if (displayInformation->ResolutionScale == Windows::Graphics::Display::ResolutionScale::Scale150Percent)
	{
		displayDevice.resolutionScale = 150;
	}
	else if (displayInformation->ResolutionScale == Windows::Graphics::Display::ResolutionScale::Scale160Percent)
	{
		displayDevice.resolutionScale = 160;
	}
	else if (displayInformation->ResolutionScale == Windows::Graphics::Display::ResolutionScale::Scale180Percent)
	{
		displayDevice.resolutionScale = 180;
	}
	else if (displayInformation->ResolutionScale == Windows::Graphics::Display::ResolutionScale::Scale225Percent)
	{
		displayDevice.resolutionScale = 225;
	}
	else
	{
		//error case
		displayDevice.resolutionScale = 0;
	}

	//return
	return displayDevice;
}
//END build display

//build a controllerStruct for a given player number and XINPUT_STATE object
struct controllerStruct buildController(struct deviceStruct superDevice, XINPUT_STATE state, DWORD userIndex)
{
	//Variable Declaration
	struct controllerStruct controller;

	//Set properties
	controller.superDevice  = superDevice; //set parent
	controller.userIndex	= (unsigned int)userIndex;
	controller.buttons		= state.Gamepad.wButtons;
	controller.packetNumber = state.dwPacketNumber;
	controller.leftTrigger	= state.Gamepad.bLeftTrigger;
	controller.rightTrigger = state.Gamepad.bLeftTrigger;
	controller.thumbLeftX	= state.Gamepad.sThumbLX;
	controller.thumbLeftY	= state.Gamepad.sThumbLY;
	controller.thumbRightX	= state.Gamepad.sThumbRX;
	controller.thumbRightY  = state.Gamepad.sThumbRY;

	return controller;
}//END build controller
//END Builders