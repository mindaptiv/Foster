//Foster.cpp
//Implements definitions of Foster functionality.
//"Bad becomes good" - Tory Foster
//josh@mindaptiv.com

//includes
#include "pch.h"  //TODO remove this if necessary for later deployments of Foster
#include "Foster.h"


//If Visual Studio freaks out about this code someday, add this line back in OR modify your project settings
//#pragma comment(lib, "Ws2_32.lib")

//Method definitions:
//getters
unsigned int getFosterMilliseconds(struct cylonStruct tf)
{
	//return
	return tf.milliseconds;
}

unsigned int getFosterSeconds(struct cylonStruct tf)
{
	//return
	return tf.seconds;
}

unsigned int getFosterMinutes(struct cylonStruct tf)
{
	//return
	return tf.minutes;
}

unsigned int getFosterHours(struct cylonStruct tf)
{
	//return
	return tf.hours;
}

unsigned int getFosterDay(struct cylonStruct tf)
{
	//return
	return tf.day;
}

unsigned int getFosterDate(struct cylonStruct tf)
{
	//return
	return tf.date;
}

unsigned int getFosterMonth(struct cylonStruct tf)
{
	//return
	return tf.month;
}

unsigned int getFosterYear(struct cylonStruct tf)
{
	//return
	return tf.year;
}

long getFosterTimeZone(struct cylonStruct tf)
{
	//return
	return tf.timeZone;
}

unsigned int getFosterDST(struct cylonStruct tf)
{
	//return
	return tf.dst;
}

std::wstring getFosterTimeZoneName(struct cylonStruct tf)
{
	//return
	return tf.wTimeZoneName;
}

std::wstring getFosterUsername(struct cylonStruct tf)
{
	//return
	return tf.wUsername;
}

std::wstring getFosterDeviceName(struct cylonStruct tf)
{
	//return
	return tf.wDeviceName;
}

unsigned short getFosterArchitecture(struct cylonStruct tf)
{
	//return
	return tf.architecture;
}

unsigned short getFosterProcessorLevel(struct cylonStruct tf)
{
	//return
	return tf.processorLevel;
}

unsigned long getFosterPageSize(struct cylonStruct tf)
{
	//return
	return tf.pageSize;
}

unsigned long getFosterProcessorCount(struct cylonStruct tf)
{
	//return
	return tf.processorCount;
}

unsigned long getFosterAllocationGranularity(struct cylonStruct tf)
{
	//return
	return tf.allocationGranularity;
}

void* getFosterMinAppAddress(struct cylonStruct tf)
{
	//return
	return tf.minAppAddress;
}

void* getFosterMaxAppAddress(struct cylonStruct tf)
{
	//return 
	return tf.maxAppAddress;
}

std::wstring getFosterPictureType(struct cylonStruct tf)
{
	//return
	return tf.pictureType;

}

Windows::Storage::IStorageFile^ getFosterPictureFile(struct cylonStruct tf)
{
	//return 
	return tf.picture;
}
//end getters


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
	

	//TODO: check if retrieved username is empty string? (therefore UserInformation::NameAccessAllowed property would be set to false)

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
		tory.dst = 0;
	}
	else if (tzResult == TIME_ZONE_ID_DAYLIGHT)
	{
		//daylight time
		tory.dst = 1;
	}
	else
	{
		//otherwise or invalid ==> shenanigans
		//"Oh hell! I have to run home and grab my broom!"
		tory.dst = 2;  //value is arbitrary
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
	
	tory.day			= st.wDayOfWeek;
	tory.date			= st.wDay;
	tory.month			= st.wMonth;
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
	tf.processorCount = (unsigned long)sysinfo.dwNumberOfProcessors;

	//set allocation granularity
	tf.allocationGranularity = (unsigned long)sysinfo.dwAllocationGranularity;

	//TODO grab hertz
}
//end produce processor info

//TEST
//TODO CLEANUP IF WORK
typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

//via Ted's Blog
HMODULE GetKernelModule()
{
	//hack to get into kernel32
	MEMORY_BASIC_INFORMATION mbi = {0};
	VirtualQuery(VirtualQuery, &mbi, sizeof(mbi));
	return reinterpret_cast<HMODULE>(mbi.AllocationBase);
}

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
	//TODO update when WinRT, etc. supports retrieving RAM information from a device
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
	tf.picture = picture;

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
	Windows::Devices::Enumeration::DeviceClass deviceType; // = Windows::Devices::Enumeration::DeviceClass::AudioRender;
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

	//TODO finish display devices
	//Variable Declaration
	Windows::Graphics::Display::DisplayInformation^ displayInformation;
	
	//Grab display info
	displayInformation = Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
	
	
	//TODO add controller devices


}//END produce device information

//produces device information for all types except the "all" filter
//for "all" use produceDeviceInformation
void produceDeviceTypesInformation(struct cylonStruct& tf)
{
	//Grab collections and counts
	produceDeviceTypeInformation(tf, "AudioCapture");
	produceDeviceTypeInformation(tf, "AudioRender");
	produceDeviceTypeInformation(tf, "Location");
	produceDeviceTypeInformation(tf, "ImageScanner");
	produceDeviceTypeInformation(tf, "VideoCapture");
	produceDeviceTypeInformation(tf, "PortableStorageDevice");

	//Grab total count
	tf.detectedDeviceCount = tf.micCount + tf.speakerCount + tf.locationCount + tf.scannerCount + tf.videoCount + tf.portableStorageCount;
	//tf.detectedDeviceCount = tf.installedDeviceCount;
}//END produce device types information
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

	//TODO add more host queries


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

	//TODO STRESS TEST
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
/*struct displayStruct buildDisplay(struct deviceStruct super)
{

}*/
//END build display
//END Builders