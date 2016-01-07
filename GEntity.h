#include "Modern Warfare 2 TU8.h"

typedef struct scr_entref_t
{
	short entnum;
	short classnum;
} scr_entref_t;

struct XenonUserData
{
	int signInState;
	char name[0x20];
	char unknownData1[0x4];
	__int64 xuid;
	char XuidString[0x10];
	char unknownData2[0x8];
	__int64 profileID;
	char profileIDString[0x10];
	char unknownData3[0x8];
};

XenonUserData * xenonUserData = (XenonUserData *)0x838BA820;
Achievements * gameAchievementList = (Achievements *)0x8253A510;

struct level_locals_t
{
	gclient_s *clients; 
	gentity_s *gentities;
	int num_entities;
	gentity_s *firstFreeEnt;
	gentity_s *lastFreeEnt;
	char unknown3[0x390];
	int maxclients; //0x3A4
	int framenum;
	int time;
	int previousTime;
	int frametime;
	int startTime;
	int teamScores[4];
	int lastTeammateHealthTime;
	int bUpdateScoresForIntermission;
	char unknown2[0x8];
	int manualNameChange;
	int numConnectedClients; //0x3E0
	char unknown[0xA54];
	int savepersist;
	gentity_s *droppedWeaponCue[32];
	float fFogOpaqueDist;
	float fFogOpaqueDistSqrd;
	int currentPlayerClone;
};
