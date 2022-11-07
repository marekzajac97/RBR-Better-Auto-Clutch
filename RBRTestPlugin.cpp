// RBRTestPlugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "TestPlugin.h"
#include "AutoClutch.h"

class IRBRGame;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		initPlugin();
	}
    return TRUE;
}


IPlugin* RBR_CreatePlugin	( IRBRGame* pGame )
{
	return new CTestPlugin( pGame );
}