//Foster.h 
//Defines functionality for Foster service.  Retrieves system details from Windows 8.1 using a combination of Windows Store API calls and legacy Win32 API calls.
//"Bad becomes good," - Tory Foster
//josh@mindaptiv.com

//pragmas
#pragma once
#pragma comment(lib, "Xinput.lib")

//includes
#include "Cylon.h"
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include "Cybro.h"
#include <collection.h>
#include <limits.h>

//includes for windows functionality
#include <WinSock2.h>
#include <Xinput.h>
#include <windows.storage.h>
#include <ppltasks.h>
#include <sstream>
#include <Windows.h>

using namespace Windows::Storage::Streams;

//definitions
//via Ted's Blog
typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

//encoding
std::string utf8_encode(const std::wstring &wstr);
std::wstring utf8_decode(const std::string &str);

//Method Declaration:
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
void		produceDeviceTypeInformation(struct cylonStruct& tf, std::string type);
void		produceDeviceTypesInformation(struct cylonStruct& tf);
void		produceDisplayInformation(struct cylonStruct& tf);
void		produceMouseInformation(struct cylonStruct& tf);
void		produceKeyboardInformation(struct cylonStruct& tf);
void		produceControllerInformation(struct cylonStruct& tf);

//for logging
void produceLog(struct cylonStruct& tf);

//for producing values for a pre-existing cylonStruct
void produceTory(struct cylonStruct& tory);

//Builders
//NOTE: not situated in a cylon.cpp because the method of construction may vary on later platforms
//build Tory for the current machine
struct cylonStruct buildTory();

//build a deviceStruct for a given DeviceInformation object
struct deviceStruct buildDevice(Windows::Devices::Enumeration::DeviceInformation^ deviceInfo, unsigned int deviceType);

//build a displayStruct for a given DisplayInformation object
struct displayStruct buildDisplay(struct deviceStruct superDevice, Windows::Graphics::Display::DisplayInformation^ displayInformation);

//build a controllerStruct for a given player number and XINPUT_STATE object
struct controllerStruct buildController(struct deviceStruct, XINPUT_STATE state, DWORD userIndex);

//build a storageStruct for a given DeviceInformation object
struct storageStruct buildStorage(Windows::Devices::Enumeration::DeviceInformation^ deviceInfo, struct deviceStruct superDevice);

//build a message to be logged to debug
void debug(std::wstring str);
//End Builders
//End methods declaration

//Credit to jeroendesloovere @ github for the Platform property management examples
namespace Centurion
{
	public ref class Tory sealed
	{
	public:
		Tory() 
		{ 
			cylonName = "0";
			nonRoamableId = "0";
			infoReady = false;
			infoCopied = false;

			grabUserInfo();
		}
		void grabUserInfo();
		property Platform::String^ CylonName
		{
			Platform::String^ get() 
				{ return cylonName; }
			void set(Platform::String^ i) 
				{ cylonName = i; }
		}
		property Platform::String^ NonRoamableId
		{
			Platform::String^ get()
			{
				return nonRoamableId;
			}

			void set(Platform::String^ i)
			{
				nonRoamableId = i;
			}
		}

		property Platform::Boolean InfoReady
		{
			Platform::Boolean get() 
				{ return infoReady; }
			void set(Platform::Boolean i) 
				{ infoReady = i; }
		}
		property Platform::Boolean InfoCopied
		{
			Platform::Boolean get()
			{
				return infoCopied;
			}
			void set(Platform::Boolean i)
			{
				infoCopied = i;
			}
		}
		property IRandomAccessStreamWithContentType^ PictureStream
		{
			IRandomAccessStreamWithContentType^ get()
			{
				return pictureStream;
			}
			void set(IRandomAccessStreamWithContentType^ i)
			{
				pictureStream = i;
			}
		}
		property uintptr_t PictureLocation
		{
			uintptr_t get()
			{
				return pictureLocation;
			}
			void set(uintptr_t i)
			{
				pictureLocation = i;
			}
		}
	
	private:
		int nextUserNumber = 1;
		Platform::String^ cylonName;
		Platform::String^ nonRoamableId;
		Platform::Boolean infoReady;
		Platform::Boolean infoCopied;
		IRandomAccessStreamWithContentType^ pictureStream;
		uintptr_t pictureLocation;
	};
}

//sync a cylonStruct with a Tory 
void syncTory(struct cylonStruct& tf, Centurion::Tory^ tory);