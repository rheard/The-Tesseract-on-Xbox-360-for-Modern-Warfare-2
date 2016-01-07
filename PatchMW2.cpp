#include <xtl.h>
#include <xboxmath.h>
#include <xbox.h>
#include <stdio.h>
#include "Hooking.h"
#include "utility.h"

//#ifdef _DEBUG
//#include <xbdm.h>
//#endif

__declspec(naked) void renderSaveStub(const void * args, int unknown)
{
	__asm
	{
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
	}
}

__declspec(naked) void physPresetLoaderSaveStub(void ** input)
{
	__asm
	{
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		li r3, 0	//Make sure to change something to prevent optimizations
	}
}

__declspec(naked) void tracerLoaderSaveStub(void ** input)
{
	__asm
	{
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		li r3, 1	//Make sure to change something to prevent optimizations
	}
}

__declspec(naked) void modelSurfLoaderSaveStub(void ** input)
{
	__asm
	{
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		li r3, 2	//Make sure to change something to prevent optimizations
	}
}

__declspec(naked) void weaponVarientDefLoaderSaveStub(void ** input)
{
	__asm
	{
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		li r3, 3	//Make sure to change something to prevent optimizations
	}
}

__declspec(naked) void lightDefLoaderSaveStub(void ** input)
{
	__asm
	{
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		li r3, 4	//Make sure to change something to prevent optimizations
	}
}

__declspec(naked) void vehicleLoaderSaveStub(void ** input)
{
	__asm
	{
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		li r3, 5	//Make sure to change something to prevent optimizations
	}
}

__declspec(naked) void playlistLoaderSaveStub(char * input)
{
	__asm
	{
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		li r3, 6	//Make sure to change something to prevent optimizations
	}
}

struct MapnameRichPresenceInfo
{
	char name[0x20];
	int context;
	int mapPack;
};

char * VERSION_NAME = "alterIW4 vTU8.2.1 1.4";
float newVersionXOffset = -80.f;

void patchTU8BootXEXOnDev()
{
	//No Fall Damage
	WriteMemory((LPVOID)0x820D3958, 2, (LPCVOID)&alwaysBranchPatch);

	//Force Ping Text on Leaderboard
	WriteMemory((LPVOID)0x82116CBC, 4, (LPCVOID)&NOP);
	
	//No BIK Video
	//DmSetMemory((LPVOID)0x8236379C, 2, (LPCVOID)&alwaysBranchPatch, (LPDWORD)&outInt);

	//External DLC Maps
	WriteMemory((LPVOID)0x8229C37C, 4, (LPCVOID)&NOP);
	
	//Allow Negative XP and Prestiging in game
	WriteMemory((LPVOID)0x8211D66C, 2, (LPCVOID)&alwaysBranchPatch);

	//Any Custom Class Name
	WriteMemory((LPVOID)0x8214D464, 2, (LPCVOID)&alwaysBranchPatch);

	//Constant Double Tap
	//WriteMemory((LPVOID)0x820DD878, 4, (LPCVOID)&NOP);

	//Version Name
	short versionNamePatch1;
	int verionNameOffset = (int)VERSION_NAME;

	if(verionNameOffset & 0x8000) // If bit 16 is 1
		versionNamePatch1 = (((verionNameOffset >> 16) & 0xFFFF) + 1); // lis %r11, dest>>16 + 1
	else
		versionNamePatch1 = ((verionNameOffset >> 16) & 0xFFFF); // lis %r11, dest>>16
	short versionNamePatch2 = (verionNameOffset & 0xFFFF);
	WriteMemory((LPVOID)0x8226DB6A, 2, (LPCVOID)&versionNamePatch1);
	WriteMemory((LPVOID)0x8226DB76, 2, (LPCVOID)&versionNamePatch2);

	//Move version text left
	verionNameOffset = (int)&newVersionXOffset;

	if(verionNameOffset & 0x8000) // If bit 16 is 1
		versionNamePatch1 = (((verionNameOffset >> 16) & 0xFFFF) + 1); // lis %r11, dest>>16 + 1
	else
		versionNamePatch1 = ((verionNameOffset >> 16) & 0xFFFF); // lis %r11, dest>>16
	versionNamePatch2 = (verionNameOffset & 0xFFFF);
	WriteMemory((LPVOID)0x8226C906, 2, (LPCVOID)&versionNamePatch1);
	WriteMemory((LPVOID)0x8226C91E, 2, (LPCVOID)&versionNamePatch2);

	//Remove RSA Checks
	WriteMemory((LPVOID)0x82168FB4, 4, (LPCVOID)&returnTrue);
	WriteMemory((LPVOID)0x82168FCC, 4, (LPCVOID)&returnTrue);

	//Allow any clan tag
	WriteMemory((LPVOID)0x82273B50, 2, (LPCVOID)&alwaysBranchPatch);
	WriteMemory((LPVOID)0x82273C08, 2, (LPCVOID)&alwaysBranchPatch);
	WriteMemory((LPVOID)0x82273D1C, 4, (LPCVOID)&NOP);
	WriteMemory((LPVOID)0x82273D10, 4, (LPCVOID)&NOP);

	//Remove all MPDATA checks
	WriteMemory((LPVOID)0x822CFB34, 2, (LPCVOID)&alwaysBranchPatch);
	WriteMemory((LPVOID)0x8222B708, 2, (LPCVOID)&alwaysBranchPatch);

	//Setup render loop
	hookFunctionStart((PDWORD)0x82285D08, (PDWORD)renderSaveStub, (DWORD)MW2TitleUpdate8InternalLoop);
	hookFunctionStart((PDWORD)0x82199B58, (PDWORD)physPresetLoaderSaveStub, (DWORD)Load_PhysPresetAssetHook);
	hookFunctionStart((PDWORD)0x8219AF30, (PDWORD)tracerLoaderSaveStub, (DWORD)Load_TracerAssetHook);
	hookFunctionStart((PDWORD)0x8219AAA8, (PDWORD)weaponVarientDefLoaderSaveStub, (DWORD)Load_weaponVarientDef_tAssetHook);
	hookFunctionStart((PDWORD)0x8219AFC0, (PDWORD)vehicleLoaderSaveStub, (DWORD)Load_VehicleAssetHook);
	hookFunctionStart((PDWORD)0x8219A798, (PDWORD)lightDefLoaderSaveStub, (DWORD)Load_lightDefAssetHook);
	//hookFunctionStart((PDWORD)0x822775C8, (PDWORD)playlistLoaderSaveStub, (DWORD)Playlist_ParsePlaylistsHook);
	//hookFunctionStart((PDWORD)0x82199D08, (PDWORD)modelSurfLoaderSaveStub, (DWORD)Load_ModelSurfAssetHook);
	//patchInJump((PDWORD)0x8222D7A8, (DWORD)Com_InitXAssets, false);
	//patchInJump((PDWORD)0x822928E0, (DWORD)&StringTable_GetAsset, false);
	patchInJump((PDWORD)0x8219C130, (DWORD)&Rawfile_GetAsset, false);
	//patchInJump((PDWORD)0x82198E90, (DWORD)&Image_GetIndex, false);
	patchInJump((PDWORD)0x822612A0, (DWORD)&Localize_GetString, false);

	patchInJump((PDWORD)0x8221AB40, (DWORD)&CM_EntityString, false);
}
