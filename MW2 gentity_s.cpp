/****************************************
 ** Note thate all this has been moved **
 * back from my cancelled Tesseract.lib *
 ****************************************/

#include "Modern Warfare 2 TU8.h"
#include "Dvar.h"
#include "Assets.h"
#include "Hooking.h"

bool gentity_s::IsAlive()
{
	return (this && *(int *)((int)this + 0x19C) > 0);
}

bool gclient_s::IsOnGround()
{
	return this && ((ps.eFlags & ( 0x400 | 0x800 )) || ps.groundEntityNum != 0x7FF);
}

float gclient_s::PlayerADS()
{
	return ps.fWeaponPosFrac;
}

typedef unsigned int (__cdecl * BG_GetPerkIndexForName_t)(const char *perkName);
BG_GetPerkIndexForName_t BG_GetPerkIndexForName = (BG_GetPerkIndexForName_t)0x820D1700;
void gclient_s::SetPerk(char * perkName, bool isCodePerk)
{
	if(!isCodePerk)
	{
		unsigned int perkIndex = BG_GetPerkIndexForName(perkName),
			myOffset = (unsigned int)this;

		__asm
		{
			mr        r31, myOffset
			rlwinm    r9, perkIndex, 29,3,29
			clrlwi    r8, perkIndex, 27
			addi      r11, r31, 0x428
			li        r7, 1
			rlwinm    r10, perkIndex, 29,3,29
			slw       r6, r7, r8
			lwzx      r5, r9, r11
			or        r4, r5, r6
			stwx      r4, r9, r11
			addi      r11, r31, 0x32BC
			lwzx      r3, r10, r11
			or        r9, r6, r3
			stwx      r9, r10, r11
		}
	}
}

bool gclient_s::ToggleNoclipMode()
{
	mFlags &= ~2;
	mFlags ^= 1;
	return GetNoclipMode();
}

bool gclient_s::GetNoclipMode()
{
	return mFlags & 1;
}

bool gclient_s::ToggleUFOMode()
{
	mFlags &= ~1;
	mFlags ^= 2;
	return GetUFOMode();
}

bool gclient_s::GetUFOMode()
{
	return mFlags & 2;
}

bool gclient_s::ToggleDisableControls()
{
	mFlags ^= 4;
	return GetDisableControls();
}

bool gclient_s::GetDisableControls()
{
	return mFlags & 4;
}

bool gclient_s::ToggleUseability()
{
	mFlags ^= 8;
	return GetUseability();
}

bool gclient_s::GetUseability()
{
	return mFlags & 8;
}

bool gclient_s::ToggleThermalVision()
{
	ps.otherFlags ^= 8;
	return GetThermalVision();
}

bool gclient_s::GetThermalVision()
{
	return ps.otherFlags & 8;
}

bool gclient_s::ToggleFOFOverlay()
{
	ps.otherFlags ^= 0x10;
	return GetFOFOverlay();
}

bool gclient_s::GetFOFOverlay()
{
	return ps.otherFlags & 0x10;
}

bool gclient_s::ToggleEMPJam()
{
	ps.otherFlags ^= 0x400;
	return GetEMPJam();
}

bool gclient_s::GetEMPJam()
{
	return ps.otherFlags & 0x400;
}

bool gclient_s::ToggleSpectating()
{
	ps.otherFlags ^= 0x800;
	return GetSpectating();
}

bool gclient_s::GetSpectating()
{
	return ps.otherFlags & 0x800;
}

bool gclient_s::MeleeButtonPressed()
{
	return (buttons|oldbuttons) & 4;
}

bool gclient_s::ToggleDisableADS()
{
	ps.weapFlags ^= 0x20;
	return GetDisableADS();
}

bool gclient_s::ADSButtonPressed()
{
	return (buttons|oldbuttons) & 0x800;
}

bool gclient_s::AttackButtonPressed()
{
	return (buttons|oldbuttons) & 1;
}

bool gclient_s::SprintButtonPressed()
{
	return (buttons|oldbuttons) & 2;
}

bool gclient_s::JumpButtonPressed()
{
	return (buttons|oldbuttons) & 0x400;
}

bool gclient_s::CrouchButtonPressed()
{
	return (buttons|oldbuttons) & 0x100;
}

bool gclient_s::StartMenuOpen()
{
	return (buttons|oldbuttons) & 0x400000;
}

bool gclient_s::GetDisableADS()
{
	return ps.weapFlags & 0x20;
}

bool gclient_s::ToggleNightVision()
{
	ps.weapFlags ^= 0x40;
	return GetNightVision();
}

bool gclient_s::GetNightVision()
{
	return ps.weapFlags & 0x40;
}

bool gclient_s::ToggleUsingEquipment()
{
	ps.weapFlags ^= 2;
	return GetUsingEquipment();
}

bool gclient_s::GetUsingEquipment()
{
	return ps.weapFlags & 2;
}

bool gclient_s::ToggleDisableWeaponUseage()
{
	ps.weapFlags ^= 0x80;
	return GetDisableWeaponUseage();
}

bool gclient_s::GetDisableWeaponUseage()
{
	return ps.weapFlags & 0x80;
}

bool gclient_s::ToggleRecoilScale()
{
	ps.weapFlags ^= 0x400;
	return ps.weapFlags & 0x400;
}

int gclient_s::SetRecoilScale(int recoilScale)
{
	if(recoilScale >= 0 && recoilScale <= 100)
	{
		if(recoilScale == 100)
			ps.weapFlags &= ~0x400;
		else
			ps.weapFlags |= 0x400;

		*(int *)((int)(this) + 0x3110) = recoilScale;
	}
	return GetRecoilScale();
}

int gclient_s::GetRecoilScale()
{
	return ps.weapFlags & 0x400 ? *(int *)((int)(this) + 0x3110) : 100;
}

int gclient_s::SetSpreadOverride(int spreadOverride)
{
	if(spreadOverride >= 0 && spreadOverride <= 64)
	{
		ps.spreadOverride = spreadOverride;
		ps.spreadOverrideState = spreadOverride == 64 ? PSOS_RESETTING : PSOS_ENABLED;
	}
	return GetSpreadOverride();
}

int gclient_s::GetSpreadOverride()
{
	return ps.spreadOverrideState == PSOS_ENABLED ? ps.spreadOverride : 64;
}

bool gclient_s::ToggleDisableWeaponSwitching()
{
	ps.weapFlags ^= 0x800;
	return GetDisableWeaponSwitching();
}

bool gclient_s::GetDisableWeaponSwitching()
{
	return ps.weapFlags & 0x800;
}

bool gclient_s::ToggleDisableOffhandWeaponUseage()
{
	ps.weapFlags ^= 0x1000;
	return GetDisableOffhandWeaponUseage();
}

bool gclient_s::GetDisableOffhandWeaponUseage()
{
	return ps.weapFlags & 0x1000;
}

float *gclient_s::SetVelocity(float *input)
{
	ps.velocity[0] = input[0];
	ps.velocity[1] = input[1];
	ps.velocity[2] = input[2];
	return GetVelocity();
}

float *gclient_s::GetVelocity()
{
	return ps.velocity;
}

float gclient_s::GetPlayerViewHeight()
{
	return ps.viewHeightCurrent;
}

bool gclient_s::IsDualWielding()
{
	return ps.isDualWeilding;
}

bool gclient_s::IsReloading()
{
	bool reloading = false;
	switch(ps.weaponStates[0].weaponstate)
	{  
	case WEAPON_RELOADING:
	case WEAPON_RELOADING_INTERUPT:
	case WEAPON_RELOAD_START:
	case WEAPON_RELOAD_START_INTERUPT:
	case WEAPON_RELOAD_END:
		reloading = true;
	}
	switch(ps.weaponStates[1].weaponstate)
	{  
	case WEAPON_RELOADING:
	case WEAPON_RELOADING_INTERUPT:
	case WEAPON_RELOAD_START:
	case WEAPON_RELOAD_START_INTERUPT:
	case WEAPON_RELOAD_END:
		reloading = true;
	}
	return reloading;
}

bool gclient_s::IsSwitchingWeapon()
{
	bool switching = false;
	switch(ps.weaponStates[0].weaponstate)
	{
	case WEAPON_RAISING:
	case WEAPON_RAISING_ALTSWITCH:
	case WEAPON_DROPPING:
	case WEAPON_DROPPING_QUICK:
	case WEAPON_DROPPING_ALTSWITCH:
		switching = true;
	}
	switch(ps.weaponStates[1].weaponstate)
	{
	case WEAPON_RAISING:
	case WEAPON_RAISING_ALTSWITCH:
	case WEAPON_DROPPING:
	case WEAPON_DROPPING_QUICK:
	case WEAPON_DROPPING_ALTSWITCH:
		switching = true;
	}
	return switching;
}

bool gclient_s::FragButtonPressed()
{
	return (buttons|oldbuttons) & 0x4000;
}

bool gclient_s::SecondaryOffhandButtonPressed()
{
	return (buttons|oldbuttons) & 0x8000;
}

bool gclient_s::UseButtonPressed()
{
	return (buttons|oldbuttons) & 0x28;
}
	
typedef unsigned __int64 (__cdecl * Live_GetPlayerXuid_t)(void * session, int clientIndex);
Live_GetPlayerXuid_t Live_GetPlayerXuid = (Live_GetPlayerXuid_t)0x8231DAD8;
unsigned __int64 gclient_s::GetXUID()
{
	return Live_GetPlayerXuid((void*)0x83AC3DB0, sess._cs.clientIndex);
}
	
typedef int (__cdecl * SV_GetGuid_t)(int clientIndex);
SV_GetGuid_t SV_GetGuid = (SV_GetGuid_t)0x82255510;
int gclient_s::GetGUID()
{
	return SV_GetGuid(sess._cs.clientIndex);
}

void gentity_s::GivePlayerGun(char * gunName, char camo, bool dualWieldOverride)
{
	int weaponIndex = G_GetWeaponIndexForName(gunName);
	G_GivePlayerWeapon(&clientData->ps, weaponIndex, camo, dualWieldOverride, 0);
	G_InitializeAmmo(this, weaponIndex, camo, 0);
}

void gentity_s::GivePlayerGun(char * gunName, char camo)
{
	GivePlayerGun(gunName, camo, strstr(gunName, "_akimbo"));
}

void gentity_s::GivePlayerGun(char * gunName)
{
	GivePlayerGun(gunName, 0);
}

gentity_s * gentity_s::GetGEntity(int clientIndex)
{
	return (gentity_s *)(0x82F03600 + (clientIndex*0x280));	
}

gclient_s * gclient_s::GetGClient(int clientIndex)
{
	return gentity_s::GetGEntity(clientIndex)->GetGClient();
}

gclient_s * gentity_s::GetGClient()
{
	return this ? clientData : 0;
}

bool gentity_s::ToggleGodMode()
{
	int d = takedamage;
 
	d &= ~FL_DEMI_GODMODE;
	d ^= FL_GODMODE;

	takedamage = d;
	return GetGodMode();
}

bool gentity_s::GetGodMode()
{
	return takedamage & FL_GODMODE;
}

bool gentity_s::ToggleDemiGodMode()
{
	int d = takedamage;
 
	d &= ~FL_GODMODE;
	d ^= FL_DEMI_GODMODE;
 
	takedamage = d;
	return GetDemiGodMode();
}
	
bool gentity_s::GetDemiGodMode()
{
	return takedamage & FL_DEMI_GODMODE;
}

bool gentity_s::ToggleNoKnockback()
{
	takedamage ^= FL_NO_KNOCKBACK;
	return GetNoKnockback();
}

bool gentity_s::GetNoKnockback()
{
	return takedamage & FL_NO_KNOCKBACK;
}

float *gentity_s::GetCurrentOrigin()
{
	return r.currentOrigin;
}

float *gentity_s::GetCurrentAngles()
{
	return r.currentAngles;
}

//Reversed from IW's GSC version by me. 
gentity_s * gentity_s::AddTestClient(int classIndex, char * teamSelection, char * inputBuffer)
{
	gentity_s * currentTestPlayer = SV_AddTestClient();

	if(currentTestPlayer && currentTestPlayer->clientData->sess.sessionState != SESS_STATE_PLAYING && (Dvar_GetInt("sv_maxclients") > currentTestPlayer->clientData->sess._cs.clientIndex))
	{
		while(currentTestPlayer->clientData->sess._cs.team != TEAM_SPECTATOR || currentTestPlayer->clientData->sess.sessionState != SESS_STATE_PLAYING || !(*(int *)(*(int *)0x83623B98 + (currentTestPlayer->clientData->sess._cs.clientIndex * 0x97F80) + 0x31AFC))) { Sleep(100); };
		sprintf(inputBuffer, "mr %i 3 %s", *(int *)0x8360922C, teamSelection);
		SV_ExecuteClientCommand((void *)(*(int *)0x83623B98 + (currentTestPlayer->clientData->sess._cs.clientIndex * 0x97F80)), inputBuffer, 1, 0);
		while(currentTestPlayer->clientData->sess._cs.team == TEAM_SPECTATOR) { Sleep(100); };
		sprintf(inputBuffer, "mr %i 10 class%i", *(int *)0x8360922C, classIndex);
		SV_ExecuteClientCommand((void *)(*(int *)0x83623B98 + (currentTestPlayer->clientData->sess._cs.clientIndex * 0x97F80)), inputBuffer, 1, 0);
	}

	return currentTestPlayer;
}
	
gentity_s * gentity_s::AddTestClient(int classIndex, char * inputBuffer)
{
	return AddTestClient(classIndex, "autoassign", inputBuffer);
}
	
gentity_s * gentity_s::AddTestClient(char * inputBuffer)
{
	return AddTestClient(0, inputBuffer);
}

//Reversed from IW's original unlock all code from the _dev.gsc
//Converted to C by me
void UnlockEverything(int clientIndex, char * inputBuffer)
{	
	//Do this so the compiler knows to save at least to r30
	__asm { li r30, 0 }

	StringTable * allchallengestable = DB_FindXAssetHeader(stringtable, "mp/allchallengestable.csv").stringTable;
	StructuredDataDef * playerdata = DB_FindXAssetHeader(structureddatadef, "mp/playerdata.def").structuredData;

	//Look up "challengeProgress" and "challengeState" now,
	//    because they will never change and its quicker to
	//    not look them up every single frame.
	int challengeProgress = 0, challengeState = 0;
	for(int i = 0; i < playerdata->structureCount; i++)
	{
		if(challengeProgress && challengeState)
			break;

			for(int k = 0; k < playerdata->internalStructure[i].structures[0].entryCount; k++)
			{
				if(STRUCTURED_DATA_ENUMARR == playerdata->internalStructure[i].structures[0].entries[k].item.type)
				{
					if(!stricmp("challengeProgress", playerdata->internalStructure[i].structures[0].entries[k].name))
						challengeProgress = playerdata->internalStructure[i].structures[0].entries[k].item.offset;
					else if(!stricmp("challengeState", playerdata->internalStructure[i].structures[0].entries[k].name))
						challengeState = playerdata->internalStructure[i].structures[0].entries[k].item.offset;
				}

				if(challengeProgress && challengeState)
					break;
			}
	}

	//Cycle through every challenge
	for(int i = 0; i < allchallengestable->rowCount; i++)
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
		sprintf(inputBuffer, "J %i %08lX", challengeIndex + challengeProgress, _byteswap_ulong(finalTarget));
		SV_GameSendServerCommand(clientIndex, 1, inputBuffer);
		sprintf(inputBuffer, "J %i %02lX", challengeIndex + challengeState, finalTier);
		SV_GameSendServerCommand(clientIndex, 1, inputBuffer);
		Sleep(6);
	}
	SV_GameSendServerCommand(clientIndex, 1, "J 6525 7F");
	SV_GameSendServerCommand(clientIndex, 1, "f \"All challenges are ^2unlocked!\"");
}
