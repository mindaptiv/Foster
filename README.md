# Foster

##Abstract
Grabs system and user information in a UWP (Universal Windows Platform) context.

##Goal
Foster is a UWP/Windows 10/Windows Store targeted library for gathering both user and system information for the current Win RT device. In order to be compatible with all UWP devices (including Surface, IoT, desktop, Xbox, Phone), Foster only makes use of traditional Win32 calls that are supported by UWP. The final version of this library will gather as much information as it can to create a profile for the logged in user with their account name and avatar, as well as in-depth stats on the hardware and system settings for their device. This profile information will be stored in a struct that can be exported and utilized by other applications.

##Capabilities (AKA App Permissions) Used
Your App's package.appxmanifest file will need the following to function properly with the methods defined in Foster.cpp
 * Internet (Client)
 * Removable Storage
 * User Account Information

##API Contracts Used
Foster makes use of the following UWP API contracts:
 * Windows.Foundation.FoundationContract
 * Windows.Foundation.UniversalApiContract
 * Windows.UserProfile.UserProfileContract
 * Windows.Devices.Portable.PortableDeviceContract

##Documentation
For more detailed documentation visit www.github.com/mindaptiv/Foster/wiki

##Contact
josh@mindaptiv.com
