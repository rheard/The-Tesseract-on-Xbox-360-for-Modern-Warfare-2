#pragma once

#include <xtl.h>
#include <xboxmath.h>
#include <xbox.h>
#include <xam.h>
#include <stdio.h>

enum $3795D074AAAAB5968479B7527F325087
{
  FL_GODMODE = 0x1,
  FL_DEMI_GODMODE = 0x2,
  FL_NOTARGET = 0x4,
  FL_NO_KNOCKBACK = 0x8,
  FL_DROPPED_ITEM = 0x10,
  FL_NO_BOTS = 0x20,
  FL_NO_HUMANS = 0x40,
  FL_TOGGLE = 0x80,
  FL_SOFTACTIVATE = 0x100,
  FL_PARACHUTE = 0x200,
  FL_NO_HEADCHECK = 0x400,
  FL_SUPPORTS_LINKTO = 0x1000,
  FL_NO_AUTO_ANIM_UPDATE = 0x2000,
  FL_GRENADE_TOUCH_DAMAGE = 0x4000,
  FL_MISSILE_DESTABILIZED = 0x10000,
  FL_STABLE_MISSILES = 0x20000,
  FL_REPEAT_ANIM_UPDATE = 0x40000,
};

typedef struct ammoType_s
{
	int ammoIndex;
	int ammoCount;
	int akimboAmmoCount;
} ammoType_s;

typedef struct ammoClipType_s
{
	int clipIndex;
	int clipCount;
} ammoClipType_s;

typedef struct usercmd_s
{
	int serverTime;
	//int buttons;
	char weapon;
	char offHandIndex;
	char undefined1[2];
	int angles[3];
	char forwardmove;
	char rightmove;
	char undefined2[2];
	float meleeChargeYaw;
	char meleeChargeDist;
	char _padding[3];
	int unknown[2];
} usercmd_s;

enum clientConnected_t : int
{
	CON_DISCONNECTED = 0x0,
	CON_CONNECTING = 0x1,
	CON_CONNECTED = 0x2,
};

enum sessionState_t : int
{
	SESS_STATE_PLAYING = 0x0,
	SESS_STATE_DEAD = 0x1,
	SESS_STATE_SPECTATOR = 0x2,
	SESS_STATE_INTERMISSION = 0x3,
};

struct playerTeamState_t
{
	int location;
};

enum team_t
{
	TEAM_FREE = 0x0,
	TEAM_AXIS = 0x1,
	TEAM_ALLIES = 0x2,
	TEAM_SPECTATOR = 0x3,
};

typedef struct clientState_s
{
	int clientIndex;
	team_t team;
	int modelindex;
	int dualWielding;
	int riotShieldNext;
	int attachModelIndex[6];
	int attachTagIndex[6];
	char name[0x20];
	float maxSprintTimeMultiplier;
	int rank;
	int prestige;
	int perks[2];
	int diveState;
	int voiceConnectivityBits;
	char clanAbb[4];
} clientState_s;

struct clientSession_t
{
	sessionState_t sessionState;
	int uknown6;
	int score;
	int deaths;
	int kills;
	int assists;
	__int16 scriptPersID;
	__int16 undefined;
	clientConnected_t connected;
	usercmd_s cmd;
	usercmd_s oldcmd;
	int localClient;
	int predictItemPickup;
	char newnetname[0x20];
	int maxHealth;							//0x3228
	int enterTime;							//0x322C
	playerTeamState_t teamState;			//0x3230
	int voteCount;							//0x3234
	int teamVoteCount;						//0x3238
	float moveSpeedScaleMultiplier;			//0x323C
	int viewmodelIndex;						//0x3240
	int noSpectate;							//0x3244
	int teamInfo;							//0x3248
	clientState_s _cs;						//0x324C
	char unknown_data6[0x10];				//0x32D0
	int psOffsetTime;
	int hasRadar;							//0x32E4
	int isRadarBlocked;
	char unknown_data5[0x130];				//0x32D0
};

struct SprintState
{
  int sprintButtonUpRequired;
  int sprintDelay;
  int lastSprintStart;
  int lastSprintEnd;
  int sprintStartMaxLength;
};

struct MantleState
{
  float yaw;
  int timer;
  int transIndex;
  int flags;
};

enum weaponstate_t : int
{
  WEAPON_READY = 0x0,
  WEAPON_RAISING = 0x1,
  WEAPON_RAISING_ALTSWITCH = 0x2,
  WEAPON_DROPPING = 0x3,
  WEAPON_DROPPING_QUICK = 0x4,
  WEAPON_DROPPING_ALTSWITCH = 0x5,
  WEAPON_FIRING = 0x6,
  WEAPON_RECHAMBERING = 0x7,
  WEAPON_RELOADING = 0x8,
  WEAPON_RELOADING_INTERUPT = 0x9,
  WEAPON_RELOAD_START = 0xA,
  WEAPON_RELOAD_START_INTERUPT = 0xB,
  WEAPON_RELOAD_END = 0xC,
  WEAPON_MELEE_INIT,
  WEAPON_MELEE_FIRE,
  WEAPON_MELEE_END,
  WEAPON_OFFHAND_INIT,
  WEAPON_OFFHAND_PREPARE,
  WEAPON_OFFHAND_HOLD,
  WEAPON_OFFHAND_START,
  WEAPON_OFFHAND,
  WEAPON_OFFHAND_END,
  WEAPON_DETONATING,
  WEAPON_SPRINT_RAISE,
  WEAPON_SPRINT_LOOP,
  WEAPON_SPRINT_DROP,
  WEAPON_NIGHTVISION_WEAR,
  WEAPON_NIGHTVISION_REMOVE
};

struct WeaponState
{
	int weapAnim;
	int weaponTime;
	int weaponDelay;
	int weaponRestrictKickTime;
	weaponstate_t weaponstate;
	int weapHandFlags;
	int weaponShotCount;
};

enum OffhandSecondaryClass
{
  PLAYER_OFFHAND_SECONDARY_SMOKE = 0x0,
  PLAYER_OFFHAND_SECONDARY_FLASH = 0x1,
  PLAYER_OFFHAND_SECONDARY_STUN = 0x2,
};

struct ActionSlotParam_SpecifyWeapon
{
  unsigned int index;
};

struct ActionSlotParam
{
  ActionSlotParam_SpecifyWeapon specifyWeapon;
};

enum ActionSlotType
{
  ACTIONSLOTTYPE_DONOTHING = 0x0,
  ACTIONSLOTTYPE_SPECIFYWEAPON = 0x1,
  ACTIONSLOTTYPE_ALTWEAPONTOGGLE = 0x2,
  ACTIONSLOTTYPE_NIGHTVISION = 0x3,
  ACTIONSLOTTYPECOUNT = 0x4,
};

enum pmtype_t : int
{
  PM_NORMAL = 0x0,
  PM_NORMAL_LINKED = 0x1,
  PM_NOCLIP = 0x2,
  PM_UFO = 0x3,
  PM_SPECTATOR = 0x4,
  PM_INTERMISSION = 0x5,
  PM_LASTSTAND = 0x6,
  PM_DEAD = 0x7,
  PM_DEAD_LINKED = 0x8,
};

enum objectiveState_t
{
  OBJST_EMPTY = 0x0,
  OBJST_ACTIVE = 0x1,
  OBJST_INVISIBLE = 0x2,
  OBJST_DONE = 0x3,
  OBJST_CURRENT = 0x4,
  OBJST_FAILED = 0x5,
  OBJST_NUMSTATES = 0x6,
};

struct objective_t
{
  objectiveState_t state;
  float origin[3];
  int entNum;
  int teamNum;
  int icon;
};

enum PlayerSpreadOverrideState
{
  PSOS_DISABLED = 0x0,
  PSOS_RESETTING = 0x1,
  PSOS_ENABLED = 0x2
};

struct playerState_s
{
	int commandTime;
	pmtype_t pm_type;
	int bobCycle;
	int pm_flags;
	/*
	0x1 - IsProne
	0x2 - AllowSprint
	0x4 - AllowJump					//Confirmed
	0x20 - SideJumpSlowdown?
	0x2000 - ForwardJumpSlowdown
	0x4000 - IsSprinting
	0x8000 - IsDamaged
	*/

	int otherFlags;
	/*
	0x4 - LaserAltModeOn
	0x8 - ThermalOn
	0x10 - FOFOverlayOn
	0x20 - RemoteCameraSoundScapeOn
	0x400 - EMPJammed
	0x800 - Spectating
	*/

	int linkFlags;
	int pm_time;

	float origin[3];
	float velocity[3];

	int weaponTime;
	int weaponDelay;
	int grenadeTimeLeft;
	int throwBackGrenadeOwner;
	int remoteEyesEnt;
	int remoteEyesTagname;
	int remoteControlEnt;
	int foliageSoundTime;
	int gravity;
	float leanf;
	int speed;
	float delta_angles[3];
	int groundEntityNum;		//If == 0x7FF, player is not on ground
	float vLadderVec[3];
	int jumpTime;
	float jumpOriginZ;
	int legsTimer;
	int legsAnim;
	int torsoTimer;
	int torsoAnim;
	int legsAnimDuration;
	int torsoAnimDuration;
	int damageTimer;
	int damageDuration;
	int flinchYawAnim;
	int corpseIndex;
	int movementDir;
	int eFlags;
	/*
	0x20 - JamRadar
	0x400 - IsOnTurret
	0x800 - IsOnTurret2?
	0x8000 - LaserOn
	*/
	int eventSequence;
	int events[4];
	unsigned int eventParms[4];
	int oldEventSequence;
	int unpredictableEventSequence;
	int unpredictableEventSequenceOld;
	int unpredictableEvents[4];
	unsigned int unpredictableEventParms[4];
	int clientNum;
	int viewmodelIndex;
	float viewAngles[3];
	int viewHeightTarget;
	float viewHeightCurrent;
	char unknown_data8[0x98];
	SprintState sprintState;
	float holdBreathScale;
	int holdBreathTimer;
	float moveSpeedScaleMultiplier;
	MantleState mantleState;
	WeaponState weaponStates[2];
	char unknown_data[0x88];
	int offHandIndex;
	OffhandSecondaryClass offhandPrimary;
	OffhandSecondaryClass offhandSecondary;
	unsigned int weapon;
	unsigned int primaryWeaponForAltMode;
	int weapFlags;
	/*
	2 - UsingEquipment
	0x20 - DisableADS
	0x40 - Toggle Night Vision
	0x80 - DisableWeapons
	0x400 - RecoilScaleEnabled
	0x800 - DisableWeaponSwitch
	0x1000 - DisableOffhandWeapons
	*/
	float fWeaponPosFrac;
	float aimSpreadScale;
	int adsDelayTime;
	int spreadOverride;
	PlayerSpreadOverrideState spreadOverrideState;
	int isDualWeilding;
	char unknown_data1[0x12C];
	int weapLockedFlags;
	int weapLockedEntnum;
	float weapLockedPos[3];
	int weaponIdleTime;
	float meleeChargeYaw;
	int meleeChargeDist;
	int meleeChargeTime;
	int perks[2];
	int perkSlots[8];
	ActionSlotType actionSlotType[4];
	ActionSlotParam actionSlotParam[4];
	int weaponHudIconOverrides[6];
	int animScriptedType;
	int shellshockIndex;
	int shellshockTime;
	int shellshockDuration;
	float dofNearStart;
	float dofNearEnd;
	float dofFarStart;
	float dofFarEnd;
	float dofNearBlur;
	float dofFarBlur;
	float dofViewmodelStart;
	float dofViewmodelEnd;
	objective_t objective[0x20];
	char unknown_data3[0x2958];				//[0x2F68];
};

#pragma pack(push, 4)
struct entityShared_t
{
  float mins[3];
  float maxs[3];
  int contents;
  float absmin[3];
  float absmax[3];
  float currentOrigin[3];
  float currentAngles[3];
  int ownerNum;
  int eventTime;
};
#pragma pack(pop)

struct Achievements
{
  char achievementName[32];
  int achievementId;
};

typedef float vec2_t[2];
typedef float vec3_t[3];
typedef float vec4_t[4];


struct gclient_s
{
	playerState_s ps;
	clientSession_t sess;
	int spectatorClient;
	int mFlags;
	/*
	0x1 - Noclip
	0x2 - UFO
	0x4 - DisableControls
	0x8 - DisableUseablity
	*/

	char unknownData2[0x8];
	//RT = 1
	//LS = 2
	//RS = 4
	//X = 0x28
	//B = 0x100
	//A = 0x400
	//LT = 0x800
	//RB = 0x4000
	//LB = 0x8000
	//Start = 0x400000
	int buttons;
	int oldbuttons;
	char unknownData[0x138];
	int gameTime;

	static gclient_s * GetGClient(int clientIndex);

	unsigned __int64 GetXUID();
	int GetGUID();
	bool FragButtonPressed();
	bool SecondaryOffhandButtonPressed();
	bool UseButtonPressed();
	bool MeleeButtonPressed();
	bool ADSButtonPressed();
	bool AttackButtonPressed();
	bool SprintButtonPressed();
	bool JumpButtonPressed();
	bool CrouchButtonPressed();
	bool StartMenuOpen();
	bool ToggleNoclipMode();
	bool GetNoclipMode();
	bool ToggleUFOMode();
	bool GetUFOMode();
	bool ToggleDisableControls();
	bool GetDisableControls();
	bool ToggleUseability();
	bool GetUseability();
	bool ToggleFOFOverlay();
	bool GetFOFOverlay();
	bool ToggleThermalVision();
	bool GetThermalVision();
	bool ToggleEMPJam();
	bool GetEMPJam();
	bool ToggleNightVision();
	bool GetNightVision();
	bool ToggleUsingEquipment();
	bool GetUsingEquipment();
	bool ToggleSpectating();
	bool GetSpectating();
	bool IsOnGround();
	bool IsDualWielding();
	bool IsReloading();
	bool IsSwitchingWeapon();
	bool ToggleRecoilScale();
	int SetRecoilScale(int recoilScale);
	int GetRecoilScale();
	float GetPlayerViewHeight();
	float PlayerADS();
	void SetPerk(char * perkName, bool isCodePerk);
	float *SetVelocity(float *input);
	float *GetVelocity();

	//Max is 64
	int SetSpreadOverride(int spreadOverride);
	int GetSpreadOverride();
	bool ToggleDisableADS();
	bool GetDisableADS();
	bool ToggleDisableWeaponSwitching();
	bool GetDisableWeaponSwitching();
	bool ToggleDisableWeaponUseage();
	bool GetDisableWeaponUseage();
	bool ToggleDisableOffhandWeaponUseage();
	bool GetDisableOffhandWeaponUseage();
};
	
struct gentity_s
{
	int clientNum;
	char unknown_data3[0x100];
	entityShared_t r;
	gclient_s * clientData;
	char unknown_data2[0x28];
	unsigned __int16 model;
	char physicsObject;
	char takedamage;
		
	static gentity_s * AddTestClient(int classIndex, char * teamSelection, char * inputBuffer);
	static gentity_s * AddTestClient(int classIndex, char * inputBuffer);
	static gentity_s * AddTestClient(char * inputBuffer);
	static gentity_s * GetGEntity(int clientIndex);
	gclient_s * GetGClient();
	bool ToggleGodMode();
	bool ToggleDemiGodMode();
	bool ToggleNoKnockback();
	bool GetGodMode();
	bool GetDemiGodMode();
	bool GetNoKnockback();
	bool IsAlive();
	void GivePlayerGun(char * gunName, char camo, bool dualWieldOverride);
	void GivePlayerGun(char * gunName, char camo);
	void GivePlayerGun(char * gunName);
	float *GetCurrentOrigin();
	float *GetCurrentAngles();
};
