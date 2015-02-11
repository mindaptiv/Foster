//Foster.cpp
//Implements definitions of Foster functionality.
//"Bad becomes good" - Tory Foster
//josh@mindaptiv.com

//includes
#include "pch.h"
#include "Foster.h"
#include <windows.foundation.h>
#include <windows.system.h>
#include <sysinfoapi.h>

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
//end getters

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
	while(	operation->Status == Windows::Foundation::AsyncStatus::Started)
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

	//test
	int one = 9;

	one = one * 9;
	//end test
	
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
	
}
//end produce processor info

//Constructor
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
	
	//TODO add more host queries


	//return
	return tory;
}
//end build tory