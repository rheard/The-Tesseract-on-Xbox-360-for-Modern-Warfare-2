#include <xtl.h>
#include <xboxmath.h>
#include <xbox.h>
#include <xam.h>
#include <stdio.h>
#include "GEntity.h"
#include "kernel.h"
#include "Hooking.h"
#include "Dvar.h"
#include "utility.h"
#include "Assets.h"
#include "Modern Warfare 2 TU8.h"

dvar_s * alterIW4_enabled,
	* alterIW4_developer,
	* player_forceFullAuto,
	* player_inventorySize,
	* player_forceDamageMOD,
	* player_forceBulletsPerShot,
	* g_speed,
	* jump_height,
	* jump_stepSize,
	* laserForceOn,
	* player_multiJumpCount,
	* player_infiniteBulletPenetration;

dvar_s ** player_sustainAmmo = (dvar_s **)0x82557530,
	** g_knockback = (dvar_s **)0x82F035D8,
	** cg_fov = (dvar_s **)0x825BE8C8;

dvar_s ** perk_weapRateMultiplier = (dvar_s **)0x825575A4,
	** testClients_watchKillcam = (dvar_s **)0x83635300,
	** testClients_doCrouch = (dvar_s **)0x8362035C,
	** testClients_doReload = (dvar_s **)0x83635344,
	** testClients_doMove = (dvar_s **)0x83635340,
	** testClients_doAttack = (dvar_s **)0x8363533C;

bool isContemplatingPrimary = false, isContemplatingSecondary = false;
int contemplatingTotalPrimary = -1, contemplatingCurrentPrimary = -1,
	contemplatingTotalSecondary = -1, contemplatingCurrentSecondary = -1;
char contemplatorTextPrimary[0x40], contemplatorTextSecondary[0x40];

char MasterCharBuffer[0x8000],
 * laserForceOnDesc = "Force laser sights on in all possible places (for debug purposes).",
 * gSpeedDesc = "Player speed",
 * forceFullAutoDesc = "Force fireType to 0 on all weapons",
 * jumpHeightDesc = "The maximum height of a player's jump",
 * bulletsPerShotDesc = "The number of bullets to force for every cartridge fired (0 loads default)",
 * multiJumpDvarDesc = "The number of times a player can jump mid-air before they need to touch the ground",
 * inventorySizeDesc = "Number of weapons a player can hold",
 * infBulletPenDesc = "All bullets will pass through all surfaces without slowing down";

XINPUT_KEYSTROKE myKeystroke;
XOVERLAPPED theOverlap;
WCHAR keyboardTitle[0x20],keyboardDesc[0x40];

int * frameCount = (int *)0x831BA238,
 currentEditedPlayer = -1;

float * colorBlack = (float *)0x82030D00,
	* colorRed = (float *)0x82030D10,
	* colorGreen = (float *)0x82030D20,
	* colorLtGreen = (float *)0x82030D30,
	* colorBlue = (float *)0x82030D40,
	* colorLtBlue = (float *)0x82030D50,
	* colorYellow = (float *)0x82030D60,
	* colorLtYellow = (float *)0x82030D70,
	* colorMdYellow = (float *)0x82030D80,
	* colorMagenta = (float *)0x82030D90,
	* colorCyan = (float *)0x82030DA0,
	* colorLtCyan = (float *)0x82030DB0,
	* colorMdCyan = (float *)0x82030DC0,
	* colorDkCyan = (float *)0x82030DD0,
	* colorWhite = (float *)0x82030DE0,
	* colorLtGrey = (float *)0x82030DF0,
	* colorMdGrey = (float *)0x82030E00,
	* colorDkGrey = (float *)0x82030E10,
	* colorOrange = (float *)0x82030E20,
	* colorLtOrange = (float *)0x82030E30,
	* colorWhiteFaded = (float *)0x82030E40,
	* colorGreenFaded = (float *)0x82030E50,
	* colorRedFaded = (float *)0x82030E60;
//This is what the ^#Text system uses
//Use like g_color_table[# * 4]
//Max for # is 7
float * g_color_table = (float *)0x82012B80;

bool applyAutoAim[18],isMenuOpen = false;
int buttonFrameCount[0xE0],selectedMenuIndex = -1, selectedItemIndex = 0,entityButtons[18],multiJumpCount[18];
float axisValues[7];

rectDef_s nullLocation = {0.f, 0.f, 0.f, 0.f, 0, 0};

#pragma region Functions in the XEX
DB_LoadXAssets_t MW2_DB_LoadXAssets = (DB_LoadXAssets_t)0x8219B2A8;

typedef bool (__cdecl * Key_IsDown_t)(int localClientNum, int keynum);
Key_IsDown_t Key_IsDown = (Key_IsDown_t)0x82141308;

typedef void (__cdecl * Item_Paint_t)(void *dc, int unknown, itemDef_s *item);
Item_Paint_t Item_Paint = (Item_Paint_t)0x822838D0;

typedef char (__cdecl * G_SoundAliasIndex_t)(const char *name);
G_SoundAliasIndex_t G_SoundAliasIndex = (G_SoundAliasIndex_t)0x8220D048;

typedef void (__cdecl * DB_InitThread_t)();
DB_InitThread_t DB_InitThread = (DB_InitThread_t)0x8219B910;

typedef bool (__cdecl * CL_LocalClientIsInGame_t)(int localClientNum);
CL_LocalClientIsInGame_t CL_LocalClientIsInGame = (CL_LocalClientIsInGame_t)0x8214D218;

typedef int (__cdecl * BG_GetViewmodelWeaponIndex_t)(playerState_s * ps);
BG_GetViewmodelWeaponIndex_t BG_GetViewmodelWeaponIndex = (BG_GetViewmodelWeaponIndex_t)0x820E23C8;

typedef void (__cdecl * G_GetPlayerViewOrigin_t)(playerState_s *ps, float *origin);
G_GetPlayerViewOrigin_t G_GetPlayerViewOrigin = (G_GetPlayerViewOrigin_t)0x821D38C8;

typedef void (__cdecl * vectoangles_t)(const float *vec, float *angles);
vectoangles_t vectoangles = (vectoangles_t)0x822899A0;

typedef void (__cdecl * SetClientViewAngle_t)(gentity_s *ent, const float *angle);
SetClientViewAngle_t SetClientViewAngle = (SetClientViewAngle_t)0x821D35E8;

SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x82254940;
SV_ExecuteClientCommand_t SV_ExecuteClientCommand = (SV_ExecuteClientCommand_t)0x822531C8;

typedef void (__cdecl * SV_KickClient_t)(void *cl, int, int, int, char * reasonText);
SV_KickClient_t SV_KickClient = (SV_KickClient_t)0x82251698;

typedef const char * (__cdecl * SL_ConvertToString_t)(unsigned int stringValue);
SL_ConvertToString_t SL_ConvertToString = (SL_ConvertToString_t)0x82241920;

SV_AddTestClient_t SV_AddTestClient = (SV_AddTestClient_t)0x822546F8;
G_GivePlayerWeapon_t G_GivePlayerWeapon = (G_GivePlayerWeapon_t)0x82210BC8;
DB_FindXAssetHeader_t DB_FindXAssetHeader = (DB_FindXAssetHeader_t)0x8219BBD8;

typedef int (__cdecl * irand_t)(int min, int max);
irand_t irand = (irand_t)0x8220E470;

typedef float (__cdecl * CL_GamepadAxisValue_t)(int localClientNum, int virtualAxis);
CL_GamepadAxisValue_t CL_GamepadAxisValue = (CL_GamepadAxisValue_t)0x8213D888;
#pragma endregion

//Attempt to define fog, for some reason. I don't need it anymore for the Tesseract, I just included it for any interested parties. 
/*typedef void (__cdecl * Scr_SetFog_t)(float start, float density, float r, float g, float b, float a, float time, bool sunEnabled, float sunR, float sunG, float sunB, float * sunDirection, float sunBeginFadeAngle, float sunEndFadeAngle, float sunFogScale);
Scr_SetFog_t Scr_SetFog = (Scr_SetFog_t)0x821FC090;

void setExpFog(float startDist, float halfwayDist, float red, float green, float blue, float maxOpacity, float transition_time, float sunRed, float sunGreen, float sunBlue, float * sunDir, float sunBeginFadeAngle, float sunEndFadeAngle, float sunFogScale, bool sunEnabled = true)
{
	if(startDist < 0) 
	{
		printf("setExpFog: startDist must be greater or equal to 0");
		startDist = 0;
	}

	if(halfwayDist <= 0)
	{
		printf("setExpFog: halfwayDist must be greater than 0");
		halfwayDist = 1;
	} 

	if(maxOpacity < 0 || maxOpacity > 1)
	{
		printf("setExpFog: maxOpacity must be between 0 and 1");
		maxOpacity = 0;
	}

	Dvar_SetColor(*(dvar_s**)0x8304361C, red, green, blue, 1);
	Dvar_SetFloat(*(dvar_s**)0x830CBF04, maxOpacity);
	Dvar_SetFloat(*(dvar_s**)0x830CBEFC, startDist);
	Dvar_SetFloat(*(dvar_s**)0x830CBE9C, halfwayDist);
	
	Dvar_SetBool(*(dvar_s**)0x830CBF08, sunEnabled);
	if(sunEnabled)
	{
		Dvar_SetColor(*(dvar_s**)0x830CBEE8, sunRed, sunGreen, sunBlue, 1);
		Dvar_SetVec3(*(dvar_s**)0x82F035A4, sunDir[0], sunDir[1], sunDir[2]);
		Dvar_SetFloat(*(dvar_s**)0x82F035A8, sunBeginFadeAngle);
		Dvar_SetFloat(*(dvar_s**)0x830CBF14, sunEndFadeAngle);
		Dvar_SetFloat(*(dvar_s**)0x830CBF24, sunFogScale);
	}

	Scr_SetFog(startDist, 0.69314718 / halfwayDist, red, green, blue, maxOpacity, transition_time, sunEnabled, sunRed, sunGreen, sunBlue, sunDir, sunBeginFadeAngle, sunEndFadeAngle, sunFogScale);
}

void setExpFog(float startDist, float halfwayDist, float red, float green, float blue, float maxOpacity, float transition_time)
{
	float tempDir[3] = {0, 0, 0};
	setExpFog(startDist, halfwayDist, red, green, blue, maxOpacity, transition_time, red, green, blue, tempDir, 0, 0, 1, false);
}*/

gentity_s * hostPlayer;

//The following 4 functions have been reversed from cod4 and modified, I need them for autoaim
//Equivelent to
//return (p2[1] - p1[1])^2 + (p2[2] - p1[2])^2 + (p2[0] - p2[0])^2;
__declspec(naked) float Vec3DistanceSq(const float *p1, const float *p2)
{
	//a^2 + b^2 = c^2 //2 args, 2 dimensions, pythagorus theorum
	//a^2 + b^2 + c^2 = d^2 //3 args, 3 dimensions, thats what this is (But doesn't sqrt it)
	//a^2 + b^2 + c^2 + d^2 = e^2 //4 args, 4 dimensions. How to calculate the diagonal of a hypercube (thats a whole nother lesson entirely)
	__asm
	{
		lfs       fp13, 4(r3)
		lfs       fp0, 4(r4)
		fsubs     fp0, fp0, fp13
		lfs       fp12, 8(r4)
		lfs       fp13, 8(r3)
		fsubs     fp13, fp12, fp13
		lfs       fp11, 0(r3)
		lfs       fp12, 0(r4)
		fsubs     fp12, fp12, fp11
		fmuls     fp0, fp0, fp0
		fmadds    fp0, fp13, fp13, fp0
		fmadds    fp1, fp12, fp12, fp0
		blr
	}
}

bool Scr_Closer(const float * startingPoint, const float * trueIfCloser, const float * falseIfCloser)
{
	return (Vec3DistanceSq(startingPoint, trueIfCloser) < Vec3DistanceSq(startingPoint, falseIfCloser));
}

void applyAutoaimToGEntity(gentity_s * thisPlayer)
{
	if(currentEditedPlayer != -1)
	{
		thisPlayer = gentity_s::GetGEntity(currentEditedPlayer);
		gclient_s * thisClient = thisPlayer->GetGClient();

		weaponVarientDef_s * currentWeapon = BG_GetWeaponVarientDef(BG_GetViewmodelWeaponIndex(&thisPlayer->clientData->ps));
		if(thisClient->IsDualWielding() || currentWeapon->WeaponDef->weaponClass == WEAPCLASS_SNIPER || thisClient->PlayerADS() < 0.25f)
			return;

		gentity_s * targetEnt = 0, * possibleEntTarget = 0;
		gclient_s * possibleClientTarget = 0;
		for(int i = 0; i < 18; i++)
		{
			possibleEntTarget = gentity_s::GetGEntity(i);
			possibleClientTarget = possibleEntTarget->GetGClient();

			if(possibleEntTarget == thisPlayer || !possibleEntTarget->IsAlive() || thisClient->sess._cs.team == possibleClientTarget->sess._cs.team && strcmp(Dvar_GetString("g_gametype"), "dm"))
				continue;

			//No matter what i do... this freezes :(
			//if(!G_LocationalTracePassed(thisPlayer->origin, possibleEntTarget->origin, 0x7FF, 0x7FF, 0x806831, 0))
			//	continue;
		
			if(!targetEnt || Scr_Closer(thisPlayer->GetCurrentOrigin(), possibleEntTarget->GetCurrentOrigin(), targetEnt->GetCurrentOrigin()))
				targetEnt = possibleEntTarget;
		}

		if(targetEnt)
		{
			float returnAngles[3],targetOrigin[3],thisOrigin[3];
			G_GetPlayerViewOrigin(&targetEnt->clientData->ps, targetOrigin);
			G_GetPlayerViewOrigin(&thisPlayer->clientData->ps, thisOrigin);
			float inputOrigin[3] = { targetOrigin[0] - thisOrigin[0],
				targetOrigin[1] - thisOrigin[1],
				targetOrigin[2] - thisOrigin[2] };
			vectoangles(inputOrigin, returnAngles);
			SetClientViewAngle(thisPlayer, returnAngles);
		}
	}
}

void toggleGodMode()
{
	if(currentEditedPlayer != -1) {
		sprintf(MasterCharBuffer, "f \"God Mode: %s\"", gentity_s::GetGEntity(currentEditedPlayer)->ToggleGodMode()? "^2On" : "^1Off");
		SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
	}
}

void toggleDemiGodMode()
{
	if(currentEditedPlayer != -1) {
		sprintf(MasterCharBuffer, "f \"Demi-God Mode: %s\"", gentity_s::GetGEntity(currentEditedPlayer)->ToggleDemiGodMode() ? "^2On" : "^1Off");
		SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
	}
}

void toggleNoKnockback()
{
	if(currentEditedPlayer != -1) {
		sprintf(MasterCharBuffer, "f \"Knockback: %s\"", !gentity_s::GetGEntity(currentEditedPlayer)->ToggleNoKnockback() ? "^2On" : "^1Off");
		SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
	}
}

void toggleThermalVision()
{
	if(currentEditedPlayer != -1) {
		sprintf(MasterCharBuffer, "f \"Thermal Vision: %s\"", gclient_s::GetGClient(currentEditedPlayer)->ToggleThermalVision() ? "^2On" : "^1Off");
		SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
	}
}

void toggleFOFOverlay()
{
	if(currentEditedPlayer != -1) {
		sprintf(MasterCharBuffer, "f \"FOFOverlay: %s\"", gclient_s::GetGClient(currentEditedPlayer)->ToggleFOFOverlay() ? "^2On" : "^1Off");
		SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
	}
}

void toggleEMPJamming()
{
	if(currentEditedPlayer != -1) {
		sprintf(MasterCharBuffer, "f \"EMP: %s\"", gclient_s::GetGClient(currentEditedPlayer)->ToggleEMPJam() ? "^2On" : "^1Off");
		SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
	}
}

bool kick(int clientIndex, char * kickReason)
{
	gentity_s * thisPlayer = gentity_s::GetGEntity(clientIndex);
	bool testVal = !(thisPlayer->clientData->sess.localClient & 1) || *(int *)(*(int *)0x83623B98 + (clientIndex * 0x97F80) + 0x31AFC) == 1;
	if(testVal)
		SV_KickClient((void *)(*(int *)0x83623B98 + (clientIndex * 0x97F80)), 0, 0, 0, kickReason);

	return testVal;
}

bool kick(int clientIndex)
{
	return kick(clientIndex, "EXE_PLAYERKICKED");
}
	
void UnlockAllAchivements()
{
	XUSER_ACHIEVEMENT mw2Cheevos[50];
	for(int i = 0; i < 50; i++)
	{
		mw2Cheevos[i].dwUserIndex = 0;
		mw2Cheevos[i].dwAchievementId = gameAchievementList[i].achievementId;
	}

	XUserWriteAchievements(50, mw2Cheevos, &theOverlap);
	while(!XHasOverlappedIoCompleted(&theOverlap)) { Sleep(10); }
}

void DoUnlockAllCheevos()
{
	UnlockAllAchivements();
}

void EnableNoclip()
{
	if(currentEditedPlayer != -1) {
		sprintf(MasterCharBuffer, "f \"Noclip Mode: %s\"", gclient_s::GetGClient(currentEditedPlayer)->ToggleNoclipMode() ? "^2On" : "^1Off");
		SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
	}
}

void EnableUFO()
{
	if(currentEditedPlayer != -1) {
		sprintf(MasterCharBuffer, "f \"UFO Mode: %s\"", gclient_s::GetGClient(currentEditedPlayer)->ToggleUFOMode() ? "^2On" : "^1Off");
		SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
	}
}

void DisableControls()
{
	if(currentEditedPlayer != -1) {
		sprintf(MasterCharBuffer, "f \"All Controls: %s\"", !gclient_s::GetGClient(currentEditedPlayer)->ToggleDisableControls() ? "^2On" : "^1Off");
		SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
	}
}

void togglePlayerADS()
{
	if(currentEditedPlayer != -1) {
		sprintf(MasterCharBuffer, "f \"Aiming: %s\"", !gclient_s::GetGClient(currentEditedPlayer)->ToggleDisableADS() ? "^2On" : "^1Off");
		SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
	}
}

void toggleWeaponUseage()
{
	if(currentEditedPlayer != -1) {
		sprintf(MasterCharBuffer, "f \"Weapons: %s\"", !gclient_s::GetGClient(currentEditedPlayer)->ToggleDisableWeaponUseage() ? "^2On" : "^1Off");
		SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
	}
}

void toggleWeaponSwitches()
{
	if(currentEditedPlayer != -1) {
		sprintf(MasterCharBuffer, "f \"Weapon Switching: %s\"", !gclient_s::GetGClient(currentEditedPlayer)->ToggleDisableWeaponSwitching() ? "^2On" : "^1Off");
		SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
	}
}

void toggleRecoilScale()
{
	if(currentEditedPlayer != -1) {
		sprintf(MasterCharBuffer, "f \"Recoil Scale: %s\"", gclient_s::GetGClient(currentEditedPlayer)->ToggleRecoilScale() ? "^2On" : "^1Off");
		SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
	}
}

void toggleOffhandWeapons()
{
	if(currentEditedPlayer != -1) {
		sprintf(MasterCharBuffer, "f \"Grenades: %s\"", !gclient_s::GetGClient(currentEditedPlayer)->ToggleDisableOffhandWeaponUseage() ? "^2On" : "^1Off");
		SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
	}
}

void setPlayersRecoilScale()
{
	if(currentEditedPlayer != -1) {
		int keyboardResult = XShowKeyboardUI(0, VKBD_LATIN_NUMERIC, 0, L"alterIW4::SetRecoilScale", L"Integer on a scale from 0 to 100", (LPWSTR)MasterCharBuffer, 0x100, &theOverlap);
		while(!XHasOverlappedIoCompleted(&theOverlap)) { Sleep(10); }

		if(keyboardResult == ERROR_IO_PENDING && theOverlap.dwExtendedError == ERROR_SUCCESS)
		{
			wcstombs(MasterCharBuffer, (wchar_t *)MasterCharBuffer, 0x100);
			int valueToSet = atoi(MasterCharBuffer);

			gclient_s::GetGClient(currentEditedPlayer)->SetRecoilScale(valueToSet);
			sprintf(MasterCharBuffer, "f \"Recoil Scale: %i%%\"", valueToSet);
			SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
		}
	}
}

void setSpreadOverride()
{
	if(currentEditedPlayer != -1) {
		int keyboardResult = XShowKeyboardUI(0, VKBD_LATIN_NUMERIC, 0, L"alterIW4::SetSpreadOverride", L"Integer on a scale from 0 to 64", (LPWSTR)MasterCharBuffer, 0x100, &theOverlap);
		while(!XHasOverlappedIoCompleted(&theOverlap)) { Sleep(10); }

		if(keyboardResult == ERROR_IO_PENDING && theOverlap.dwExtendedError == ERROR_SUCCESS)
		{
			wcstombs(MasterCharBuffer, (wchar_t *)MasterCharBuffer, 0x100);
			int valueToSet = atoi(MasterCharBuffer);

			gclient_s::GetGClient(currentEditedPlayer)->SetSpreadOverride(valueToSet);
			sprintf(MasterCharBuffer, "f \"Spread Overide: %f%%\"", (float)valueToSet / 64);
			SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
		}
	}
}

void sendCustomKeyboardCmd()
{
	if(currentEditedPlayer != -1) {
		int keyboardResult = XShowKeyboardUI(0, VKBD_HIGHLIGHT_TEXT, 0, L"alterIW4::CustomCmd", L"This text command will be sent to the player", (LPWSTR)MasterCharBuffer, 0x100, &theOverlap);
		while(!XHasOverlappedIoCompleted(&theOverlap)) { Sleep(10); }

		if(keyboardResult == ERROR_IO_PENDING && theOverlap.dwExtendedError == ERROR_SUCCESS)
		{
			wcstombs(MasterCharBuffer, (wchar_t *)MasterCharBuffer, 0x100);
			SV_GameSendServerCommand(currentEditedPlayer, 0, MasterCharBuffer);
		}
	}
}

void GivePlayerGunByName(char * gunName, char camo, bool dualWield)
{
	if(currentEditedPlayer != -1)
		gentity_s::GetGEntity(currentEditedPlayer)->GivePlayerGun(gunName, camo, dualWield);
}

void kickThisPlayer()
{
	if(currentEditedPlayer != -1) {
		gclient_s * prekickEntity = gclient_s::GetGClient(currentEditedPlayer);

		if (prekickEntity && kick(currentEditedPlayer)) {
			ZeroMemory(prekickEntity->sess.newnetname, 32);
			ZeroMemory(prekickEntity->sess._cs.name, 32);
		}
	}
}

void toggleCodedAutoaim(gentity_s * thisPlayer)
{
	if(currentEditedPlayer != -1) {
		thisPlayer = gentity_s::GetGEntity(currentEditedPlayer);

		applyAutoAim[thisPlayer->clientData->sess._cs.clientIndex] = !applyAutoAim[thisPlayer->clientData->sess._cs.clientIndex];
	
		sprintf(MasterCharBuffer, "f \"Auto Aim: %s\"", applyAutoAim[thisPlayer->clientData->sess._cs.clientIndex] ? "^2On" : "^1Off");
		SV_GameSendServerCommand(thisPlayer->clientData->sess._cs.clientIndex, 0, MasterCharBuffer);
	}
}

void SendSeverCommand_ChangeStat(int index)
{
	int keyboardResult = XShowKeyboardUI(0, VKBD_LATIN_PHONE, 0, keyboardTitle, keyboardDesc, (LPWSTR)MasterCharBuffer, 0x100, &theOverlap);
	while(!XHasOverlappedIoCompleted(&theOverlap)) { Sleep(10); }

	if(keyboardResult == ERROR_IO_PENDING && theOverlap.dwExtendedError == ERROR_SUCCESS)
	{
		wcstombs(MasterCharBuffer, (wchar_t *)MasterCharBuffer, 0x100);
		int valueToSet = atoi(MasterCharBuffer);
		int maxValue;
		switch(index)
		{
		case 2056: maxValue = 2516000; break;
		case 2064: maxValue = 11; break;
		default: maxValue = INT_MAX; break;
		}

		if(valueToSet <= maxValue)
		{
			if(index == 2064)
				sprintf(MasterCharBuffer, "J %i %04hX0", index, _byteswap_ushort((unsigned short)valueToSet));
			else
				sprintf(MasterCharBuffer, "J %i %08lX", index, _byteswap_ulong(valueToSet));
			SV_GameSendServerCommand(currentEditedPlayer, 1, MasterCharBuffer);
		}
	}
}

//NOTE: This is exactly the same as UnlockEverything in MW2 gentity_s.cpp, the only difference is that
//	this version is integrated in the menu to load the menu selected player, to use the standard buffer, 
//	and to update the progress bars. 
void unlockAll()
{	
	int clientIndex;

	//Do this so the compiler knows to save at least to r30
	__asm { li r30, 0 }

	if(currentEditedPlayer == -1) return;
	else clientIndex = currentEditedPlayer;

	StringTable * allchallengestable = DB_FindXAssetHeader(stringtable, "mp/allchallengestable.csv").stringTable;
	StructuredDataDef * playerdata = DB_FindXAssetHeader(structureddatadef, "mp/playerdata.def").structuredData;

	//Look up "challengeProgress" and "challengeState" now,
	//    because they will never change and its quicker to
	//    not look them up every single frame.
	int challengeProgress = 0, challengeState = 0;
	contemplatingTotalPrimary = allchallengestable->rowCount + 1;
	contemplatingCurrentPrimary = 0;
	strcpy(contemplatorTextPrimary, "Unlocking Everything...");
	isContemplatingPrimary = true;
	for(int i = 0; i < playerdata->structureCount; i++)
	{
		if(challengeProgress && challengeState)
			break;

			for(int k = 0; k < playerdata->internalStructure[i].structures[0].entryCount; k++)
			{
				if(STRUCTURED_DATA_ENUMARR == playerdata->internalStructure[i].structures[0].entries[k].item.type)
				{
					if(!strcmp("challengeProgress", playerdata->internalStructure[i].structures[0].entries[k].name))
						challengeProgress = playerdata->internalStructure[i].structures[0].entries[k].item.offset;
					else if(!strcmp("challengeState", playerdata->internalStructure[i].structures[0].entries[k].name))
						challengeState = playerdata->internalStructure[i].structures[0].entries[k].item.offset;
				}

				if(challengeProgress && challengeState)
					break;
			}
	}

	contemplatingCurrentPrimary++;
	//Cycle through every challenge
	for(int i = 0; i < allchallengestable->rowCount; i++, contemplatingCurrentPrimary++)
	{
		int finalTarget = 0, finalTier = 0;

		//Cycle through every possible challenge tier to find the final target
		for(int tierID = 1; tierID < 11; tierID++)
		{
			char * currentEntry = allchallengestable->values[(allchallengestable->columnCount * i) + (6 + ((tierID-1)*2))].value;
			if(!currentEntry || !*currentEntry)
				break;
			else
			{
				finalTarget = atoi(currentEntry);
				finalTier = tierID + 1;
			}
		}

		//Now we have our finalTarget and finalTier,
		//   We now need to look up this challenges index
		int challengeIndex = -1;
		for(int j = 0; j < playerdata->structureCount; j++)
		{
			if(challengeIndex != -1)
				break;

			for(int k = 0; k < playerdata->internalStructure[j].enums[3].entryCount; k++)
			{
				if(!strcmp(playerdata->internalStructure[j].enums[3].entries[k].name, allchallengestable->values[allchallengestable->columnCount * i].value))
					challengeIndex = playerdata->internalStructure[j].enums[3].entries[k].index;

				if(challengeIndex != -1)
					break;
			}
		}

		//We have everything we need to unlock this challenge... Lets do it...
		sprintf(MasterCharBuffer, "J %i %08lX", challengeIndex + challengeProgress, _byteswap_ulong(finalTarget));
		SV_GameSendServerCommand(clientIndex, 1, MasterCharBuffer);
		sprintf(MasterCharBuffer, "J %i %02lX", challengeIndex + challengeState, finalTier);
		SV_GameSendServerCommand(clientIndex, 1, MasterCharBuffer);
		Sleep(6);
	}
	//Nothing unlocks the 10th spinning emblem, unlock it manually. 
	SV_GameSendServerCommand(clientIndex, 1, "J 6525 7F");
	SV_GameSendServerCommand(clientIndex, 1, "f \"All challenges are ^2unlocked!\"");
	isContemplatingPrimary = false;
	contemplatingTotalPrimary = 
	contemplatingCurrentPrimary = -1;
}

#pragma optimize("", off)
//this is a piece of shit
//im detecting available memory as a way to hopefully detect when no more clients can be added... idk why the game runs out of memory so easily (probably all this code).
void AddTestClient()
{
	MEMORYSTATUS memStatus;
	GlobalMemoryStatus(&memStatus);

	if(memStatus.dwAvailPhys > 0x10000) 
		gentity_s::AddTestClient(0,MasterCharBuffer);
}
#pragma optimize("", on)

#pragma region Hardware
//I would classify all this as the "hardware" of my menu
//This contains everything that I draw directly. The menu
//will set things like color/position/text before drawing these
#pragma region Header
//The header spacer is what makes it look like a real menu
	//Background = DB_FindXAssetHeader(material, "gradient_fadein").material
itemDef_s headerSpacer = {{"alterIW4_header_spacer", {127.5f, 97.5f, 216.f, 1.f, 0, 0}, {127.5f, 97.5f, 216.f, 1.f, 0, 0}, 0, 3, 0, 0, 0, 0.f, 8 | 0x50000000, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhiteFaded[0], colorWhiteFaded[1], colorWhiteFaded[2], colorWhiteFaded[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 0, 0.f, 0.f, 0.f, 0, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry};

itemDef_s headerSubText = {{"alterIW4_header_subText", {185.f, 35.f, 200.f, 60.f, 1, 1}, {185.f, 35.f, 200.f, 60.f, 1, 1}, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0xA, 6, 0.f, 0.f, 0.4f, 6, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry};

itemDef_s headerTitle = {{"alterIW4_header_title", {185.f, 10.f, 200.f, 60.f, 1, 1}, {185.f, 10.f, 200.f, 60.f, 1, 1}, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0xA, 6, 0.f, 0.f, 0.5625f, 6, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry};

newsticker_t newsTickerSettings = {0, 75};
itemDef_s newsTicker = {{"alterIW4_tickerTape_text", {-70.f, 380.f, 880.f, 20.f, 1, 0}, {-70.f, 380.f, 880.f, 20.f, 1, 0}, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0.125f}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 20, 20, 0, 0, 1, 0.f, 0.f, 0.375f, 6, 0, 0, "Welcome  to  ^2alterIW4  ^7for  TU8.                With more research done than by any other coders, we hope to outperform them all aswell.                                Made by CraigChrist8239                Research by CraigChrist8239 and Aerosoul1994                Image data by ihatecompvir", 0, &g_menuDefPool->entries[20].entry, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0,0,0,0}, &newsTickerSettings};
#pragma endregion

#pragma region Selector Bar
//The (A) button in the selector bar
itemDef_s selectorAButton = {{"alterIW4_selector_AButton", {210.f, 70.f, 175.f, 22.f, 1, 1}, {210.f, 70.f, 175.f, 22.f, 1, 1}, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 1, 6, 20.f, 3.5f, 0.28f, 0, 0, 0, "@PLATFORM_UI_SELECTBUTTON", 0, &g_menuDefPool->entries[20].entry};

//The dark bar itself that highlights selections
	//Background = DB_FindXAssetHeader(material, "gradient_fadein").material
itemDef_s selectorBar = {{"alterIW4_selector_bar", {170.f, 70.f, 246.f, 20.f, 1, 1}, {170.f, 70.f, 246.f, 20.f, 1, 1}, 0, 3, 0, 0, 0, 0.f, 8 | 0x50000000, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorBlack[0],colorBlack[1],colorBlack[2],colorBlack[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 0, 0.f, 0.f, 0.f, 0, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry};
#pragma endregion

#pragma region Background
//This is a little trick I learned in the GSC days...
//By applying a subtle white shader on the whole background, we can see pitch black objects in pitch black places
itemDef_s alterIWBackground = {{"alterIW4_background", {-100.f, 0.f, 840.f, 480.f, 0, 0}, {-100.f, 0.f, 840.f, 480.f, 0, 0}, 0, 1, 0, 0, 0, 0.f, 8 | 0x50000000, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {0,0,0,0}, {1.f, 1.f, 1.f, 0.005}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 0, 0.f, 0.f, 0.55f, 0, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry};

//This is the fading black that the buttons appear on
	//Background = DB_FindXAssetHeader(material, "gradient_fadein_fadebottom").material
itemDef_s alterIWBackground2 = {{"alterIW4_background2", {-35.f, 0.f, 380.f, 480.f, 0, 0}, {-35.f, 0.f, 380.f, 480.f, 0, 0}, 0, 3, 0, 0, 0, 0.f, 8 | 0x50000000, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {0.f, 0.f, 0.f, 0.25f}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 0, 0.f, 0.f, 0.55f, 0, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry};
#pragma endregion

#pragma region Buttons
/***************************************
 * My menu can display a maximum of 10 *
 * buttons at a time. Code handles any *
 * scrolling of menu items through the *
 *************** buttons ***************/
windowDef_t button1Window = {"alterIW4_button1", {224.f, 70.f, 175.f, 20.f, 1, 1}, {224.f, 70.f, 175.f, 20.f, 1, 1}, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0},
	button2Window = {"alterIW4_button2", {224.f, 90.f, 175.f, 20.f, 1, 1}, {224.f, 90.f, 175.f, 20.f, 1, 1}, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0},
	button3Window = {"alterIW4_button3", {224.f, 110.f, 175.f, 20.f, 1, 1}, {224.f, 110.f, 175.f, 20.f, 1, 1}, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0},
	button4Window = {"alterIW4_button4", {224.f, 130.f, 175.f, 20.f, 1, 1}, {224.f, 130.f, 175.f, 20.f, 1, 1}, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0},
	button5Window = {"alterIW4_button5", {224.f, 150.f, 175.f, 20.f, 1, 1}, {224.f, 150.f, 175.f, 20.f, 1, 1}, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0},
	button6Window = {"alterIW4_button6", {224.f, 170.f, 175.f, 20.f, 1, 1}, {224.f, 170.f, 175.f, 20.f, 1, 1}, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0},
	button7Window = {"alterIW4_button7", {224.f, 190.f, 175.f, 20.f, 1, 1}, {224.f, 190.f, 175.f, 20.f, 1, 1}, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0},
	button8Window = {"alterIW4_button8", {224.f, 210.f, 175.f, 20.f, 1, 1}, {224.f, 210.f, 175.f, 20.f, 1, 1}, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0},
	button9Window = {"alterIW4_button9", {224.f, 230.f, 175.f, 20.f, 1, 1}, {224.f, 230.f, 175.f, 20.f, 1, 1}, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0},
	button10Window = {"alterIW4_button10", {224.f, 250.f, 175.f, 20.f, 1, 1}, {224.f, 250.f, 175.f, 20.f, 1, 1}, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}; 

//If button is currentlySelected then window->style is 4 and textStyle = 1
//else its 1 and 3
itemDef_s buttons[10] = { {button1Window, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 6, -17.5f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{button2Window, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 6, -17.5f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{button3Window, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 6, -17.5f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{button4Window, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 6, -17.5f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{button5Window, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 6, -17.5f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{button6Window, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 6, -17.5f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{button7Window, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 6, -17.5f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{button8Window, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 6, -17.5f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{button9Window, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 6, -17.5f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{button10Window, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 6, -17.5f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
};

//The button hint is located directly below the last button and says what the currently selected button does
itemDef_s buttonHint = {{"alterIW4_buttonHint", {134.f, 300.f, 250.f, 40.f, 1, 1}, {134.f, 300.f, 250.f, 40.f, 1, 1}, 0, 1, 0, 0, 0, 0.f, 0x00900000, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 6, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry};
#pragma endregion

#pragma region Menu Hints
/****************************************
 * My menu can display up to 15 "hints" *
 ***** regarding how the menu works *****
 ****************************************
 *********** One hint per line **********
 ** Positions and background sizes are **
 ********* calculated in code ***********
 ****************************************/
rectDef_s menuHintTextLocation = {450.f, 0, 200.f, 20.f, 0, 0};

//Y = (240.f - (menuArray[currentMenuIndex]->hintCount * 10.f)) + (i * 20.f)
//If menuHint is in use then set backColor alpha to 0.25, that way hints draw their own background
itemDef_s menuHintText[15] = { {{"alterIW4_menuHint_hint1", menuHintTextLocation, menuHintTextLocation, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.325f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{{"alterIW4_menuHint_hint2", menuHintTextLocation, menuHintTextLocation, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{{"alterIW4_menuHint_hint3", menuHintTextLocation, menuHintTextLocation, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{{"alterIW4_menuHint_hint4", menuHintTextLocation, menuHintTextLocation, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{{"alterIW4_menuHint_hint5", menuHintTextLocation, menuHintTextLocation, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{{"alterIW4_menuHint_hint6", menuHintTextLocation, menuHintTextLocation, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{{"alterIW4_menuHint_hint7", menuHintTextLocation, menuHintTextLocation, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{{"alterIW4_menuHint_hint8", menuHintTextLocation, menuHintTextLocation, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{{"alterIW4_menuHint_hint9", menuHintTextLocation, menuHintTextLocation, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{{"alterIW4_menuHint_hint10", menuHintTextLocation, menuHintTextLocation, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{{"alterIW4_menuHint_hint11", menuHintTextLocation, menuHintTextLocation, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{{"alterIW4_menuHint_hint12", menuHintTextLocation, menuHintTextLocation, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{{"alterIW4_menuHint_hint13", menuHintTextLocation, menuHintTextLocation, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{{"alterIW4_menuHint_hint14", menuHintTextLocation, menuHintTextLocation, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry},
	{{"alterIW4_menuHint_hint15", menuHintTextLocation, menuHintTextLocation, 0, 1, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry}
};
#pragma endregion

#pragma region Devloper System
char originText[0x38] = "Origin: 0.000000 0.000000 0.000000";
char anglesText[0x38] = "Angles: 0.000000 0.000000 0.000000";
itemDef_s originDisplay = {
	{"alterIW4_dev_origin", 
		{600.f, 100.f, 100.f, 20.f, 1, 1}, 
		{600.f, 100.f, 100.f, 20.f, 1, 1}, 0, 0, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0},
	{nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 6, 0.f, 0.f, 0.315f, 0, 0, 0, originText, 0, &g_menuDefPool->entries[20].entry};

itemDef_s angleDisplay = {
	{"alterIW4_dev_angles", 
		{600.f, 120.f, 100.f, 20.f, 1, 1}, 
		{600.f, 120.f, 100.f, 20.f, 1, 1}, 0, 0, 0, 0, 0, 0.f, 0, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0},
	{nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 6, 0.f, 0.f, 0.315f, 0, 0, 0, anglesText, 0, &g_menuDefPool->entries[20].entry};
#pragma endregion

#pragma region Contemplator
windowDef_t alterIWPrimaryContemplatorTextWindow = {"alterIW4_contemplator_primary_text", {-100.f, -40.f, 200.f, 20.f, 2, 2}, {-100.f, -40.f, 200.f, 20.f, 2, 2}, 0, 1, 0, 0, 0, 0.f, 8, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}; 
itemDef_s alterIWPrimaryContemplatorText = {alterIWPrimaryContemplatorTextWindow, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry};
itemDef_s alterIWPrimaryContemplatorBackground = {{"alterIW4_contemplator_primary_background", {-100.f, -20.f, 205.f, 22.5f, 2, 2}, {-100.f, -20.f, 205.f, 22.5f, 2, 2}, 0, 1, 0, 0, 0, 0.f, 8, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {0,0,0,0}, {0.f, 0.f, 0.f, 1.f}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 0, 0.f, 0.f, 0.55f, 0, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry};
itemDef_s alterIWPrimaryContemplatorForeground = {{"alterIW4_contemplator_primary_foreground", {-97.5f, -17.5f, 200.f, 17.5f, 2, 2}, {-97.5f, -17.5f, 200.f, 17.5f, 2, 2}, 0, 1, 0, 0, 0, 0.f, 8, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {0,0,0,0}, {1.f, 1.f, 1.f, 1.f}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 0, 0.f, 0.f, 0.55f, 0, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry};
windowDef_t alterIWSecondaryContemplatorTextWindow = {"alterIW4_contemplator_secondary_text", {-100.f, 0.f, 200.f, 20.f, 2, 2}, {-100.f, 0.f, 200.f, 20.f, 2, 2}, 0, 1, 0, 0, 0, 0.f, 8, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}; 
itemDef_s alterIWSecondaryContemplatorText = {alterIWSecondaryContemplatorTextWindow, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 9, 0.f, 0.f, 0.375f, 3, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry};
itemDef_s alterIWSecondaryContemplatorBackground = {{"alterIW4_contemplator_secondary_background", {-100.f, 20.f, 205.f, 22.5f, 2, 2}, {-100.f, 20.f, 205.f, 22.5f, 2, 2}, 0, 1, 0, 0, 0, 0.f, 8, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {0,0,0,0}, {0.f, 0.f, 0.f, 1.f}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 0, 0.f, 0.f, 0.55f, 0, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry};
itemDef_s alterIWSecondaryContemplatorForeground = {{"alterIW4_contemplator_secondary_foreground", {-97.5f, 22.5f, 200.f, 17.5f, 2, 2}, {-97.5f, 22.5f, 200.f, 17.5f, 2, 2}, 0, 1, 0, 0, 0, 0.f, 8, {0x00010004, 0x00010004, 0x00010004, 0x00010004}, 0, {0,0,0,0}, {1.f, 1.f, 1.f, 1.f}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 0}, {nullLocation, nullLocation, nullLocation, nullLocation}, 0, 0, 0, 0, 0, 0.f, 0.f, 0.55f, 0, 0, 0, 0, 0, &g_menuDefPool->entries[20].entry};
#pragma endregion
#pragma endregion

#pragma region Software
//This is the "software" behind my menu, its going to hold
//things like what buttons do, where they go, and the text/colors
//they should hold
#pragma region Structures
//I need these structures to neatly organize my menu info
enum cItemType_t : byte
{
	OpenMenu = 0,

	//Can execute up to 10 with one button
	ExecuteFunction = 1,

	//Can toggle up to 10 with one button
	ToggleBool = 2,
	
	//"   "      "  "  "  "    "   "
	SetFloat = 3,
	
	//"   "      "  "  "  "    "   "
	SetInt = 4,

	//Can set up to    "  "    "   "
	SetString = 5
};

struct cItemType_function_t
{
	void (*function)(void * arg);
	void * argument[3];
};

struct cItemType_float_t
{
	float * floatToSet;
	float minimum;
	float maximum;
};

struct cItemType_int_t
{
	int * intToSet;
	int minimum;
	int maximum;
};

struct cItemType_string_t
{
	char * stringToSet;
	int stringLength;
};

union cItemData_t
{
	cItemType_function_t * functionData[10];
	int menuIndex;
	bool * boolToToggle[10];
	cItemType_float_t * floatToSet[10];
	cItemType_int_t * intToSet[10];
	cItemType_string_t * stringToSet[10];
};

struct cItem_t
{
	char * Text;
	char * HintText;
	byte color;
	byte glowColor;
	cItemType_t type;
	byte itemDataCount;
	cItemData_t itemData;
};

struct cMenu_t
{
	char * HeaderText;
	char * SubHeaderText;
	byte parentIndex;
	byte menuIndex;
	byte itemCount;
	byte hintCount;
	byte headerColor;
	byte subHeaderColor;
	byte headerGlowColor;
	byte subHeaderGlowColor;
	char * hints[15];
	cItem_t items[255];
};
#pragma endregion

#pragma region Definitions
cItemType_function_t unlockAllCheevos = {(void(*)(void*))&DoUnlockAllCheevos};
cMenu_t mainMenu = { "alterIW4 v2.1", "by CraigChrist8239", -1, 0, 5, 3, -1, -1, 1, -1, {"Thumbstick or () to scroll",
	" to select",
	" to exit"
},
{
	{"Players Menu", "Opens a menu that allows you to edit the setting of every player", -1, -1, OpenMenu, 0, {(cItemType_function_t *)1} },
	{"Unlock All Achievements", "Unlock all 50 achievements at once.", -1, -1, ExecuteFunction, 1, {&unlockAllCheevos} },
	{"Dvar Menu", "Useful dvars to change", -1, -1, OpenMenu, 0, {(cItemType_function_t*)2}},
	{"Bots Menu", "Unstable. Dev only.", -1, -1, OpenMenu, 0, {(cItemType_function_t*)4}},
	{"Asset Menu", "Unstable. Used to dump assets for debugging", -1, -1, OpenMenu, 0, {(cItemType_function_t*)10}}
}
};

cMenu_t playersMenu = { "Players Menu", "Client Specific Settings", 0, 1, 18, 3, -1, -1, -1, -1, {"Thumbstick or () to scroll",
	" to select",
	" to exit"
},
{
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} },
	{0, 0, -1, -1, OpenMenu, 0, {(cItemType_function_t *)3} }
}
};

cItemType_float_t weapRateMultiplierSettings = {0, -FLT_MAX, FLT_MAX},
 knockbackSettings = {0, -FLT_MAX, FLT_MAX},
 jumpHeightSettings = {0, 0.f, 1000.f};
cItemType_int_t gSpeedSettings = {0, INT_MIN, INT_MAX},
 gunFireCountSettings = {0, 0, INT_MAX},
 multiJumpSettings = {0, 0, INT_MAX},
 maxInventorySizeSettings = {0, 0, 0xF};
cMenu_t dvarsMenu = { "Dvar Menu", "Useful dvar mods", 0, 2, 12, 3, -1, -1, -1, -1, {"Thumbstick or () to scroll",
	" to select",
	" to exit"
},
{
	{"Toggle Lasers", laserForceOnDesc, -1, -1, ToggleBool, 1, {(cItemType_function_t *)0} },
	{"Toggle Infinite Ammo", "Firing never decreases clip size", -1, -1, ToggleBool, 1, {(cItemType_function_t *)0} },
	{"Set Weapon Rate Multiplier", "The percentage of actual weapon fire time to use", -1, -1, SetFloat, 1, {(cItemType_function_t*)&weapRateMultiplierSettings} },
	{"Set G_Speed", gSpeedDesc, -1, -1, SetInt, 1, {(cItemType_function_t *)&gSpeedSettings} },
	{"Set Jump Height", jumpHeightDesc, -1, -1, SetFloat, 1, {(cItemType_function_t *)&jumpHeightSettings} },
	{"Set Gun Fire Count", bulletsPerShotDesc, -1, -1, SetInt, 1, {(cItemType_function_t *)&gunFireCountSettings} },
	{"Toggle Explosive Bullets", "All bullets will be forced to explode", -1, -1, ToggleBool, 1, {(cItemType_function_t *)0} },
	{"Set Multi-jump Count", multiJumpDvarDesc, -1, -1, SetInt, 1, {(cItemType_function_t *)&multiJumpSettings} },
	{"Set Max Inventory Size", inventorySizeDesc, -1, -1, SetInt, 1, {(cItemType_function_t*)&maxInventorySizeSettings} },
	{"Set G_Knockback", "Maximum knockback", -1, -1, SetFloat, 1, {(cItemType_function_t*)&knockbackSettings} },
	{"Toggle Full Auto", forceFullAutoDesc, -1, -1, ToggleBool, 1, {(cItemType_function_t *)0} }, 
	{"Toggle Infinite Bullet Penetration", infBulletPenDesc, -1, -1, ToggleBool, 1, {(cItemType_function_t *)0} }
}
};

cItemType_function_t kickPlayerSettings = {(void(*)(void*))&kickThisPlayer},
 toggleAimbotSettings = {(void(*)(void*))&toggleCodedAutoaim},
 GivePlayerGoldDeagleSettings = {(void(*)(void*))&GivePlayerGunByName, {(void*)"deserteaglegold_mp"}},
 GivePlayerDefaultWeaponSettings = {(void(*)(void*))&GivePlayerGunByName, {(void*)"defaultweapon_mp"}},
 GivePlayerDualShieldsSettings = {(void(*)(void*))&GivePlayerGunByName, {(void*)"riotshield_mp", (void*)0, (void*)true}},
 GivePlayerDualLaunchersSettings = {(void(*)(void*))&GivePlayerGunByName, {(void*)"m79_mp", (void*)0, (void*)true}},
 GivePlayerDualFALSettings = {(void(*)(void*))&GivePlayerGunByName, {(void*)"fal_mp", (void*)6, (void*)true}},
 GivePlayerM1014Settings = {(void(*)(void*))&GivePlayerGunByName, {(void*)"m1014_grip_silencer_mp", (void*)7, (void*)false}},
 GivePlayerPP2000Settings = {(void(*)(void*))&GivePlayerGunByName, {(void*)"pp2000_akimbo_fmj_mp", (void*)8, (void*)true}},
 sendCustomServerCmdSettings = {(void(*)(void*))&sendCustomKeyboardCmd},
 setRecoilScaleSettings = {(void(*)(void*))&setPlayersRecoilScale},
 setSpreadOverrideSettings = {(void(*)(void*))&setSpreadOverride},
 unlockAllSettings = {(void(*)(void*))&unlockAll};
cItemType_string_t setLocalName = {xenonUserData[0].name, 0x20},
 setPublicName1 = {0, 0x20},
 setPublicName2 = {0, 0x20};
cMenu_t playerMenu = { "Player Menu", "Tweak player settings", 1, 3, 13, 3, -1, -1, -1, -1, {"Thumbstick or () to scroll",
	" to select",
	" to exit"
},
{
	{"TakeDamage Flags", "Change how this player handles damage", -1, -1, OpenMenu, 0, {(cItemType_function_t *)6} },
	{"mFlags", "Toggle the cheats stored in the mFlags", -1, -1, OpenMenu, 0, {(cItemType_function_t *)5} },
	{"Weapon Flags", "Features set using weapon settings", -1, -1, OpenMenu, 0, {(cItemType_function_t *)9} },
	{"Other Flags", "", -1, -1, OpenMenu, 0, {(cItemType_function_t *)8} },
	{"Kick", "Kicks this person from the game", -1, -1, ExecuteFunction, 1, {&kickPlayerSettings} },
	{"Basic Aimbot", "Constantly orients this player to the origin of the nearest enemy player", -1, -1, ExecuteFunction, 1, {&toggleAimbotSettings} },
	{"Give Cool Weapons", "Some cool guns to get you started ;)", -1, -1, ExecuteFunction, 7, {&GivePlayerGoldDeagleSettings, &GivePlayerDefaultWeaponSettings, &GivePlayerDualShieldsSettings, &GivePlayerDualLaunchersSettings, &GivePlayerDualFALSettings, &GivePlayerM1014Settings, &GivePlayerPP2000Settings} },
	{"Set Recoil Scale", "Change the player's recoil scale", -1, -1, ExecuteFunction, 1, {&setRecoilScaleSettings} },
	{"Set Spread Override", "Change the player's gun spread", -1, -1, ExecuteFunction, 1, {&setSpreadOverrideSettings} },
	{"Set Stats", "Dial in the player's stats", -1, -1, OpenMenu, 0, {(cItemType_function_t *)7} },
	{"Unlock Everything", "Cycles through and unlocks everything properly", -1, -1, ExecuteFunction, 1, {&unlockAllSettings} },
	{"Rename", "Change the name of this player in this session ONLY (Unless you're setting your own name)", -1, -1, SetString, 2, {(cItemType_function_t*)&setPublicName1, (cItemType_function_t*)&setPublicName2, (cItemType_function_t*)&setLocalName}},
	{"Send Custom Server Command", "Send a custom server command to this client. (You must know the syntax)", -1, -1, ExecuteFunction, 1, {&sendCustomServerCmdSettings} },
}
};

cItemType_function_t addTestClientSettings = {(void(*)(void*))&AddTestClient};
cMenu_t botsMenu = { "Bots Menu", "UNSTABLE Bot Options", 0, 4, 6, 3, -1, -1, -1, -1, {"Thumbstick or () to scroll",
	" to select",
	" to exit"
},
{
	{"Add Test Client", "DEV TESTING ONLY!", -1, -1, ExecuteFunction, 1, {&addTestClientSettings} },
	{"Watch Killcam", "Testclients will watch killcams.", -1, -1, ToggleBool, 1, {(cItemType_function_t *)0} },
	{"Do Crouch", "Testclients will crouch.", -1, -1, ToggleBool, 1, {(cItemType_function_t *)0} },
	{"Do Reload", "Testclients will reload.", -1, -1, ToggleBool, 1, {(cItemType_function_t *)0} },
	{"Do Move", "Testclients will move.", -1, -1, ToggleBool, 1, {(cItemType_function_t *)0} },
	{"Do Attack", "Testclients will attack.", -1, -1, ToggleBool, 1, {(cItemType_function_t *)0} }
}
};

cItemType_function_t noclipSettings = {(void(*)(void*))&EnableNoclip},
 ufoSettings = {(void(*)(void*))&EnableUFO},
 disableControlSettings = {(void(*)(void*))&DisableControls};
cMenu_t mFlagsMenu = { "mFlags", "Movement Flags", 3, 5, 3, 3, -1, -1, -1, -1, {"Thumbstick or () to scroll",
	" to select",
	" to exit"
},
{
	{"Noclip Mode (0x1)", "No clipping in all 3 axis", -1, -1, ExecuteFunction, 1, {&noclipSettings} },
	{"UFO Mode (0x2)", "No clipping in 2 axis; the up axis is controlled with triggers", -1, -1, ExecuteFunction, 1, {&ufoSettings} },
	{"Disable Controls (0x4)", "Freeze any player actions", -1, -1, ExecuteFunction, 1, {&disableControlSettings} }
}
};

cItemType_function_t toggleGodSettings = {(void(*)(void*))&toggleGodMode},
 toggleDemiGodSettings = {(void(*)(void*))&toggleDemiGodMode},
 toggleNoKnockbackSettings = {(void(*)(void*))&toggleNoKnockback};
cMenu_t takeDamageMenu = { "Take-Damage Flags", "Change the takedamage flags", 3, 6, 3, 3, -1, -1, -1, -1, {"Thumbstick or () to scroll",
	" to select",
	" to exit"
},
{
	{"God Mode (0x1)", "Ignore all damage completely", -1, -1, ExecuteFunction, 1, {&toggleGodSettings} },
	{"DemiGod Mode (0x2)", "Ignore all damage, but still disply hit effects", -1, -1, ExecuteFunction, 1, {&toggleDemiGodSettings} },
	{"No Knockback (0x8)", "No kick from getting hit with bullets", -1, -1, ExecuteFunction, 1, {&toggleNoKnockbackSettings} }
}
};

cItemType_function_t setExpSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2056}},
 setPrestigeSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2064}},
 setScoreSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2072}},
 setKillsSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2076}},
 setLongestKillstreakSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2080}},
 setAccuracySettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2164}},
 setDeathSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2084}},
 setAssistSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2092}},
 setHeadshotsSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2096}},
 setTPAlliesSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2108}},
 setTPAxisSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2112}},
 setTPOtherSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2116}},
 setTPTotalSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2120}},
 setWinsSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2128}},
 setLossesSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2132}},
 setTiesSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2136}},
 setLongestWinstreakSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2140}},
 setBulletsHitSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2152}},
 setBulletsMissedSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2156}},
 setBulletsShotSettings = {(void(*)(void*))&SendSeverCommand_ChangeStat, {(void*)2160}};
cMenu_t playerStatsMenu = { "Set Stats Menu", 0, 3, 7, 20, 3, -1, -1, -1, -1, {"Thumbstick or () to scroll",
	" to select",
	" to exit"
},
{
	{"Accuracy", 0, -1, -1, ExecuteFunction, 1, {&setAccuracySettings} },
	{"@MENU_EXPERIENCE", 0, -1, -1, ExecuteFunction, 1, {&setExpSettings} },
	{"@MPUI_PRESTIGE", 0, -1, -1, ExecuteFunction, 1, {&setPrestigeSettings} },
	{"@MPUI_SCORE", 0, -1, -1, ExecuteFunction, 1, {&setScoreSettings} },
	{"@MPUI_KILLS", 0, -1, -1, ExecuteFunction, 1, {&setKillsSettings} },
	{"@MPUI_LONGEST_KILLSTREAK", 0, -1, -1, ExecuteFunction, 1, {&setLongestKillstreakSettings} },
	{"@MENU_DEATHS", 0, -1, -1, ExecuteFunction, 1, {&setDeathSettings} },
	{"@MPUI_ASSISTS", 0, -1, -1, ExecuteFunction, 1, {&setAssistSettings} },
	{"Headshots", 0, -1, -1, ExecuteFunction, 1, {&setHeadshotsSettings} },
	{"Time Played: Allies", "Change the number seconds this player has played as \"allies\"", -1, -1, ExecuteFunction, 1, {&setTPAlliesSettings} },
	{"Time Played: Axis", "Change the number seconds this player has played as \"axis\"", -1, -1, ExecuteFunction, 1, {&setTPAxisSettings} },
	{"Time Played: Other", "Change the number seconds this player has played as \"other\"", -1, -1, ExecuteFunction, 1, {&setTPOtherSettings} },
	{"Time Played: Total", "Change the number seconds this player has ever played", -1, -1, ExecuteFunction, 1, {&setTPTotalSettings} },
	{"@MENU_WINS", 0, -1, -1, ExecuteFunction, 1, {&setWinsSettings} },
	{"@MPUI_LOSSES", 0, -1, -1, ExecuteFunction, 1, {&setLossesSettings} },
	{"Ties", 0, -1, -1, ExecuteFunction, 1, {&setTiesSettings} },
	{"Longest Winstreak", 0, -1, -1, ExecuteFunction, 1, {&setLongestWinstreakSettings} },
	{"Total Bullets Hit", "Change the number of bullets this player has ever shot and hit a target", -1, -1, ExecuteFunction, 1, {&setBulletsHitSettings} },
	{"Total Bullets Missed", "Change the number of bullets this player has ever shot and missed a target", -1, -1, ExecuteFunction, 1, {&setBulletsMissedSettings} },
	{"Total Bullets Shot", "Change the number of bullets this player has ever shot", -1, -1, ExecuteFunction, 1, {&setBulletsShotSettings} }
}
};

cItemType_function_t toggleThermalSettings = {(void(*)(void*))&toggleThermalVision},
 toggleFOFSettings = {(void(*)(void*))&toggleFOFOverlay},
 toggleEMPJamSettings = {(void(*)(void*))&toggleEMPJamming};
cMenu_t weapFlagsMenu = { "Other Flags", "\"Other\" Flags?", 3, 8, 3, 3, -1, -1, -1, -1, {"Thumbstick or () to scroll",
	" to select",
	" to exit"
},
{
	{"Thermal Vision (0x8)", "Grey-ed vision with no blur, enemies who are not cold blooded will show white hot", -1, -1, ExecuteFunction, 1, {&toggleThermalSettings} },
	{"FOFOverlay (0x10)", "Enemies who are not cold blooded will be highlighted with a red box through walls", -1, -1, ExecuteFunction, 1, {&toggleFOFSettings} },
	{"EMP Jammed (0x400)", "Any backward movement", -1, -1, ExecuteFunction, 1, {&toggleEMPJamSettings} }
}
};

cItemType_function_t toggleDADSSettings = {(void(*)(void*))&togglePlayerADS},
 toggleDWeapSettings = {(void(*)(void*))&toggleWeaponUseage},
 toggleDWeapSwitchSettings = {(void(*)(void*))&toggleWeaponSwitches},
 toggleRecoilScaleSettings = {(void(*)(void*))&toggleRecoilScale},
 toggleDOffhandsSettings = {(void(*)(void*))&toggleOffhandWeapons};
cMenu_t useabilityFlagsMenu = { "Weapon Flags", "Certain Weapon Aspects", 3, 9, 5, 3, -1, -1, -1, -1, {"Thumbstick or () to scroll",
	" to select",
	" to exit"
},
{
	{"Disable ADS (0x20)", 0, -1, -1, ExecuteFunction, 1, {&toggleDADSSettings} },
	{"Disable Weapons (0x80)", 0, -1, -1, ExecuteFunction, 1, {&toggleDWeapSettings} },
	{"Enable Recoil Scale (0x400)", 0, -1, -1, ExecuteFunction, 1, {&toggleRecoilScaleSettings} },
	{"Disable Weapon Switching (0x800)", 0, -1, -1, ExecuteFunction, 1, {&toggleDWeapSwitchSettings} },
	{"Disable Offhand Weapons (0x1000)", 0, -1, -1, ExecuteFunction, 1, {&toggleDOffhandsSettings} }
}
};

cItemType_function_t dumpPPSettings = {(void(*)(void*))&PhysPreset::dumpAllPPs},
 dumpSurfSettings = {(void(*)(void*))&ModelSurface::dumpAllSurfaces},
 dumpImageSettings = {(void(*)(void*))&GfxImage::dumpAllImages},
 dumpColMapSettings = {(void(*)(void*))&g_colMap->dumpCollisionMap},
 dumpMapEntSettings = {(void(*)(void*))&MapEnts::dumpAllMapEntities},
 dumpGFXMapSettings = {(void(*)(void*))&GfxWorld::dumpGFXMap},
 dumpLightSettings = {(void(*)(void*))&GfxLightDef::dumpAllLights},
 dumpWeaponSettings = {(void(*)(void*))&weaponVarientDef_s::dumpAllWeapons},
 dumpSTSettings = {(void(*)(void*))&StringTable::DumpAllStringTables},
 dumpSDDSettings = {(void(*)(void*))&StructuredDataDef::DumpAllSDDs},
 dumpTracerSettings = {(void(*)(void*))&Tracer::dumpAllTracers},
 dumpLocalizeSettings = {(void(*)(void*))&LocalizeEntry::dumpAllLocalizeEntries},
 dumpVehicleSettings = {(void(*)(void*))&Vehicle::dumpAllVehicles};
cMenu_t assetControlMenu = { "Assets", "", 0, 10, 12, 3, -1, -1, -1, -1, {"Thumbstick or () to scroll",
	" to select",
	" to exit"
},
{
	{"Dump All Physical Presets", 0, -1, -1, ExecuteFunction, 1, {&dumpPPSettings} },
	{"Dump All Model Surfaces", "Dumps models to .FBX files for use in 3D modeling software.", -1, -1, ExecuteFunction, 1, {&dumpSurfSettings} },
	{"Dump All Images", "Dumps all images. Images are without a header and swizzled.", -1, -1, ExecuteFunction, 1, {&dumpImageSettings} },
	{"Dump The Collision Map", "Dumps the collision surface of the map to a .FBX file for use in 3D modeling software.", -1, -1, ExecuteFunction, 1, {&dumpColMapSettings} },
	{"Dump All Map Entities", "Dumps the map_ent string, which can later be recompiled to add models to the map.", -1, -1, ExecuteFunction, 1, {&dumpMapEntSettings} },
	//{"Dump The GFX Map", "", -1, -1, ExecuteFunction, 1, {&dumpGFXMapSettings} },
	{"Dump All Lights", "", -1, -1, ExecuteFunction, 1, {&dumpLightSettings} },
	{"Dump All Localized Strings", "", -1, -1, ExecuteFunction, 1, {&dumpLocalizeSettings} },
	{"Dump All Weapons", "Dumps all the compilable settings for weapons, so they can be recompiled later.", -1, -1, ExecuteFunction, 1, {&dumpWeaponSettings} },
	{"Dump All Stringtables", "", -1, -1, ExecuteFunction, 1, {&dumpSTSettings} },
	{"Dump All Structures", "These represent data structures used by the game.", -1, -1, ExecuteFunction, 1, {&dumpSDDSettings} },
	{"Dump All Tracers", "Dumps the tracer settings so they can be recompiled later.", -1, -1, ExecuteFunction, 1, {&dumpTracerSettings} },
	{"Dump All Vehicles", 0, -1, -1, ExecuteFunction, 1, {&dumpVehicleSettings} }
}
};

cMenu_t * menuArray[] = {&mainMenu, &playersMenu, &dvarsMenu, &playerMenu, &botsMenu, &mFlagsMenu, &takeDamageMenu, &playerStatsMenu, &weapFlagsMenu, &useabilityFlagsMenu, &assetControlMenu};
#pragma endregion
#pragma endregion

void AddCustomDvars()
{
	alterIW4_enabled = Dvar_RegisterBool("alterIW4_enabled", 1, 0x8C, "Is alterIW4 running? (Used by scripts to check if we're running with a hacked XEX)");
	alterIW4_developer = Dvar_RegisterBool("alterIW4_developer", 0, 0x8C, "Enables unstable options not fit for public release");
	player_forceFullAuto = Dvar_RegisterBool("player_forceFullAuto", false, 0x0, forceFullAutoDesc);
	player_inventorySize = Dvar_RegisterInt("player_inventorySize", 2, 1, 0xF, 0x8C, inventorySizeDesc);
	player_forceDamageMOD = Dvar_RegisterInt("player_forceDamageMOD", 0, 0, 0xF, 0, "The damage MOD that should be forced in all damage situations");
	player_forceBulletsPerShot = Dvar_RegisterInt("player_forceBulletsPerShot", 0, 0, 0x7FFF, 0, bulletsPerShotDesc);
	player_multiJumpCount = Dvar_RegisterInt("player_multiJumpCount", 2, 0x80000000, 0x7FFFFFFF, 0, multiJumpDvarDesc);
	player_infiniteBulletPenetration = Dvar_RegisterBool("player_infiniteBulletPenetration", 0, 0x8C, infBulletPenDesc);
	//Reversed from TU6
	jump_stepSize = Dvar_RegisterFloat("jump_stepSize", 18.f, 0.f, 100000.f, 0x8C, "The maximum step up to the top of a jump arc");
	g_speed = Dvar_RegisterInt("g_speed", 0xBE, 0x80000000, 0x7FFFFFFF, 0x80, gSpeedDesc);
	jump_height = Dvar_RegisterFloat("jump_height", 39.f, 0.f, 1000.f, 0x8C, jumpHeightDesc);
	laserForceOn = Dvar_RegisterBool("laserForceOn", false, 0x80, laserForceOnDesc);
}

void WorkTheButtons()
{
	bool playSound = true;
	bool select = (buttonFrameCount[BUTTON_A] == 1 || buttonFrameCount[ENTER] == 1);

	if(buttonFrameCount[BUTTON_B] == 1 || (!(buttonFrameCount[BUTTON_B] % 5) && buttonFrameCount[BUTTON_B] > 0) || buttonFrameCount[BUTTON_BACK] >= 1 || buttonFrameCount[BUTTON_START] >= 1 || buttonFrameCount[BACKSPACE] >= 1)
	{
		if(menuArray[selectedMenuIndex]->parentIndex == (byte)-1 || buttonFrameCount[BUTTON_BACK] >= 1 || buttonFrameCount[BUTTON_START] >= 1)
			isMenuOpen = false;
		else
			selectedMenuIndex = menuArray[selectedMenuIndex]->parentIndex;
		selectedItemIndex = 0;
	}
	else if(buttonFrameCount[DPAD_UP] == 1 || (!(buttonFrameCount[DPAD_UP] % 5) && buttonFrameCount[DPAD_UP] && buttonFrameCount[DPAD_UP] <= 30) || (!(buttonFrameCount[DPAD_UP] % 3) && buttonFrameCount[DPAD_UP] > 30 && buttonFrameCount[DPAD_UP] <= 60) || (!(buttonFrameCount[DPAD_UP] % 2) && buttonFrameCount[DPAD_UP] > 60) ||
		buttonFrameCount[APAD_UP] == 1 || (!(buttonFrameCount[APAD_UP] % 5) && buttonFrameCount[APAD_UP] && buttonFrameCount[APAD_UP] <= 30) || (!(buttonFrameCount[APAD_UP] % 3) && buttonFrameCount[APAD_UP] > 30 && buttonFrameCount[APAD_UP] <= 60) || (!(buttonFrameCount[APAD_UP] % 2) && buttonFrameCount[APAD_UP] > 60) ||
		buttonFrameCount['W'] == 1 || !(buttonFrameCount['W'] % 3) && buttonFrameCount['W'] ||
		buttonFrameCount[VK_UP] == 1 || !(buttonFrameCount[VK_UP] % 3) && buttonFrameCount[VK_UP])
	{
		if(selectedItemIndex > 0)
			selectedItemIndex--;
		else
			selectedItemIndex = menuArray[selectedMenuIndex]->itemCount - 1;
	}
	else if((buttonFrameCount[DPAD_DOWN] == 1 || (!(buttonFrameCount[DPAD_DOWN] % 5) && buttonFrameCount[DPAD_DOWN] && buttonFrameCount[DPAD_DOWN] <= 30) || (!(buttonFrameCount[DPAD_DOWN] % 3) && buttonFrameCount[DPAD_DOWN] > 30 && buttonFrameCount[DPAD_DOWN] <= 60) || (!(buttonFrameCount[DPAD_DOWN] % 2) && buttonFrameCount[DPAD_DOWN] > 60) ||
		buttonFrameCount[APAD_DOWN] == 1 || (!(buttonFrameCount[APAD_DOWN] % 5) && buttonFrameCount[APAD_DOWN] && buttonFrameCount[APAD_DOWN] <= 30) || (!(buttonFrameCount[APAD_DOWN] % 3) && buttonFrameCount[APAD_DOWN] > 30 && buttonFrameCount[APAD_DOWN] <= 60) || (!(buttonFrameCount[APAD_DOWN] % 2) && buttonFrameCount[APAD_DOWN] > 60) ||
		buttonFrameCount['S'] == 1 || !(buttonFrameCount['S'] % 3) && buttonFrameCount['S'] ||
		buttonFrameCount[VK_DOWN] == 1 || !(buttonFrameCount[VK_DOWN] % 3) && buttonFrameCount[VK_DOWN]) || 
		(buttonFrameCount[BUTTON_RTRIG] == 1 || (buttonFrameCount[BUTTON_RTRIG] && !(buttonFrameCount[BUTTON_RTRIG] % 2))) ||
		(buttonFrameCount[BUTTON_RSHLDR] == 1 || (buttonFrameCount[BUTTON_RSHLDR] && !(buttonFrameCount[BUTTON_RSHLDR] % 3))))
		selectedItemIndex++;
	else if((buttonFrameCount[BUTTON_LTRIG] == 1 || (buttonFrameCount[BUTTON_LTRIG] && !(buttonFrameCount[BUTTON_LTRIG] % 2))) ||
		(buttonFrameCount[BUTTON_LSHLDR] == 1 || (buttonFrameCount[BUTTON_LSHLDR] && !(buttonFrameCount[BUTTON_LSHLDR] % 3))))
		selectedItemIndex--;
	else if(select && menuArray[selectedMenuIndex]->items[selectedItemIndex].type == ExecuteFunction)
	{
		if(selectedMenuIndex == 1)
			currentEditedPlayer = selectedItemIndex;

		for(int i = 0; i < menuArray[selectedMenuIndex]->items[selectedItemIndex].itemDataCount; i++)
		{
			SV_GameSendServerCommand(hostPlayer->clientData->sess._cs.clientIndex, 1, "s loc_warnings 0");
			SV_GameSendServerCommand(hostPlayer->clientData->sess._cs.clientIndex, 1, "s loc_warningsUI 0");

			if(menuArray[selectedMenuIndex] &&
				menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.functionData[i] &&
				menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.functionData[i]->function)
				((void(*)(void*,void*,void*))menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.functionData[i]->function)(menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.functionData[i]->argument[0], menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.functionData[i]->argument[1], menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.functionData[i]->argument[2]);
		}
	}
	else if(select && menuArray[selectedMenuIndex]->items[selectedItemIndex].type == ToggleBool)
	{
		for(int i = 0; i < menuArray[selectedMenuIndex]->items[selectedItemIndex].itemDataCount; i++)
			*menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.boolToToggle[i] = !*menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.boolToToggle[i];
	}
	else if(select && menuArray[selectedMenuIndex]->items[selectedItemIndex].type == OpenMenu)
	{
		if(menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.menuIndex == 3) currentEditedPlayer = selectedItemIndex;

		selectedMenuIndex = menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.menuIndex;
		selectedItemIndex = 0;
	}
	else if(select && menuArray[selectedMenuIndex]->items[selectedItemIndex].type == SetInt)
	{
		ZeroMemory(MasterCharBuffer, 0x8000);
		ZeroMemory(keyboardDesc, 64 * sizeof(WCHAR));
		sprintf(MasterCharBuffer, "Number must be between %i and %i", menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.intToSet[0]->minimum, menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.intToSet[0]->maximum);
		mbstowcs(keyboardDesc, MasterCharBuffer, strlen(MasterCharBuffer));

		int keyboardResult = XShowKeyboardUI(0, VKBD_LATIN_PHONE, 0, 0, keyboardDesc, (LPWSTR)MasterCharBuffer, 0x100, &theOverlap);
		while(!XHasOverlappedIoCompleted(&theOverlap)) { Sleep(10); }

		if(keyboardResult == ERROR_IO_PENDING && theOverlap.dwExtendedError == ERROR_SUCCESS)
		{
			wcstombs(MasterCharBuffer, (wchar_t *)MasterCharBuffer, 0x100);
			int valueToSet = atoi(MasterCharBuffer);
				
			for(int i = 0; i < menuArray[selectedMenuIndex]->items[selectedItemIndex].itemDataCount; i++)
			if(valueToSet <= menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.intToSet[i]->maximum &&
				valueToSet >= menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.intToSet[i]->minimum)
				*menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.intToSet[i]->intToSet = valueToSet;
		}
	}
	else if(select && menuArray[selectedMenuIndex]->items[selectedItemIndex].type == SetFloat)
	{
		ZeroMemory(MasterCharBuffer, 0x8000);
		ZeroMemory(keyboardDesc, 64 * sizeof(WCHAR));
		sprintf(MasterCharBuffer, "Number must be between %g and %g", menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.floatToSet[0]->minimum, menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.floatToSet[0]->maximum);
		mbstowcs(keyboardDesc, MasterCharBuffer, strlen(MasterCharBuffer));

		int keyboardResult = XShowKeyboardUI(0, VKBD_LATIN_PHONE, 0, 0, keyboardDesc, (LPWSTR)MasterCharBuffer, 0x100, &theOverlap);
		while(!XHasOverlappedIoCompleted(&theOverlap)) { Sleep(10); }

		if(keyboardResult == ERROR_IO_PENDING && theOverlap.dwExtendedError == ERROR_SUCCESS)
		{
			wcstombs(MasterCharBuffer, (wchar_t *)MasterCharBuffer, 0x100);
			float valueToSet = atof(MasterCharBuffer);
				
			for(int i = 0; i < menuArray[selectedMenuIndex]->items[selectedItemIndex].itemDataCount; i++)
			if(valueToSet <= menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.floatToSet[i]->maximum &&
				valueToSet >= menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.floatToSet[i]->minimum)
				*menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.floatToSet[i]->floatToSet = valueToSet;
		}
	}
	else if(select && menuArray[selectedMenuIndex]->items[selectedItemIndex].type == SetString)
	{
		ZeroMemory(MasterCharBuffer, 0x8000);
		mbstowcs((wchar_t *)MasterCharBuffer, menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.stringToSet[0]->stringToSet, strlen(menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.stringToSet[0]->stringToSet));
		
		int keyboardResult = XShowKeyboardUI(0, VKBD_HIGHLIGHT_TEXT, (LPCWSTR)MasterCharBuffer, 0, 0, (LPWSTR)MasterCharBuffer, menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.stringToSet[0]->stringLength, &theOverlap);
		while(!XHasOverlappedIoCompleted(&theOverlap)) { Sleep(10); }

		if(keyboardResult == ERROR_IO_PENDING && theOverlap.dwExtendedError == ERROR_SUCCESS)
		{
			for(int i = 0; i < menuArray[selectedMenuIndex]->items[selectedItemIndex].itemDataCount; i++)
				wcstombs(menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.stringToSet[i]->stringToSet, (wchar_t *)MasterCharBuffer, menuArray[selectedMenuIndex]->items[selectedItemIndex].itemData.stringToSet[0]->stringLength);
		}
	}
	else
		playSound = false;

	if(playSound)
	{
		sprintf(MasterCharBuffer,  "p %i", G_SoundAliasIndex("mouse_over"));
		SV_GameSendServerCommand(hostPlayer->clientData->ps.clientNum, 0, MasterCharBuffer);
	}
}

void patchTU8XEXEveryFrameOnDev()
{
	if(!(*player_sustainAmmo))
		return;

	int outInt = 0;
	
	int sustainAmmoPatchOn = 0x7D284B78,sustainAmmoPatchOff = 0x7D1D4850;
	//player_sustainAmmo
	WriteMemory((LPVOID)0x820E1724, 4, (*player_sustainAmmo)->current.enabled ? (LPCVOID)&sustainAmmoPatchOn : (LPCVOID)&sustainAmmoPatchOff);

	int forceFullAutoPatchOn = 0x39400001,forceFullAutoPatchOff = 0x556A07FE;
	//player_forceFullAuto
	if(player_forceFullAuto->current.enabled)
		WriteMemory((LPVOID)0x820E018C, 4, (LPCVOID)&forceFullAutoPatchOn);
	else
		WriteMemory((LPVOID)0x820E018C, 4, (LPCVOID)&forceFullAutoPatchOff);
	
	//TODO: Fix not working for FMJ bullets
	//Went to TU0 and copied how it works... Still doesn't work on fmj bullets
	int forceExplosiveBulletsPatchOff2 = 0x419A000C,forceExplosiveBulletsPatchOff = 0x419A000C,forceExplosiveBulletsPatchOff3 = 0x419A0014,forceExplosiveBulletsPatchOff4 = 0x419A0018;
	//bg_forceExplosiveBullets
	if(Dvar_GetBool("bg_forceExplosiveBullets"))
	{
		WriteMemory((LPVOID)0x820E3BD0, 4, (LPCVOID)&NOP);
		WriteMemory((LPVOID)0x820E3BF4, 4, (LPCVOID)&NOP);
		WriteMemory((LPVOID)0x820E3C3C, 4, (LPCVOID)&NOP);
		WriteMemory((LPVOID)0x820E3CA0, 4, (LPCVOID)&NOP);
		WriteMemory((LPVOID)0x820E4280, 4, (LPCVOID)&NOP);
	}
	else
	{
		WriteMemory((LPVOID)0x820E3BD0, 4, (LPCVOID)&forceExplosiveBulletsPatchOff2);
		WriteMemory((LPVOID)0x820E3BF4, 4, (LPCVOID)&forceExplosiveBulletsPatchOff3);
		WriteMemory((LPVOID)0x820E3C3C, 4, (LPCVOID)&forceExplosiveBulletsPatchOff4);
		WriteMemory((LPVOID)0x820E3CA0, 4, (LPCVOID)&forceExplosiveBulletsPatchOff4);
		WriteMemory((LPVOID)0x820E4280, 4, (LPCVOID)&forceExplosiveBulletsPatchOff);
	}

	int damageModForcerPatchOn = 0x3BE00000 + player_forceDamageMOD->current.integer,damageModForcerPatchOff = 0x7D1F4378;
	//player_forceDamageMOD
	if(player_forceDamageMOD->current.integer > 0)
		WriteMemory((LPVOID)0x8220541C, 4, (LPCVOID)&damageModForcerPatchOn);
	else
		WriteMemory((LPVOID)0x8220541C, 4, (LPCVOID)&damageModForcerPatchOff);

	//player_forceBulletsPerShot
	int bulletsPerShot = player_forceBulletsPerShot->current.integer,bulletsPerShotPatch1On = 0x3BE00000 + bulletsPerShot,
		bulletsPerShotPatch2P1On = 0x39600000 + bulletsPerShot,
		bulletsPerShotPatch2P2On = 0x3BC00000 + bulletsPerShot,
		bulletsPerShotPatch1P1Off = 0x83F90220,
		bulletsPerShotPatch1P2Off = 0x3BE00001,
		bulletsPerShotPatch2P1Off = 0x816A0220,
		bulletsPerShotPatch2P2Off = 0x3BC00001;
	if(bulletsPerShot)
	{
		WriteMemory((LPVOID)0x821354F0, 4, (LPCVOID)&bulletsPerShotPatch1On);
		WriteMemory((LPVOID)0x82135500, 4, (LPCVOID)&bulletsPerShotPatch1On);
		WriteMemory((LPVOID)0x821CF464, 4, (LPCVOID)&bulletsPerShotPatch2P1On);
		WriteMemory((LPVOID)0x821CF478, 4, (LPCVOID)&bulletsPerShotPatch2P2On);
	}
	else
	{
		WriteMemory((LPVOID)0x821354F0, 4, (LPCVOID)&bulletsPerShotPatch1P1Off);
		WriteMemory((LPVOID)0x82135500, 4, (LPCVOID)&bulletsPerShotPatch1P2Off);
		WriteMemory((LPVOID)0x821CF464, 4, (LPCVOID)&bulletsPerShotPatch2P1Off);
		WriteMemory((LPVOID)0x821CF478, 4, (LPCVOID)&bulletsPerShotPatch2P2Off);
	}
	
	unsigned short inventorySize = (unsigned short)player_inventorySize->current.integer;
	//player_inventorySize
	WriteMemory((LPVOID)0x820E35B2, 2, (LPCVOID)&inventorySize);

	//jump_stepSize
	int jumpStepSizeOffset = (int)&jump_stepSize->current.value;
	short jumpStepSizeOffset1,jumpStepSizeOffset2 = (jumpStepSizeOffset & 0xFFFF);
	if(jumpStepSizeOffset & 0x8000) // If bit 16 is 1
		jumpStepSizeOffset1 = (((jumpStepSizeOffset >> 16) & 0xFFFF) + 1); // lis %r11, dest>>16 + 1
	else
		jumpStepSizeOffset1 = ((jumpStepSizeOffset >> 16) & 0xFFFF); // lis %r11, dest>>16
	WriteMemory((LPVOID)0x820CAD0E, 2, (LPCVOID)&jumpStepSizeOffset1);
	WriteMemory((LPVOID)0x820CAD12, 2, (LPCVOID)&jumpStepSizeOffset2);
	
	//jump_height
	int jumpHeightOffset = (int)&jump_height->current.value;
	short jumpHeightOffset1,jumpHeightOffset2 = (jumpHeightOffset & 0xFFFF);
	if(jumpHeightOffset & 0x8000) // If bit 16 is 1
		jumpHeightOffset1 = (((jumpHeightOffset >> 16) & 0xFFFF) + 1); // lis %r11, dest>>16 + 1
	else
		jumpHeightOffset1 = ((jumpHeightOffset >> 16) & 0xFFFF); // lis %r11, dest>>16
	WriteMemory((LPVOID)0x820CB6A6, 2, (LPCVOID)&jumpHeightOffset1);
	WriteMemory((LPVOID)0x820CB6B2, 2, (LPCVOID)&jumpHeightOffset2);
	
	//g_speed
	short gSpeedValue = (g_speed->current.integer & 0xFFFF);
	WriteMemory((LPVOID)0x821D1E7A, 2, (LPCVOID)&gSpeedValue);

	//Infinite Bullet Penetration
	int infPenetrationPatch1On = 0xFD806890, infPenetrationPatch1Off = 0xEC25217A,
		infPenetrationPatch2On = 0xED8D0028, infPenetrationPatch2Off = 0xEC2611BA;
	if(player_infiniteBulletPenetration->current.enabled)
	{
		short srt_max = SHRT_MAX;
		WriteMemory((LPVOID)0x82134C14, 4, (LPCVOID)&infPenetrationPatch1On);
		WriteMemory((LPVOID)0x821CEE28, 4, (LPCVOID)&infPenetrationPatch1On);
		WriteMemory((LPVOID)0x821CEE90, 4, (LPCVOID)&infPenetrationPatch2On);
		WriteMemory((LPVOID)0x82134C64, 4, (LPCVOID)&infPenetrationPatch2On);
		WriteMemory((LPVOID)0x82134D4A, 2, (LPCVOID)&srt_max);
		WriteMemory((LPVOID)0x821CEF2A, 2, (LPCVOID)&srt_max);
	}
	else
	{
		short srt_five = 5;
		WriteMemory((LPVOID)0x82134C14, 4, (LPCVOID)&infPenetrationPatch1Off);
		WriteMemory((LPVOID)0x821CEE28, 4, (LPCVOID)&infPenetrationPatch1Off);
		WriteMemory((LPVOID)0x821CEE90, 4, (LPCVOID)&infPenetrationPatch2Off);
		WriteMemory((LPVOID)0x82134C64, 4, (LPCVOID)&infPenetrationPatch2Off);
		WriteMemory((LPVOID)0x82134D4A, 2, (LPCVOID)&srt_five);
		WriteMemory((LPVOID)0x821CEF2A, 2, (LPCVOID)&srt_five);
	}

	//laserForceOn
	int forceLaserValue = 0x38600000 + laserForceOn->current.enabled;
	WriteMemory((LPVOID)0x82104090, 4, (LPCVOID)&forceLaserValue);
}

void updateStickInfo()
{
	for(int i = 0; i <= 6; i++)
	{
		float tempAxisValue = CL_GamepadAxisValue(0, i);
		tempAxisValue = tempAxisValue < 0.075f && tempAxisValue > -0.075f ? 0.0f : tempAxisValue;
		tempAxisValue = tempAxisValue > 0.95f ? 1.0f : tempAxisValue;
		tempAxisValue = tempAxisValue < -0.95f ? -1.0f : tempAxisValue;
		axisValues[i] = tempAxisValue;
	}
}

bool hasRemovedGayButton = false;
void updateKeyboardInfo()
{
	XInputGetKeystroke(0, XINPUT_FLAG_KEYBOARD, &myKeystroke);

	for(int i = 0x20; i < 0x96; i++) if(buttonFrameCount[i] > 0) buttonFrameCount[i]++;

	if(myKeystroke.VirtualKey < 0x20 || myKeystroke.VirtualKey > 0x96)
	{
		if(myKeystroke.VirtualKey == VK_RETURN)
		{
			if(myKeystroke.Flags & XINPUT_KEYSTROKE_KEYDOWN)
				buttonFrameCount[ENTER] = 1;
			else if(myKeystroke.Flags & XINPUT_KEYSTROKE_KEYUP)
				buttonFrameCount[ENTER] = 0;
		}
		else if(myKeystroke.VirtualKey == VK_BACK)
		{
			if(myKeystroke.Flags & XINPUT_KEYSTROKE_KEYDOWN)
				buttonFrameCount[BACKSPACE] = 1;
			else if(myKeystroke.Flags & XINPUT_KEYSTROKE_KEYUP)
				buttonFrameCount[BACKSPACE] = 0;
		}
		else if(myKeystroke.VirtualKey == VK_PAUSE)
		{
			if(myKeystroke.Flags & XINPUT_KEYSTROKE_KEYDOWN)
				buttonFrameCount[PAUSE] = 1;
			else if(myKeystroke.Flags & XINPUT_KEYSTROKE_KEYUP)
				buttonFrameCount[PAUSE] = 0;
		}
	}
	else
	{
		if(myKeystroke.Flags & XINPUT_KEYSTROKE_KEYDOWN)
			buttonFrameCount[myKeystroke.VirtualKey] = 1;
		else if(myKeystroke.Flags & XINPUT_KEYSTROKE_KEYUP)
			buttonFrameCount[myKeystroke.VirtualKey] = 0;
	}
}

void updateButtonInfo()
{
	for(int i = 0; i <= 0xDE; i++)
	{
		if(Key_IsDown(0, i))
			buttonFrameCount[i]++;
		else
			buttonFrameCount[i] = 0;
	}
}

bool doIt = false;

DWORD WINAPI MW2_TU8_Loop(LPVOID)
{
	//I'm the only one who even tried to use the alterIW4.patch_mp.ff loader.
	//I removed it long ago because the only thing people might possibly edit can now be edited with the asset loaders.
	//This was before I knew how to make asset loaders. 
	/*#pragma region alterIW4.patch_mp.ff Loader
	{
		char * zoneName = "patch_mp2";
		sprintf(MasterCharBuffer, "game:\\%s.ff", zoneName);
		XZoneInfo alterIW_patch_mp = {zoneName, 1, 0};

		bool * g_zoneInited = (bool *)0x82D1B535, doesFileExist = fileExists(MasterCharBuffer);
		while(!*g_zoneInited) { Sleep(10); };

		if(doesFileExist)
			MW2_DB_LoadXAssets(&alterIW_patch_mp, 1, 1, 0);
	}
	#pragma endregion*/

	#pragma region Declare My Dvars
	{
		bool * isDvarSystemActive = (bool *)0x83749581;
		while(!*isDvarSystemActive) { Sleep(10); };

		AddCustomDvars();
	}
	#pragma endregion

	for(;;)
	{
		Sleep(50);
		
		patchTU8XEXEveryFrameOnDev();
		updateButtonInfo();
		updateKeyboardInfo();
		updateStickInfo();
		
		//I added this patch to remove the "DOWNLOAD CONTENT" button on the main MP screen.
		//I think it caused some people problems so I removed it long ago. 
		/*menuDef_t * mainmenu = &g_menuDefPool->entries[16].entry;
		if(!hasRemovedGayButton &&
			g_menuDefPool->freeHead != &g_menuDefPool->entries[16].entry &&
			g_menuDefPool->entries[16].next != 0 &&
			g_menuDefPool->entries[16].next != &g_menuDefPool->entries[17].entry && 
			g_menuDefPool->entries[16].entry.items != 0)
		{
			for(int i = 0; i < g_menuDefPool->entries[16].entry.itemCount; i++)
			{
				 if(mainmenu->items[i] != 0 &&
					 !strcmp("@PATCH_DOWNLOAD_CONTENT_CAPS", mainmenu->items[i]->text))
				 {
					printf("EDITING %s\n", mainmenu->window.name);
					mainmenu->items[i]->visibleExp->entryCount = 1;
					mainmenu->items[i]->visibleExp->entries[0].type = 1;
					mainmenu->items[i]->visibleExp->entries[0].Id = 0;
					mainmenu->items[i]->visibleExp->entries[0].data = 0;
					mainmenu->items[i+1]->visibleExp->entryCount = 1;
					mainmenu->items[i+1]->visibleExp->entries[0].type = 1;
					mainmenu->items[i+1]->visibleExp->entries[0].Id = 0;
					mainmenu->items[i+1]->visibleExp->entries[0].data = 0;
					hasRemovedGayButton = true;
					break;
				 }
			}
		}*/

		for(int i = 0; i < 18; i++)
		{
			hostPlayer = gentity_s::GetGEntity(i);
			if(!(int)hostPlayer->clientData || !*(int *)0x83623B98)
			{
				hostPlayer = 0;
				break;
			}

			if(hostPlayer->clientData->sess.localClient & 1 && !*(int *)(*(int *)0x83623B98 + (i * 0x97F80) + 0x31AFC))
				break;

			if(i == 17)
				hostPlayer = 0;
			
		}

		if(CL_LocalClientIsInGame(0))
		for(int i = 0; i < 18; i++)
		{
			//gentity_s * tempEnt = getEntity(i);
			gentity_s * temporaryEnt = gentity_s::GetGEntity(i);
			gclient_s * temporaryClient = temporaryEnt->GetGClient();
			bool iAmOnGround = temporaryClient->IsOnGround(),
				iAmAlive = temporaryEnt->IsAlive();

			//if(IsOnGround(tempEnt))
			if(iAmOnGround)
				multiJumpCount[i] = 0;

			//if(applyAutoAim[i] && IsAlive(tempEnt))
			if(applyAutoAim[i] && iAmAlive)
				applyAutoaimToGEntity(temporaryEnt);

			if(temporaryEnt && temporaryClient)
			if(!iAmOnGround && temporaryClient->JumpButtonPressed() && iAmAlive && !(entityButtons[i] & 0x400))
			{
				multiJumpCount[i]++;

				entityButtons[i] = temporaryClient->buttons;

				if(multiJumpCount[i] > 1 && (multiJumpCount[i]-1) <= player_multiJumpCount->current.integer)
				{
					float * currentVelocity = temporaryClient->GetVelocity();
					float targetVelocity[3];
					targetVelocity[0] = currentVelocity[0];
					targetVelocity[1] = currentVelocity[1];
					targetVelocity[2] = currentVelocity[2] < 0 ? 0 : currentVelocity[2];
							
					targetVelocity[2] += sqrt(jump_height->current.value * temporaryClient->ps.gravity * 2);

					currentVelocity[0] = targetVelocity[0];
					currentVelocity[1] = targetVelocity[1];
					currentVelocity[2] = targetVelocity[2];
				}
			}
			else if(!temporaryClient->JumpButtonPressed())
				entityButtons[i] = temporaryClient->buttons;
		}

		if(CL_LocalClientIsInGame(0) && hostPlayer && hostPlayer->clientData && hostPlayer->clientData->sess.localClient && hostPlayer->IsAlive())
		{
			(*cg_fov)->current.value = 80.f;

			if(alterIW4_developer->current.enabled)
			{
				sprintf(originDisplay.text, "Origin: %f %f %f\0", hostPlayer->r.currentOrigin[0], hostPlayer->r.currentOrigin[1], hostPlayer->r.currentOrigin[2]);
				sprintf(angleDisplay.text, "Angles: %f %f %f\0", hostPlayer->r.currentAngles[0], hostPlayer->r.currentAngles[1], hostPlayer->r.currentAngles[2]);
			}

			if(hostPlayer)
			if(!isMenuOpen && (hostPlayer->clientData->mFlags & 4))
				hostPlayer->clientData->mFlags &= ~4;
			else if(isMenuOpen)
				hostPlayer->clientData->mFlags |= 4;

			//if(menu isn't open and we open it
			if(isMenuOpen)
			{
				Dvar_FindVar("r_blur")->current.value = 10.f;
				WorkTheButtons();

				headerTitle.text = menuArray[selectedMenuIndex]->HeaderText;
				headerSubText.text = menuArray[selectedMenuIndex]->SubHeaderText;
				headerTitle.SetY(headerTitle.window.rect.y);
				headerSubText.SetY(headerSubText.window.rect.y);
				headerTitle.SetForeColor(menuArray[selectedMenuIndex]->headerColor != (byte)-1 ? &g_color_table[menuArray[selectedMenuIndex]->headerColor * 4] : colorWhite);
				headerTitle.SetGlowColor(menuArray[selectedMenuIndex]->headerGlowColor != (byte)-1 ? &g_color_table[menuArray[selectedMenuIndex]->headerGlowColor * 4] : 0);
				headerSubText.SetForeColor(menuArray[selectedMenuIndex]->subHeaderColor != (byte)-1 ? &g_color_table[menuArray[selectedMenuIndex]->subHeaderColor * 4] : colorWhite);
				headerSubText.SetGlowColor(menuArray[selectedMenuIndex]->subHeaderGlowColor != (byte)-1 ? &g_color_table[menuArray[selectedMenuIndex]->subHeaderGlowColor * 4] : 0);

				for(int i = 0; i < 18; i++)
					playersMenu.items[i].Text = gclient_s::GetGClient(i) ? gclient_s::GetGClient(i)->sess.newnetname : 0;

				dvarsMenu.items[0].itemData.boolToToggle[0] = &laserForceOn->current.enabled;
				dvarsMenu.items[0].glowColor = laserForceOn->current.enabled ? 2 : -1;
				dvarsMenu.items[1].itemData.boolToToggle[0] = &(*player_sustainAmmo)->current.enabled;
				dvarsMenu.items[1].glowColor = (*player_sustainAmmo)->current.enabled ? 2 : -1;
				dvarsMenu.items[2].itemData.floatToSet[0]->floatToSet = &(*perk_weapRateMultiplier)->current.value;
				dvarsMenu.items[2].glowColor = *dvarsMenu.items[2].itemData.floatToSet[0]->floatToSet == 0.75f ? -1 : 2;
				dvarsMenu.items[3].itemData.intToSet[0]->intToSet = &g_speed->current.integer;
				dvarsMenu.items[3].glowColor = g_speed->current.integer == 190 ? -1 : 2;
				dvarsMenu.items[4].itemData.floatToSet[0]->floatToSet = &jump_height->current.value;
				dvarsMenu.items[4].glowColor = *dvarsMenu.items[4].itemData.floatToSet[0]->floatToSet == 39.f ? -1 : 2;
				dvarsMenu.items[5].itemData.intToSet[0]->intToSet = &player_forceBulletsPerShot->current.integer;
				dvarsMenu.items[5].glowColor = player_forceBulletsPerShot->current.integer == 0 ? -1 : 2;
				dvarsMenu.items[6].itemData.boolToToggle[0] = &Dvar_FindVar("bg_forceExplosiveBullets")->current.enabled;
				dvarsMenu.items[6].glowColor = Dvar_GetBool("bg_forceExplosiveBullets") ? 2 : -1;
				dvarsMenu.items[7].itemData.intToSet[0]->intToSet = &player_multiJumpCount->current.integer;
				dvarsMenu.items[7].glowColor = player_multiJumpCount->current.integer == 2 ? -1 : 2;
				dvarsMenu.items[8].itemData.intToSet[0]->intToSet = &player_inventorySize->current.integer;
				dvarsMenu.items[8].glowColor = player_inventorySize->current.integer == 2 ? -1 : 2;
				dvarsMenu.items[9].itemData.floatToSet[0]->floatToSet = &(*g_knockback)->current.value;
				dvarsMenu.items[9].glowColor = (*g_knockback)->current.value == 1000.f ? -1 : 2;
				dvarsMenu.items[10].itemData.boolToToggle[0] = &player_forceFullAuto->current.enabled;
				dvarsMenu.items[10].glowColor = player_forceFullAuto->current.enabled ? 2 : -1;
				dvarsMenu.items[11].itemData.boolToToggle[0] = &player_infiniteBulletPenetration->current.enabled;
				dvarsMenu.items[11].glowColor = player_infiniteBulletPenetration->current.enabled ? 2 : -1;

				if(currentEditedPlayer != -1)
				{
					gentity_s * selectedPlayer = gentity_s::GetGEntity(currentEditedPlayer);
					if(selectedPlayer->clientData)
					{
						//sprintf(playerMenu.HeaderText, "%s Menu", &selectedPlayer->clientData->sess.newnetname);
						playerMenu.items[5].glowColor = applyAutoAim[currentEditedPlayer] && selectedPlayer->IsAlive() ? 2 : -1;
						playerMenu.items[7].glowColor = *(int *)((int)selectedPlayer->clientData + 0x2BC) & 0x400 ? 2 : -1;
						playerMenu.items[8].glowColor = *(int *)((int)selectedPlayer->clientData + 0x2D0) == 2 ? 2 : -1;

						setPublicName1.stringToSet = selectedPlayer->clientData->sess.newnetname;
						setPublicName2.stringToSet = selectedPlayer->clientData->sess._cs.name;
						playerMenu.items[10].itemDataCount = selectedPlayer == hostPlayer ? 3 : 2;

						mFlagsMenu.items[0].glowColor = selectedPlayer->clientData->mFlags & 1 ? 2 : -1;
						mFlagsMenu.items[1].glowColor = selectedPlayer->clientData->mFlags & 2 ? 2 : -1;
						mFlagsMenu.items[2].glowColor = selectedPlayer->clientData->mFlags & 4 ? 2 : -1;
						playerMenu.items[1].glowColor = selectedPlayer->clientData->mFlags & ((selectedPlayer == hostPlayer) ? (1|2) : (1|2|4)) ? 2 : -1;

						takeDamageMenu.items[0].glowColor = selectedPlayer->takedamage & FL_GODMODE ? 2 : -1;
						takeDamageMenu.items[1].glowColor = selectedPlayer->takedamage & FL_DEMI_GODMODE ? 2 : -1;
						takeDamageMenu.items[2].glowColor = selectedPlayer->takedamage & FL_NO_KNOCKBACK ? 2 : -1;
						playerMenu.items[0].glowColor = selectedPlayer->takedamage & (FL_GODMODE|FL_DEMI_GODMODE|FL_NO_KNOCKBACK) ? 2 : -1;

						weapFlagsMenu.items[0].glowColor = selectedPlayer->clientData->ps.otherFlags & 8 ? 2 : -1;
						weapFlagsMenu.items[1].glowColor = selectedPlayer->clientData->ps.otherFlags & 0x10 ? 2 : -1;
						weapFlagsMenu.items[2].glowColor = selectedPlayer->clientData->ps.otherFlags & 0x400 ? 2 : -1;
						playerMenu.items[3].glowColor = selectedPlayer->clientData->ps.otherFlags & (8|0x10|0x400) ? 2 : -1;

						useabilityFlagsMenu.items[0].glowColor = selectedPlayer->clientData->ps.weapFlags & 0x20 ? 2 : -1;
						useabilityFlagsMenu.items[1].glowColor = selectedPlayer->clientData->ps.weapFlags & 0x80 ? 2 : -1;
						useabilityFlagsMenu.items[2].glowColor = selectedPlayer->clientData->ps.weapFlags & 0x400 ? 2 : -1;
						useabilityFlagsMenu.items[3].glowColor = selectedPlayer->clientData->ps.weapFlags & 0x800 ? 2 : -1;
						useabilityFlagsMenu.items[4].glowColor = selectedPlayer->clientData->ps.weapFlags & 0x1000 ? 2 : -1;
						playerMenu.items[2].glowColor = selectedPlayer->clientData->ps.weapFlags & (0x20|0x80|0x400|0x800|0x1000) ? 2 : -1;
					}
				}

				botsMenu.items[1].itemData.boolToToggle[0] = &(*testClients_watchKillcam)->current.enabled;
				botsMenu.items[1].glowColor = (*testClients_watchKillcam)->current.enabled ? 2 : -1;
				botsMenu.items[2].itemData.boolToToggle[0] = &(*testClients_doCrouch)->current.enabled;
				botsMenu.items[2].glowColor = (*testClients_doCrouch)->current.enabled ? 2 : -1;
				botsMenu.items[3].itemData.boolToToggle[0] = &(*testClients_doReload)->current.enabled;
				botsMenu.items[3].glowColor = (*testClients_doReload)->current.enabled ? 2 : -1;
				botsMenu.items[4].itemData.boolToToggle[0] = &(*testClients_doMove)->current.enabled;
				botsMenu.items[4].glowColor = (*testClients_doMove)->current.enabled ? 2 : -1;
				botsMenu.items[5].itemData.boolToToggle[0] = &(*testClients_doAttack)->current.enabled;
				botsMenu.items[5].glowColor = (*testClients_doAttack)->current.enabled ? 2 : -1;

				for(int i = 0; i < 15; i++)
				{
					menuHintText[i].SetY((240.f - (menuArray[selectedMenuIndex]->hintCount * 10.f)) + (i * 20.f));

					if(i >= menuArray[selectedMenuIndex]->hintCount)
					{
						menuHintText[i].text = 0;
						menuHintText[i].window.backColor[3] = 0.f;
					}
					else
					{
						menuHintText[i].text = menuArray[selectedMenuIndex]->hints[i];
						menuHintText[i].window.backColor[3] = 0.125f;
					}
				}

				unsigned int maxValue = 10,minValue = 0,itemCount = menuArray[selectedMenuIndex]->itemCount;

				if(selectedItemIndex < 0)
					selectedItemIndex = itemCount - 1;
				else if(selectedItemIndex >= itemCount)
					selectedItemIndex = 0;

				if(selectedItemIndex <= 5 || itemCount < 10) { } //Took it from my "Steal This Patch", not changing it either
				else
				{
					if(selectedItemIndex + 5 < itemCount)
					{
						minValue = selectedItemIndex - 5;
						maxValue = selectedItemIndex + 5;
					}
					else
					{
						minValue = itemCount - 10;
						maxValue = itemCount;
					}
				}

				for(unsigned int i = minValue, buttonIndex = 0; i < maxValue; i++, buttonIndex++)
				{
					buttons[buttonIndex].window.style = (selectedItemIndex == i) ? 4 : 1;
					buttons[buttonIndex].SetGlowColor(menuArray[selectedMenuIndex]->items[i].glowColor != (byte)-1 ? &g_color_table[menuArray[selectedMenuIndex]->items[i].glowColor * 4] : 0);
					buttons[buttonIndex].SetForeColor(menuArray[selectedMenuIndex]->items[i].color != (byte)-1 ? &g_color_table[menuArray[selectedMenuIndex]->items[i].color * 4] : colorWhite);
					buttons[buttonIndex].textStyle = (selectedItemIndex == i) ? 1 : 3;
					buttons[buttonIndex].SetY(70.f + (buttonIndex * 20.f));
					buttons[buttonIndex].text = i < menuArray[selectedMenuIndex]->itemCount ? menuArray[selectedMenuIndex]->items[i].Text : 0;

					if(selectedItemIndex == i)
					{
						selectorAButton.SetY(selectorBar.SetY(70.f + (buttonIndex * 20.f)));
						buttonHint.text = menuArray[selectedMenuIndex]->items[i].HintText;
						buttonHint.SetY(buttonHint.window.rect.y);
					}
				}
			}
			else 
			{
				if(buttonFrameCount[DPAD_UP] && !(buttonFrameCount[DPAD_UP]%3))
				{
						isMenuOpen = true;
						selectedMenuIndex = selectedItemIndex = 0;

						headerSpacer.window.background = selectorBar.window.background = DB_FindXAssetHeader(material, "gradient_fadein").material;
						alterIWBackground2.window.background = DB_FindXAssetHeader(material, "gradient_fadein_fadebottom").material;
				}
				Dvar_FindVar("r_blur")->current.value = 0.f;
			}
		}
		else
			isMenuOpen = false;
	}
}

//This should handle all code before returning to main and the game...
//Things like patching and thread creation should be done here
void MW2_TU8_Init()
{
	//patchTU8BootXEXOnDev();

	HANDLE hHand;
	DWORD threadID;

	//hHand = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&MW2_TU8_Loop, 0, 0, &threadID);

	//if(hHand != NULL)
		//CloseHandle(hHand);
}

void itemDef_s::SetGlowColor(float * color)
{
	glowColor[0] = color ? color[0] : 0;
	glowColor[1] = color ? color[1] : 0;
	glowColor[2] = color ? color[2] : 0;
	glowColor[3] = color ? color[3] * 0.25f : 0;
}

void itemDef_s::SetXY(float x, float y)
{
	SetX(x);
	SetY(y);
}

float itemDef_s::SetX(float x)
{
	window.rect.x = window.rectClient.x = x;
	ZeroMemory(textRect, sizeof(rectDef_s)*4);
	
	return x;
}

float itemDef_s::SetY(float y)
{
	window.rect.y = window.rectClient.y = y;
	ZeroMemory(textRect, sizeof(rectDef_s)*4);
	return y;
}

float itemDef_s::SetW(float w)
{
	window.rect.w = window.rectClient.w = w;
	ZeroMemory(textRect, sizeof(rectDef_s)*4);
	return w;
}

void SetColorTarget(float * colorToSet, float * settingToColor)
{
	for(int i=0;i<4;i++)colorToSet[i] = settingToColor ? settingToColor[i] : 0;
}

void itemDef_s::SetForeColor(float * color)
{
	SetColorTarget(window.foreColor, color);

}

void itemDef_s::SetBackColor(float * color)
{
	SetColorTarget(window.backColor, color);
}

void itemDef_s::SetBorderColor(float * color)
{
	window.borderColor[0] = color ? color[0] : 0;
	window.borderColor[1] = color ? color[1] : 0;
	window.borderColor[2] = color ? color[2] : 0;
	window.borderColor[3] = color ? color[3] : 0;
}

void itemDef_s::SetOutlineColor(float * color)
{
	window.outlineColor[0] = color ? color[0] : 0;
	window.outlineColor[1] = color ? color[1] : 0;
	window.outlineColor[2] = color ? color[2] : 0;
	window.outlineColor[3] = color ? color[3] : 0;
}

void itemDef_s::SetDisableColor(float * color)
{
	window.disableColor[0] = color ? color[0] : 0;
	window.disableColor[1] = color ? color[1] : 0;
	window.disableColor[2] = color ? color[2] : 0;
	window.disableColor[3] = color ? color[3] : 0;
}

char theAssetBuffer[0x4000];
bool usingAssetBuffer = false;

void MW2TitleUpdate8InternalLoop(const void * args, int unknown)
{ 
	if(alterIW4_developer->current.enabled && CL_LocalClientIsInGame(0) && hostPlayer->IsAlive())
	{
		Item_Paint((void *)0x825AE948, 0x82690F48, &originDisplay);
		Item_Paint((void *)0x825AE948, 0x82690F48, &angleDisplay);
	}

	if(isMenuOpen)
	{
		Item_Paint((void *)0x825AE948, 0x82690F48, &alterIWBackground);

		//Keep order intact
		if(!isContemplatingPrimary) 
			Item_Paint((void *)0x825AE948, 0x82690F48, &alterIWBackground2);

		Item_Paint((void *)0x825AE948, 0x82690F48, &newsTicker);

		if(!isContemplatingPrimary)
		{
			Item_Paint((void *)0x825AE948, 0x82690F48, &headerSpacer);
			Item_Paint((void *)0x825AE948, 0x82690F48, &headerSubText);
			Item_Paint((void *)0x825AE948, 0x82690F48, &headerTitle);

			Item_Paint((void *)0x825AE948, 0x82690F48, &selectorBar);
			Item_Paint((void *)0x825AE948, 0x82690F48, &selectorAButton);

			for(int i = 0; i < 10; i++)
				Item_Paint((void *)0x825AE948, 0x82690F48, &buttons[i]);

			Item_Paint((void *)0x825AE948, 0x82690F48, &buttonHint);
	
			for(int i = 0; i < 15; i++)
				Item_Paint((void *)0x825AE948, 0x82690F48, &menuHintText[i]);
		}
		else if(contemplatingTotalPrimary != -1 && contemplatingCurrentPrimary != -1)
		{
			float primaryPercentage = (float)contemplatingCurrentPrimary / (float)contemplatingTotalPrimary;

			//alterIWPrimaryContemplatorForeground.SetX((primaryPercentage*200)/2 - 100);
			alterIWPrimaryContemplatorForeground.SetW(primaryPercentage*200);
			alterIWPrimaryContemplatorText.text = contemplatorTextPrimary;
			ZeroMemory(alterIWPrimaryContemplatorText.textRect, sizeof(rectDef_s)*4);
			Item_Paint((void *)0x825AE948, 0x82690F48, &alterIWPrimaryContemplatorBackground);
			Item_Paint((void *)0x825AE948, 0x82690F48, &alterIWPrimaryContemplatorForeground);
			Item_Paint((void *)0x825AE948, 0x82690F48, &alterIWPrimaryContemplatorText);

			if(isContemplatingSecondary && contemplatingTotalSecondary != -1 && contemplatingCurrentSecondary != -1)
			{
				float secondaryPercentage = (float)contemplatingCurrentSecondary / (float)contemplatingTotalSecondary;

				//alterIWSecondaryContemplatorForeground.SetX((secondaryPercentage*200)/2 - 100);
				alterIWSecondaryContemplatorForeground.SetW(secondaryPercentage*200);
				alterIWSecondaryContemplatorText.text = contemplatorTextSecondary;
				ZeroMemory(alterIWSecondaryContemplatorText.textRect, sizeof(rectDef_s)*4);
				Item_Paint((void *)0x825AE948, 0x82690F48, &alterIWSecondaryContemplatorBackground);
				Item_Paint((void *)0x825AE948, 0x82690F48, &alterIWSecondaryContemplatorForeground);
				Item_Paint((void *)0x825AE948, 0x82690F48, &alterIWSecondaryContemplatorText);
			}
		}
	}

	renderSaveStub(args, unknown);
}

//NOTE: I had this in the BO2 Tesseract but removed it in favor of a .wad loader.
//	I haven't finished the .wad loader for BO2 cause I can't go online to code it,
//	as such I won't even try to convert this to a .wad loader. 

//	The playlist here is whats called the "playlist update", even containing the playlist update version number
//	The .wad is essentailly a very basic .f file and contains this and other files, mostly .json files.
void Playlist_ParsePlaylistsHook(char * playlistString)
{
	while(usingAssetBuffer) { Sleep(10); }

	usingAssetBuffer = true;
	sprintf(theAssetBuffer, "game:\\dump\\title.41560817\\41560817\\%s", Dvar_GetString("playlistFilename"));
	if(!fileExists(theAssetBuffer))
	{
		CreateDirectoryB(theAssetBuffer);
		FILE * dumpPlaylist;
		if(dumpPlaylist = fopen(theAssetBuffer, "wb"))
		{
			fprintf(dumpPlaylist, playlistString);
			fclose(dumpPlaylist);
		}
	}

	usingAssetBuffer = true;
	sprintf(theAssetBuffer, "game:\\raw\\title.41560817\\41560817\\%s", Dvar_GetString("playlistFilename"));
	if(fileExists(theAssetBuffer))
	{
		HANDLE playlistFile = CreateFile(theAssetBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		int numberOfBytesRead;
		ZeroMemory((void*)0x831805B8, 0x20000);
 		if(playlistFile != INVALID_HANDLE_VALUE && GetFileSize(playlistFile, NULL)+1 < 0x20000 && ReadFile(playlistFile, (void*)0x831805B8, GetFileSize(playlistFile, NULL), (LPDWORD)&numberOfBytesRead, 0))
			CloseHandle(playlistFile);
	}
	usingAssetBuffer = false;

	playlistLoaderSaveStub(playlistString);
}
