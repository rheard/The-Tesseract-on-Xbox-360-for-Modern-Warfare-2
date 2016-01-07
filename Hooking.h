#ifndef _HOOKING_H
#define _HOOKING_H

#include <xtl.h>
#include <xboxmath.h>
#include <xbox.h>
#include <stdio.h>//,alwaysReturnTrue = 0x38600001;
#include "Modern Warfare 2 TU8.h"

void CreateDirectoryB(char * name);

extern char MasterCharBuffer[0x8000];
extern char theAssetBuffer[0x4000];
extern bool usingAssetBuffer;
extern int runningID;

//Contains any global definitions and hooking functions

//The following externs are used in patching PPC
extern int returnFalse,
		returnTrue,
		liR40,
		liR50,
		liR60,
		liR70,
		liR90,
		BLR,
		NOP;
extern bool thisIsTrue, thisIsFalse;
extern short alwaysBranchPatch;

void patchMinecraftBootXEXOnDev();
DWORD WINAPI MinecraftLoop(LPVOID);

void MW2TitleUpdate8InternalLoop(const void * args, int unknown);
void renderSaveStub(const void * args, int unknown);
void StringTable_GetAsset(char* filename, void** table);
int Rawfile_GetAsset(char* filename, int unknown1, int unknown2);
void Playlist_ParsePlaylistsHook(char * playlistString);
void * Image_GetIndex(unsigned int index);
char * Localize_GetString(char * nameOfLocalize);
void Load_PhysPresetAssetHook(void ** input);
void physPresetLoaderSaveStub(void ** input);
void Load_TracerAssetHook(void ** input);
void tracerLoaderSaveStub(void ** input);
void physPresetLoaderSaveStub(void ** input);
void Load_ModelSurfAssetHook(void ** input);
void modelSurfLoaderSaveStub(void **input);
void weaponVarientDefLoaderSaveStub(void ** input);
void Load_weaponVarientDef_tAssetHook(void ** input);
void lightDefLoaderSaveStub(void ** input);
void Load_lightDefAssetHook(void ** input);
void vehicleLoaderSaveStub(void ** input);
void Load_VehicleAssetHook(void ** input);
void playlistLoaderSaveStub(char * input);
const char * CM_EntityString();
void MW2_TU8_Init();

char *improved_strtok(char *buf, const char *delim);

extern bool isContemplatingPrimary, isContemplatingSecondary;
extern int contemplatingTotalPrimary, contemplatingCurrentPrimary,
	contemplatingTotalSecondary, contemplatingCurrentSecondary;
extern char contemplatorTextPrimary[0x40], contemplatorTextSecondary[0x40];
extern int previouslyLoadedSize;
extern char * previouslyLoadedMapEnts;
extern void * customLoadedLocalize[0x100];
extern int customLoadedLocalizeCount;
extern void * customLoadedTables[0x20];
extern int customLoadedTableCount;
extern void * customLoadedRawfiles[0x40];
extern int customLoadedRawfileCount;

extern HANDLE thisApplication;

struct XZoneInfo
{
  const char *name;
  int allocFlags;
  int freeFlags;
};

typedef void (__cdecl * DB_LoadXAssets_t)(XZoneInfo * zoneInfo, int zoneCount, int sync, int);

enum stickNum : int
{
	VA_SIDE = 0x00,
	VA_FORWARD = 0x01,
	VA_UP = 0x02,
	VA_YAW = 0x03,
	VA_PITCH = 0x04,
	VA_ATTACK = 0x05
};
enum keyNum : int
{
	BUTTON_A			= 0x01,
	BUTTON_B			= 0x02,
	BUTTON_X			= 0x03,
	BUTTON_Y			= 0x04,
	BUTTON_LSHLDR		= 0x05,
	BUTTON_RSHLDR		= 0x06,
	TAB					= 0x09,
	ENTER				= 0x0D,
	BUTTON_START		= 0x0E,
	BUTTON_BACK			= 0x0F,
	BUTTON_LSTICK		= 0x10,
	BUTTON_RSTICK		= 0x11,
	BUTTON_LTRIG		= 0x12,
	BUTTON_RTRIG		= 0x13,
	DPAD_UP				= 0x14,
	DPAD_DOWN			= 0x15,
	DPAD_LEFT			= 0x16,
	DPAD_RIGHT			= 0x17,
	ESCAPE				= 0x1B,
	APAD_UP				= 0x1C,
	APAD_DOWN			= 0x1D,
	APAD_LEFT			= 0x1E,
	APAD_RIGHT			= 0x1F,
	SPACE				= 0x20,
	SEMICOLON			= 0x3B,
	BACKSPACE			= 0x7F,
	COMMAND				= 0x96,
	CAPSLOCK			= 0x97,
	PAUSE				= 0x99,
	UPARROW				= 0x9A,
	DOWNARROW			= 0x9B,
	LEFTARROW			= 0x9C, 
	RIGHTARROW			= 0x9D,
	ALT					= 0x9E,
	CTRL				= 0x9F,
	SHIFT				= 0xA0,
	INS					= 0xA1,
	DEL					= 0xA2,
	PGDN				= 0xA3,
	PGUP				= 0xA4,
	HOME				= 0xA5,
	END					= 0xA6,
	F1					= 0xA7,
	F2					= 0xA8,
	F3					= 0xA9,
	F4					= 0xAA,
	F5					= 0xAB,
	F6					= 0xAC,
	F7					= 0xAD,
	F8					= 0xAE,
	F9					= 0xAF,
	F10					= 0xB0,
	F11					= 0xB1,
	F12					= 0xB2,
	KP_HOME				= 0xB6,
	KP_UPARROW			= 0xB7,
	KP_PGUP				= 0xB8,
	KP_LEFTARROW		= 0xB9,
	KP_5				= 0xBA,
	KP_RIGHTARROW		= 0xBB,
	KP_END				= 0xBC,
	KP_DOWNARROW		= 0xBD,
	KP_PGDN				= 0xBE,
	KP_ENTER			= 0xBF,
	KP_INS				= 0xC0,
	KP_DEL				= 0xC1,
	KP_SLASH			= 0xC2,
	KP_MINUS			= 0xC3,
	KP_PLUS				= 0xC4,
	KP_NUMLOCK			= 0xC5,
	KP_STAR				= 0xC6,
	KP_EQUALS			= 0xC7,
	MOUSE1				= 0xC8,
	MOUSE2				= 0xC9,
	MOUSE3				= 0xCA,
	MOUSE4				= 0xCB,
	MOUSE5				= 0xCC,
	MWHEELUP			= 0xCE,
	MWHEELDOWN			= 0xCD,
	AUX1				= 0xCF,
	AUX2				= 0xD0,
	AUX3				= 0xD1,
	AUX4				= 0xD2,
	AUX5				= 0xD3,
	AUX6				= 0xD4,
	AUX7				= 0xD5,
	AUX8				= 0xD6,
	AUX9				= 0xD7,
	AUX10				= 0xD8,
	AUX11				= 0xD9,
	AUX12				= 0xDA,
	AUX13				= 0xDB,
	AUX14				= 0xDC,
	AUX15				= 0xDD,
	AUX16				= 0xDE
};

template<typename T> union XAssetPoolEntry
{
	T entry;
	T * next;							//Pointer to the next asset, only if this asset is free
};

template<typename T, int count> struct XAssetPool
{
	T * freeHead;						//Pointer to the first free asset
	XAssetPoolEntry<T> entries[count];
};

enum AssetEntryType
{
	ASSET_ENTRY_BYTE,
	ASSET_ENTRY_BOOL,
	ASSET_ENTRY_BOOL_AS_INT,
	ASSET_ENTRY_SHORT,
	ASSET_ENTRY_UNSIGNED_SHORT,
	ASSET_ENTRY_TIME_AS_INT,
	ASSET_ENTRY_INT,
	ASSET_ENTRY_UNSIGNED_INT,
	ASSET_ENTRY_FLOAT,
	ASSET_ENTRY_STRING,
	ASSET_ENTRY_ASSET_PTR,
	ASSET_ENTRY_CONTAINED,
	ASSET_ENTRY_CONTAINED_REVERSED,
	ASSET_ENTRY_EMBEDDED,
	ASSET_ENTRY_EMBEDDED_REVERSED,
	ASSET_ENTRY_ENUM
};

typedef struct AssetEntry
{
	const char * name;
	const int offset;
	const AssetEntryType type;
	const AssetEntry * next;
	unsigned int enumCount;
};

bool DumpAssetEntry(FILE * writeLog, AssetEntryType currentEntryType, const char * currentEntryName, void * assetData, const AssetEntry * containedItems = 0, int enumCount = 0, const char * parentName = "");
bool WriteAssetEntry(const AssetEntry * currentEntries, char * tokenName, char * tokenValue, void * assetHeader, const char * parentName = "", bool parentReversed = false);
extern const AssetEntry WeaponUniqueAttachmentDefEntries[];
extern const AssetEntry WeaponAttachmentDefEntries[];
extern const AssetEntry VectorEntry[];
extern const AssetEntry OffsetEntry[];
extern const AssetEntry AngleEntry[];
extern const AssetEntry MinMax_IntEntry[];
extern const AssetEntry MinMax_FloatEntry[];
extern const AssetEntry HipSpreadSettings[];
extern const AssetEntry GunKickSettings[];

enum weaponIconRatioType_t : int
{
	WEAPON_ICON_RATIO_1TO1 = 0,
	WEAPON_ICON_RATIO_2TO1 = 1,
	WEAPON_ICON_RATIO_4TO1 = 2,
	WEAPON_ICON_RATIO_MAX
};

enum PenetrateType : int
{
	PENETRATE_TYPE_NONE = 0,
	PENETRATE_TYPE_SMALL = 1,
	PENETRATE_TYPE_MEDIUM = 2,
	PENETRATE_TYPE_LARGE = 3,
	PENETRATE_TYPE_MAX 
};

enum weapStance_t : int
{
  WEAPSTANCE_STAND = 0x0,
  WEAPSTANCE_DUCK = 0x1,
  WEAPSTANCE_PRONE = 0x2,
  WEAPSTANCE_MAX
};

enum ammoCounterClipType_t : int
{
	AMMO_COUNTER_CLIP_NONE = 0,
	AMMO_COUNTER_CLIP_MAGAZINE = 1,
	AMMO_COUNTER_CLIP_SHORTMAGAZINE = 2,
	AMMO_COUNTER_CLIP_SHOTGUN = 3,
	AMMO_COUNTER_CLIP_ROCKET = 4,
	AMMO_COUNTER_CLIP_BELTFED = 5,
	AMMO_COUNTER_CLIP_ALTWEAPON = 6,
	AMMO_COUNTER_CLIP_MAX
};

enum weapOverlayReticle_t
{
  WEAPOVERLAYRETICLE_NONE = 0x0,
  WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,
  WEAPOVERLAYRETICLE_MAX
};

enum weapOverlayInterface_t
{
  WEAPOVERLAYINTERFACE_NONE = 0x0,
  WEAPOVERLAYINTERFACE_JAVELIN = 0x1,
  WEAPOVERLAYINTERFACE_TURRETSCOPE = 0x2,
  WEAPOVERLAYINTERFACE_MAX
};

extern char * weapOverlayInterface_s[];
extern char * weapOverlayReticle_s[];
extern char * weaponIconRatioType_s[];
extern char * ammoCounterClipType_s[];
extern char * weapStance_s[];

void patchTU8BootXEXOnDev();
//void patchTU8BootXEXOnRetail();
//void patchTU6BootXEXOnDev();
void patchTU8XEXEveryFrameOnDev();
//void patchTU6XEXEveryFrameOnDev();

VOID DoPatches( PDWORD pBuffer, DWORD dwLength );
void patchMemory(DWORD addr, DWORD sourceAddr, int size);

///<summary>
///This will cleanly handle code for writing mem
///  depending on wether or not we're on a retail
///  or on a dev image.
///</summary>
///<returns>Returns the number of bytes that were actually written.</returns>
///<param name="address">Pointer to where you want to copy data to.</param>
///<param name="size">Number of bytes to copy.</param>
///<param name="data">Pointer to where you want to copy data from.</param>
///<param name="optionalOutput">Pointer to an HRESULT that will return how this execution went.
///  This will almost always return XBDM_NOERR unless
///  something stupid has been done.</param>
DWORD WriteMemory(LPVOID address, DWORD size, LPCVOID data, HRESULT * optionalOutput = 0);

char * ConvertAssetNameToFileName(char * assetName);
char * ConvertAssetNameToFileName(char * assetName, char * outputBuff);

typedef void (__cdecl * G_GivePlayerWeapon_t)(playerState_s * ps, int iWeaponIndex, char altModelIndex, bool isDualWeild, int unknown1);
extern G_GivePlayerWeapon_t G_GivePlayerWeapon;

typedef void (__cdecl * SV_ExecuteClientCommand_t)(void *cl, const char *s, int clientOK, int);
extern SV_ExecuteClientCommand_t SV_ExecuteClientCommand;

typedef gentity_s * (__cdecl * SV_AddTestClient_t)();
extern SV_AddTestClient_t SV_AddTestClient;

typedef void (__cdecl * SV_GameSendServerCommand_t)(int clientNum, int type, const char *text);
extern SV_GameSendServerCommand_t SV_GameSendServerCommand;

// we don't like Boost
/*#define strtk_no_tr1_or_boost

// conflicting windows.h definitions
#undef max
#undef min

// strtk code
#include "strtk\strtk.hpp"
#define  _XOPEN_SOURCE_EXTENDED 1
#include <string>
extern strtk::token_grid::options strtkGridOptions;*/
#define offsetof(s,m)   (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))

#endif
