#include "stdafx.h"
#include "AutoClutch.h"
#include "Windows.h"
#include "ini.h"

#include "stdio.h"
#include "stdlib.h"

bool hook(void* toHook, void* ourFunct, size_t len, bool as_call) {
    if (len < 5) {
        return false;
    }

    DWORD curProtection;
    if (!VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection)) {
        return false;
    }

    memset(toHook, 0x90, len);
    DWORD relativeAddress = ((DWORD)ourFunct - (DWORD)toHook) - 5;
    *(BYTE*)toHook = as_call ? 0xE8 : 0xE9; // E8 - call, E9 - jmp
    *(DWORD*)((DWORD)toHook + 1) = relativeAddress;

    DWORD temp;
    if (!VirtualProtect(toHook, len, curProtection, &temp)) {
        return false;
    }

    return true;
}

struct ControlerInput {
	float throttle;     // 0x00
	float brake;	    // 0x04
	float handbrake;	// 0x08	
	float clutch;	    // 0x0C
	float steering;	    // 0x10
};

struct CarControls {
	__int32 _0x00;      // 0x00
	__int32 _0x04;      // 0x04
	__int32 _0x08;      // 0x08
	float throttle;	    // 0x0C
	float brake;		// 0x10	
	float handbrake;	// 0x14
	float steering;	    // 0x18
	float clutch;		// 0x1C
};

struct CarInfo {
	__int32   hudPositionX;  // 0x00
	__int32   hudPositionY;  // 0x04
	__int32   raceStarted;   // 0x08
	float speed;			 // 0x0C
	float rpm;				 // 0x10
	float temp;				 // 0x14
	float turbo;			 // 0x18
};


unsigned int(__thiscall* unknownMehtod)(CarControls*, ControlerInput*) = (unsigned int(__thiscall*)(CarControls*, ControlerInput*))0x537A80;

bool isEnabled = true;
int minRpm = 3000;
int maxRpm = 4000;

void __fastcall updateCarControls(CarControls* _this, int, ControlerInput* input) {
	_this->throttle = input->throttle;
	_this->brake = input->brake;
	_this->handbrake = input->handbrake;
	_this->steering = input->steering;
	if (isEnabled) {
		CarInfo* carInfo = *(CarInfo**)0x0165FC68;
		float rpm = carInfo->rpm;
		if (rpm < minRpm) {
			_this->clutch = 1.0f;
		}
		else if (rpm > maxRpm) {
			_this->clutch = 0.0f;
		}
		else {
			_this->clutch = (rpm - minRpm) / (maxRpm - minRpm);
		}
	}
	else {
		_this->clutch = input->clutch;
	}
	
	unknownMehtod(_this, input);
}

void loadSettings()
{
	char* pStr, strPath[255];
	GetModuleFileName(NULL, strPath, 255);
	pStr = strrchr(strPath, '\\');
	if (pStr != NULL)
		*(++pStr) = '\0';

	sprintf(strMySystemFile, "%s%s", strPath, "\\plugins\\autoclutch.ini");
	if (!ReadFromFile("MAIN", "Enabled") == 0) isEnabled = atoi(ReadFromFile("MAIN", "Enabled"));
	if (!ReadFromFile("MAIN", "MinRpm") == 0) minRpm = atoi(ReadFromFile("MAIN", "MinRpm"));
	if (!ReadFromFile("MAIN", "MaxRpm") == 0) maxRpm = atoi(ReadFromFile("MAIN", "MaxRpm"));
}

void saveSettings()
{
	char* pStr, strPath[255];
	GetModuleFileName(NULL, strPath, 255);
	pStr = strrchr(strPath, '\\');
	if (pStr != NULL)
		*(++pStr) = '\0';

	char tmpStr[255];

	sprintf(strMySystemFile, "%s%s", strPath, "\\plugins\\autoclutch.ini");
	itoa(isEnabled, tmpStr, 10);
	WriteToFile("MAIN", "Enabled", tmpStr);
	itoa(minRpm, tmpStr, 10);
	WriteToFile("MAIN", "MinRpm", tmpStr);
	itoa(maxRpm, tmpStr, 10);
	WriteToFile("MAIN", "MaxRpm", tmpStr);
}

void initPlugin() {
	loadSettings();
    hook((void*)0x004E924F, updateCarControls, 5, true);
}
