#include "Assets.h"
#include "Hooking.h"
#include <xtl.h>
#include <xboxmath.h>
#include <xbox.h>
#include <xam.h>
#include <stdio.h>
#include "kernel.h"
#include "utility.h"

char * * g_defaultAssetName = (char **)0x823D2560;
int (__cdecl** DB_GetXAssetSizeHandler)() = (int (__cdecl**)())0x823D2100;
void (__cdecl ** DB_XAssetSetNameHandler)(void * header, char * name) = (void (__cdecl**)(void*,char*))0x823D2058;
const char * (__cdecl ** DB_XAssetGetNameHandler)(void * header) = (const char * (__cdecl**)(void*))0x823D1FB0;
char * * g_HitLocNames = (char * *)0x825321F8;				//Used as locDamageMultiplier type in weapon assets, count 20
infoParm_t * infoParms = (infoParm_t *)0x825398D0;			//Used as bounce type in weapon assets, count 64
char ** weapFireType_s = (char**)0x823D0FC4;
char ** weapInventoryType_s = (char **)0x823D0FAC;
char * * PenetrateType_s = (char**)0x823D0FDC;
char * * weaponType_s = (char**)0x823D0F6C;
char * * weaponClass_s = (char **)0x823D0F7C;
char * * VehicleTypes_s = (char **)0x8253C440;

XAssetPool<PhysPreset, 0x40> * g_physPresetPool = (XAssetPool<PhysPreset, 0x40> *)0x82752D10;
XAssetPool<PhysCollMap, 0x400> * g_physCollMap = (XAssetPool<PhysCollMap, 0x400> *)0x82A53818;
XAssetPool<XAnimParts, 0x1000> * g_xAnimPool = (XAssetPool<XAnimParts, 0x1000> *)0x8282ECE0;
XAssetPool<ModelSurface, 0x1000> * g_modelSurfPool = (XAssetPool<ModelSurface, 0x1000> *)0x827C4E08;
XAssetPool<XModel, 0x600> * g_xModelPool = (XAssetPool<XModel, 0x600> *)0x82886CE8;
XAssetPool<Material, 0x1000> * g_materialPool = (XAssetPool<Material, 0x1000> *)0x829FB790;
XAssetPool<MaterialPixelShader, 0x1FA0> * g_pixelShaderPool = (XAssetPool<MaterialPixelShader, 0x1FA0> *)0x828F2CF0;
XAssetPool<MaterialTechniqueSet, 0x300> * g_techSetPool = (XAssetPool<MaterialTechniqueSet, 0x300> *)0x82CFC128;
XAssetPool<GfxImage, 0xE00> * g_imagePool = (XAssetPool<GfxImage, 0xE00> *)0x82999788;
XAssetPool<snd_alias_list_t, 0x3E80> * g_soundPool = (XAssetPool<snd_alias_list_t, 0x3E80> *)0x82A6A330;
XAssetPool<SndCurve, 0x40> * g_SNDCurvePool = (XAssetPool<SndCurve, 0x40> *)0x82CF9F20;
XAssetPool<LoadedSound, 0x546> * g_loadedSoundPool = (XAssetPool<LoadedSound, 0x546> *)0x827FA118;
clipMap_t * g_colMap = (clipMap_t *)0x8312AE80;
ComWorld * g_comMap = (ComWorld *)0x83176B90;
XAssetPool<MapEnts, 2> * g_mapEntsPool = (XAssetPool<MapEnts, 2> * )0x82A53798;
GfxWorld * g_GFXMap = (GfxWorld *)0x83CAC558;
XAssetPool<GfxLightDef, 0x20> * g_lightDefPool = (XAssetPool<GfxLightDef, 0x20> *)0x829126F8;
XAssetPool<Font_s, 0x10> * g_fontPool = (XAssetPool<Font_s, 0x10> *)0x82997600;
XAssetPool<MenuList, 0x80> * g_menuListPool = (XAssetPool<MenuList, 0x80> *)0x82752708;

//mainmenu = 16
XAssetPool<menuDef_t, 0x264> * g_menuDefPool = (XAssetPool<menuDef_t, 0x264> *)0x82754840;
XAssetPool<LocalizeEntry, 0x1B58> * g_LocalizeEntryPool = (XAssetPool<LocalizeEntry, 0x1B58> *)0x82CEC430;
XAssetPool<weaponVarientDef_s, 0x578> * g_weaponPool = (XAssetPool<weaponVarientDef_s, 0x578> *)0x82AB2248;
XAssetPool<SndDriverGlobals, 1> * g_sndDriverGlobalsPool = (XAssetPool<SndDriverGlobals, 1> *)0x82A6A324;			//Check this! Pool size 1 means no pool
XAssetPool<FxEffectDef, 0x258> * g_FXPool = (XAssetPool<FxEffectDef, 0x258> *)0x82A65820;
XAssetPool<FxImpactTable, 4> * g_impactFxPool = (XAssetPool<FxImpactTable, 4> *)0x82A537F4;
XAssetPool<Rawfile, 0x400> * g_rawfilePool = (XAssetPool<Rawfile, 0x400> * )0x82D17408;
XAssetPool<StringTable, 0x190> * g_stringTablePool = (XAssetPool<StringTable, 0x190> *)0x82AAF940;
XAssetPool<LeaderboardDef, 0x64> * g_leaderboardDefPool = (XAssetPool<LeaderboardDef, 0x64> *)0x827E8E10;
XAssetPool<StructuredDataDef, 0x18> * g_structuredDataDefPool = (XAssetPool<StructuredDataDef, 0x18> *)0x82D1B410;
XAssetPool<Tracer, 0x20> * g_tracerPool = (XAssetPool<Tracer, 0x20> *)0x82753818;
XAssetPool<Vehicle, 0x80> * g_vehiclePool = (XAssetPool<Vehicle, 0x80> *)0x82A99138;

BG_GetWeaponDef_t BG_GetWeaponDef = (BG_GetWeaponDef_t)0x820E22C0;
BG_GetWeaponVarientDef_t BG_GetWeaponVarientDef = (BG_GetWeaponVarientDef_t)0x820E22D8;
G_GetWeaponIndexForName_t G_GetWeaponIndexForName = (G_GetWeaponIndexForName_t)0x82210640;
BG_ClipForWeapon_t BG_ClipForWeapon = (BG_ClipForWeapon_t)0x820E13A0;
G_InitializeAmmo_t G_InitializeAmmo = (G_InitializeAmmo_t)0x821D4A00;

const int g_poolSize[0x29]= {
		0x40,
		0x400,
		0x1000,
		0x1000,
		0x600,
		0x1000,
		0x1FA0,
		0x300,
		0xE00,
		0x3E80,
		0x40,
		0x546,
		1,
		1,
		1,
		1,
		1,
		2,
		1,
		1,
		0x20,
		0,
		0x10,
		0x80,
		0x264,
		0x1B58,
		0x578,
		1,
		0x258,
		4,
		0,
		0,
		0,
		0,
		0x400,
		0x190,
		0x64,
		0x18,
		0x20,
		0x80,
		1
	};

//TODO: These are identical except for the FindXAssetHeader call, mergre them!
bool WriteAssetEntry(const AssetEntry * currentEntries, char * tokenName, char * tokenValue, void * assetHeader, const char * parentName, bool parentReversed)
{
	const AssetEntry * thisEntry = 0;

	bool hasFoundEntry = false;
	if(currentEntries && tokenName && assetHeader)
	for(int i = 0; currentEntries[i].name; i++)
	{
		if(currentEntries[i].type == ASSET_ENTRY_EMBEDDED)
			hasFoundEntry = WriteAssetEntry(currentEntries[i].next, tokenName, tokenValue, (void*)((unsigned int)assetHeader + currentEntries[i].offset), currentEntries[i].name, false);
		else if(currentEntries[i].type == ASSET_ENTRY_EMBEDDED_REVERSED)
			hasFoundEntry = WriteAssetEntry(currentEntries[i].next, tokenName, tokenValue, (void*)((unsigned int)assetHeader + currentEntries[i].offset), currentEntries[i].name, true);
		else if(currentEntries[i].type != ASSET_ENTRY_CONTAINED)
		{
			if((!parentReversed && *parentName && !strnicmp(parentName, tokenName, strlen(parentName)) && !stricmp(currentEntries[i].name, tokenName + strlen(parentName))) ||
				(*parentName && !strnicmp(currentEntries[i].name, tokenName, strlen(currentEntries[i].name)) && !stricmp(parentName, tokenName + strlen(currentEntries[i].name))) ||
				!stricmp(currentEntries[i].name, tokenName))
			{
				thisEntry = &currentEntries[i];
				break;
			}
		}
		else if(*(void**)((unsigned int)assetHeader + currentEntries[i].offset))
			hasFoundEntry = WriteAssetEntry(currentEntries[i].next, tokenName, tokenValue, *(void**)((unsigned int)assetHeader + currentEntries[i].offset), currentEntries[i].name, (currentEntries[i].type == ASSET_ENTRY_CONTAINED_REVERSED));

		if(hasFoundEntry) break;
	}

	int sizeToCopy = 0, possibleIntValue = 0;
	short possibleShortValue = 0;
	float possibleFloatValue = 0.f;
	byte possibleByteValue = 0;
	void * assetPointerToWrite = 0;
	char * possibleStringValue = 0;
	void * dataToWrite = 0;

	if(!hasFoundEntry && thisEntry && tokenValue)
	{

	switch(thisEntry->type)
	{
		case ASSET_ENTRY_BYTE:
		case ASSET_ENTRY_BOOL:
			sizeToCopy = 1;
			possibleByteValue = (byte)atoi(tokenValue);
			dataToWrite = &possibleByteValue;
			break;
		case ASSET_ENTRY_SHORT:
		case ASSET_ENTRY_UNSIGNED_SHORT:
			sizeToCopy = 2;
			possibleShortValue = (short)atoi(tokenValue);
			dataToWrite = &possibleShortValue;
			break;
		case ASSET_ENTRY_BOOL_AS_INT:
			possibleIntValue = (atoi(tokenValue) != 0);
			goto COPY_INT;
		case ASSET_ENTRY_INT:
			possibleIntValue = atoi(tokenValue);
			goto COPY_INT;
		case ASSET_ENTRY_UNSIGNED_INT:
			possibleIntValue = strtoul(tokenValue, NULL, 10);
			goto COPY_INT;
		case ASSET_ENTRY_TIME_AS_INT:
			possibleIntValue = (int)(atof(tokenValue) * 1000);
			goto COPY_INT;
		case ASSET_ENTRY_FLOAT:
			possibleFloatValue = atof(tokenValue);
			dataToWrite= &possibleFloatValue;
			goto COPY_4;
		case ASSET_ENTRY_STRING:
			if(!(*(unsigned int*)((unsigned int)assetHeader + thisEntry->offset)) || strcmpi(tokenValue, *(char**)((unsigned int)assetHeader + thisEntry->offset)))
			{
				if(!*tokenValue)
					possibleStringValue = 0;
				else
				{
					sizeToCopy = strlen(tokenValue);
					possibleStringValue = new char[sizeToCopy + 1];
					ZeroMemory(possibleStringValue, sizeToCopy + 1);
					strcpy(possibleStringValue, tokenValue);
				}
				dataToWrite = &possibleStringValue;
				goto COPY_4;
			}
			break;
		case ASSET_ENTRY_ASSET_PTR:
			assetPointerToWrite = *tokenValue ? 
				DB_FindXAssetHeader((XAssetType)((int)(thisEntry->next)), tokenValue).data
				: 0;
			dataToWrite = &assetPointerToWrite;
			goto COPY_4;
		case ASSET_ENTRY_ENUM:
			for(int i = 0; *(char**)((int)(thisEntry->next) + (4*i)) && i < thisEntry->enumCount; i++)
			if(!strcmpi(*(char**)((int)(thisEntry->next) + (4*i)), tokenValue))
			{
				possibleIntValue = i;
				goto COPY_INT;
			}
		COPY_INT:
			dataToWrite = &possibleIntValue;
			goto COPY_4;
		COPY_4:
			sizeToCopy = 4;
			break;
		default: break;
	}

	if(dataToWrite)
	{
		WriteMemory((void*)(thisEntry->offset + (unsigned int)assetHeader), sizeToCopy, dataToWrite);
		hasFoundEntry = true;
	}
	}

	return hasFoundEntry;
}

bool DumpAssetEntry(FILE * writeLog, AssetEntryType currentEntryType, const char * currentEntryName, void * assetData, const AssetEntry * containedItems, int enumCount, const char * parentName)
{
	if(!writeLog || !currentEntryName || !assetData)
		return false;

	//printf("Writing: %s%s\n", parentName, currentEntryName);

	if(currentEntryType != ASSET_ENTRY_CONTAINED && currentEntryType != ASSET_ENTRY_EMBEDDED &&
		currentEntryType != ASSET_ENTRY_CONTAINED_REVERSED && currentEntryType != ASSET_ENTRY_EMBEDDED_REVERSED)
		fprintf(writeLog, "\\%s%s", parentName, currentEntryName);

	switch(currentEntryType)
	{
		case ASSET_ENTRY_BYTE:
		case ASSET_ENTRY_BOOL:
			fprintf(writeLog, "\\%i", *(byte*)assetData);
			break;
		case ASSET_ENTRY_SHORT:
			fprintf(writeLog, "\\%hd", *(short*)assetData);
			break;
		case ASSET_ENTRY_UNSIGNED_SHORT:
			fprintf(writeLog, "\\%hu", *(unsigned short*)assetData);
			break;
		case ASSET_ENTRY_BOOL_AS_INT:
			fprintf(writeLog, "\\%i", *(int*)assetData != 0);
			break;
		case ASSET_ENTRY_INT:
			fprintf(writeLog, "\\%i", *(int*)assetData);
			break;
		case ASSET_ENTRY_UNSIGNED_INT:
			fprintf(writeLog, "\\%u", *(unsigned int*)assetData);
			break;
		case ASSET_ENTRY_TIME_AS_INT:
			fprintf(writeLog, "\\%g", ((float)*(unsigned int*)assetData) / 1000);
			break;
		case ASSET_ENTRY_FLOAT:
			fprintf(writeLog, "\\%g", *(float*)assetData);
			break;
		case ASSET_ENTRY_STRING:
			fprintf(writeLog, "\\%s", *(char**)assetData ? *(char**)assetData : "");
			break;
		case ASSET_ENTRY_ASSET_PTR:
			fprintf(writeLog, "\\%s", *(char***)assetData ? (**(char***)assetData) : "");
			break;
		case ASSET_ENTRY_ENUM:
			fprintf(writeLog, "\\%s", *(int*)assetData < enumCount ? *(char**)((int)(containedItems) + (4*(*(int*)assetData))) : "");
			break;
		case ASSET_ENTRY_EMBEDDED:
			if(containedItems)
			for(int i = 0; containedItems[i].name; i++)
				DumpAssetEntry(writeLog, containedItems[i].type, containedItems[i].name, (void*)((int)assetData + containedItems[i].offset), containedItems[i].next, containedItems[i].enumCount, currentEntryName);
			break;
		case ASSET_ENTRY_EMBEDDED_REVERSED:
			if(containedItems)
			for(int i = 0; containedItems[i].name; i++)
				DumpAssetEntry(writeLog, containedItems[i].type, currentEntryName, (void*)((int)assetData + containedItems[i].offset), containedItems[i].next, containedItems[i].enumCount, containedItems[i].name);
			break;
		case ASSET_ENTRY_CONTAINED_REVERSED:
			if(containedItems && *(int*)assetData)
			for(int i = 0; containedItems[i].name; i++)
				DumpAssetEntry(writeLog, containedItems[i].type, currentEntryName, (void*)(*(int*)assetData + containedItems[i].offset), containedItems[i].next, containedItems[i].enumCount, containedItems[i].name);
			break;
		case ASSET_ENTRY_CONTAINED:
			if(containedItems && *(int*)assetData)
			for(int i = 0; containedItems[i].name; i++)
				DumpAssetEntry(writeLog, containedItems[i].type, containedItems[i].name, (void*)(*(int*)assetData + containedItems[i].offset), containedItems[i].next, containedItems[i].enumCount, currentEntryName);
		default: 
			break;
	}

	return true;
}

#pragma region physpreset (0x0)
const AssetEntry PhysPresetEntries[] = {
	{"mass",					offsetof(PhysPreset, mass),						ASSET_ENTRY_FLOAT,0,0},
	{"bounce",					offsetof(PhysPreset, bounce),					ASSET_ENTRY_FLOAT,0,0},
	{"friction",				offsetof(PhysPreset, friction),					ASSET_ENTRY_FLOAT,0,0},
	{"bulletForceScale",		offsetof(PhysPreset, bulletForceScale),			ASSET_ENTRY_FLOAT,0,0},
	{"explosiveForceScale",		offsetof(PhysPreset, explosiveForceScale),		ASSET_ENTRY_FLOAT,0,0},
	{"sndAliasPrefix",			offsetof(PhysPreset, sndAliasPrefix),			ASSET_ENTRY_STRING,0,0},
	{"piecesSpreadFraction",	offsetof(PhysPreset, piecesSpreadFraction),		ASSET_ENTRY_FLOAT,0,0},
	{"piecesUpwardVelocity",	offsetof(PhysPreset, piecesUpwardVelocity),		ASSET_ENTRY_FLOAT,0,0},
	{"tempDefaultToCylinder",	offsetof(PhysPreset, tempDefaultToCylinder),	ASSET_ENTRY_BOOL,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

int PhysPreset::dumpPhysPreset()
{
	FILE * writeLog;
	sprintf(MasterCharBuffer, "game:\\dump\\physic\\%s", (char*)name);
	CreateDirectoryB(MasterCharBuffer);

	if(fileExists(MasterCharBuffer))
		return ERROR_DUP_NAME;

	writeLog = fopen(MasterCharBuffer, "w");

	if(!writeLog)
		return ERROR_BAD_NETPATH;

	fprintf(writeLog, "PHYSIC\\isFrictionInfinity\\%i", friction >= 340282346638528860000000000000000000000.f);

	for(int i = 0; PhysPresetEntries[i].name; i++)
	if(PhysPresetEntries[i].offset == 0x10 && friction >= 340282346638528860000000000000000000000.f)
		fprintf(writeLog,"\\friction\\0.5");
	else
		DumpAssetEntry(writeLog, PhysPresetEntries[i].type, PhysPresetEntries[i].name, (void*)((int)(this) + PhysPresetEntries[i].offset));

	fclose(writeLog);

	return ERROR_SUCCESS;
}

void PhysPreset::dumpAllPPs()
{
	int assetCount = 0;
	for(int i = 0; i < 0x40 &&
		g_physPresetPool->freeHead != &g_physPresetPool->entries[i].entry &&
		g_physPresetPool->entries[i].next != &g_physPresetPool->entries[i+1].entry; i++)
		if(g_physPresetPool->entries[i].entry.dumpPhysPreset() != ERROR_SUCCESS)
			printf("Failed to dump %s\n", g_physPresetPool->entries[i].entry.name);
		else
			assetCount++;

	printf("Dumped %i physpresets\n", assetCount);
}

void Load_PhysPresetAssetHook(void ** input)
{
	PhysPreset * currentPhysPreset = *(PhysPreset**)input;

	//Can we load this physpreset custom?
	while(usingAssetBuffer) Sleep(10);
	
	usingAssetBuffer = true;
	sprintf(theAssetBuffer, "game:\\raw\\physic\\%s", (char*)currentPhysPreset->name);
	if(fileExists(theAssetBuffer))
	{
		HANDLE physicsFile = CreateFile(theAssetBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		int numberOfBytesRead;
		if(physicsFile != INVALID_HANDLE_VALUE && ReadFile(physicsFile, theAssetBuffer, GetFileSize(physicsFile, NULL), (LPDWORD)&numberOfBytesRead, 0))
		{
			CloseHandle(physicsFile);
			char * currentToken = improved_strtok(theAssetBuffer,"\\");
			if(!strcmp(currentToken, "PHYSIC"))
			{
				bool isFrictionInfinity = false;
				float readFriction = FLT_MIN;
				for(currentToken = improved_strtok(NULL,"\\"); currentToken; currentToken = improved_strtok(NULL,"\\"))
				{
					char * currentName = currentToken,
						* currentValue = improved_strtok(NULL, "\\");

					if(!strcmpi(currentName, "isFrictionInfinity"))
					{
						isFrictionInfinity = atoi(currentValue) == 1;
						if(isFrictionInfinity)
							currentPhysPreset->friction = 340282346638528860000000000000000000000.f;
						else if(readFriction != FLT_MIN)
							currentPhysPreset->friction = readFriction;
					}
					else if(!strcmpi(currentName, "friction"))
					{
						if(!isFrictionInfinity)
							currentPhysPreset->friction = readFriction = atof(currentValue);
					}
					else if(!WriteAssetEntry(PhysPresetEntries, currentName, currentValue, currentPhysPreset)) 
						printf("Couldn't recognize '%s' setting with '%s' value in 0x%X type.\n", currentName, currentValue, physpreset);
				}
			}
			else
				printf("PhysPreset source file not preceeded by PHYSIC. I'm throwing it away\n");
		}
	}
	usingAssetBuffer = false;

	physPresetLoaderSaveStub(input);
}
#pragma endregion

#pragma region modelsurfs (0x3)
//This function dumps to the old .obj format. .obj sucks. .fbx is much better. Be sure to use the .fbx dumper found below. 
/*int ModelSurface::dumpSurface(FILE * writeLog, int verticiesLastSurface, float * offset, char * objectName)
{
	for(int j = 0; j < xSurficiesCount; j++)
	{
		fwrite("g default\n",strlen("g default\n"),1,writeLog);
		int verticiesThisSurface = 0;
		for(int k = 0; k < xSurficies[j].vertCount; k++)
		{
			sprintf(MasterCharBuffer, "v %g %g %g\n", xSurficies[j].packedVerticies[k].xyz[0] + (!offset ? 0.f : offset[0]),
				xSurficies[j].packedVerticies[k].xyz[1] + (!offset ? 0.f : offset[1]),
				xSurficies[j].packedVerticies[k].xyz[2] + (!offset ? 0.f : offset[2]));

			fwrite(MasterCharBuffer,strlen(MasterCharBuffer),1,writeLog);
			verticiesThisSurface++;
		}

		for(int k = 0; k < xSurficies[j].vertCount; k++)
		{
			unsigned short hexValue1 = (xSurficies[j].packedVerticies[k].texCoord.packed >> 16) & 0xFFFF,
				hexValue2 = xSurficies[j].packedVerticies[k].texCoord.packed & 0xFFFF;

			sprintf(MasterCharBuffer, "vt %g %g\n", UnpackShortFloat(hexValue1), UnpackShortFloat(hexValue2));
			fwrite(MasterCharBuffer,strlen(MasterCharBuffer),1,writeLog);
		}
				
		sprintf(MasterCharBuffer, "s off\ng %s_%i\n", !objectName ? "object" : objectName, j);
		fwrite(MasterCharBuffer,strlen(MasterCharBuffer),1,writeLog);
		for(int k = 0; k < xSurficies[j].triCount; k++)
		{
			unsigned int firstIndicie = xSurficies[j].triIndices[k * 3] + 1 + verticiesLastSurface,
				secondIndicie = xSurficies[j].triIndices[k * 3 + 1] + 1 + verticiesLastSurface,
				thirdIndicie = xSurficies[j].triIndices[k * 3 + 2] + 1 + verticiesLastSurface;

			if(firstIndicie == secondIndicie ||
				firstIndicie == thirdIndicie ||
				secondIndicie == thirdIndicie)
				continue;

			sprintf(MasterCharBuffer, "f %i/%i/ %i/%i/ %i/%i/\n", firstIndicie, firstIndicie,
				secondIndicie, secondIndicie,
				thirdIndicie, thirdIndicie);
						
			fwrite(MasterCharBuffer,strlen(MasterCharBuffer),1,writeLog);
		}

		verticiesLastSurface += verticiesThisSurface;
	}

	return verticiesLastSurface;
}*/

int ModelSurface::dumpSurface(FILE * writeLog, int verticiesLastSurface, float * offset, char * objectName)
{
	fprintf(writeLog, "Objects: {\n");
	for(int j = 0; j < xSurficiesCount; j++)
	{
		fprintf(writeLog, "\tGeometry: %i, \"Geometry::\", \"Mesh\" {\n\t\tVertices: *%i {\n\t\t\ta: ", 859843040 + j, xSurficies[j].vertCount*3);
		int verticiesThisSurface = 0;
		for(int k = 0; k < xSurficies[j].vertCount; k++)
		{
			fprintf(writeLog, "%g,%g,%g%s", xSurficies[j].packedVerticies[k].origin[0] + (!offset ? 0.f : offset[0]),
				xSurficies[j].packedVerticies[k].origin[1] + (!offset ? 0.f : offset[1]),
				xSurficies[j].packedVerticies[k].origin[2] + (!offset ? 0.f : offset[2]),
				(k < (xSurficies[j].vertCount - 1)) ? "," : "\n");
		}
		int invalidTriCount = 0;
		for(int k = 0; k < xSurficies[j].triCount; k++)
		{
			int firstIndicie = xSurficies[j].triIndices[k][0],
				secondIndicie = xSurficies[j].triIndices[k][1],
				thirdIndicie = xSurficies[j].triIndices[k][2];

			if(firstIndicie == secondIndicie ||
				firstIndicie == thirdIndicie ||
				secondIndicie == thirdIndicie)
				invalidTriCount++;
		}

		fprintf(writeLog, "\t\t}\n\t\tPolygonVertexIndex: *%i {\n\t\t\ta: ", (xSurficies[j].triCount*3) - (invalidTriCount*3));

		for(int k = 0; k < xSurficies[j].triCount; k++)
		{
			int firstIndicie = xSurficies[j].triIndices[k][0],
				secondIndicie = xSurficies[j].triIndices[k][1],
				thirdIndicie = -(xSurficies[j].triIndices[k][2]) - 1;

			if(firstIndicie == secondIndicie ||
				firstIndicie == xSurficies[j].triIndices[k][2] ||
				secondIndicie == xSurficies[j].triIndices[k][2])
				continue;
			else if(k != 0)
				fprintf(writeLog,",");

			fprintf(writeLog, "%i,%i,%i", firstIndicie,
				secondIndicie,
				thirdIndicie);
		}

		fprintf(writeLog, "\n\t\t}\n\t}\n\tModel: %i, \"Model::%s_%i\", \"Mesh\" {\n\t\tVersion: 232\n\t\tProperties70: {\n\t\t\tP: \"PreRotation\", \"Vector3D\", \"Vector\", \"\",-90,0,0\n\t\t\tP: \"RotationActive\", \"bool\", \"\", \"\",1\n\t\t\tP: \"InheritType\", \"enum\", \"\", \"\",1\n\t\t\tP: \"DefaultAttributeIndex\", \"int\", \"Integer\", \"\",0\n\t\t}\n\t}\n", 863543520 + j, !objectName ? "object" : objectName, j);
	}

	fprintf(writeLog, "}\n\nConnections:\n{\n");

	for(int j = 0; j < xSurficiesCount; j++)
		fprintf(writeLog, "\tC: \"OO\",%i,0\n\tC: \"OO\",%i,%i\n", 863543520+j, 859843040+j, 863543520+j);

	fprintf(writeLog,"}");

	return verticiesLastSurface;
}

char * FBXHeader = 
"; FBX 7.2.0 project file\n"
"; Copyright (C) 1997-2010 Autodesk Inc. and/or its licensors.\n"
"; All rights reserved.\n"
"; ----------------------------------------------------\n"
"\n"
"FBXHeaderExtension: {\n"
"\tFBXHeaderVersion: 1003\n"
"\tFBXVersion: 7200\n"
"}\n"
"\n"
"Definitions: {\n"
"\tVersion: 100\n"
"\tCount: 2\n"
"\tObjectType: \"Geometry\" {\n"
"\t\tCount: 1\n"
"\t\tPropertyTemplate: \"KFbxMesh\" {\n"
"\t\t\tProperties70: {\n"
"\t\t\t\tP: \"Color\", \"ColorRGB\", \"Color\", \"\",0.8,0.8,0.8\n"
"\t\t\t}\n"
"\t\t}\n"
"\t}\n"
"\tObjectType: \"Model\" {\n"
"\t\tCount: 1\n"
"\t\tPropertyTemplate: \"KFbxNode\" {\n"
"\t\t\tProperties70: {\n"
"\t\t\t\tP: \"AxisLen\", \"double\", \"Number\", \"\",10\n"
"\t\t\t\tP: \"Show\", \"bool\", \"\", \"\",1\n"
"\t\t\t\tP: \"NegativePercentShapeSupport\", \"bool\", \"\", \"\",1\n"
"\t\t\t\tP: \"DefaultAttributeIndex\", \"int\", \"Integer\", \"\",-1\n"
"\t\t\t\tP: \"Visibility\", \"Visibility\", \"\", \"A\",1\n"
"\t\t\t\tP: \"Visibility Inheritance\", \"Visibility Inheritance\", \"\", \"\",1\n"
"\t\t\t}\n"
"\t\t}\n"
"\t}\n"
"}\n"
"\n";

void ModelSurface::dumpAllSurfaces()
{
	contemplatingTotalPrimary = ((unsigned int)g_modelSurfPool->freeHead - (unsigned int)g_modelSurfPool)/sizeof(ModelSurface) + 1;
	contemplatingCurrentPrimary = 0;
	strcpy(contemplatorTextPrimary, "Dumping Model Surfaces");
	isContemplatingPrimary = true;
	int assetCount = 0;
	for(int i = 0; i < 0x1000 &&
		g_modelSurfPool->freeHead != &g_modelSurfPool->entries[i].entry &&
		g_modelSurfPool->entries[i].next != &g_modelSurfPool->entries[i+1].entry; i++, contemplatingCurrentPrimary++)
	{
		if(strlen(g_modelSurfPool->entries[i].entry.name) > 28)
			printf("Model name %s is too long, trimming it to %.28s\n", g_modelSurfPool->entries[i].entry.name, g_modelSurfPool->entries[i].entry.name);

		sprintf(MasterCharBuffer, "game:\\dump\\modelsurfs\\%.28s.fbx", g_modelSurfPool->entries[i].entry.name);
		CreateDirectoryB(MasterCharBuffer);

		if(fileExists(MasterCharBuffer))
		{
			printf("%.28s already exists, can't dump...\n", g_modelSurfPool->entries[i].entry.name);
			continue;
		}

		FILE * writeLog = fopen(MasterCharBuffer, "w");
		if(writeLog)
		{
			fprintf(writeLog, FBXHeader);
			g_modelSurfPool->entries[i].entry.dumpSurface(writeLog, 0, 0, g_modelSurfPool->entries[i].entry.name);
			fclose(writeLog);
			assetCount++;
		}
	}
	isContemplatingPrimary = false;
	contemplatingTotalPrimary = contemplatingCurrentPrimary = -1;

	printf("Dumped %i modelsurfs\n", assetCount);
}
#pragma endregion

#pragma region image (0x8)
#define DDSCAPS2_CUBEMAP			0x200
#define DDSCAPS2_CUBEMAP_POSITIVEX	0x400
#define DDSCAPS2_CUBEMAP_NEGATIVEX	0x800
#define DDSCAPS2_CUBEMAP_POSITIVEY	0x1000
#define DDSCAPS2_CUBEMAP_NEGATIVEY	0x2000
#define DDSCAPS2_CUBEMAP_POSITIVEZ	0x4000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ	0x8000
#define DDSCAPS2_VOLUME				0x200000

typedef void * (__cdecl * sub_8219C730_t)(GfxImage * asset, bool);
extern sub_8219C730_t sub_8219C730 = (sub_8219C730_t)0x8219C730;

#include <xgraphics.h>
#include <d3dx9.h>
//WARNING: When dumping streamed images, I haven't figured out a way to deallocate the streamed images. Therefor, the game will crash. 
//	You can dump all the images by just restarting the game without clearing out the dump/images/ folder. Ones that have been dumped before
//  will not be dumped again (because they don't need to be). It may take a few runs, but eventually you will have ALL images and the game won't crash.
int GfxImage::dumpImage(bool dumpStreamedImage)
{
	if(!dumpStreamedImage && streaming) {
		printf("Couldn't dump image %s because its being streamed from a .pak\n", name);
		return ERROR_CALL_NOT_IMPLEMENTED;
	}
	
	if(texture.basemap.Format.Type != GPUCONSTANTTYPE_TEXTURE) {
		printf("Couldn't dump image %s because its type is %i\n", name, texture.basemap.Format.Type);
		return ERROR_CALL_NOT_IMPLEMENTED;
	}

	//Types needing implementation: 
	if(texture.basemap.Format.DataFormat != GPUTEXTUREFORMAT_DXT1 && 
		texture.basemap.Format.DataFormat != GPUTEXTUREFORMAT_DXT2_3 && 
		texture.basemap.Format.DataFormat != GPUTEXTUREFORMAT_DXT4_5 &&
		texture.basemap.Format.DataFormat != GPUTEXTUREFORMAT_DXN &&
		texture.basemap.Format.DataFormat != GPUTEXTUREFORMAT_8_8_8_8 &&
		texture.basemap.Format.DataFormat != GPUTEXTUREFORMAT_8_8 &&
		texture.basemap.Format.DataFormat != GPUTEXTUREFORMAT_8) {
		printf("Couldn't dump image %s because its format value is %i\n", name, texture.basemap.Format.DataFormat);
		return ERROR_CALL_NOT_IMPLEMENTED;
	}

	//Types needing implementation: GPUDIMENSION_CUBEMAP
	if(texture.basemap.Format.Dimension != GPUDIMENSION_2D &&
		texture.basemap.Format.Dimension != GPUDIMENSION_CUBEMAP) {
		printf("Couldn't dump image %s because its dimension value is %i\n", name, texture.basemap.Format.Dimension);
		return ERROR_CALL_NOT_IMPLEMENTED;
	}

	sprintf(MasterCharBuffer, "game:\\dump\\images\\%s.dds", name);
	CreateDirectoryB(MasterCharBuffer);

	if(fileExists(MasterCharBuffer))
		return ERROR_DUP_NAME;

	int vWidth = width, vHeight = height, padAmount = 128;
	//	while(padAmount < vWidth && padAmount < vHeight) padAmount <<= 1;
		//if(Levels > 1) padAmount <<= 1;
	if(vWidth % padAmount != 0) vWidth += (padAmount - vWidth % padAmount);
	if(vHeight % padAmount != 0) vHeight += (padAmount - vHeight % padAmount);

	char * outputBuff;
	if(cardMemory.platform[0] <= sizeof(MasterCharBuffer))
		outputBuff = MasterCharBuffer;
	else
		outputBuff = (char*)malloc(cardMemory.platform[0]);
	if(outputBuff) {
		FILE * writeLog = fopen(MasterCharBuffer, "wb");
		ZeroMemory(outputBuff, cardMemory.platform[0]);

		if(!writeLog) {
			printf("Couldn't dump image %s because its name contains characters that are incompatible with FATX\n", name);
			if(outputBuff != MasterCharBuffer)
				free(outputBuff);
			return ERROR_BAD_NETPATH;
		}

		int dwMagic = 0x44445320;
		fwrite(&dwMagic, 4, 1, writeLog);
		int dwSize = _byteswap_ulong(124);
		fwrite(&dwSize, 4, 1, writeLog);
        int dwFlags = (DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT);
		int dwPitchOrLinearSize;
		
		XGTEXTURE_DESC SourceDesc;
		XGGetTextureDesc( &texture.basemap, 0, &SourceDesc );
		if(texture.basemap.Format.DataFormat == GPUTEXTUREFORMAT_8_8_8_8 ||
			texture.basemap.Format.DataFormat == GPUTEXTUREFORMAT_8_8 ||
			texture.basemap.Format.DataFormat == GPUTEXTUREFORMAT_8)
		{
			dwFlags |= DDSD_PITCH;
			dwPitchOrLinearSize = _byteswap_ulong(width * SourceDesc.BitsPerPixel / 8);
		}
		else
		{
			dwFlags |= DDSD_LINEARSIZE;
			dwPitchOrLinearSize = _byteswap_ulong(cardMemory.platform[0]);
		}
		int Levels = mipmap;
		int dwMipMapCount = 0;
		if(Levels > 1)
		{
			dwMipMapCount = Levels;
			dwFlags |= DDSD_MIPMAPCOUNT;
		}
		
		dwFlags = _byteswap_ulong(dwFlags);
		fwrite(&dwFlags, 4, 1, writeLog);
		int dwHeight = _byteswap_ulong(height);
		int dwWidth = _byteswap_ulong(width);
		fwrite(&dwHeight, 4, 1, writeLog);
		fwrite(&dwWidth, 4, 1, writeLog);
		fwrite(&dwPitchOrLinearSize, 4, 1, writeLog);
		int nullOut = 0;
		fwrite(&nullOut, 4, 1, writeLog);
		dwMipMapCount = _byteswap_ulong(dwMipMapCount);
		fwrite(&dwMipMapCount, 4, 1, writeLog);
		for(int i = 0; i < 11; i++)
			fwrite(&nullOut, 4, 1, writeLog);
		int dwSize2 = _byteswap_ulong(32);
		fwrite(&dwSize2, 4, 1, writeLog);
		int dwFlags2 = 0, dwFourCC = 0;
		switch(texture.basemap.Format.DataFormat) {
		case GPUTEXTUREFORMAT_DXT1: dwFourCC = 0x31545844; dwFlags2 |= 4; break;
		case GPUTEXTUREFORMAT_DXT2_3: dwFourCC = 0x33545844; dwFlags2 |= 4; break;
		case GPUTEXTUREFORMAT_DXT5A:
		case GPUTEXTUREFORMAT_DXT4_5: dwFourCC = 0x35545844; dwFlags2 |= 4; break;
		case GPUTEXTUREFORMAT_DXN: dwFourCC = 0x35545844; dwFlags2 |= 4; break;
		case GPUTEXTUREFORMAT_8: dwFlags2 |= 0x2; break;
		case GPUTEXTUREFORMAT_8_8: dwFlags2 |= 0x20001; break;
		case GPUTEXTUREFORMAT_8_8_8_8: dwFlags2 |= 0x41; break;
		default: break;
		}
		
        int dwCaps1 = 0,
				dwCaps2 = 0;
        dwCaps1 |= 0x00001000;
		if(Levels > 1)
			dwCaps1 |= 0x00400008;
		if(texture.basemap.Format.Dimension == GPUDIMENSION_CUBEMAP) {
			dwCaps1 |= 2;
			dwCaps2 |= (DDSCAPS2_CUBEMAP|DDSCAPS2_CUBEMAP_POSITIVEX|DDSCAPS2_CUBEMAP_NEGATIVEX|DDSCAPS2_CUBEMAP_POSITIVEY|DDSCAPS2_CUBEMAP_NEGATIVEY|DDSCAPS2_CUBEMAP_POSITIVEZ|DDSCAPS2_CUBEMAP_NEGATIVEZ);
		}
		dwFlags2 = _byteswap_ulong(dwFlags2);
		fwrite(&dwFlags2, 4, 1, writeLog);
		dwFourCC = _byteswap_ulong(dwFourCC);
		fwrite(&dwFourCC, 4, 1, writeLog);
		int dwRGBBitCount, dwRBitMask, dwGBitMask, dwBBitMask, dwRGBAlphaBitMask;
		if (texture.basemap.Format.DataFormat == GPUTEXTUREFORMAT_8_8_8_8)
		{
			dwRGBBitCount = _byteswap_ulong(SourceDesc.BitsPerPixel);
			dwRBitMask = _byteswap_ulong(0x00ff0000);
			dwGBitMask = _byteswap_ulong(0x0000ff00);
			dwBBitMask = _byteswap_ulong(0x000000ff);
			dwRGBAlphaBitMask = _byteswap_ulong(0xff000000);
		}
		else if (texture.basemap.Format.DataFormat == GPUTEXTUREFORMAT_8_8)
		{
			dwRGBBitCount = _byteswap_ulong(SourceDesc.BitsPerPixel);
			dwRBitMask = _byteswap_ulong(0x00ff);
			dwGBitMask = 0x00000000;
			dwBBitMask = 0x00000000;
			dwRGBAlphaBitMask = _byteswap_ulong(0xff00);
		}
		else if (texture.basemap.Format.DataFormat == GPUTEXTUREFORMAT_8)
		{
			dwRGBBitCount = _byteswap_ulong(SourceDesc.BitsPerPixel);
			dwRBitMask = 0x00000000;
			dwGBitMask = 0x00000000;
			dwBBitMask = 0x00000000;
			dwRGBAlphaBitMask = _byteswap_ulong(0xff);
		}
		else
		{
			dwRGBBitCount = 0x00000000;
			dwRBitMask = 0x00000000;
			dwGBitMask = 0x00000000;
			dwBBitMask = 0x00000000;
			dwRGBAlphaBitMask = 0x00000000;
		}
		fwrite(&dwRGBBitCount, 4, 1, writeLog);
		fwrite(&dwRBitMask, 4, 1, writeLog);
		fwrite(&dwGBitMask, 4, 1, writeLog);
		fwrite(&dwBBitMask, 4, 1, writeLog);
		fwrite(&dwRGBAlphaBitMask, 4, 1, writeLog);

		dwCaps1 = _byteswap_ulong(dwCaps1);
		fwrite(&dwCaps1, 4, 1, writeLog);
		dwCaps2 = _byteswap_ulong(dwCaps2);
		fwrite(&dwCaps2, 4, 1, writeLog);
		for(int i = 0; i < 3; i++)
			fwrite(&nullOut, 4, 1, writeLog);
		
		int texelPitch;
        int blockSizeRow, blockSizeColumn;
		
		//Using memcpy, don't detile
		//memcpy(outputBuff, pixels, cardMemory.platform[0]);

		//Using XGUntileTextureLevel
		BOOL IsBorderTexture = XGIsBorderTexture(&texture.basemap);
		UINT MipMapTailLevel = XGGetMipTailBaseLevel(SourceDesc.Width, SourceDesc.Height, IsBorderTexture);
		UINT Slices = (texture.basemap.Format.Dimension == GPUDIMENSION_CUBEMAP) ? 6 : 1;
		char * outBuff = outputBuff,
			//* inBuff = (char*)(texture.basemap.Format.BaseAddress << 12);
			*inBuff = (char*)sub_8219C730(this,true),
			*startBuff;
		if((int)inBuff < 0xE00) inBuff = (char*)(texture.basemap.Format.BaseAddress << 12);
		startBuff = inBuff;
		UINT BaseSize;
		XGGetTextureLayout(&texture.basemap, 0, &BaseSize, 0,0,0,0,0,0,0,0);
		if(texture.basemap.Format.Dimension == GPUDIMENSION_CUBEMAP) BaseSize /= 6;

		for(int Slice = 0; Slice < Slices && inBuff < startBuff + cardMemory.platform[0] && outBuff < outputBuff + cardMemory.platform[0]; Slice++) {
			UINT MipMapOffset = XGGetMipLevelOffset(&texture.basemap, Slice, 0);
			outBuff = outputBuff + (Slice * SourceDesc.SlicePitch);
			inBuff = startBuff + MipMapOffset;

			for(int Level = 0; Level < Levels && inBuff < startBuff + cardMemory.platform[0] && outBuff < outputBuff + cardMemory.platform[0]; Level++) {
				//VOID* pSourceDataCachedReadOnly = (VOID*) ( GPU_CONVERT_CPU_TO_CPU_CACHED_READONLY_ADDRESS( inBuff ) );
				DWORD CalculatedRowPitch = (SourceDesc.WidthInBlocks * SourceDesc.BytesPerBlock) / (1 << Level);
				//DWORD CalculatedRowPitch = (SourceDesc.RowPitch) / (1 << Level);

				if(CalculatedRowPitch < SourceDesc.BytesPerBlock)
					CalculatedRowPitch = SourceDesc.BytesPerBlock;

				XGUntileTextureLevel( SourceDesc.Width, \
					SourceDesc.Height, \
					Level, \
					XGGetGpuFormat(SourceDesc.Format), \
					(XGIsPackedTexture(&texture.basemap) ? 0 : XGTILE_NONPACKED) | (IsBorderTexture ? XGTILE_BORDER : 0), \
					outBuff, \
					CalculatedRowPitch, \
					NULL, \
					inBuff, \
					NULL);

				UINT CurrentMipMapSize = BaseSize / (1 << (2 * Level));

				if(CurrentMipMapSize < SourceDesc.BytesPerBlock)
					CurrentMipMapSize = SourceDesc.BytesPerBlock;
			
				outBuff += CurrentMipMapSize;
				MipMapOffset = XGGetMipLevelOffset(&texture.basemap, Slice, Level + 1);
				inBuff = startBuff + BaseSize + MipMapOffset;
			}
		}

		switch(texture.basemap.Format.Endian)
		{
			case GPUENDIAN_8IN16:
				XGEndianSwapMemory(outputBuff, outputBuff, XGENDIAN_8IN16, 2, cardMemory.platform[0] / 2);
			break;
			case GPUENDIAN_8IN32:
				XGEndianSwapMemory(outputBuff, outputBuff, XGENDIAN_8IN32, 4, cardMemory.platform[0] / 4);
			break;
			case GPUENDIAN_16IN32:
				XGEndianSwapMemory(outputBuff, outputBuff, XGENDIAN_16IN32, 4, cardMemory.platform[0] / 4);
			break;
			default:break;
        }

		fwrite(outputBuff, cardMemory.platform[0], 1, writeLog);
		fclose(writeLog);
		
		if(outputBuff != MasterCharBuffer)
			free(outputBuff);
	}
	else {
		printf("Couldn't dump image %s because there is not enough memory, we needed 0x%X\n", name, cardMemory.platform[0]);
		return ERROR_NOT_ENOUGH_MEMORY;
	}

	return ERROR_SUCCESS;
}

void GfxImage::dumpAllImages()
{
	//g_imagePool->entries[3].entry.dumpImage();
	int assetCount = 0;
	for(int i = 0; i < 0xE00 &&
		g_imagePool->freeHead != &g_imagePool->entries[i].entry &&
		g_imagePool->entries[i].next != &g_imagePool->entries[i+1].entry; i++)
		if(g_imagePool->entries[i].entry.dumpImage(true) != ERROR_SUCCESS)
			printf("Failed to dump image %s\n", g_imagePool->entries[i].entry.name);
		else
			assetCount++;

	printf("Dumped %i images\n", assetCount);
}
#pragma endregion

#pragma region col_map (0xC && 0xD)
char dumpBuffer[0x40];
int clipMap_t::dumpCollisionMap()
{
	sprintf(MasterCharBuffer, "game:\\dump\\modelsurfs\\%s.fbx", ConvertAssetNameToFileName(g_colMap->name));
	CreateDirectoryB(MasterCharBuffer);

	if(fileExists(MasterCharBuffer))
		return ERROR_DUP_NAME;

	FILE *writeLog = fopen(MasterCharBuffer, "w");

	if (!writeLog)
		return ERROR_BAD_NETPATH;
	
	fprintf(writeLog, FBXHeader);
	fprintf(writeLog, "Objects: {\n");
	fprintf(writeLog, "\tGeometry: 859843040, \"Geometry::\", \"Mesh\" {\n\t\tVertices: *%i {\n\t\t\ta: ", g_colMap->vertCount*3);
		
	int verticiesLastSurface = contemplatingCurrentPrimary = contemplatingCurrentSecondary = 0;
	contemplatingTotalPrimary = 3;
	strcpy(contemplatorTextPrimary, "Dumping Collision Map");
	contemplatingTotalSecondary = g_colMap->vertCount;
	strcpy(contemplatorTextSecondary, "Dumping Verts");
	isContemplatingPrimary = isContemplatingSecondary = true;
	for(unsigned int k = 0; k < g_colMap->vertCount; k++, contemplatingCurrentSecondary++, verticiesLastSurface++)
		fprintf(writeLog, "%g,%g,%g%s", g_colMap->verts[k][0],
			g_colMap->verts[k][1],
			g_colMap->verts[k][2],
			(k < (g_colMap->vertCount - 1)) ? "," : "\n");

	contemplatingCurrentPrimary = 1;
	contemplatingTotalSecondary = g_colMap->triCount;
	contemplatingCurrentSecondary = 0;
	strcpy(contemplatorTextSecondary, "Dumping Faces");
	int invalidTriCount = 0;
	for(int k = 0; k < g_colMap->triCount; k++)
	{
		int firstIndicie = g_colMap->triIndices[k][0],
			secondIndicie = g_colMap->triIndices[k][1],
			thirdIndicie = g_colMap->triIndices[k][2];

		if(firstIndicie == secondIndicie ||
			firstIndicie == thirdIndicie ||
			secondIndicie == thirdIndicie)
			invalidTriCount++;
	}
		
	fprintf(writeLog, "\t\t}\n\t\tPolygonVertexIndex: *%i {\n\t\t\ta: ", (g_colMap->triCount*3) - (invalidTriCount*3));
	for(int k = 0; k < g_colMap->triCount; k++, contemplatingCurrentSecondary++)
	{
		unsigned int firstIndicie = g_colMap->triIndices[k][0],
			secondIndicie = g_colMap->triIndices[k][1],
			thirdIndicie = -(g_colMap->triIndices[k][2])-1;

		if(firstIndicie == secondIndicie ||
			firstIndicie == g_colMap->triIndices[k][2] ||
			secondIndicie == g_colMap->triIndices[k][2])
			continue;
		else if(k != 0)
			fprintf(writeLog,",");

		fprintf(writeLog, "%i,%i,%i", firstIndicie,
			secondIndicie,
			thirdIndicie);
	}
	
	fprintf(writeLog, "\n\t\t}\n\t}\n\tModel: 863543520, \"Model::Ground\", \"Mesh\" {\n\t\tVersion: 232\n\t\tProperties70: {\n\t\t\tP: \"PreRotation\", \"Vector3D\", \"Vector\", \"\",-90,0,0\n\t\t\tP: \"RotationActive\", \"bool\", \"\", \"\",1\n\t\t\tP: \"InheritType\", \"enum\", \"\", \"\",1\n\t\t\tP: \"DefaultAttributeIndex\", \"int\", \"Integer\", \"\",0\n\t\t}\n\t}\n}\n\nConnections:\n{\n\tC: \"OO\",863543520,0\n\tC: \"OO\",859843040,863543520\n}");

	isContemplatingPrimary = isContemplatingSecondary = false;
	contemplatingTotalPrimary = contemplatingCurrentPrimary = contemplatingTotalSecondary = contemplatingCurrentSecondary = -1;

	fclose(writeLog);
	return ERROR_SUCCESS;
}
#pragma endregion

#pragma region gfx_map (0x13)
//Hmm... Looks like I never updated this to dump to .fbxs. It'd only take a few minutes to convert it, but it really isn't necessary and I don't feel like doing it. 
int GfxWorld::dumpGFXMap()
{
	sprintf(MasterCharBuffer, "game:\\dump\\%s.obj", ConvertAssetNameToFileName((char*)g_GFXMap->name));
	CreateDirectoryB(MasterCharBuffer);

	if(fileExists(MasterCharBuffer))
		return ERROR_DUP_NAME;

	FILE *writeLog = fopen(MasterCharBuffer, "w");

	if (!writeLog)
		return ERROR_BAD_NETPATH;

	fprintf(writeLog, "g default\n");
		
	int verticiesLastSurface = contemplatingCurrentPrimary = contemplatingCurrentSecondary = 0;
	contemplatingTotalPrimary = 2;
	strcpy(contemplatorTextPrimary, "Dumping GFX Map");
	contemplatingTotalSecondary = g_GFXMap->unknownStruct2.vertCount;
	strcpy(contemplatorTextSecondary, "Dumping Verts");
	isContemplatingPrimary = isContemplatingSecondary = true;
	for(unsigned int k = 0; k < g_GFXMap->unknownStruct2.vertCount; k++, contemplatingCurrentSecondary++, verticiesLastSurface++)
		fprintf(writeLog, "v %g %g %g\n", g_GFXMap->unknownStruct2.vertexData.vertices[k].xyz[0],
			g_GFXMap->unknownStruct2.vertexData.vertices[k].xyz[1],
			g_GFXMap->unknownStruct2.vertexData.vertices[k].xyz[2]);

	fprintf(writeLog, "s off\ng Ground\n");
	fflush(writeLog);

	contemplatingCurrentPrimary = 1;
	contemplatingTotalSecondary = g_GFXMap->unknownStruct2.indexCount;
	contemplatingCurrentSecondary = 0;
	strcpy(contemplatorTextSecondary, "Dumping Faces");
	for(int k = 0; k < g_GFXMap->unknownStruct2.indexCount; k++, contemplatingCurrentSecondary++)
	{
		unsigned int firstIndicie = g_GFXMap->unknownStruct2.indices[(k*3)],
			secondIndicie = g_GFXMap->unknownStruct2.indices[(k*3) + 1],
			thirdIndicie = g_GFXMap->unknownStruct2.indices[(k*3) + 2];

		firstIndicie++;
		secondIndicie++;
		thirdIndicie++;

		if(firstIndicie == secondIndicie ||
			firstIndicie == thirdIndicie ||
			secondIndicie == thirdIndicie)
			continue;

		fprintf(writeLog, "f %i/%i/ %i/%i/ %i/%i/\n", firstIndicie, firstIndicie,
			secondIndicie, secondIndicie,
			thirdIndicie, thirdIndicie);
	}
	isContemplatingPrimary = isContemplatingSecondary = false;
	contemplatingTotalPrimary = contemplatingCurrentPrimary = contemplatingTotalSecondary = contemplatingCurrentSecondary = -1;
	
	fflush(writeLog);
	fclose(writeLog);
	return ERROR_SUCCESS;
}
#pragma endregion

#pragma region map_ents (0x11)
int MapEnts::dumpMapEntities()
{
	sprintf(MasterCharBuffer, "game:\\dump\\%s", ConvertAssetNameToFileName(name));
	CreateDirectoryB(MasterCharBuffer);

	if(fileExists(MasterCharBuffer))
		return ERROR_DUP_NAME;
	
	FILE * writeLog = fopen(MasterCharBuffer, "wb");

	if (!writeLog)
		return ERROR_BAD_NETPATH;
	
	fwrite(entityString, strlen(entityString), 1, writeLog);

	fclose(writeLog);
	return ERROR_SUCCESS;
}

void MapEnts::dumpAllMapEntities()
{
	int assetCount = 0;
	for(int i = 0; i < 2 &&
		g_mapEntsPool->freeHead != &g_mapEntsPool->entries[i].entry &&
		g_mapEntsPool->entries[i].next != &g_mapEntsPool->entries[i+1].entry &&
		g_mapEntsPool->entries[i].next &&
		g_mapEntsPool->entries[i].entry.entityString; i++)
		if(g_mapEntsPool->entries[i].entry.dumpMapEntities() == ERROR_SUCCESS)
			assetCount++;

	printf("Dumped %i map entities\n", assetCount);
}

int previouslyLoadedSize;
char * previouslyLoadedMapEnts;
const char * CM_EntityString()
{
	while(usingAssetBuffer) Sleep(10);

	usingAssetBuffer = true;
	sprintf(theAssetBuffer, "game:\\raw\\%s", ConvertAssetNameToFileName(g_colMap->mapEnts->name));
	if(fileExists(theAssetBuffer))
	{
		HANDLE mapEntsFile = CreateFile(theAssetBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		int numberOfBytesRead = GetFileSize(mapEntsFile, NULL);

		if(previouslyLoadedSize-0 == numberOfBytesRead && numberOfBytesRead == g_colMap->mapEnts->numEntityChars-1)
			g_colMap->mapEnts->entityString = previouslyLoadedMapEnts;
		else
		{
			if(mapEntsFile != INVALID_HANDLE_VALUE)
			{
				if(numberOfBytesRead <= g_colMap->mapEnts->numEntityChars-1)
				{
					ZeroMemory(g_colMap->mapEnts->entityString, g_colMap->mapEnts->numEntityChars);
					ReadFile(mapEntsFile, g_colMap->mapEnts->entityString, numberOfBytesRead, (LPDWORD)&numberOfBytesRead, 0);
					previouslyLoadedSize = numberOfBytesRead;
					g_colMap->mapEnts->numEntityChars = numberOfBytesRead+1;
					previouslyLoadedMapEnts = g_colMap->mapEnts->entityString;
				}
				else if(numberOfBytesRead <= 0x20000)
				{
					if(previouslyLoadedMapEnts) { delete[] previouslyLoadedMapEnts; }
				
					char * newStringBuffer = NULL;
					newStringBuffer = new char[numberOfBytesRead+1];
					//newStringBuffer = (char*)VirtualAlloc(numberOfBytesRead+1, MAXULONG_PTR, 4, PAGE_READWRITE);
					//newStringBuffer = (char*)VirtualAlloc((LPVOID)0x8A000000, numberOfBytesRead+1, MEM_RESERVE, PAGE_READWRITE);
					//newStringBuffer = (char*)malloc(numberOfBytesRead+1);
					//newStringBuffer = entString;

					if(newStringBuffer)
					{
						ZeroMemory(newStringBuffer, numberOfBytesRead+1);
						ReadFile(mapEntsFile, newStringBuffer, numberOfBytesRead, (LPDWORD)&numberOfBytesRead, 0);
						previouslyLoadedMapEnts = g_colMap->mapEnts->entityString = newStringBuffer;
						previouslyLoadedSize = numberOfBytesRead;
						g_colMap->mapEnts->numEntityChars = numberOfBytesRead+1;
					}
				}
			}
		}

		CloseHandle(mapEntsFile);

	}
	usingAssetBuffer = false;

	return g_colMap->mapEnts->entityString;
}
#pragma endregion

#pragma region lightdef (0x14)
int GfxLightDef::dumpLight()
{
	sprintf(MasterCharBuffer, "game:\\dump\\lights\\%s.def", (char*)name);
	CreateDirectoryB(MasterCharBuffer);

	if(fileExists(MasterCharBuffer))
		return ERROR_DUP_NAME;

	FILE * writeLog = fopen(MasterCharBuffer, "w");

	if(!writeLog)
		return ERROR_BAD_NETPATH;

	fprintf(writeLog, "%c%s", attenuation.samplerState, attenuation.image->name);
	fclose(writeLog);
	return ERROR_SUCCESS;
}

void GfxLightDef::dumpAllLights()
{
	int assetCount = 0;
	for(int i = 0; i < 0x20 &&
		g_lightDefPool->freeHead != &g_lightDefPool->entries[i].entry &&
		g_lightDefPool->entries[i].next != &g_lightDefPool->entries[i+1].entry; i++)
		if(g_lightDefPool->entries[i].entry.dumpLight() == ERROR_SUCCESS)
			assetCount++;

	printf("Dumped %i lights\n", assetCount);
}

void Load_lightDefAssetHook(void ** input)
{
	GfxLightDef * currentLightDef = *(GfxLightDef**)input;

	//Can we load this weapon custom?
	while(usingAssetBuffer) Sleep(10);

	usingAssetBuffer = true;
	sprintf(theAssetBuffer, "game:\\raw\\lights\\%s.def", (char*)currentLightDef->name);
	if(fileExists(theAssetBuffer))
	{
		HANDLE lightDefFile = CreateFile(theAssetBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		int numberOfBytesRead;
		ZeroMemory(theAssetBuffer, 0x4000);
		if(lightDefFile != INVALID_HANDLE_VALUE && ReadFile(lightDefFile, theAssetBuffer, GetFileSize(lightDefFile, NULL), (LPDWORD)&numberOfBytesRead, 0))
		{
			CloseHandle(lightDefFile);
			currentLightDef->attenuation.samplerState = theAssetBuffer[0];

			if(!theAssetBuffer[1])
				currentLightDef->attenuation.image = 0;
			else
				currentLightDef->attenuation.image = DB_FindXAssetHeader(image, &theAssetBuffer[1]).image;
		}
	}
	usingAssetBuffer = false;

	lightDefLoaderSaveStub(input);
}
#pragma endregion

#pragma region localize (0x19)
void LocalizeEntry::dumpAllLocalizeEntries()
{
	CreateDirectory("game:\\dump\\", NULL);

	if(fileExists("game:\\dump\\localize.str"))
		return;

	FILE * writeLog = fopen("game:\\dump\\localize.str","w");

	if(!writeLog)
		return;
	
	contemplatingTotalPrimary = ((unsigned int)g_LocalizeEntryPool->freeHead - (unsigned int)g_LocalizeEntryPool)/sizeof(LocalizeEntry) + 1;
	int assetCount = contemplatingCurrentPrimary = 0;
	strcpy(contemplatorTextPrimary, "Dumping Localized Strings");
	isContemplatingPrimary = true;
	fprintf(writeLog, "//Dumped by the Tesseract\n//CraigChrist8239\n\n");
	for(int i = 0; i < 0x1B58 &&
		g_LocalizeEntryPool->freeHead != &g_LocalizeEntryPool->entries[i].entry &&
		g_LocalizeEntryPool->entries[i].next != &g_LocalizeEntryPool->entries[i+1].entry; i++, assetCount++, contemplatingCurrentPrimary++)
		fprintf(writeLog, "REFERENCE           %s\nLANG_ENGLISH        \"%s\"\n\n", g_LocalizeEntryPool->entries[i].entry.name, g_LocalizeEntryPool->entries[i].entry.value);
	
	fprintf(writeLog,"ENDMARKER");
	fclose(writeLog);
	isContemplatingPrimary = false;
	contemplatingTotalPrimary = contemplatingCurrentPrimary = -1;

	printf("Dumped %i localized strings\n", assetCount);
}

void * customLoadedLocalize[0x100];
int customLoadedLocalizeCount = 0;
char * Localize_GetString(char * nameOfLocalize)
{
	LocalizeEntry * oldEntry = DB_FindXAssetHeader(localize, nameOfLocalize).localize;

	//Have we loaded this localize custom before?
	for(int i = 0; i < customLoadedLocalizeCount; i++)
	{
		if(!customLoadedLocalize[i])
			break;

		if(!strcmp(nameOfLocalize, ((LocalizeEntry*)customLoadedLocalize[i])->name))
			return ((LocalizeEntry*)customLoadedLocalize[i])->value;
	}

	//Can we load this rawfile custom?
	while(usingAssetBuffer) Sleep(10);
	
	usingAssetBuffer = true;
	sprintf(theAssetBuffer, "game:\\raw\\english\\%s", nameOfLocalize);
	if(fileExists(theAssetBuffer))
	{
		HANDLE localizeFile = CreateFile(theAssetBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		int numberOfBytesRead;
 		if(localizeFile != INVALID_HANDLE_VALUE && ReadFile(localizeFile, theAssetBuffer, GetFileSize(localizeFile, NULL), (LPDWORD)&numberOfBytesRead, 0))
		{
			LocalizeEntry * newEntry;

			if(!oldEntry)
			{
				newEntry = new LocalizeEntry;
				ZeroMemory(newEntry, sizeof(LocalizeEntry));
			}
			else
				newEntry = oldEntry;

			if(!oldEntry)
			{
				newEntry->name = new char[strlen(nameOfLocalize) + 1];
				ZeroMemory(newEntry->name, strlen(nameOfLocalize) + 1);
				strcpy(newEntry->name, nameOfLocalize);
			}

			theAssetBuffer[numberOfBytesRead] = 0;
			if(!oldEntry || strcmp(newEntry->value, theAssetBuffer))
			{
				newEntry->value = new char[numberOfBytesRead+1];
				ZeroMemory(newEntry->value, numberOfBytesRead+1);
				memcpy(newEntry->value, theAssetBuffer, numberOfBytesRead);
			}

			customLoadedLocalize[customLoadedLocalizeCount] = oldEntry = newEntry;
			customLoadedLocalizeCount++;
		}
		CloseHandle(localizeFile);
	}
	usingAssetBuffer = false;

	return !oldEntry ? 0 : oldEntry->value;
}
#pragma endregion

#pragma region weapon (0x1A)
char * activeReticleType_s[] = {
	"None",
	"Pip-on-a-Stick",
	"Bouncing Diamond",
	0
};

char * guidedMissileType_s[] = {
	"None",
	"Sidewinder",
	"Hellfire",
	"Javelin",
	0
};

char * weaponIconRatioType_s[] = {
	"1:1",
	"2:1",
	"4:1",
	0
};

char * WeapStickinessType_s[] = {
	"Don't stick",
	"Stick to all",
	"Stick to all, orient to surface",
	"Stick to ground",
	"Stick to ground, maintain yaw",
	"Knife",
	0
};

char * ammoCounterClipType_s[] = {
	"None",
	"Magazine",
	"ShortMagazine",
	"Shotgun",
	"Rocket",
	"Beltfed",
	"AltWeap",
	0
};

char * OffhandClass_s[] = 
{
	"None",
	"Frag Grenade",
	"Smoke Grenade",
	"Flash Greande",
	0
};

char* playerAnimType_s[] = { "none",
	"other",
	"pistol",
	"smg",
	"autorifle",
	"mg",
	"sniper",
	"rocketlauncher",
	"explosive",
	"grenade",
	"turret",
	"c4", 
	"m203", 
	"hold",
	"briefcase",
	"riotshield",
	"laptop",
	"throwingknife", 
	0
};

char * weapStance_s[] =
{
  "Stand",
  "Duck",
  "Prone", 
  0
};

char * weapProjExplosion_s[] = {
	"Grenade",
	"Rocket",
	"Flashbang",
	"None",
	"Dud",
	"Smoke",
	"Heavy",
	0
};

char * VehicleAxles_s[] = {
	"front",
	"rear",
	"all",
	0
};

char * weapOverlayInterface_s[] = {
	"None",
	"Javelin",
	"TurretScope",
	0
};

char * weapOverlayReticle_s[] = {
	"None",
	"Crosshair",
	0
};

//Check these
char * ImpactType_s[] = 
{
	"none",
	"bullet_small",
	"bullet_large",
	"bullet_ap",
	"shotgun_fmj",
	"shotgun",
	"grenade_bounce",
	"grenade_explode",
	"rocket_explode",
	"dud",
	0
};

const AssetEntry SurfaceSoundEntries[] = {
	{"default",			0,					ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"bark",			4,					ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"brick",			8,					ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"carpet",			0xC,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"cloth",			0x10,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"concrete",		0x14,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"dirt",			0x18,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"flesh",			0x1C,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"foliage",			0x20,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"glass",			0x24,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"grass",			0x28,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"gravel",			0x2C,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"ice",				0x30,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"metal",			0x34,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"mud",				0x38,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"paper",			0x3C,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"plaster",			0x40,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"rock",			0x44,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"sand",			0x48,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"snow",			0x4C,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"water",			0x50,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"wood",			0x54,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"asphalt",			0x58,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"ceramic",			0x5C,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"plastic",			0x60,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"rubber",			0x64,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"cushion",			0x68,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"fruit",			0x6C,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"paintedMetal",	0x70,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"riotshield",		0x74,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"slush",			0x78,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry LocDamageMultEntries[] = {
	{"None",						0,					ASSET_ENTRY_FLOAT,0,0},
	{"Helmet",					4,					ASSET_ENTRY_FLOAT,0,0},
	{"Head",						8,					ASSET_ENTRY_FLOAT,0,0},
	{"Neck",						0x0C,				ASSET_ENTRY_FLOAT,0,0},
	{"TorsoUpper",				0x10,				ASSET_ENTRY_FLOAT,0,0},
	{"TorsoLower",				0x14,				ASSET_ENTRY_FLOAT,0,0},
	{"RightArmUpper",			0x18,				ASSET_ENTRY_FLOAT,0,0},
	{"RightArmLower",			0x1C,				ASSET_ENTRY_FLOAT,0,0},
	{"RightHand",				0x20,				ASSET_ENTRY_FLOAT,0,0},
	{"LeftArmUpper",				0x24,				ASSET_ENTRY_FLOAT,0,0},
	{"LeftArmLower",				0x28,				ASSET_ENTRY_FLOAT,0,0},
	{"LeftHand",					0x2C,				ASSET_ENTRY_FLOAT,0,0},
	{"RightLegUpper",			0x30,				ASSET_ENTRY_FLOAT,0,0},
	{"RightLegLower",			0x34,				ASSET_ENTRY_FLOAT,0,0},
	{"RightFoot",				0x38,				ASSET_ENTRY_FLOAT,0,0},
	{"LeftLegUpper",				0x3C,				ASSET_ENTRY_FLOAT,0,0},
	{"LeftLegLower",				0x40,				ASSET_ENTRY_FLOAT,0,0},
	{"LeftFoot",					0x44,				ASSET_ENTRY_FLOAT,0,0},
	{"Gun",						0x48,				ASSET_ENTRY_FLOAT,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry GunModelEntries[] = {
	{"",		0,		ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)xmodel,0},
	{"1",		4,		ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)xmodel,0},
	{"2",		8,		ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)xmodel,0},
	{"3",		0xC,	ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)xmodel,0},
	{"4",		0x10,	ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)xmodel,0},
	{"5",		0x14,	ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)xmodel,0},
	{"6",		0x18,	ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)xmodel,0},
	{"7",		0x1C,	ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)xmodel,0},
	{"8",		0x20,	ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)xmodel,0},
	{"9",		0x24,	ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)xmodel,0},
	{"10",		0x28,	ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)xmodel,0},
	{"11",		0x2C,	ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)xmodel,0},
	{"12",		0x30,	ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)xmodel,0},
	{"13",		0x34,	ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)xmodel,0},
	{"14",		0x38,	ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)xmodel,0},
	{"15",		0x3C,	ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)xmodel,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry BounceEntries[] = {
	{"DefaultBounce",			0,					ASSET_ENTRY_FLOAT,0,0},
	{"BarkBounce",				4,					ASSET_ENTRY_FLOAT,0,0},
	{"BrickBounce",				8,					ASSET_ENTRY_FLOAT,0,0},
	{"CarpetBounce",			0xC,				ASSET_ENTRY_FLOAT,0,0},
	{"ClothBounce",				0x10,				ASSET_ENTRY_FLOAT,0,0},
	{"ConcreteBounce",			0x14,				ASSET_ENTRY_FLOAT,0,0},
	{"DirtBounce",				0x18,				ASSET_ENTRY_FLOAT,0,0},
	{"FleshBounce",				0x1C,				ASSET_ENTRY_FLOAT,0,0},
	{"FoliageBounce",			0x20,				ASSET_ENTRY_FLOAT,0,0},
	{"GlassBounce",				0x24,				ASSET_ENTRY_FLOAT,0,0},
	{"GrassBounce",				0x28,				ASSET_ENTRY_FLOAT,0,0},
	{"GravelBounce",			0x2C,				ASSET_ENTRY_FLOAT,0,0},
	{"IceBounce",				0x30,				ASSET_ENTRY_FLOAT,0,0},
	{"MetalBounce",				0x34,				ASSET_ENTRY_FLOAT,0,0},
	{"MudBounce",				0x38,				ASSET_ENTRY_FLOAT,0,0},
	{"PaperBounce",				0x3C,				ASSET_ENTRY_FLOAT,0,0},
	{"PlasterBounce",			0x40,				ASSET_ENTRY_FLOAT,0,0},
	{"RockBounce",				0x44,				ASSET_ENTRY_FLOAT,0,0},
	{"SandBounce",				0x48,				ASSET_ENTRY_FLOAT,0,0},
	{"SnowBounce",				0x4C,				ASSET_ENTRY_FLOAT,0,0},
	{"WaterBounce",				0x50,				ASSET_ENTRY_FLOAT,0,0},
	{"WoodBounce",				0x54,				ASSET_ENTRY_FLOAT,0,0},
	{"AsphaltBounce",			0x58,				ASSET_ENTRY_FLOAT,0,0},
	{"CeramicBounce",			0x5C,				ASSET_ENTRY_FLOAT,0,0},
	{"PlasticBounce",			0x60,				ASSET_ENTRY_FLOAT,0,0},
	{"RubberBounce",			0x64,				ASSET_ENTRY_FLOAT,0,0},
	{"CushionBounce",			0x68,				ASSET_ENTRY_FLOAT,0,0},
	{"FruitBounce",				0x6C,				ASSET_ENTRY_FLOAT,0,0},
	{"PaintedMetalBounce",		0x70,				ASSET_ENTRY_FLOAT,0,0},
	{"RiotshieldBounce",		0x74,				ASSET_ENTRY_FLOAT,0,0},
	{"SlushBounce",				0x78,				ASSET_ENTRY_FLOAT,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry AnimEntries[] = {
	{"idleAnim",					4,						ASSET_ENTRY_STRING,0,0},
	{"emptyIdleAnim",				8,						ASSET_ENTRY_STRING,0,0},
	{"fireAnim",					0xC,					ASSET_ENTRY_STRING,0,0},
	{"holdFireAnim",				0x10,					ASSET_ENTRY_STRING,0,0},
	{"lastShotAnim",				0x14,					ASSET_ENTRY_STRING,0,0},
	{"detonateAnim",				0x18,					ASSET_ENTRY_STRING,0,0},
	{"rechamberAnim",				0x1C,					ASSET_ENTRY_STRING,0,0},
	{"meleeAnim",					0x20,					ASSET_ENTRY_STRING,0,0},
	{"meleeChargeAnim",				0x24,					ASSET_ENTRY_STRING,0,0},
	{"reloadAnim",					0x28,					ASSET_ENTRY_STRING,0,0},
	{"reloadEmptyAnim",				0x2C,					ASSET_ENTRY_STRING,0,0},
	{"reloadStartAnim",				0x30,					ASSET_ENTRY_STRING,0,0},
	{"reloadEndAnim",				0x34,					ASSET_ENTRY_STRING,0,0},
	{"raiseAnim",					0x38,					ASSET_ENTRY_STRING,0,0},
	{"dropAnim",					0x3C,					ASSET_ENTRY_STRING,0,0},
	{"firstRaiseAnim",				0x40,					ASSET_ENTRY_STRING,0,0},
	{"breachRaiseAnim",				0x44,					ASSET_ENTRY_STRING,0,0},
	{"altRaiseAnim",				0x48,					ASSET_ENTRY_STRING,0,0},
	{"altDropAnim",					0x4C,					ASSET_ENTRY_STRING,0,0},
	{"quickRaiseAnim",				0x50,					ASSET_ENTRY_STRING,0,0},
	{"quickDropAnim",				0x54,					ASSET_ENTRY_STRING,0,0},
	{"emptyRaiseAnim",				0x58,					ASSET_ENTRY_STRING,0,0},
	{"emptyDropAnim",				0x5C,					ASSET_ENTRY_STRING,0,0},
	{"sprintInAnim",				0x60,					ASSET_ENTRY_STRING,0,0},
	{"sprintLoopAnim",				0x64,					ASSET_ENTRY_STRING,0,0},
	{"sprintOutAnim",				0x68,					ASSET_ENTRY_STRING,0,0},
	{"stunnedAnimStart",			0x6C,					ASSET_ENTRY_STRING,0,0},
	{"stunnedAnimLoop",				0x70,					ASSET_ENTRY_STRING,0,0},
	{"stunnedAnimEnd",				0x74,					ASSET_ENTRY_STRING,0,0},
	{"nightVisionWearAnim",			0x78,					ASSET_ENTRY_STRING,0,0},
	{"nightVisionRemoveAnim",		0x7C,					ASSET_ENTRY_STRING,0,0},
	{"adsFireAnim",					0x80,					ASSET_ENTRY_STRING,0,0},
	{"adsLastShotAnim",				0x84,					ASSET_ENTRY_STRING,0,0},
	{"adsRechamberAnim",			0x88,					ASSET_ENTRY_STRING,0,0},
	{"adsUpAnim",					0x8C,					ASSET_ENTRY_STRING,0,0},
	{"adsDownAnim",					0x90,					ASSET_ENTRY_STRING,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry TurretBarrelSpinEntries[] = {
	{"Rumble",					0,					ASSET_ENTRY_STRING,0,0},
	{"UpTime",					4,					ASSET_ENTRY_INT,0,0},
	{"DownTime",				8,					ASSET_ENTRY_INT,0,0},
	{"Speed",					0xC,				ASSET_ENTRY_INT,0,0},
	{"MaxSnd",					0x10,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"UpSnd1",					0x14,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"UpSnd2",					0x18,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"UpSnd3",					0x1C,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"UpSnd4",					0x20,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"DownSnd1",				0x24,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"DownSnd2",				0x28,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"DownSnd3",				0x2C,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"DownSnd4",				0x30,				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry MissileConeSoundEntries[] = {
	{"Alias",					0,					ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"AliasAtBase",				4,					ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"RadiusAtTop",				8,					ASSET_ENTRY_FLOAT,0,0},
	{"RadiusAtBase",			0xC,				ASSET_ENTRY_FLOAT,0,0},
	{"Height",					0x10,				ASSET_ENTRY_FLOAT,0,0},
	{"OriginOffset",			0x14,				ASSET_ENTRY_FLOAT,0,0},
	{"VolumescaleAtCore",		0x18,				ASSET_ENTRY_FLOAT,0,0},
	{"VolumescaleAtEdge",		0x1C,				ASSET_ENTRY_FLOAT,0,0},
	{"VolumescaleCoreSize",		0x20,				ASSET_ENTRY_FLOAT,0,0},
	{"PitchAtTop",				0x24,				ASSET_ENTRY_FLOAT,0,0},
	{"PitchAtBottom",			0x28,				ASSET_ENTRY_FLOAT,0,0},
	{"PitchTopSize",			0x2C,				ASSET_ENTRY_FLOAT,0,0},
	{"PitchBottomSize",			0x30,				ASSET_ENTRY_FLOAT,0,0},
	{"CrossfadeTopSize",		0x34,				ASSET_ENTRY_FLOAT,0,0},
	{"CrossfadeBottomSize",		0x38,				ASSET_ENTRY_FLOAT,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry MinMax_IntEntry[] = {
	{"Min",					0,					ASSET_ENTRY_INT,0,0},
	{"Max",					4,					ASSET_ENTRY_INT,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry MinMax_FloatEntry[] = {
	{"Min",					0,					ASSET_ENTRY_FLOAT,0,0},
	{"Max",					4,					ASSET_ENTRY_FLOAT,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry VectorEntry[] = {
	{"F", 0, ASSET_ENTRY_FLOAT,0,0},
	{"R", 4, ASSET_ENTRY_FLOAT,0,0},
	{"U", 8, ASSET_ENTRY_FLOAT,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry AngleEntry[] = {
	{"Pitch",	0, ASSET_ENTRY_FLOAT,0,0},
	{"Yaw",		4, ASSET_ENTRY_FLOAT,0,0},
	{"Roll",	8, ASSET_ENTRY_FLOAT,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry HipSpreadSettings[] = {
	{"StandMin",						0,						ASSET_ENTRY_FLOAT,0,0},
	{"DuckedMin",						4,						ASSET_ENTRY_FLOAT,0,0},
	{"ProneMin",						8,						ASSET_ENTRY_FLOAT,0,0},
	{"StandMax",						0xC,						ASSET_ENTRY_FLOAT,0,0},
	{"DuckedMax",						0x10,						ASSET_ENTRY_FLOAT,0,0},
	{"ProneMax",						0x14,						ASSET_ENTRY_FLOAT,0,0},
	{"DecayRate",						0x18,						ASSET_ENTRY_FLOAT,0,0},
	{"FireAdd",							0x1C,						ASSET_ENTRY_FLOAT,0,0},
	{"TurnAdd",							0x20,						ASSET_ENTRY_FLOAT,0,0},
	{"MoveAdd",							0x24,						ASSET_ENTRY_FLOAT,0,0},
	{"DuckedDecay",						0x28,					ASSET_ENTRY_FLOAT,0,0},
	{"ProneDecay",						0x2C,					ASSET_ENTRY_FLOAT,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry GunKickSettings[] = {
	{"ReducedKickBullets",			0,			ASSET_ENTRY_INT,0,0},
	{"ReducedKickPercent",			4,			ASSET_ENTRY_FLOAT,0,0},
	{"PitchMin",						8,					ASSET_ENTRY_FLOAT,0,0},
	{"PitchMax",						0xC,					ASSET_ENTRY_FLOAT,0,0},
	{"YawMin",						0x10,					ASSET_ENTRY_FLOAT,0,0},
	{"YawMax",						0x14,					ASSET_ENTRY_FLOAT,0,0},
	{"Accel",							0x18,						ASSET_ENTRY_FLOAT,0,0},
	{"SpeedMax",						0x1C,						ASSET_ENTRY_FLOAT,0,0},
	{"SpeedDecay",					0x20,					ASSET_ENTRY_FLOAT,0,0},
	{"StaticDecay",					0x24,					ASSET_ENTRY_FLOAT,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry IconEntry[] = {
	{"",								0,								ASSET_ENTRY_ASSET_PTR,					(AssetEntry*)material,0},
	{"Ratio",							4,								ASSET_ENTRY_ENUM,						(AssetEntry*)weaponIconRatioType_s,	WEAPON_ICON_RATIO_MAX},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry ClientSideSoundEntry[] = {
	{"",								0,								ASSET_ENTRY_ASSET_PTR,					(AssetEntry*)sound,0},
	{"Player",							4,								ASSET_ENTRY_ASSET_PTR,					(AssetEntry*)sound,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry WeaponDefEntries[] = {
	{"gunModel",								offsetof(weaponDef_s, gunModel),								ASSET_ENTRY_CONTAINED,					GunModelEntries,0},
	{"handModel",								offsetof(weaponDef_s, handModel),								ASSET_ENTRY_ASSET_PTR,					(AssetEntry*)xmodel,0},
	{"R",									offsetof(weaponDef_s, animR),									ASSET_ENTRY_CONTAINED_REVERSED,					AnimEntries,0},
	{"L",									offsetof(weaponDef_s, animL),									ASSET_ENTRY_CONTAINED_REVERSED,					AnimEntries,0},
	{"modeName",								offsetof(weaponDef_s, szModeName),								ASSET_ENTRY_STRING,0,0},
	{"playerAnimType",							offsetof(weaponDef_s, playerAnimType),							ASSET_ENTRY_ENUM,						(AssetEntry*)playerAnimType_s,				PLAYER_ANIM_TYPE_MAX},
	{"weaponType",								offsetof(weaponDef_s, weaponType),								ASSET_ENTRY_ENUM,						(AssetEntry*)weaponType_s,					WEAPTYPE_MAX},
	{"weaponClass",								offsetof(weaponDef_s, weaponClass),								ASSET_ENTRY_ENUM,						(AssetEntry*)weaponClass_s,					WEAPCLASS_MAX},
	{"penetrateType",							offsetof(weaponDef_s, penetrateType),							ASSET_ENTRY_ENUM,						(AssetEntry*)PenetrateType_s,				PENETRATE_TYPE_MAX},
	{"inventoryType",							offsetof(weaponDef_s, inventoryType),							ASSET_ENTRY_ENUM,						(AssetEntry*)weapInventoryType_s,			WEAPINVENTORY_MAX},
	{"fireType",								offsetof(weaponDef_s, fireType),								ASSET_ENTRY_ENUM,						(AssetEntry*)weapFireType_s,				WEAPON_FIRETYPE_MAX},
	{"offhandClass",							offsetof(weaponDef_s, offhandClass),							ASSET_ENTRY_ENUM,						(AssetEntry*)OffhandClass_s,				OFFHAND_CLASS_MAX},
	{"stance",									offsetof(weaponDef_s, stance),									ASSET_ENTRY_ENUM,						(AssetEntry*)weapStance_s,					WEAPSTANCE_MAX},
	{"viewFlashEffect",							offsetof(weaponDef_s, viewFlashEffect),							ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)fx,0},
	{"worldFlashEffect",						offsetof(weaponDef_s, worldFlashEffect),						ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)fx,0},
	{"pickUpSound",								offsetof(weaponDef_s, pickUpSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"ammoPickupSound",							offsetof(weaponDef_s, ammoPickupSound),							ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"projectileSound",							offsetof(weaponDef_s, projectileSound),							ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"pullbackSound",							offsetof(weaponDef_s, pullbackSound),							ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"fireSound",								offsetof(weaponDef_s, fireSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"fireSoundPlayerAkimbo",					offsetof(weaponDef_s, fireSoundPlayerAkimbo),					ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"loopFireSound",								offsetof(weaponDef_s, loopFireSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"stopFireSound",								offsetof(weaponDef_s, stopFireSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"lastShotSound",								offsetof(weaponDef_s, lastShotSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"emptyFireSound",								offsetof(weaponDef_s, emptyFireSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"meleeSwipeSound",								offsetof(weaponDef_s, meleeSwipeSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"meleeHitSound",							offsetof(weaponDef_s, meleeHitSound),							ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"meleeMissSound",							offsetof(weaponDef_s, meleeMissSound),							ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"rechamberSound",								offsetof(weaponDef_s, rechamberSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"reloadSound",								offsetof(weaponDef_s, reloadSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"reloadEmptySound",								offsetof(weaponDef_s, reloadEmptySound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"reloadStartSound",								offsetof(weaponDef_s, reloadStartSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"reloadEndSound",								offsetof(weaponDef_s, reloadEndSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"detonateSound",								offsetof(weaponDef_s, detonateSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"nightVisionWearSound",								offsetof(weaponDef_s, nightVisionWearSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"nightVisionRemoveSound",								offsetof(weaponDef_s, nightVisionRemoveSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"altSwitchSound",								offsetof(weaponDef_s, altSwitchSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"raiseSound",								offsetof(weaponDef_s, raiseSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"firstRaiseSound",								offsetof(weaponDef_s, firstRaiseSound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"putawaySound",								offsetof(weaponDef_s, putawaySound),								ASSET_ENTRY_EMBEDDED,						ClientSideSoundEntry,0},
	{"scanSound",								offsetof(weaponDef_s, scanSound),								ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)sound,0},
	{"BounceSound",							offsetof(weaponDef_s, bounceSound),								ASSET_ENTRY_CONTAINED_REVERSED,					SurfaceSoundEntries,0},
	{"viewShellEjectEffect",					offsetof(weaponDef_s, viewShellEjectEffect),					ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)fx,0},
	{"worldShellEjectEffect",					offsetof(weaponDef_s, worldShellEjectEffect),					ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)fx,0},
	{"viewLastShotEjectEffect",					offsetof(weaponDef_s, viewLastShotEjectEffect),					ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)fx,0},
	{"worldLastShotEjectEffect",				offsetof(weaponDef_s, worldLastShotEjectEffect),				ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)fx,0},
	{"reticleCenter",							offsetof(weaponDef_s, reticleCenter),							ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)material,0},
	{"reticleSide",								offsetof(weaponDef_s, reticleSide),								ASSET_ENTRY_ASSET_PTR,						(AssetEntry*)material,0},
	{"reticleCenterSize",						offsetof(weaponDef_s, iReticleCenterSize),						ASSET_ENTRY_INT,0,0},
	{"reticleSideSize",							offsetof(weaponDef_s, iReticleSideSize),						ASSET_ENTRY_INT,0,0},
	{"reticleMinOfs",							offsetof(weaponDef_s, iReticleMinOfs),							ASSET_ENTRY_INT,0,0},
	{"activeReticleType",						offsetof(weaponDef_s, activeReticleType),						ASSET_ENTRY_ENUM,						(AssetEntry*)activeReticleType_s,			VEH_ACTIVE_RETICLE_MAX},
	{"standMove",								offsetof(weaponDef_s, vStandMove),									ASSET_ENTRY_EMBEDDED,					VectorEntry,0},
	{"standRot",								offsetof(weaponDef_s, vStandRot),									ASSET_ENTRY_EMBEDDED,					AngleEntry,0},
	{"strafeMove",								offsetof(weaponDef_s, vStrafeMove),									ASSET_ENTRY_EMBEDDED,					VectorEntry,0},
	{"strafeRot",								offsetof(weaponDef_s, vStrafeRot),									ASSET_ENTRY_EMBEDDED,					AngleEntry,0},
	{"duckedOfs",								offsetof(weaponDef_s, vDuckedOfs),									ASSET_ENTRY_EMBEDDED,					VectorEntry,0},
	{"duckedMove",								offsetof(weaponDef_s, vDuckedMove),									ASSET_ENTRY_EMBEDDED,					VectorEntry,0},
	{"duckedRot",								offsetof(weaponDef_s, vDuckedRot),									ASSET_ENTRY_EMBEDDED,					AngleEntry,0},
	{"proneOfs",								offsetof(weaponDef_s, vProneOfs),									ASSET_ENTRY_EMBEDDED,					VectorEntry,0},
	{"proneMove",								offsetof(weaponDef_s, vProneMove),									ASSET_ENTRY_EMBEDDED,					VectorEntry,0},
	{"proneRot",								offsetof(weaponDef_s, vProneRot),									ASSET_ENTRY_EMBEDDED,					AngleEntry,0},
	{"posMoveRate",								offsetof(weaponDef_s, fPosMoveRate),							ASSET_ENTRY_FLOAT,0,0},
	{"posProneMoveRate",						offsetof(weaponDef_s, fPosProneMoveRate),						ASSET_ENTRY_FLOAT,0,0},
	{"standMoveMinSpeed",						offsetof(weaponDef_s, fStandMoveMinSpeed),						ASSET_ENTRY_FLOAT,0,0},
	{"duckedMoveMinSpeed",						offsetof(weaponDef_s, fDuckedMoveMinSpeed),						ASSET_ENTRY_FLOAT,0,0},
	{"proneMoveMinSpeed",						offsetof(weaponDef_s, fProneMoveMinSpeed),						ASSET_ENTRY_FLOAT,0,0},
	{"posRotRate",								offsetof(weaponDef_s, fPosRotRate),								ASSET_ENTRY_FLOAT,0,0},
	{"posProneRotRate",							offsetof(weaponDef_s, fPosProneRotRate),						ASSET_ENTRY_FLOAT,0,0},
	{"standRotMinSpeed",						offsetof(weaponDef_s, fStandRotMinSpeed),						ASSET_ENTRY_FLOAT,0,0},
	{"duckedRotMinSpeed",						offsetof(weaponDef_s, fDuckedRotMinSpeed),						ASSET_ENTRY_FLOAT,0,0},
	{"proneRotMinSpeed",						offsetof(weaponDef_s, fProneRotMinSpeed),						ASSET_ENTRY_FLOAT,0,0},
	{"worldModel",								offsetof(weaponDef_s, worldModel),								ASSET_ENTRY_CONTAINED,							GunModelEntries,0},
	{"worldClipModel",							offsetof(weaponDef_s, worldClipModel),							ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)xmodel,0},
	{"rocketModel",								offsetof(weaponDef_s, rocketModel),								ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)xmodel,0},
	{"knifeModel",								offsetof(weaponDef_s, knifeModel),								ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)xmodel,0},
	{"worldKnifeModel",							offsetof(weaponDef_s, worldKnifeModel),							ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)xmodel,0},
	{"hudIcon",							offsetof(weaponDef_s, hudIcon),							ASSET_ENTRY_EMBEDDED,							IconEntry,0},
	{"pickupIcon",							offsetof(weaponDef_s, pickupIcon),							ASSET_ENTRY_EMBEDDED,							IconEntry,0},
	{"ammoCounterIcon",							offsetof(weaponDef_s, ammoCounterIcon),							ASSET_ENTRY_EMBEDDED,							IconEntry,0},
	{"ammoCounterClip",							offsetof(weaponDef_s, ammoCounterClip),							ASSET_ENTRY_ENUM,								(AssetEntry*)ammoCounterClipType_s,	AMMO_COUNTER_CLIP_MAX},
	{"startAmmo",								offsetof(weaponDef_s, iStartAmmo),								ASSET_ENTRY_INT,0,0},
	{"ammoName",								offsetof(weaponDef_s, szAmmoName),								ASSET_ENTRY_STRING,0,0},
	{"ammoIndex",								offsetof(weaponDef_s, iAmmoIndex),								ASSET_ENTRY_INT,0,0},
	{"clipName",								offsetof(weaponDef_s, szClipName),								ASSET_ENTRY_STRING,0,0},
	{"clipIndex",								offsetof(weaponDef_s, iClipIndex),								ASSET_ENTRY_INT,0,0},
	{"maxAmmo",									offsetof(weaponDef_s, iMaxAmmo),								ASSET_ENTRY_INT,0,0},
	{"shotCount",								offsetof(weaponDef_s, shotCount),								ASSET_ENTRY_INT,0,0},
	{"sharedAmmoCapName",						offsetof(weaponDef_s, szSharedAmmoCapName),						ASSET_ENTRY_STRING,0,0},
	{"sharedAmmoCapIndex",						offsetof(weaponDef_s, iSharedAmmoCapIndex),						ASSET_ENTRY_INT,0,0},
	{"sharedAmmoCap",							offsetof(weaponDef_s, iSharedAmmoCap),							ASSET_ENTRY_INT,0,0},
	{"damage",									offsetof(weaponDef_s, damage),									ASSET_ENTRY_INT,0,0},
	{"playerDamage",							offsetof(weaponDef_s, playerDamage),							ASSET_ENTRY_INT,0,0},
	{"meleeDamage",								offsetof(weaponDef_s, iMeleeDamage),							ASSET_ENTRY_INT,0,0},
	{"damageType",								offsetof(weaponDef_s, iDamageType),								ASSET_ENTRY_INT,0,0},
	{"fireDelay",								offsetof(weaponDef_s, iFireDelay),								ASSET_ENTRY_TIME_AS_INT,0,0},
	{"meleeDelay",								offsetof(weaponDef_s, iMeleeDelay),								ASSET_ENTRY_TIME_AS_INT,0,0},
	{"meleeChargeDelay",						offsetof(weaponDef_s, meleeChargeDelay),						ASSET_ENTRY_TIME_AS_INT,0,0},
	{"detonateDelay",							offsetof(weaponDef_s, iDetonateDelay),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"rechamberTime",							offsetof(weaponDef_s, iRechamberTime),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"rechamberOneHandedTime",					offsetof(weaponDef_s, iRechamberOneHanded),						ASSET_ENTRY_TIME_AS_INT,0,0},
	{"rechamberBoltTime",						offsetof(weaponDef_s, iRechamberBoltTime),						ASSET_ENTRY_TIME_AS_INT,0,0},
	{"holdFireTime",							offsetof(weaponDef_s, iHoldFireTime),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"detonateTime",							offsetof(weaponDef_s, iDetonateTime),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"meleeTime",								offsetof(weaponDef_s, iMeleeTime),								ASSET_ENTRY_TIME_AS_INT,0,0},
	{"meleeChargeTime",							offsetof(weaponDef_s, meleeChargeTime),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"reloadTime",								offsetof(weaponDef_s, iReloadTime),								ASSET_ENTRY_TIME_AS_INT,0,0},
	{"reloadShowRocketTime",					offsetof(weaponDef_s, reloadShowRocketTime),					ASSET_ENTRY_TIME_AS_INT,0,0},
	{"reloadEmptyTime",							offsetof(weaponDef_s, iReloadEmptyTime),						ASSET_ENTRY_TIME_AS_INT,0,0},
	{"reloadAddTime",							offsetof(weaponDef_s, iReloadAddTime),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"reloadStartTime",							offsetof(weaponDef_s, iReloadStartTime),						ASSET_ENTRY_TIME_AS_INT,0,0},
	{"reloadStartAddTime",						offsetof(weaponDef_s, iReloadStartAddTime),						ASSET_ENTRY_TIME_AS_INT,0,0},
	{"reloadEndTime",							offsetof(weaponDef_s, iReloadEndTime),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"dropTime",								offsetof(weaponDef_s, iDropTime),								ASSET_ENTRY_TIME_AS_INT,0,0},
	{"raiseTime",								offsetof(weaponDef_s, iRaiseTime),								ASSET_ENTRY_TIME_AS_INT,0,0},
	{"altDropTime",								offsetof(weaponDef_s, iAltDropTime),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"quickDropTime",							offsetof(weaponDef_s, quickDropTime),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"quickRaiseTime",							offsetof(weaponDef_s, quickRaiseTime),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"firstRaiseTime",							offsetof(weaponDef_s, iFirstRaiseTime),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"emptyRaiseTime",							offsetof(weaponDef_s, iEmptyRaiseTime),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"emptyDropTime",							offsetof(weaponDef_s, iEmptyDropTime),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"sprintInTime",							offsetof(weaponDef_s, sprintInTime),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"sprintLoopTime",							offsetof(weaponDef_s, sprintLoopTime),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"sprintOutTime",							offsetof(weaponDef_s, sprintOutTime),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"stunnedTimeBegin",						offsetof(weaponDef_s, stunnedTimeBegin),						ASSET_ENTRY_TIME_AS_INT,0,0},
	{"stunnedTimeLoop",							offsetof(weaponDef_s, stunnedTimeLoop),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"stunnedTimeEnd",							offsetof(weaponDef_s, stunnedTimeEnd),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"nightVisionWearTime",						offsetof(weaponDef_s, nightVisionWearTime),						ASSET_ENTRY_TIME_AS_INT,0,0},
	{"nightVisionWearTimeFadeOutEnd",			offsetof(weaponDef_s, nightVisionWearTimeFadeOutEnd),			ASSET_ENTRY_TIME_AS_INT,0,0},
	{"nightVisionWearTimePowerUp",				offsetof(weaponDef_s, nightVisionWearTimePowerUp),				ASSET_ENTRY_TIME_AS_INT,0,0},
	{"nightVisionRemoveTime",					offsetof(weaponDef_s, nightVisionRemoveTime),					ASSET_ENTRY_TIME_AS_INT,0,0},
	{"nightVisionRemoveTimePowerDown",			offsetof(weaponDef_s, nightVisionRemoveTimePowerDown),			ASSET_ENTRY_TIME_AS_INT,0,0},
	{"nightVisionRemoveTimeFadeInStart",		offsetof(weaponDef_s, nightVisionRemoveTimeFadeInStart),		ASSET_ENTRY_TIME_AS_INT,0,0},
	{"fuseTime",								offsetof(weaponDef_s, fuseTime),								ASSET_ENTRY_TIME_AS_INT,0,0},
	{"aifuseTime",								offsetof(weaponDef_s, aifuseTime),								ASSET_ENTRY_TIME_AS_INT,0,0},
	{"autoAimRange",							offsetof(weaponDef_s, autoAimRange),							ASSET_ENTRY_FLOAT,0,0},
	{"aimAssistRange",							offsetof(weaponDef_s, aimAssistRange),							ASSET_ENTRY_FLOAT,0,0},
	{"aimAssistRangeAds",						offsetof(weaponDef_s, aimAssistRangeAds),						ASSET_ENTRY_FLOAT,0,0},
	{"aimPadding",								offsetof(weaponDef_s, aimPadding),								ASSET_ENTRY_FLOAT,0,0},
	{"enemyCrosshairRange",						offsetof(weaponDef_s, enemyCrosshairRange),						ASSET_ENTRY_FLOAT,0,0},
	{"moveSpeedScale",							offsetof(weaponDef_s, moveSpeedScale),							ASSET_ENTRY_FLOAT,0,0},
	{"adsMoveSpeedScale",						offsetof(weaponDef_s, adsMoveSpeedScale),						ASSET_ENTRY_FLOAT,0,0},
	{"sprintDurationScale",						offsetof(weaponDef_s, sprintDurationScale),						ASSET_ENTRY_FLOAT,0,0},
	{"adsZoomInFrac",							offsetof(weaponDef_s, adsZoomInFrac),							ASSET_ENTRY_FLOAT,0,0},
	{"adsZoomOutFrac",							offsetof(weaponDef_s, adsZoomOutFrac),							ASSET_ENTRY_FLOAT,0,0},
	{"AdsOverlayShader",						offsetof(weaponDef_s, AdsOverlayShader),						ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)material,0},
	{"AdsOverlayShaderLowRes",					offsetof(weaponDef_s, AdsOverlayShaderLowRes),					ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)material,0},
	{"AdsOverlayShaderEMP",						offsetof(weaponDef_s, AdsOverlayShaderEMP),						ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)material,0},
	{"AdsOverlayShaderEMPLowRes",				offsetof(weaponDef_s, AdsOverlayShaderEMPLowRes),				ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)material,0},
	{"adsOverlayReticle",						offsetof(weaponDef_s, adsOverlayReticle),						ASSET_ENTRY_ENUM,								(AssetEntry*)weapOverlayReticle_s,	WEAPOVERLAYRETICLE_MAX},
	{"adsOverlayInterface",						offsetof(weaponDef_s, adsOverlayInterface),						ASSET_ENTRY_ENUM,								(AssetEntry*)weapOverlayInterface_s,WEAPOVERLAYINTERFACE_MAX},
	{"adsOverlayWidth",							offsetof(weaponDef_s, adsOverlayWidth),							ASSET_ENTRY_FLOAT,0,0},
	{"adsOverlayHeight",						offsetof(weaponDef_s, adsOverlayHeight),						ASSET_ENTRY_FLOAT,0,0},
	{"adsOverlayWidthSplitscreen",				offsetof(weaponDef_s, adsOverlayWidthSplitscreen),				ASSET_ENTRY_FLOAT,0,0},
	{"adsOverlayHeightSplitscreen",				offsetof(weaponDef_s, adsOverlayHeightSplitscreen),				ASSET_ENTRY_FLOAT,0,0},
	{"adsBobFactor",							offsetof(weaponDef_s, fAdsBobFactor),							ASSET_ENTRY_FLOAT,0,0},
	{"adsViewBobMult",							offsetof(weaponDef_s, fAdsViewBobMult),							ASSET_ENTRY_FLOAT,0,0},
	{"hipSpread",								offsetof(weaponDef_s, fHipSpreadStandMin),						ASSET_ENTRY_EMBEDDED,							HipSpreadSettings,0},
	{"hipReticleSidePos",						offsetof(weaponDef_s, fHipReticleSidePos),						ASSET_ENTRY_FLOAT,0,0},
	{"adsIdleAmount",							offsetof(weaponDef_s, fAdsIdleAmount),							ASSET_ENTRY_FLOAT,0,0},
	{"hipIdleAmount",							offsetof(weaponDef_s, fHipIdleAmount),							ASSET_ENTRY_FLOAT,0,0},
	{"adsIdleSpeed",							offsetof(weaponDef_s, adsIdleSpeed),							ASSET_ENTRY_FLOAT,0,0},
	{"hipIdleSpeed",							offsetof(weaponDef_s, hipIdleSpeed),							ASSET_ENTRY_FLOAT,0,0},
	{"idleCrouchFactor",						offsetof(weaponDef_s, fIdleCrouchFactor),						ASSET_ENTRY_FLOAT,0,0},
	{"idleProneFactor",							offsetof(weaponDef_s, fIdleProneFactor),						ASSET_ENTRY_FLOAT,0,0},
	{"gunMaxPitch",								offsetof(weaponDef_s, fGunMaxPitch),							ASSET_ENTRY_FLOAT,0,0},
	{"gunMaxYaw",								offsetof(weaponDef_s, fGunMaxYaw),								ASSET_ENTRY_FLOAT,0,0},
	{"swayMaxAngle",							offsetof(weaponDef_s, swayMaxAngle),							ASSET_ENTRY_FLOAT,0,0},
	{"swayLerpSpeed",							offsetof(weaponDef_s, swayLerpSpeed),							ASSET_ENTRY_FLOAT,0,0},
	{"swayPitchScale",							offsetof(weaponDef_s, swayPitchScale),							ASSET_ENTRY_FLOAT,0,0},
	{"swayYawScale",							offsetof(weaponDef_s, swayYawScale),							ASSET_ENTRY_FLOAT,0,0},
	{"swayHorizScale",							offsetof(weaponDef_s, swayHorizScale),							ASSET_ENTRY_FLOAT,0,0},
	{"swayVertScale",							offsetof(weaponDef_s, swayVertScale),							ASSET_ENTRY_FLOAT,0,0},
	{"swayShellShockScale",						offsetof(weaponDef_s, swayShellShockScale),						ASSET_ENTRY_FLOAT,0,0},
	{"adsSwayMaxAngle",							offsetof(weaponDef_s, adsSwayMaxAngle),							ASSET_ENTRY_FLOAT,0,0},
	{"adsSwayLerpSpeed",						offsetof(weaponDef_s, adsSwayLerpSpeed),						ASSET_ENTRY_FLOAT,0,0},
	{"adsSwayPitchScale",						offsetof(weaponDef_s, adsSwayPitchScale),						ASSET_ENTRY_FLOAT,0,0},
	{"adsSwayYawScale",							offsetof(weaponDef_s, adsSwayYawScale),							ASSET_ENTRY_FLOAT,0,0},
	{"adsSwayHorizScale",						offsetof(weaponDef_s, adsSwayHorizScale),						ASSET_ENTRY_FLOAT,0,0},
	{"adsSwayVertScale",						offsetof(weaponDef_s, adsSwayVertScale),						ASSET_ENTRY_FLOAT,0,0},
	{"adsViewError",							offsetof(weaponDef_s, adsViewErrorMin),							ASSET_ENTRY_EMBEDDED,							MinMax_FloatEntry,0},
	{"physCollmap",								offsetof(weaponDef_s, collisions),								ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)phys_collmap,0},
	{"dualWieldViewModelOffset",				offsetof(weaponDef_s, dualWieldViewModelOffset),				ASSET_ENTRY_FLOAT,0,0},
	{"killIconRatio",							offsetof(weaponDef_s, killIconRatio),							ASSET_ENTRY_ENUM,								(AssetEntry*)weaponIconRatioType_s,	WEAPON_ICON_RATIO_MAX},
	{"reloadAmmoAdd",							offsetof(weaponDef_s, iReloadAmmoAdd),							ASSET_ENTRY_INT,0,0},
	{"reloadStartAdd",							offsetof(weaponDef_s, iReloadStartAdd),							ASSET_ENTRY_INT,0,0},
	{"dropAmmoMin",								offsetof(weaponDef_s, iDropAmmoMin),							ASSET_ENTRY_INT,0,0},
	{"ammoDropClipPercent",						offsetof(weaponDef_s, ammoDropClipPercentMin),					ASSET_ENTRY_EMBEDDED,							MinMax_IntEntry,0},
	{"explosionRadius",							offsetof(weaponDef_s, iExplosionRadius),						ASSET_ENTRY_INT,0,0},
	{"explosionRadiusMin",						offsetof(weaponDef_s, iExplosionRadiusMin),						ASSET_ENTRY_INT,0,0},
	{"explosionInnerDamage",					offsetof(weaponDef_s, iExplosionInnerDamage),					ASSET_ENTRY_INT,0,0},
	{"explosionOuterDamage",					offsetof(weaponDef_s, iExplosionOuterDamage),					ASSET_ENTRY_INT,0,0},
	{"damageConeAngle",							offsetof(weaponDef_s, damageConeAngle),							ASSET_ENTRY_FLOAT,0,0},
	{"bulletExplDmgMult",						offsetof(weaponDef_s, bulletExplDmgMult),						ASSET_ENTRY_FLOAT,0,0},
	{"bulletExplRadiusMult",					offsetof(weaponDef_s, bulletExplRadiusMult),					ASSET_ENTRY_FLOAT,0,0},
	{"projectileSpeed",							offsetof(weaponDef_s, iProjectileSpeed),						ASSET_ENTRY_INT,0,0},
	{"projectileSpeedUp",						offsetof(weaponDef_s, iProjectileSpeedUp),						ASSET_ENTRY_INT,0,0},
	{"projectileSpeedForward",					offsetof(weaponDef_s, iProjectileSpeedForward),					ASSET_ENTRY_INT,0,0},
	{"projectileActivateDist",					offsetof(weaponDef_s, iProjectileActivateDist),					ASSET_ENTRY_INT,0,0},
	{"projectileLifetime",						offsetof(weaponDef_s, projLifetime),							ASSET_ENTRY_FLOAT,0,0},
	{"timeToAccelerate",						offsetof(weaponDef_s, timeToAccelerate),						ASSET_ENTRY_FLOAT,0,0},
	{"projectileCurvature",						offsetof(weaponDef_s, projectileCurvature),						ASSET_ENTRY_FLOAT,0,0},
	{"projectileModel",							offsetof(weaponDef_s, projectileModel),							ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)xmodel,0},
	{"projExplosion",							offsetof(weaponDef_s, projExplosiveType),						ASSET_ENTRY_ENUM,								(AssetEntry*)weapProjExplosion_s,	WEAPPROJEXP_MAX},
	{"projExplosionEffect",						offsetof(weaponDef_s, projExplosionEffect),						ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)fx,0},
	{"projDudEffect",							offsetof(weaponDef_s, projDudEffect),							ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)fx,0},
	{"projExplosionSound",						offsetof(weaponDef_s, projExplosionSound),						ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"projDudSound",							offsetof(weaponDef_s, projDudSound),							ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"stickiness",								offsetof(weaponDef_s, stickiness),								ASSET_ENTRY_ENUM,								(AssetEntry*)WeapStickinessType_s,	WEAP_STICK_MAX},
	{"lowAmmoWarningThreshold",					offsetof(weaponDef_s, lowAmmoWarningThreshold),					ASSET_ENTRY_FLOAT,0,0},
	{"ricochetChance",							offsetof(weaponDef_s, ricochetChance),							ASSET_ENTRY_FLOAT,0,0},
	{"parallel",							offsetof(weaponDef_s, parallelBounce),							ASSET_ENTRY_CONTAINED,							BounceEntries,0},
	{"perpendicular",						offsetof(weaponDef_s, perpendicularBounce),						ASSET_ENTRY_CONTAINED,							BounceEntries,0},
	{"projTrailEffect",							offsetof(weaponDef_s, projTrailEffect),							ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)fx,0},
	{"projBeaconEffect",						offsetof(weaponDef_s, projBeaconEffect),						ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)fx,0},
	{"projectileRed",							offsetof(weaponDef_s, vProjectileColor[0]),						ASSET_ENTRY_FLOAT,0,0},
	{"projectileGreen",							offsetof(weaponDef_s, vProjectileColor[1]),						ASSET_ENTRY_FLOAT,0,0},
	{"projectileBlue",							offsetof(weaponDef_s, vProjectileColor[2]),						ASSET_ENTRY_FLOAT,0,0},
	{"guidedMissileType",						offsetof(weaponDef_s, guidedMissileType),						ASSET_ENTRY_ENUM,								(AssetEntry*)guidedMissileType_s,	MISSILE_GUIDANCE_MAX},
	{"maxSteeringAccel",						offsetof(weaponDef_s, maxSteeringAccel),						ASSET_ENTRY_FLOAT,0,0},
	{"projIgnitionDelay",						offsetof(weaponDef_s, projIgnitionDelay),						ASSET_ENTRY_FLOAT,0,0},
	{"projIgnitionEffect",						offsetof(weaponDef_s, projIgnitionEffect),						ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)fx,0},
	{"adsAimPitch",								offsetof(weaponDef_s, fAdsAimPitch),							ASSET_ENTRY_FLOAT,0,0},
	{"adsCrosshairInFrac",						offsetof(weaponDef_s, fAdsCrosshairInFrac),						ASSET_ENTRY_FLOAT,0,0},
	{"adsCrosshairOutFrac",						offsetof(weaponDef_s, fAdsCrosshairOutFrac),					ASSET_ENTRY_FLOAT,0,0},
	{"adsGunKick",						offsetof(weaponDef_s, adsGunKickReducedKickBullets),					ASSET_ENTRY_EMBEDDED,							GunKickSettings,0},
	{"adsViewKickPitch",						offsetof(weaponDef_s, fAdsViewKickPitchMin),					ASSET_ENTRY_EMBEDDED,							MinMax_FloatEntry,0},
	{"adsViewKickYaw",							offsetof(weaponDef_s, fAdsViewKickYawMin),						ASSET_ENTRY_EMBEDDED,							MinMax_FloatEntry,0},
	{"adsViewScatter",							offsetof(weaponDef_s, fAdsViewScatterMin),						ASSET_ENTRY_EMBEDDED,							MinMax_FloatEntry,0},
	{"adsSpread",								offsetof(weaponDef_s, fAdsSpread),								ASSET_ENTRY_FLOAT,0,0},
	{"hipGunKick",							offsetof(weaponDef_s, hipGunKickReducedKickBullets),						ASSET_ENTRY_EMBEDDED,							GunKickSettings,0},
	{"hipViewKickPitch",						offsetof(weaponDef_s, fHipViewKickPitchMin),					ASSET_ENTRY_EMBEDDED,							MinMax_FloatEntry,0},
	{"hipViewKickYaw",							offsetof(weaponDef_s, fHipViewKickYawMin),						ASSET_ENTRY_EMBEDDED,							MinMax_FloatEntry,0},
	{"hipViewScatter",							offsetof(weaponDef_s, fHipViewScatterMin),						ASSET_ENTRY_EMBEDDED,							MinMax_FloatEntry,0},
	{"fightDist",								offsetof(weaponDef_s, fightDist),								ASSET_ENTRY_FLOAT,0,0},
	{"maxDist",									offsetof(weaponDef_s, maxDist),									ASSET_ENTRY_FLOAT,0,0},
	{"aiVsAiAccuracyGraph",						offsetof(weaponDef_s, accuracyGraphName[0]),					ASSET_ENTRY_STRING,0,0},
	{"aiVsPlayerAccuracyGraph",					offsetof(weaponDef_s, accuracyGraphName[1]),					ASSET_ENTRY_STRING,0,0},
	{"positionReloadTransTime",					offsetof(weaponDef_s, iPositionReloadTransTime),				ASSET_ENTRY_INT,0,0},
	{"leftArc",									offsetof(weaponDef_s, leftArc),									ASSET_ENTRY_FLOAT,0,0},
	{"rightArc",								offsetof(weaponDef_s, rightArc),								ASSET_ENTRY_FLOAT,0,0},
	{"topArc",									offsetof(weaponDef_s, topArc),									ASSET_ENTRY_FLOAT,0,0},
	{"bottomArc",								offsetof(weaponDef_s, bottomArc),								ASSET_ENTRY_FLOAT,0,0},
	{"accuracy",								offsetof(weaponDef_s, accuracy),								ASSET_ENTRY_FLOAT,0,0},
	{"aiSpread",								offsetof(weaponDef_s, aiSpread),								ASSET_ENTRY_FLOAT,0,0},
	{"playerSpread",							offsetof(weaponDef_s, playerSpread),							ASSET_ENTRY_FLOAT,0,0},
	{"minVertTurnSpeed",						offsetof(weaponDef_s, minVertTurnSpeed),						ASSET_ENTRY_FLOAT,0,0},
	{"minHorTurnSpeed",							offsetof(weaponDef_s, minHorTurnSpeed),							ASSET_ENTRY_FLOAT,0,0},
	{"maxVertTurnSpeed",						offsetof(weaponDef_s, maxVertTurnSpeed),						ASSET_ENTRY_FLOAT,0,0},
	{"maxHorTurnSpeed",							offsetof(weaponDef_s, maxHorTurnSpeed),							ASSET_ENTRY_FLOAT,0,0},
	{"pitchConvergenceTime",					offsetof(weaponDef_s, pitchConvergenceTime),					ASSET_ENTRY_FLOAT,0,0},
	{"yawConvergenceTime",						offsetof(weaponDef_s, yawConvergenceTime),						ASSET_ENTRY_FLOAT,0,0},
	{"suppressTime",							offsetof(weaponDef_s, suppressTime),							ASSET_ENTRY_FLOAT,0,0},
	{"maxRange",								offsetof(weaponDef_s, maxRange),								ASSET_ENTRY_FLOAT,0,0},
	{"animHorRotateInc",						offsetof(weaponDef_s, fAnimHorRotateInc),						ASSET_ENTRY_FLOAT,0,0},
	{"playerPositionDist",						offsetof(weaponDef_s, fPlayerPositionDist),						ASSET_ENTRY_FLOAT,0,0},
	{"useHintString",							offsetof(weaponDef_s, szUseHintString),							ASSET_ENTRY_STRING,0,0},
	{"dropHintString",							offsetof(weaponDef_s, dropHintString),							ASSET_ENTRY_STRING,0,0},
	{"useHintStringIndex",						offsetof(weaponDef_s, iUseHintStringIndex),						ASSET_ENTRY_INT,0,0},
	{"dropHintStringIndex",						offsetof(weaponDef_s, dropHintStringIndex),						ASSET_ENTRY_INT,0,0},
	{"horizViewJitter",							offsetof(weaponDef_s, horizViewJitter),							ASSET_ENTRY_FLOAT,0,0},
	{"vertViewJitter",							offsetof(weaponDef_s, vertViewJitter),							ASSET_ENTRY_FLOAT,0,0},
	{"scanSpeed",								offsetof(weaponDef_s, scanSpeed),								ASSET_ENTRY_FLOAT,0,0},
	{"scanAccel",								offsetof(weaponDef_s, scanAccel),								ASSET_ENTRY_FLOAT,0,0},
	{"scanPauseTime",							offsetof(weaponDef_s, scanPauseTime),							ASSET_ENTRY_TIME_AS_INT,0,0},
	{"script",									offsetof(weaponDef_s, szScript),								ASSET_ENTRY_STRING,0,0},
	{"minDamage",								offsetof(weaponDef_s, minDamage),								ASSET_ENTRY_INT,0,0},
	{"minPlayerDamage",							offsetof(weaponDef_s, minPlayerDamage),							ASSET_ENTRY_INT,0,0},
	{"maxDamageRange",							offsetof(weaponDef_s, maxDamageRange),							ASSET_ENTRY_FLOAT,0,0},
	{"minDamageRange",							offsetof(weaponDef_s, minDamageRange),							ASSET_ENTRY_FLOAT,0,0},
	{"destabilizationRateTime",					offsetof(weaponDef_s, destabilizationRateTime),					ASSET_ENTRY_FLOAT,0,0},
	{"destabilizationCurvatureMax",				offsetof(weaponDef_s, destabilizationCurvatureMax),				ASSET_ENTRY_FLOAT,0,0},
	{"destabilizeDistance",						offsetof(weaponDef_s, destabilizeDistance),						ASSET_ENTRY_INT,0,0},
	{"loc",				offsetof(weaponDef_s, locationDamageMultipliers),				ASSET_ENTRY_CONTAINED,							LocDamageMultEntries,0},
	{"fireRumble",								offsetof(weaponDef_s, fireRumble),								ASSET_ENTRY_STRING,0,0},
	{"meleeImpactRumble",						offsetof(weaponDef_s, meleeImpactRumble),						ASSET_ENTRY_STRING,0,0},
	{"tracer",									offsetof(weaponDef_s, tracer),									ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)tracer,0},
	{"turretScopeZoomRate",						offsetof(weaponDef_s, turretScopeZoomRate),						ASSET_ENTRY_INT,0,0},
	{"turretScopeZoom",						offsetof(weaponDef_s, turretScopeZoomMin),					ASSET_ENTRY_EMBEDDED,							MinMax_IntEntry,0},
	{"turretOverheatUpRate",					offsetof(weaponDef_s, turretOverheatUpRate),					ASSET_ENTRY_INT,0,0},
	{"turretOverheatDownRate",					offsetof(weaponDef_s, turretOverheatDownRate),					ASSET_ENTRY_INT,0,0},
	{"turretOverheatPenalty",					offsetof(weaponDef_s, turretOverheatPenalty),					ASSET_ENTRY_INT,0,0},
	{"turretOverheatSound",						offsetof(weaponDef_s, turretOverheatSound),						ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"turretOverheatEffect",					offsetof(weaponDef_s, turretOverheatEffect),					ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)fx,0},
	{"turretBarrelSpin",					offsetof(weaponDef_s, turretBarrelSpinRumble),					ASSET_ENTRY_EMBEDDED,							TurretBarrelSpinEntries,0},
	{"missileConeSound",						offsetof(weaponDef_s, missileConeSoundAlias),					ASSET_ENTRY_EMBEDDED,						MissileConeSoundEntries,0},
	{"shareAmmo",								offsetof(weaponDef_s, shareAmmo),								ASSET_ENTRY_BOOL,0,0},
	{"lockonSupported",							offsetof(weaponDef_s, lockonSupported),							ASSET_ENTRY_BOOL,0,0},
	{"requireLockonToFire",						offsetof(weaponDef_s, requireLockonToFire),						ASSET_ENTRY_BOOL,0,0},
	{"bigExplosion",							offsetof(weaponDef_s, bigExplosion),							ASSET_ENTRY_BOOL,0,0},
	{"noAdsWhenMagEmpty",						offsetof(weaponDef_s, noAdsWhenMagEmpty),						ASSET_ENTRY_BOOL,0,0},
	{"avoidDropCleanup",						offsetof(weaponDef_s, avoidDropCleanup),						ASSET_ENTRY_BOOL,0,0},
	{"inheritsPerks",							offsetof(weaponDef_s, inheritsPerks),							ASSET_ENTRY_BOOL,0,0},
	{"crosshairColorChange",					offsetof(weaponDef_s, crosshairColorChange),					ASSET_ENTRY_BOOL,0,0},
	{"rifleBullet",								offsetof(weaponDef_s, rifleBullet),								ASSET_ENTRY_BOOL,0,0},
	{"armorPiercing",							offsetof(weaponDef_s, armorPiercing),							ASSET_ENTRY_BOOL,0,0},
	{"boltAction",								offsetof(weaponDef_s, boltAction),								ASSET_ENTRY_BOOL,0,0},
	{"aimDownSight",							offsetof(weaponDef_s, aimDownSight),							ASSET_ENTRY_BOOL,0,0},
	{"rechamberWhileAds",						offsetof(weaponDef_s, rechamberWhileAds),						ASSET_ENTRY_BOOL,0,0},
	{"bulletExplosiveDamage",					offsetof(weaponDef_s, bBulletExplosiveDamage),					ASSET_ENTRY_BOOL,0,0},
	{"cookOffHold",								offsetof(weaponDef_s, cookOffHold),								ASSET_ENTRY_BOOL,0,0},
	{"clipOnly",								offsetof(weaponDef_s, clipOnly),								ASSET_ENTRY_BOOL,0,0},
	{"noAmmoPickup",							offsetof(weaponDef_s, noAmmoPickup),							ASSET_ENTRY_BOOL,0,0},
	{"adsFire",									offsetof(weaponDef_s, adsFire),									ASSET_ENTRY_BOOL,0,0},
	{"cancelAutoHolsterWhenEmpty",				offsetof(weaponDef_s, cancelAutoHolsterWhenEmpty),				ASSET_ENTRY_BOOL,0,0},
	{"disableSwitchToWhenEmpty",				offsetof(weaponDef_s, disableSwitchToWhenEmpty),				ASSET_ENTRY_BOOL,0,0},
	{"suppressAmmoReserveDisplay",				offsetof(weaponDef_s, suppressAmmoReserveDisplay),				ASSET_ENTRY_BOOL,0,0},
	{"laserSightDuringNightvision",				offsetof(weaponDef_s, laserSightDuringNightvision),				ASSET_ENTRY_BOOL,0,0},
	{"markableViewmodel",						offsetof(weaponDef_s, markableViewmodel),						ASSET_ENTRY_BOOL,0,0},
	{"noDualWield",								offsetof(weaponDef_s, noDualWield),								ASSET_ENTRY_BOOL,0,0},
	{"flipKillIcon",							offsetof(weaponDef_s, flipKillIcon),							ASSET_ENTRY_BOOL,0,0},
	{"noPartialReload",							offsetof(weaponDef_s, noPartialReload),							ASSET_ENTRY_BOOL,0,0},
	{"segmentedReload",							offsetof(weaponDef_s, segmentedReload),							ASSET_ENTRY_BOOL,0,0},
	{"blocksProne",								offsetof(weaponDef_s, blocksProne),								ASSET_ENTRY_BOOL,0,0},
	{"silenced",								offsetof(weaponDef_s, silenced),								ASSET_ENTRY_BOOL,0,0},
	{"isRollingGrenade",						offsetof(weaponDef_s, isRollingGrenade),						ASSET_ENTRY_BOOL,0,0},
	{"projExplosionEffectForceNormalUp",		offsetof(weaponDef_s, projExplosionEffectForceNormalUp),		ASSET_ENTRY_BOOL,0,0},
	{"projImpactExplode",						offsetof(weaponDef_s, projImpactExplode),						ASSET_ENTRY_BOOL,0,0},
	{"stickToPlayers",							offsetof(weaponDef_s, stickToPlayers),							ASSET_ENTRY_BOOL,0,0},
	{"hasDetonator",							offsetof(weaponDef_s, hasDetonator),							ASSET_ENTRY_BOOL,0,0},
	{"disableFiring",							offsetof(weaponDef_s, disableFiring),							ASSET_ENTRY_BOOL,0,0},
	{"timedDetonation",							offsetof(weaponDef_s, timedDetonation),							ASSET_ENTRY_BOOL,0,0},
	{"rotate",									offsetof(weaponDef_s, rotate),									ASSET_ENTRY_BOOL,0,0},
	{"holdButtonToThrow",						offsetof(weaponDef_s, holdButtonToThrow),						ASSET_ENTRY_BOOL,0,0},
	{"freezeMovementWhenFiring",				offsetof(weaponDef_s, freezeMovementWhenFiring),				ASSET_ENTRY_BOOL,0,0},
	{"thermalScope",							offsetof(weaponDef_s, thermalScope),							ASSET_ENTRY_BOOL,0,0},
	{"altModeSameWeapon",						offsetof(weaponDef_s, altModeSameWeapon),						ASSET_ENTRY_BOOL,0,0},
	{"turretBarrelSpinEnabled",					offsetof(weaponDef_s, turretBarrelSpinEnabled),					ASSET_ENTRY_BOOL,0,0},
	{"missileConeSoundEnabled",					offsetof(weaponDef_s, missileConeSoundEnabled),					ASSET_ENTRY_BOOL,0,0},
	{"missileConeSoundPitchshiftEnabled",		offsetof(weaponDef_s, missileConeSoundPitchshiftEnabled),		ASSET_ENTRY_BOOL,0,0},
	{"missileConeSoundCrossfadeEnabled",		offsetof(weaponDef_s, missileConeSoundCrossfadeEnabled),		ASSET_ENTRY_BOOL,0,0},
	{"offhandHoldIsCancelable",					offsetof(weaponDef_s, offhandHoldIsCancelable),					ASSET_ENTRY_BOOL,0,0},
	{0, 0, (AssetEntryType)0,0,0}		
};

const AssetEntry WeaponVarientDefEntries[] = {
	{"displayName",				offsetof(weaponVarientDef_s, szDisplayName),			ASSET_ENTRY_STRING,0,0},
	{"",				offsetof(weaponVarientDef_s, WeaponDef),				ASSET_ENTRY_CONTAINED,					WeaponDefEntries,0},
	{"",					offsetof(weaponVarientDef_s, szXAnims),					ASSET_ENTRY_CONTAINED,					AnimEntries,0},
	{"adsZoomFov",				offsetof(weaponVarientDef_s, adsZoomFov),				ASSET_ENTRY_FLOAT,0,0},
	{"adsTransInTime",			offsetof(weaponVarientDef_s, adsTransInTime),			ASSET_ENTRY_TIME_AS_INT,0,0},
	{"adsTransOutTime",			offsetof(weaponVarientDef_s, adsTransInTime),			ASSET_ENTRY_TIME_AS_INT,0,0},
	{"clipSize",				offsetof(weaponVarientDef_s, clipSize),					ASSET_ENTRY_UNSIGNED_INT,0,0},
	{"impactType",				offsetof(weaponVarientDef_s, impactType),				ASSET_ENTRY_ENUM,						(AssetEntry*)ImpactType_s,			IMPACT_TYPE_MAX},
	{"fireTime",				offsetof(weaponVarientDef_s, fireTime),					ASSET_ENTRY_TIME_AS_INT,0,0},
	{"dpadIconRatio",			offsetof(weaponVarientDef_s, dpadIconRatio),			ASSET_ENTRY_ENUM,						(AssetEntry*)weaponIconRatioType_s,	WEAPON_ICON_RATIO_MAX},
	{"penetrateMultiplier",		offsetof(weaponVarientDef_s, penetrateMultiplier),		ASSET_ENTRY_FLOAT,0,0},
	{"adsViewKickCenterSpeed",	offsetof(weaponVarientDef_s, adsViewKickCenterSpeed),	ASSET_ENTRY_FLOAT,0,0},
	{"hipViewKickCenterSpeed",	offsetof(weaponVarientDef_s, hipViewKickCenterSpeed),	ASSET_ENTRY_FLOAT,0,0},
	{"altWeaponName",			offsetof(weaponVarientDef_s, szAltWeaponName),			ASSET_ENTRY_STRING,0,0},
	{"altWeaponIndex",			offsetof(weaponVarientDef_s, altWeaponIndex),			ASSET_ENTRY_INT,0,0},
	{"altRaiseTime",			offsetof(weaponVarientDef_s, altRaiseTime),				ASSET_ENTRY_TIME_AS_INT,0,0},
	{"killIcon",				offsetof(weaponVarientDef_s, killIcon),					ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)material,0},
	{"dpadIcon",				offsetof(weaponVarientDef_s, dpadIcon),					ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)material,0},
	{"firstRaiseTime",			offsetof(weaponVarientDef_s, firstRaiseTime),			ASSET_ENTRY_TIME_AS_INT,0,0},
	{"dropAmmoMax",				offsetof(weaponVarientDef_s, dropAmmoMax),				ASSET_ENTRY_INT,0,0},
	{"adsDofStart",				offsetof(weaponVarientDef_s, adsDofStart),				ASSET_ENTRY_FLOAT,0,0},
	{"adsDofEnd",				offsetof(weaponVarientDef_s, adsDofEnd),				ASSET_ENTRY_FLOAT,0,0},
	{"motionTracker",			offsetof(weaponVarientDef_s, motionTracker),			ASSET_ENTRY_BOOL,0,0},
	{"enhanced",				offsetof(weaponVarientDef_s, enhanced),					ASSET_ENTRY_BOOL,0,0},
	{"dpadIconShowsAmmo",		offsetof(weaponVarientDef_s, dpadIconShowsAmmo),		ASSET_ENTRY_BOOL,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

int weaponVarientDef_s::dumpWeaponVarient()
{
	sprintf(MasterCharBuffer, "game:\\dump\\weapons\\mp\\%s", szInternalName);
	CreateDirectoryB(MasterCharBuffer);

	if(fileExists(MasterCharBuffer))
		return ERROR_DUP_NAME;

	FILE * writeLog = fopen(MasterCharBuffer, "w");

	if(!writeLog)
		return ERROR_BAD_NETPATH;

	fprintf(writeLog,"WEAPONFILE");

	for(int i = 0; WeaponVarientDefEntries[i].name; i++, contemplatingCurrentSecondary++)
		DumpAssetEntry(writeLog, WeaponVarientDefEntries[i].type, WeaponVarientDefEntries[i].name, (void*)((int)(this) + WeaponVarientDefEntries[i].offset), WeaponVarientDefEntries[i].next, WeaponVarientDefEntries[i].enumCount);

	fclose(writeLog);
	return ERROR_SUCCESS;
}

void weaponVarientDef_s::dumpAllWeapons()
{
	contemplatingTotalPrimary = ((unsigned int)g_weaponPool->freeHead - (unsigned int)g_weaponPool)/sizeof(weaponVarientDef_s) + 1;
	int assetCount = contemplatingCurrentSecondary = contemplatingCurrentPrimary = 0;
	strcpy(contemplatorTextPrimary, "Dumping Weapons");
	contemplatingTotalSecondary = 26;
	isContemplatingSecondary = isContemplatingPrimary = true;
	for(int i = 0; i < 0x578 &&
		g_weaponPool->freeHead != &g_weaponPool->entries[i].entry &&
		g_weaponPool->entries[i].next != &g_weaponPool->entries[i+1].entry &&
		g_weaponPool->entries[i].next; i++, contemplatingCurrentPrimary++)
	{
		contemplatingCurrentSecondary = 0;
		sprintf(contemplatorTextSecondary, "Dumping %.20s%s", g_weaponPool->entries[i].entry.szInternalName, strlen(g_weaponPool->entries[i].entry.szInternalName) > 20 ? "...":"");

		if(g_weaponPool->entries[i].entry.dumpWeaponVarient() != ERROR_SUCCESS)
			printf("Failed to dump weapon %s\n", g_weaponPool->entries[i].entry.szInternalName);
		else
			assetCount++;
	}

	printf("Dumped %i weapons\n", assetCount);
	isContemplatingPrimary = isContemplatingSecondary = false;
	contemplatingTotalPrimary = contemplatingCurrentPrimary = contemplatingTotalSecondary = contemplatingCurrentSecondary = -1;
}

void Load_weaponVarientDef_tAssetHook(void ** input)
{
	weaponVarientDef_s * currentWeaponVarientDef = *(weaponVarientDef_s**)input;

	//Can we load this weapon custom?
	while(usingAssetBuffer) Sleep(10);

	usingAssetBuffer = true;
	ZeroMemory(theAssetBuffer, 0x4000);
	sprintf(theAssetBuffer, "game:\\raw\\weapons\\mp\\%s", currentWeaponVarientDef->szInternalName);
	if(fileExists(theAssetBuffer))
	{
		HANDLE weaponDefFile = CreateFile(theAssetBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		int numberOfBytesRead;
		if(weaponDefFile != INVALID_HANDLE_VALUE && ReadFile(weaponDefFile, theAssetBuffer, GetFileSize(weaponDefFile, NULL), (LPDWORD)&numberOfBytesRead, 0))
		{
			char * currentName = 0, * currentValue = 0;

			CloseHandle(weaponDefFile);
			char * currentToken = improved_strtok(theAssetBuffer,"\\");

			if(!strcmp(currentToken, "WEAPONFILE"))
			for(currentToken = improved_strtok(NULL,"\\"); currentToken; currentToken = improved_strtok(NULL,"\\"))
			{
				currentName = currentToken;
				currentValue = improved_strtok(NULL,"\\");

				if(!WriteAssetEntry(WeaponVarientDefEntries, currentName, currentValue, currentWeaponVarientDef))
					printf("Cannont handle %s with value %s in weapon %s, it isn't supported.\n", currentName, currentValue, currentWeaponVarientDef->szInternalName);
			}
		}
	}
	usingAssetBuffer = false;

	weaponVarientDefLoaderSaveStub(input);
}
#pragma endregion

#pragma region rawfile (0x22)
typedef bool (__cdecl * DB_IsXAssetDefault_t)(XAssetType type, const char *name);
DB_IsXAssetDefault_t DB_IsXAssetDefault = (DB_IsXAssetDefault_t)0x82197738;

typedef void (__cdecl * unknownFunc1_t)(Rawfile * arg1, int, int);
unknownFunc1_t unknownFunc1 = (unknownFunc1_t)0x8219BA98;

void * customLoadedRawfiles[0x40];
int customLoadedRawfileCount = 0;
int Rawfile_GetAsset(char* filename, int unknown1, int unknown2)
{
	if(!filename || filename == (char*)-1 || DB_IsXAssetDefault(rawfile, filename))
		return 0;
	else
	{
		Rawfile * fileToUse = 0, *oldRawfileUsed = 0;

		//Have we loaded this rawfile custom before?
		for(int i = 0; i < customLoadedRawfileCount; i++)
		{
			if(!customLoadedRawfiles[i])
				break;

			if(!strcmp(filename, ((Rawfile*)customLoadedRawfiles[i])->name))
				fileToUse = ((Rawfile*)customLoadedRawfiles[i]);
		}
		
		if(!fileToUse) 
			oldRawfileUsed = DB_FindXAssetHeader(rawfile, filename).rawfile;

		//Can we load this rawfile custom?
		while(usingAssetBuffer) Sleep(10);
		
		usingAssetBuffer = true;
		sprintf(theAssetBuffer, "game:\\raw\\%s", ConvertAssetNameToFileName(filename));
		if(!fileToUse && fileExists(theAssetBuffer))
		{
			HANDLE rawfileFile = CreateFile(theAssetBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			int numberOfBytesRead;
 			if(rawfileFile != INVALID_HANDLE_VALUE && ReadFile(rawfileFile, theAssetBuffer, GetFileSize(rawfileFile, NULL), (LPDWORD)&numberOfBytesRead, 0))
			{
				Rawfile * newRawfile = oldRawfileUsed ? oldRawfileUsed : new Rawfile;

				if(!oldRawfileUsed)
				{
					newRawfile->name = new char[strlen(filename) + 1];
					ZeroMemory(newRawfile->name, strlen(filename) + 1);
					strcpy(newRawfile->name, filename);
				}

				//I know its highly unlikely that the new decompressed size will be less than the old compressed size, but add a check for it cause that'd be nice...
				if(!oldRawfileUsed || (oldRawfileUsed->compressedSize && oldRawfileUsed->compressedSize < numberOfBytesRead + 1) || (!oldRawfileUsed->compressedSize && oldRawfileUsed->decompressedSize < numberOfBytesRead))
				{
					newRawfile->data = new char[numberOfBytesRead+1];
					ZeroMemory(newRawfile->data, numberOfBytesRead+1);
					memcpy(newRawfile->data, theAssetBuffer, numberOfBytesRead);
				}
				else
				{
					ZeroMemory(newRawfile->data, !newRawfile->compressedSize ? newRawfile->decompressedSize + 1 : newRawfile->compressedSize);
					memcpy(newRawfile->data, theAssetBuffer, numberOfBytesRead);
				}

				usingAssetBuffer = false;
				newRawfile->compressedSize = 0;
				newRawfile->decompressedSize = numberOfBytesRead;
				customLoadedRawfiles[customLoadedRawfileCount] = fileToUse = newRawfile;
				CloseHandle(rawfileFile);
				customLoadedRawfileCount++;
			}
		}
		usingAssetBuffer = false;

		if(!fileToUse)
			fileToUse = oldRawfileUsed;

		unknownFunc1(fileToUse, unknown1, unknown2);
		return unknown1;
	}
}
#pragma endregion

#pragma region stringtable (0x23)
int StringTable::dumpStringTable()
{
	sprintf(MasterCharBuffer, "game:\\dump\\stringtables\\%s", ConvertAssetNameToFileName(name));
	CreateDirectoryB(MasterCharBuffer);

	if(fileExists(MasterCharBuffer))
		return ERROR_DUP_NAME;

	FILE * writeLog = fopen(MasterCharBuffer, "w");

	if(!writeLog)
		return ERROR_BAD_NETPATH;

	for(int j = 0; j < rowCount; j++)
	for(int k = 0; k < columnCount; k++)
		fprintf(writeLog, k == columnCount-1 ? "%s\n" : "%s,", values[columnCount * j + k].value);

	fclose(writeLog);
	return ERROR_SUCCESS;
}

void StringTable::DumpAllStringTables()
{
	contemplatingTotalPrimary = ((unsigned int)g_stringTablePool->freeHead - (unsigned int)g_stringTablePool)/sizeof(StringTable) + 1;
	int assetCount = contemplatingCurrentPrimary = 0;
	int returnValue;
	strcpy(contemplatorTextPrimary, "Dumping Stringtables");
	isContemplatingPrimary = true;
	for(int i = 0; i < 0x190 &&
		g_stringTablePool->freeHead != &g_stringTablePool->entries[i].entry &&
		g_stringTablePool->entries[i].next != &g_stringTablePool->entries[i+1].entry; i++, contemplatingCurrentPrimary++)
		if((returnValue = g_stringTablePool->entries[i].entry.dumpStringTable()) == ERROR_SUCCESS)
			assetCount++;
		else if(returnValue == ERROR_BAD_NETPATH)
			printf("Failed to dump %s stringtable\n", g_stringTablePool->entries[i].entry.name);

	isContemplatingPrimary = false;
	contemplatingTotalPrimary = contemplatingCurrentPrimary = -1;

	printf("Dumped %i stringtables\n", assetCount);
}

void * customLoadedTables[0x20];
int customLoadedTableCount = 0;
void StringTableEntry::UpdateHash()
{
	hash = 0;
	char * data = value;

	while (*data)
	{
		hash = tolower(*data) + (31 * hash);

		data++;
	}
}

char * oldValues[0x100];
int oldValueCount = 0;
//strtk::token_grid::options strtkGridOptions(strtk::split_options::default_mode, strtk::split_options::default_mode, "\r\n", ",");
void StringTable_GetAsset(char* filename, void** table)
{
	StringTable * oldTable = DB_FindXAssetHeader(stringtable, filename).stringTable;

	//Have we loaded this stringtable custom before?
	/*for(int i = 0; i < customLoadedTableCount; i++)
	{
		if(!customLoadedTables[i])
			break;

		if(!strcmpi(filename, ((StringTable*)customLoadedTables[i])->name))
		{
			*table = customLoadedTables[i];
			return;
		}
	}

	//Can we load this stringtable custom?
	while(usingAssetBuffer) Sleep(10);
	
	usingAssetBuffer = true;
	sprintf(theAssetBuffer, "game:\\raw\\%s", ConvertAssetNameToFileName(filename));
	if(fileExists(theAssetBuffer))
	{
		StringTable* newTable = 0;
		HANDLE rawStringtable = CreateFile(theAssetBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		RtlZeroMemory(theAssetBuffer, 0x4000);
		int rawStringtableLength;
		if(rawStringtable != INVALID_HANDLE_VALUE && ReadFile(rawStringtable, theAssetBuffer, GetFileSize(rawStringtable, NULL), (LPDWORD)&rawStringtableLength, 0))
		{
			CloseHandle(rawStringtable);
			strtk::token_grid grid(theAssetBuffer, rawStringtableLength, strtkGridOptions);
			int columns = grid.max_column_count();
			int rows = grid.row_count();

			StringTableEntry * oldTableValues = 0;
			if(!oldTable)
			{
				newTable = new StringTable;
				newTable->name = new char[strlen(filename) + 1];
				ZeroMemory(newTable->name, strlen(filename) + 1);
				strcpy(newTable->name, filename);
			}
			else
			{
				newTable = oldTable;
				oldTableValues = newTable->values;
			}

			if(!oldTable || oldTable->rowCount * oldTable->columnCount < rows * columns)
			{
				newTable->values = new StringTableEntry[rows * columns];
				ZeroMemory(newTable->values, rows * columns * sizeof(StringTableEntry));
			}

			for (std::size_t ri = 0; ri < (std::size_t)rows; ri++)
			{
				strtk::token_grid::row_type r = grid.row(ri);

				for (std::size_t ci = 0; ci < r.size(); ci++)
				{
					std::string strSource = r.get<std::string>(ci);
					if(!oldTable ||													//There is no oldTable OR
						oldTable->rowCount * oldTable->columnCount < rows * columns && (ri >= oldTable->rowCount || ci >= oldTable->columnCount) ||	//The oldTable itemlist was too small and we are out of the range of the old values OR
						strcmp(strSource.c_str(), oldTableValues[(ri * oldTable->columnCount) + ci].value))	//The new value is not the old value
					{
						char * str = 0;

						for(int superLoop = 0; superLoop < oldValueCount; superLoop++)
						if(!stricmp(oldValues[superLoop], strSource.c_str()))
						{	str = oldValues[superLoop];	}

						if(!str)
						{
							str = new char[strSource.length() + 1];
							strcpy(str, strSource.c_str());
							if(oldValueCount < 0x200)
							{
								oldValues[oldValueCount] = str;
								oldValueCount++;
							}
						}
						newTable->values[(ri * columns) + ci].value = str;
					}
					else if(oldTable && //There is an oldTable AND
						oldTable->rowCount * oldTable->columnCount >= rows * columns || (ri < oldTable->rowCount && ci < oldTable->columnCount))	//We are inside the range of the old values
						newTable->values[(ri * columns) + ci].value = oldTableValues[(ri * oldTable->columnCount) + ci].value;
					
					newTable->values[(ri * columns) + ci].UpdateHash();
				}
			}
			grid.clear();

			newTable->columnCount = columns;
			newTable->rowCount = rows;
			customLoadedTables[customLoadedTableCount] = newTable;
			*table = newTable;
			customLoadedTableCount++;
			usingAssetBuffer = false;
			return;
		}
	}
	usingAssetBuffer = false;

	//We can't do anything custom, do what StringTable_GetAsset normally does...*/
	*table = oldTable;
}
#pragma endregion

#pragma region structureddatadef (0x25)
int StructuredDataDef::DumpSDD()
{
	sprintf(MasterCharBuffer, "game:\\dump\\sdd\\%s", ConvertAssetNameToFileName(name));
	CreateDirectoryB(MasterCharBuffer);

	if(fileExists(MasterCharBuffer))
		return ERROR_DUP_NAME;

	FILE * writeLog = fopen(MasterCharBuffer, "w");

	if(!writeLog)
		return ERROR_BAD_NETPATH;

	fprintf(writeLog, "//Internal Structured Information Count = %i\n\n", structureCount);

	for(int i = 0; i < structureCount; i++)
	{
		fprintf(writeLog, "//Infomation %i\n//Version: %i\n//Hash: %i\n\n", i, internalStructure[i].version, internalStructure[i].hash);

		for(int j = 0; j < internalStructure[i].enumCount; j++)
		{
			fprintf(writeLog, "enum e%i\n{\n", j);

			for(int k = 0; k < internalStructure[i].enums[j].entryCount; k++)
				fprintf(writeLog, "\t%s = %i%s\n", internalStructure[i].enums[j].entries[k].name, internalStructure[i].enums[j].entries[k].index, (k == internalStructure[i].enums[j].entryCount-1) ? "" : ",");

			fprintf(writeLog,"}\n\n");
		}

		for(int j = 0; j < internalStructure[i].structureCount; j++)
		{
			fprintf(writeLog, "struct s%i\n{\n", j);

			for(int k = 0; k < internalStructure[i].structures[j].entryCount; k++)
			{
				fprintf(writeLog, "\t//Index: %i\n", internalStructure[i].structures[j].entries[k].item.offset);
				switch(internalStructure[i].structures[j].entries[k].item.type)
				{
				case STRUCTURED_DATA_INT:
					fprintf(writeLog, "\tint %s;\n\n", internalStructure[i].structures[j].entries[k].name);break;
				case STRUCTURED_DATA_BYTE:
					fprintf(writeLog, "\tbyte %s;\n\n", internalStructure[i].structures[j].entries[k].name);break;
				case STRUCTURED_DATA_BOOL:
					fprintf(writeLog, "\tbool %s;\n\n", internalStructure[i].structures[j].entries[k].name);break;
				case STRUCTURED_DATA_STRING:
					fprintf(writeLog, "\tchar * %s;\n\n", internalStructure[i].structures[j].entries[k].name);break;
				case STRUCTURED_DATA_ENUM:
					fprintf(writeLog, "\te%i %s;\n\n", internalStructure[i].structures[j].entries[k].item.index, internalStructure[i].structures[j].entries[k].name);break;
				case STRUCTURED_DATA_STRUCT:
					fprintf(writeLog, "\ts%i %s;\n\n", internalStructure[i].structures[j].entries[k].item.index, internalStructure[i].structures[j].entries[k].name);break;
				case STRUCTURED_DATA_FLOAT:
					fprintf(writeLog, "\tfloat %s;\n\n", internalStructure[i].structures[j].entries[k].name);break;
				case STRUCTURED_DATA_SHORT:
					fprintf(writeLog, "\tshort %s;\n\n", internalStructure[i].structures[j].entries[k].name);break;
				default:
					fprintf(writeLog, "\t//Extra Data: %i\n\tu%i %s;\n\n", internalStructure[i].structures[j].entries[k].item.index, internalStructure[i].structures[j].entries[k].item.type, internalStructure[i].structures[j].entries[k].name);break;
				}
			}

			fprintf(writeLog, "}\n\n");
		}
	}

	fclose(writeLog);
	return ERROR_SUCCESS;
}

void StructuredDataDef::DumpAllSDDs()
{
	int assetCount = 0;
	for(int i = 0; i < 0x18 &&
		g_structuredDataDefPool->freeHead != &g_structuredDataDefPool->entries[i].entry &&
		g_structuredDataDefPool->entries[i].next != &g_structuredDataDefPool->entries[i].entry; i++)
		if(g_structuredDataDefPool->entries[i].entry.DumpSDD() != ERROR_SUCCESS)
			printf("%s failed to dump\n", g_structuredDataDefPool->entries[i].entry.name);
		else
			assetCount++;

	printf("Dumped %i structureddatadefs", assetCount);
}
#pragma endregion

#pragma region tracer (0x26)
const AssetEntry TracerEntries[] = {
	{"material",		offsetof(Tracer, material),			ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)material,0},
	{"drawInterval",	offsetof(Tracer, drawInterval),		ASSET_ENTRY_INT,0,0},
	{"speed",			offsetof(Tracer, speed),			ASSET_ENTRY_FLOAT,0,0},
	{"beamLength",		offsetof(Tracer, beamLength),		ASSET_ENTRY_FLOAT,0,0},
	{"beamWidth",		offsetof(Tracer, beamWidth),		ASSET_ENTRY_FLOAT,0,0},
	{"screwRadius",		offsetof(Tracer, screwRadius),		ASSET_ENTRY_FLOAT,0,0},
	{"screwDist",		offsetof(Tracer, screwDist),		ASSET_ENTRY_FLOAT,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

int Tracer::dumpTracer()
{
	sprintf(MasterCharBuffer, "game:\\dump\\tracer\\%s", name);
	CreateDirectoryB(MasterCharBuffer);

	if(fileExists(MasterCharBuffer))
		return ERROR_DUP_NAME;

	FILE * writeLog = fopen(MasterCharBuffer, "w");

	if(!writeLog)
		return ERROR_BAD_NETPATH;

	fprintf(writeLog,"TRACER");

	for(int i = 0; TracerEntries[i].name; i++)
		DumpAssetEntry(writeLog, TracerEntries[i].type, TracerEntries[i].name, (void*)((int)(this) + TracerEntries[i].offset));

	for(int i = 0; i < 5; i++)
		fprintf(writeLog, "\\color%iR\\%g\\color%iG\\%g\\color%iB\\%g\\color%iA\\%g", i, colors[i][0], i, colors[i][1], i, colors[i][2], i, colors[i][3]);

	fclose(writeLog);
	return ERROR_SUCCESS;
}

void Tracer::dumpAllTracers()
{
	int assetCount = 0;
	for(int i = 0; i < 0x20 &&
		g_tracerPool->freeHead != &g_tracerPool->entries[i].entry &&
		g_tracerPool->entries[i].next != &g_tracerPool->entries[i+1].entry; i++)
		if(g_tracerPool->entries[i].entry.dumpTracer() == ERROR_SUCCESS)
			assetCount++;

	printf("Dumped %i tracers\n", assetCount);
}

void Load_TracerAssetHook(void ** input)
{
	Tracer * currentTracer = *(Tracer**)input;

	//Can we load this tracer custom?
	while(usingAssetBuffer) Sleep(10);
	
	usingAssetBuffer = true;
	sprintf(theAssetBuffer, "game:\\raw\\tracer\\%s", currentTracer->name);
	if(fileExists(theAssetBuffer))
	{
		HANDLE tracerFile = CreateFile(theAssetBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		int numberOfBytesRead;
		if(tracerFile != INVALID_HANDLE_VALUE && ReadFile(tracerFile, theAssetBuffer, GetFileSize(tracerFile, NULL), (LPDWORD)&numberOfBytesRead, 0))
		{
			char * currentName = 0, * currentValue = 0;

			CloseHandle(tracerFile);
			char * currentToken = improved_strtok(theAssetBuffer,"\\");

			if(!strcmp(currentToken, "TRACER"))
			for(currentToken = improved_strtok(NULL,"\\"); currentToken; currentToken = improved_strtok(NULL,"\\"))
			{
				currentName = currentToken;
				currentValue = improved_strtok(NULL,"\\");

				if(WriteAssetEntry(TracerEntries, currentName, currentValue, currentTracer))
					continue;
				else if(!strnicmp(currentName, "color", 5))
				{
					int currentIndex = currentName[5] - 0x30;
					switch(currentName[6])
					{
					case 'R':
					case 'r':
						currentTracer->colors[currentIndex][0] = atof(currentValue);
						break;
					case 'G':
					case 'g':
						currentTracer->colors[currentIndex][1] = atof(currentValue);
						break;
					case 'B':
					case 'b':
						currentTracer->colors[currentIndex][2] = atof(currentValue);
						break;
					case 'A':
					case 'a':
						currentTracer->colors[currentIndex][3] = atof(currentValue);
						break;
					}
				}
				else 
					printf("Couldn't recognize '%s' setting with '%s' value in 0x%X type.\n", currentName, currentValue, tracer);
			}
		}
	}
	usingAssetBuffer = false;

	tracerLoaderSaveStub(input);
}
#pragma endregion

#pragma region vehicle (0x27)
const AssetEntry VEH_BoatBouncingEntries[] = {
	{"MinForce",				0,						ASSET_ENTRY_FLOAT,0,0},
	{"MaxForce",				4,						ASSET_ENTRY_FLOAT,0,0},
	{"Rate",					8,							ASSET_ENTRY_FLOAT,0,0},
	{"FadeinSpeed",				0xC,						ASSET_ENTRY_FLOAT,0,0},
	{"FadeoutSteeringAngle",	0x10,			ASSET_ENTRY_FLOAT,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry VEH_FakeBody_BoatRockingEntries[] = {
	{"Amplitude",		0,				ASSET_ENTRY_FLOAT,0,0},
	{"Period",			4,					ASSET_ENTRY_FLOAT,0,0},
	{"RotationPeriod",	8,			ASSET_ENTRY_FLOAT,0,0},
	{"FadeoutSpeed",	0xC,				ASSET_ENTRY_FLOAT,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry VEH_FakeBodyEntries[] = {
	{"AccelPitch",					0,							ASSET_ENTRY_FLOAT,0,0},
	{"AccelRoll",					4,							ASSET_ENTRY_FLOAT,0,0},
	{"VelPitch",					8,							ASSET_ENTRY_FLOAT,0,0},
	{"VelRoll",						0xC,								ASSET_ENTRY_FLOAT,0,0},
	{"SideVelPitch",				0x10,						ASSET_ENTRY_FLOAT,0,0},
	{"SidePitchStrength",			0x14,					ASSET_ENTRY_FLOAT,0,0},
	{"RollStrength",				0x18,						ASSET_ENTRY_FLOAT,0,0},
	{"PitchDampening",				0x1C,						ASSET_ENTRY_FLOAT,0,0},
	{"RollDampening",				0x20,						ASSET_ENTRY_FLOAT,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry VEH_EngineEntries[] = {
	{"SndSpeed",						0,								ASSET_ENTRY_FLOAT,0,0},
	{"StartUpSnd",					4,							ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"StartUpLength",					8,							ASSET_ENTRY_INT,0,0},
	{"ShutdownSnd",					0xC,							ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"IdleSnd",						0x10,								ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"SustainSnd",					0x14,							ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"RampUpSnd",					0x18,								ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"RampUpLength",				0x1C,							ASSET_ENTRY_INT,0,0},
	{"RampDownSnd",					0x20,							ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"RampDownLength",				0x24,						ASSET_ENTRY_INT,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

const AssetEntry VehicleEntries[] = {
	{"type",								offsetof(Vehicle, type),										ASSET_ENTRY_ENUM,						(const AssetEntry*)VehicleTypes_s,				VEH_TYPE_MAX},
	{"useHintString",						offsetof(Vehicle, useHintString),								ASSET_ENTRY_STRING,0,0},
	{"health",								offsetof(Vehicle, health),										ASSET_ENTRY_INT,0,0},
	{"quadBarrel",							offsetof(Vehicle, quadBarrel),									ASSET_ENTRY_BOOL_AS_INT,0,0},
	{"texureScrollScale",					offsetof(Vehicle, texureScrollScale),							ASSET_ENTRY_FLOAT,0,0},
	{"maxSpeed",							offsetof(Vehicle, maxSpeed),									ASSET_ENTRY_FLOAT,0,0},
	{"accel",								offsetof(Vehicle, accel),										ASSET_ENTRY_FLOAT,0,0},
	{"rotRate",								offsetof(Vehicle, rotRate),										ASSET_ENTRY_FLOAT,0,0},
	{"rotAccel",							offsetof(Vehicle, rotAccel),									ASSET_ENTRY_FLOAT,0,0},
	{"maxBodyPitch",						offsetof(Vehicle, maxBodyPitch),								ASSET_ENTRY_FLOAT,0,0},
	{"maxBodyRoll",							offsetof(Vehicle, maxBodyRoll),									ASSET_ENTRY_FLOAT,0,0},
	{"fakeBody",							offsetof(Vehicle, fakeBodyAccelPitch),							ASSET_ENTRY_EMBEDDED,					VEH_FakeBodyEntries,0},
	{"fakeBodyBoatRocking",					offsetof(Vehicle, fakeBodyBoatRockingAmplitude),				ASSET_ENTRY_EMBEDDED,					VEH_FakeBody_BoatRockingEntries,0},
	{"boatBouncing",						offsetof(Vehicle, boatBouncingMinForce),						ASSET_ENTRY_EMBEDDED,					VEH_BoatBouncingEntries,0},
	{"collisionDamage",						offsetof(Vehicle, collisionDamage),								ASSET_ENTRY_FLOAT,0,0},
	{"collisionSpeed",						offsetof(Vehicle, collisionSpeed),								ASSET_ENTRY_FLOAT,0,0},
	{"killcamZDist",						offsetof(Vehicle, killcamZDist),								ASSET_ENTRY_FLOAT,0,0},
	{"killcamBackDist",						offsetof(Vehicle, killcamBackDist),								ASSET_ENTRY_FLOAT,0,0},
	{"killcamUpDist",						offsetof(Vehicle, killcamUpDist),								ASSET_ENTRY_FLOAT,0,0},
	{"playerProtected",						offsetof(Vehicle, playerProtected),								ASSET_ENTRY_BOOL_AS_INT,0,0},
	{"bulletDamage",						offsetof(Vehicle, bulletDamage),								ASSET_ENTRY_BOOL_AS_INT,0,0},
	{"armorPiercingDamage",					offsetof(Vehicle, armorPiercingDamage),							ASSET_ENTRY_BOOL_AS_INT,0,0},
	{"grenadeDamage",						offsetof(Vehicle, grenadeDamage),								ASSET_ENTRY_BOOL_AS_INT,0,0},
	{"projectileDamage",					offsetof(Vehicle, projectileDamage),							ASSET_ENTRY_BOOL_AS_INT,0,0},
	{"projectileSplashDamage",				offsetof(Vehicle, projectileSplashDamage),						ASSET_ENTRY_BOOL_AS_INT,0,0},
	{"heavyExplosiveDamage",				offsetof(Vehicle, heavyExplosiveDamage),						ASSET_ENTRY_BOOL_AS_INT,0,0},
	{"physicsEnabled",						offsetof(Vehicle, vehiclePhysics.physicsEnabled),				ASSET_ENTRY_BOOL_AS_INT,0,0},
	{"physicsPreset",						offsetof(Vehicle, vehiclePhysics.physicsPreset),				ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)physpreset,0},
	{"accelerationGraph",					offsetof(Vehicle, vehiclePhysics.accelerationGraph),			ASSET_ENTRY_STRING,0,0},
	{"steeringAxle",						offsetof(Vehicle, vehiclePhysics.steeringAxle),					ASSET_ENTRY_ENUM,						(const AssetEntry*)VehicleAxles_s,				VEH_MAX},
	{"powerAxle",							offsetof(Vehicle, vehiclePhysics.powerAxle),					ASSET_ENTRY_ENUM,						(const AssetEntry*)VehicleAxles_s,				VEH_MAX},
	{"brakingAxle",							offsetof(Vehicle, vehiclePhysics.brakingAxle),					ASSET_ENTRY_ENUM,						(const AssetEntry*)VehicleAxles_s,				VEH_MAX},
	{"reverseSpeed",						offsetof(Vehicle, vehiclePhysics.reverseSpeed),					ASSET_ENTRY_FLOAT,0,0},
	{"maxVelocity",							offsetof(Vehicle, vehiclePhysics.maxVelocity),					ASSET_ENTRY_FLOAT,0,0},
	{"maxPitch",							offsetof(Vehicle, vehiclePhysics.maxPitch),						ASSET_ENTRY_FLOAT,0,0},
	{"maxRoll",								offsetof(Vehicle, vehiclePhysics.maxRoll),						ASSET_ENTRY_FLOAT,0,0},
	{"maxYaw",								offsetof(Vehicle, vehiclePhysics.maxYaw),						ASSET_ENTRY_FLOAT,0,0},
	{"suspensionTravelRear",				offsetof(Vehicle, vehiclePhysics.suspensionTravelRear),			ASSET_ENTRY_FLOAT,0,0},
	{"suspensionStrengthFront",				offsetof(Vehicle, vehiclePhysics.suspensionStrengthFront),		ASSET_ENTRY_FLOAT,0,0},
	{"suspensionDampingFront",				offsetof(Vehicle, vehiclePhysics.suspensionDampingFront),		ASSET_ENTRY_FLOAT,0,0},
	{"suspensionStrengthRear",				offsetof(Vehicle, vehiclePhysics.suspensionStrengthRear),		ASSET_ENTRY_FLOAT,0,0},
	{"suspensionDampingRear",				offsetof(Vehicle, vehiclePhysics.suspensionDampingRear),		ASSET_ENTRY_FLOAT,0,0},
	{"frictionBraking",						offsetof(Vehicle, vehiclePhysics.frictionBraking),				ASSET_ENTRY_FLOAT,0,0},
	{"frictionCoasting",					offsetof(Vehicle, vehiclePhysics.frictionCoasting),				ASSET_ENTRY_FLOAT,0,0},
	{"frictionTopSpeed",					offsetof(Vehicle, vehiclePhysics.frictionTopSpeed),				ASSET_ENTRY_FLOAT,0,0},
	{"frictionSide",						offsetof(Vehicle, vehiclePhysics.frictionSide),					ASSET_ENTRY_FLOAT,0,0},
	{"frictionSideRear",					offsetof(Vehicle, vehiclePhysics.frictionSideRear),				ASSET_ENTRY_FLOAT,0,0},
	{"velocityDependentSlip",				offsetof(Vehicle, vehiclePhysics.velocityDependentSlip),		ASSET_ENTRY_FLOAT,0,0},
	{"rollStability",						offsetof(Vehicle, vehiclePhysics.rollStability),				ASSET_ENTRY_FLOAT,0,0},
	{"rollResistance",						offsetof(Vehicle, vehiclePhysics.rollResistance),				ASSET_ENTRY_FLOAT,0,0},
	{"pitchResistance",						offsetof(Vehicle, vehiclePhysics.pitchResistance),				ASSET_ENTRY_FLOAT,0,0},
	{"yawResistance",						offsetof(Vehicle, vehiclePhysics.yawResistance),				ASSET_ENTRY_FLOAT,0,0},
	{"uprightStrengthPitch",				offsetof(Vehicle, vehiclePhysics.uprightStrengthPitch),			ASSET_ENTRY_FLOAT,0,0},
	{"uprightStrengthRoll",					offsetof(Vehicle, vehiclePhysics.uprightStrengthRoll),			ASSET_ENTRY_FLOAT,0,0},
	{"targetAirPitch",						offsetof(Vehicle, vehiclePhysics.targetAirPitch),				ASSET_ENTRY_FLOAT,0,0},
	{"airYawTorque",						offsetof(Vehicle, vehiclePhysics.airYawTorque),					ASSET_ENTRY_FLOAT,0,0},
	{"airPitchTorque",						offsetof(Vehicle, vehiclePhysics.airPitchTorque),				ASSET_ENTRY_FLOAT,0,0},
	{"minimumMomentumForCollision",			offsetof(Vehicle, vehiclePhysics.minimumMomentumForCollision),	ASSET_ENTRY_FLOAT,0,0},
	{"collisionLaunchForceScale",			offsetof(Vehicle, vehiclePhysics.collisionLaunchForceScale),	ASSET_ENTRY_FLOAT,0,0},
	{"wreckedMassScale",					offsetof(Vehicle, vehiclePhysics.wreckedMassScale),				ASSET_ENTRY_FLOAT,0,0},
	{"wreckedBodyFriction",					offsetof(Vehicle, vehiclePhysics.wreckedBodyFriction),			ASSET_ENTRY_FLOAT,0,0},
	{"minimumJoltForNotify",				offsetof(Vehicle, vehiclePhysics.minimumJoltForNotify),			ASSET_ENTRY_FLOAT,0,0},
	{"slipThreshholdFront",					offsetof(Vehicle, vehiclePhysics.slipThreshholdFront),			ASSET_ENTRY_FLOAT,0,0},
	{"slipThreshholdRear",					offsetof(Vehicle, vehiclePhysics.slipThreshholdRear),			ASSET_ENTRY_FLOAT,0,0},
	{"slipFricScaleFront",					offsetof(Vehicle, vehiclePhysics.slipFricScaleFront),			ASSET_ENTRY_FLOAT,0,0},
	{"slipFricScaleRear",					offsetof(Vehicle, vehiclePhysics.slipFricScaleRear),			ASSET_ENTRY_FLOAT,0,0},
	{"slipFricRateFront",					offsetof(Vehicle, vehiclePhysics.slipFricRateFront),			ASSET_ENTRY_FLOAT,0,0},
	{"slipFricRateRear",					offsetof(Vehicle, vehiclePhysics.slipFricRateRear),				ASSET_ENTRY_FLOAT,0,0},
	{"slipYawTorque",						offsetof(Vehicle, vehiclePhysics.slipYawTorque),				ASSET_ENTRY_FLOAT,0,0},
	{"boostDuration",						offsetof(Vehicle, boostDuration),								ASSET_ENTRY_FLOAT,0,0},
	{"boostRechargeTime",					offsetof(Vehicle, boostRechargeTime),							ASSET_ENTRY_FLOAT,0,0},
	{"boostAcceleration",					offsetof(Vehicle, boostAcceleration),							ASSET_ENTRY_FLOAT,0,0},
	{"supensionTravel",						offsetof(Vehicle, supensionTravel),								ASSET_ENTRY_FLOAT,0,0},
	{"maxSteeringAngle",					offsetof(Vehicle, maxSteeringAngle),							ASSET_ENTRY_FLOAT,0,0},
	{"minSteeringScale",					offsetof(Vehicle, minSteeringScale),							ASSET_ENTRY_FLOAT,0,0},
	{"minSteeringSpeed",					offsetof(Vehicle, minSteeringSpeed),							ASSET_ENTRY_FLOAT,0,0},
	{"camLookEnabled",						offsetof(Vehicle, camLookEnabled),								ASSET_ENTRY_BOOL_AS_INT,0,0},
	{"camLerp",								offsetof(Vehicle, camLerp),										ASSET_ENTRY_FLOAT,0,0},
	{"camPitchInfluence",					offsetof(Vehicle, camPitchInfluence),							ASSET_ENTRY_FLOAT,0,0},
	{"camRollInfluence",					offsetof(Vehicle, camRollInfluence),							ASSET_ENTRY_FLOAT,0,0},
	{"camFovIncrease",						offsetof(Vehicle, camFovIncrease),								ASSET_ENTRY_FLOAT,0,0},
	{"camFovOffset",						offsetof(Vehicle, camFovOffset),								ASSET_ENTRY_FLOAT,0,0},
	{"camFovSpeed",							offsetof(Vehicle, camFovSpeed),									ASSET_ENTRY_FLOAT,0,0},
	{"turretWeapon",						offsetof(Vehicle, turretWeapon),								ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)weapon,0},
	{"turretHorizSpanLeft",					offsetof(Vehicle, turretHorizSpanLeft),							ASSET_ENTRY_FLOAT,0,0},
	{"turretHorizSpanRight",				offsetof(Vehicle, turretHorizSpanRight),						ASSET_ENTRY_FLOAT,0,0},
	{"turretVertSpanUp",					offsetof(Vehicle, turretVertSpanUp),							ASSET_ENTRY_FLOAT,0,0},
	{"turretVertSpanDown",					offsetof(Vehicle, turretVertSpanDown),							ASSET_ENTRY_FLOAT,0,0},
	{"turretRotRate",						offsetof(Vehicle, turretRotRate),								ASSET_ENTRY_FLOAT,0,0},
	{"turretSpinSnd",						offsetof(Vehicle, turretSpinSnd),								ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"turretStopSnd",						offsetof(Vehicle, turretStopSnd),								ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"trophyEnabled",						offsetof(Vehicle, trophyEnabled),								ASSET_ENTRY_BOOL_AS_INT,0,0},
	{"trophyRadius",						offsetof(Vehicle, trophyRadius),								ASSET_ENTRY_FLOAT,0,0},
	{"trophyInactiveRadius",				offsetof(Vehicle, trophyInactiveRadius),						ASSET_ENTRY_FLOAT,0,0},
	{"trophyAmmoCount",						offsetof(Vehicle, trophyAmmoCount),								ASSET_ENTRY_INT,0,0},
	{"trophyReloadTime",					offsetof(Vehicle, trophyReloadTime),							ASSET_ENTRY_FLOAT,0,0},
	{"compassFriendlyIcon",					offsetof(Vehicle, compassFriendlyIcon),							ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)material,0},
	{"compassEnemyIcon",					offsetof(Vehicle, compassEnemyIcon),							ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)material,0},
	{"compassIconWidth",					offsetof(Vehicle, compassIconWidth),							ASSET_ENTRY_INT,0,0},
	{"compassIconHeight",					offsetof(Vehicle, compassIconHeight),							ASSET_ENTRY_INT,0,0},
	{"lowIdleSound",						offsetof(Vehicle, lowIdleSound),								ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"highIdleSound",						offsetof(Vehicle, highIdleSound),								ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"lowEngineSound",						offsetof(Vehicle, lowEngineSound),								ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"highEngineSound",						offsetof(Vehicle, highEngineSound),								ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"engine",								offsetof(Vehicle, engineSndSpeed),								ASSET_ENTRY_EMBEDDED,					VEH_EngineEntries,0},
	{"suspensionSoftSnd",					offsetof(Vehicle, suspensionSoftSnd),							ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"suspensionSoftCompression",			offsetof(Vehicle, suspensionSoftCompression),					ASSET_ENTRY_FLOAT,0,0},
	{"suspensionHardSnd",					offsetof(Vehicle, suspensionHardSnd),							ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"suspensionHardConpression",			offsetof(Vehicle, suspensionHardConpression),					ASSET_ENTRY_FLOAT,0,0},
	{"collisionSnd",						offsetof(Vehicle, collisionSnd),								ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"collisionBlendSpeed",					offsetof(Vehicle, collisionBlendSpeed),							ASSET_ENTRY_FLOAT,0,0},
	{"speedSnd",							offsetof(Vehicle, speedSnd),									ASSET_ENTRY_ASSET_PTR,							(AssetEntry*)sound,0},
	{"speedSndBlendSpeed",					offsetof(Vehicle, collisionBlendSpeed),							ASSET_ENTRY_FLOAT,0,0},
	{"surfaceSndPrefix",					offsetof(Vehicle, surfaceSndPrefix),							ASSET_ENTRY_STRING,0,0},
	{"SurfaceSound",					offsetof(Vehicle, surfaceSounds),							ASSET_ENTRY_EMBEDDED_REVERSED,							SurfaceSoundEntries,0},
	{"surfaceSndBlendSpeed",				offsetof(Vehicle, surfaceSndBlendSpeed),						ASSET_ENTRY_FLOAT,0,0},
	{"slideVolume",							offsetof(Vehicle, slideVolume),									ASSET_ENTRY_FLOAT,0,0},
	{"slideBlendSpeed",						offsetof(Vehicle, slideBlendSpeed),								ASSET_ENTRY_FLOAT,0,0},
	{"inAirPitch",							offsetof(Vehicle, inAirPitch),									ASSET_ENTRY_FLOAT,0,0},
	{0, 0, (AssetEntryType)0,0,0}
};

int Vehicle::dumpVehicle()
{
	sprintf(MasterCharBuffer, "game:\\dump\\vehicles\\mp\\%s", name);
	CreateDirectoryB(MasterCharBuffer);

	if(fileExists(MasterCharBuffer))
		return ERROR_DUP_NAME;

	FILE * writeLog = fopen(MasterCharBuffer, "w");

	if(!writeLog)
		return ERROR_BAD_NETPATH;

	fprintf(writeLog, "VEHICLEFILE");

	for(int i = 0; VehicleEntries[i].name; i++)
		DumpAssetEntry(writeLog, VehicleEntries[i].type, VehicleEntries[i].name, (void*)((int)(this) + VehicleEntries[i].offset), VehicleEntries[i].next, VehicleEntries[i].enumCount);

	fclose(writeLog);
	return ERROR_SUCCESS;
}

void Vehicle::dumpAllVehicles()
{
	int assetCount = 0;
	for(int i = 0; i < 0x80 &&
		g_vehiclePool->freeHead != &g_vehiclePool->entries[i].entry &&
		g_vehiclePool->entries[i].next != &g_vehiclePool->entries[i+1].entry &&
		g_vehiclePool->entries[i].next; i++)
		if(g_vehiclePool->entries[i].entry.dumpVehicle() != ERROR_SUCCESS)
			printf("Failed to dump vehicle %s\n", g_vehiclePool->entries[i].entry.name);
		else
			assetCount++;

	printf("Dumped %i vehicles\n", assetCount);
}

void Load_VehicleAssetHook(void ** input)
{
	Vehicle * currentVehicle = *(Vehicle**)input;

	//Can we load this weapon custom?
	while(usingAssetBuffer) Sleep(10);

	usingAssetBuffer = true;
	sprintf(theAssetBuffer, "game:\\raw\\vehicles\\mp\\%s", currentVehicle->name);
	if(fileExists(theAssetBuffer))
	{
		HANDLE vehicleFile = CreateFile(theAssetBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		int numberOfBytesRead;
		ZeroMemory(theAssetBuffer, 0x4000);
		if(vehicleFile != INVALID_HANDLE_VALUE && ReadFile(vehicleFile, theAssetBuffer, GetFileSize(vehicleFile, NULL), (LPDWORD)&numberOfBytesRead, 0))
		{
			char * currentName = 0, * currentValue = 0;

			CloseHandle(vehicleFile);
			char * currentToken = improved_strtok(theAssetBuffer,"\\");

			if(!strcmp(currentToken, "VEHICLEFILE"))
			for(currentToken = improved_strtok(NULL,"\\"); currentToken; currentToken = improved_strtok(NULL,"\\"))
			{
				currentName = currentToken;
				currentValue = improved_strtok(NULL,"\\");
				
				if(!WriteAssetEntry(VehicleEntries, currentName, currentValue, currentVehicle))
					printf("Cannont handle %s with value %s in vehicle %s, it isn't supported.\n", currentName, currentValue, currentVehicle->name);
			}
		}
	}
	usingAssetBuffer = false;

	vehicleLoaderSaveStub(input);
}
#pragma endregion
