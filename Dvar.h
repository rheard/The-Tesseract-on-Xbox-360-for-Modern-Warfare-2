#pragma once
#include <xtl.h>
#include <xboxmath.h>
#include <xbox.h>
#include <stdio.h>

enum DvarSetSource
{
	DVAR_SOURCE_INTERNAL = 0,
	DVAR_SOURCE_EXTERNAL = 1,
	DVAR_SOURCE_SCRIPT = 2
};

union DvarLimits {
	struct
	{
		int stringCount;
		char * * strings;
	} enumeration;
	struct
	{
		int min;
		int max;
	} integer;
	struct
	{
		float min;
		float max;
	} decimal;
};

union DvarValue {
	bool enabled;
	int integer;
	unsigned int unsignedInt;
	float value;
	float vector[4];
	char * string;
	byte color[4];
};

enum DvarType : char
{
	Dvar_Bool = 0x00,
	Dvar_Float = 0x01,
	Dvar_Vec2 = 0x02,
	Dvar_Vec3 = 0x03,
	Dvar_Vec4 = 0x04,
	Dvar_Int = 0x05,
	Dvar_Enum = 0x06,
	Dvar_String = 0x07,
	Dvar_Color = 0x08,
	Dvar_DevTweak = 0x09
};

//I think CVAR_USER_CREATED 1, check it
#define CVAR_LATCH      2  // will only change when C code next does
                 // a Cvar_Get(), so it can't be changed
                 // without proper initialization.  modified
                 // will be set, even though the value hasn't
                 // changed yet
#define CVAR_CHEAT			4	// can not be changed if cheats are disabled
#define CVAR_USERINFO   0x200 // sent to server on connect or change
#define CVAR_SERVERINFO   0x400 // sent in response to front end requests
#define	CVAR_INIT			0x800	// don't allow change from console at all,
								// but can be set from the command line
#define	CVAR_NOTIFY			0x1000	// Will notify players when changed.
#define	CVAR_ROM			0x2000	// display only, cannot be set by user at all

struct dvar_s
{
	char * dvarName;
	short flags;
	DvarType type;
	bool modified;
	DvarValue current;
	DvarValue latched;
	DvarValue reset;
	DvarLimits domain;
	dvar_s * next;
};

typedef bool (__cdecl * Dvar_IsSystemActive_t)();
extern Dvar_IsSystemActive_t Dvar_IsSystemActive;										//Returns true if the dvar system has been initiated

typedef bool (__cdecl * Dvar_IsValidName_t)(const char *dvarName);
extern Dvar_IsValidName_t Dvar_IsValidName;

typedef void (__cdecl * Dvar_AssignCurrentString_t)(dvar_s *dvar, const char *string);
extern Dvar_AssignCurrentString_t Dvar_AssignCurrentString;

typedef void (__cdecl * Dvar_AssignLatchedString_t)(dvar_s *dvar, const char *string);
extern Dvar_AssignLatchedString_t Dvar_AssignLatchedString;

typedef void (__cdecl * Dvar_AssignResetString_t)(dvar_s *dvar, const char *string);
extern Dvar_AssignResetString_t Dvar_AssignResetString;

typedef const char * (__cdecl * Dvar_EnumToString_t)(dvar_s *dvar);
extern Dvar_EnumToString_t Dvar_EnumToString;

typedef const char * (__cdecl * Dvar_IndexStringToEnumString_t)(dvar_s *dvar, const char *indexString);
extern Dvar_IndexStringToEnumString_t Dvar_IndexStringToEnumString;

typedef const char * (__cdecl * Dvar_ValueToString_t)(dvar_s *dvar, DvarValue value);
extern Dvar_ValueToString_t Dvar_ValueToString;

typedef bool (__cdecl * Dvar_StringToBool_t)(const char *string);
extern Dvar_StringToBool_t Dvar_StringToBool;

typedef int (__cdecl * Dvar_StringToInt_t)(const char *string);
extern Dvar_StringToInt_t Dvar_StringToInt;

typedef float (__cdecl * Dvar_StringToFloat_t)(const char *string);
extern Dvar_StringToFloat_t Dvar_StringToFloat;

typedef void (__cdecl * Dvar_StringToVec2_t)(const char *string, float *vector);
extern Dvar_StringToVec2_t Dvar_StringToVec2;

typedef void (__cdecl * Dvar_StringToVec3_t)(const char *string, float *vector);
extern Dvar_StringToVec3_t Dvar_StringToVec3;

typedef void (__cdecl * Dvar_StringToVec4_t)(const char *string, float *vector);
extern Dvar_StringToVec4_t Dvar_StringToVec4;

typedef int (__cdecl * Dvar_StringToEnum_t)(DvarLimits *domain, const char *string);
extern Dvar_StringToEnum_t Dvar_StringToEnum;

typedef void (__cdecl * Dvar_StringToColor_t)(const char *string, char *color);
extern Dvar_StringToColor_t Dvar_StringToColor;

typedef DvarValue * (__cdecl * Dvar_StringToValue_t)(DvarValue *result, const char type, DvarLimits domain, const char *string);
extern Dvar_StringToValue_t Dvar_StringToValue;

typedef const char * (__cdecl * Dvar_DisplayableValue_t)(dvar_s *dvar);
extern Dvar_DisplayableValue_t Dvar_DisplayableValue;

typedef const char * (__cdecl * Dvar_DisplayableResetValue_t)(dvar_s *dvar);
extern Dvar_DisplayableResetValue_t Dvar_DisplayableResetValue;

typedef const char * (__cdecl * Dvar_DisplayableLatchedValue_t)(dvar_s *dvar);
extern Dvar_DisplayableLatchedValue_t Dvar_DisplayableLatchedValue;

typedef void (__cdecl * Dvar_ClampVectorToDomain_t)(float *vector, int components, float min, float max);
extern Dvar_ClampVectorToDomain_t Dvar_ClampVectorToDomain;

typedef bool (__cdecl * Dvar_VectorInDomain_t)(const float *vector, int components, float min, float max);
extern Dvar_VectorInDomain_t Dvar_VectorInDomain;

typedef DvarValue * (__cdecl * Dvar_ClampValueToDomain_t)(DvarValue *result, char type, DvarValue value, DvarValue resetValue, DvarLimits domain);
extern Dvar_ClampValueToDomain_t Dvar_ClampValueToDomain;

typedef bool (__cdecl * Dvar_ValueInDomain_t)(char type, DvarValue value, DvarLimits domain);
extern Dvar_ValueInDomain_t Dvar_ValueInDomain;

typedef void (__cdecl * Dvar_VectorDomainToString_t)(int components, DvarLimits domain, char *outBuffer, int outBufferLen);
extern Dvar_VectorDomainToString_t Dvar_VectorDomainToString;

typedef const char * (__cdecl * Dvar_DomainToString_Internal_t)(char type, DvarLimits domain, char *outBuffer, int outBufferLen, int *outLineCount);
extern Dvar_DomainToString_Internal_t Dvar_DomainToString_Internal;

typedef const char * (__cdecl * Dvar_DomainToString_t)(char type, DvarLimits domain, char *outBuffer, int outBufferLen);
extern Dvar_DomainToString_t Dvar_DomainToString;

typedef const char * (__cdecl * Dvar_DomainToString_GetLines_t)(char type, DvarLimits domain, char *outBuffer, int outBufferLen, int *outLineCount);
extern Dvar_DomainToString_GetLines_t Dvar_DomainToString_GetLines;

typedef int (__cdecl * Dvar_ValuesEqual_t)(char type, DvarValue val0, DvarValue val1);
extern Dvar_ValuesEqual_t Dvar_ValuesEqual;

typedef void (__cdecl * Dvar_SetLatchedValue_t)(dvar_s *dvar, DvarValue value);
extern Dvar_SetLatchedValue_t Dvar_SetLatchedValue;

typedef bool (__cdecl * Dvar_HasLatchedValue_t)(dvar_s *dvar);
extern Dvar_HasLatchedValue_t Dvar_HasLatchedValue;

typedef void (__cdecl * Dvar_SetVariant_t)(dvar_s *dvar, DvarValue value, DvarSetSource source);
extern Dvar_SetVariant_t Dvar_SetVariant;

typedef dvar_s * (__cdecl * Dvar_FindMalleableVar_t)(const char* dvarName);
extern Dvar_FindMalleableVar_t Dvar_FindMalleableVar;

typedef dvar_s * (__cdecl * Dvar_FindVar_t)(const char* dvarName);
extern Dvar_FindVar_t Dvar_FindVar;

typedef void (__cdecl * Dvar_ClearModified_t)(dvar_s *dvar);
extern Dvar_ClearModified_t Dvar_ClearModified;

typedef void (__cdecl * Dvar_SetModified_t)(dvar_s *dvar);
extern Dvar_SetModified_t Dvar_SetModified;

typedef void (__cdecl * Dvar_UpdateEnumDomain_t)(dvar_s *dvar, const char **stringTable);
extern Dvar_UpdateEnumDomain_t Dvar_UpdateEnumDomain;

typedef bool (__cdecl * Dvar_GetBool_t)(const char * dvarName);
extern Dvar_GetBool_t Dvar_GetBool;

typedef int (__cdecl * Dvar_GetInt_t)(const char * dvarName);
extern Dvar_GetInt_t Dvar_GetInt;

typedef float (__cdecl * Dvar_GetFloat_t)(const char * dvarName);
extern Dvar_GetFloat_t Dvar_GetFloat;

typedef void (__cdecl * Dvar_GetVec3_t)(const char *dvarName, float *result);
extern Dvar_GetVec3_t Dvar_GetVec3;

typedef const char * (__cdecl * Dvar_GetString_t)(const char *dvarName);
extern Dvar_GetString_t Dvar_GetString;

typedef const char * (__cdecl * Dvar_GetVariantString_t)(const char *dvarName);
extern Dvar_GetVariantString_t Dvar_GetVariantString;

typedef void (__cdecl * Dvar_GetUnpackedColor_t)(dvar_s *dvar, float *expandedColor);
extern Dvar_GetUnpackedColor_t Dvar_GetUnpackedColor;

typedef void (__cdecl * Dvar_GetUnpackedColorByName_t)(const char *dvarName, float *expandedColor);
extern Dvar_GetUnpackedColorByName_t Dvar_GetUnpackedColorByName;

typedef void (__cdecl * Dvar_Shutdown_t)();
extern Dvar_Shutdown_t Dvar_Shutdown;

typedef void (__cdecl * Dvar_PerformUnregistration_t)(dvar_s *dvar);
extern Dvar_PerformUnregistration_t Dvar_PerformUnregistration;

typedef void (__cdecl * Dvar_UpdateResetValue_t)(dvar_s *dvar, DvarValue value);
extern Dvar_UpdateResetValue_t Dvar_UpdateResetValue;

typedef void (__cdecl * Dvar_ChangeResetValue_t)(dvar_s *dvar, DvarValue value);
extern Dvar_ChangeResetValue_t Dvar_ChangeResetValue;

typedef void (__cdecl * Dvar_UpdateValue_t)(dvar_s *dvar, DvarValue value);
extern Dvar_UpdateValue_t Dvar_UpdateValue;

typedef void (__cdecl * Dvar_MakeExplicitType_t)(dvar_s *dvar, const char *dvarName, char type, unsigned __int16 flags, DvarValue resetValue, DvarLimits domain);
extern Dvar_MakeExplicitType_t Dvar_MakeExplicitType;

typedef DvarValue * (__cdecl * Dvar_GetReinterpretedResetValue_t)(DvarValue *result, dvar_s *__formal, DvarValue value, char type, unsigned __int16 flags, DvarLimits domain);
extern Dvar_GetReinterpretedResetValue_t Dvar_GetReinterpretedResetValue;

typedef void (__cdecl * Dvar_ReinterpretDvar_t)(dvar_s *dvar, const char *dvarName, char type, unsigned __int16 flags, DvarValue value, DvarLimits domain);
extern Dvar_ReinterpretDvar_t Dvar_ReinterpretDvar;

typedef void (__cdecl * Dvar_MakeLatchedValueCurrent_t)(dvar_s *dvar);
extern Dvar_MakeLatchedValueCurrent_t Dvar_MakeLatchedValueCurrent;

typedef void (__cdecl * Dvar_ClearLatchedValue_t)(dvar_s *dvar);
extern Dvar_ClearLatchedValue_t Dvar_ClearLatchedValue;

typedef void (__cdecl * Dvar_Reregister_t)(dvar_s *dvar, const char *dvarName, char type, unsigned __int16 flags, DvarValue resetValue, DvarLimits domain, const char *description);
extern Dvar_Reregister_t Dvar_Reregister;

typedef dvar_s * (__cdecl * Dvar_RegisterNew_t)(const char *dvarName, char type, unsigned __int16 flags, DvarValue value, DvarLimits domain, const char *description);
extern Dvar_RegisterNew_t Dvar_RegisterNew;

typedef dvar_s * (__cdecl * Dvar_RegisterVarient_t)(const char *dvarName, char type, unsigned __int16 flags, DvarValue value, DvarLimits domain, const char *description);
extern Dvar_RegisterVarient_t Dvar_RegisterVarient;

typedef dvar_s * (__cdecl * Dvar_RegisterBool_t)(const char* dvarName, bool value, unsigned __int16 flags, const char* description);
extern Dvar_RegisterBool_t Dvar_RegisterBool;

typedef dvar_s * (__cdecl * Dvar_RegisterInt_t)(const char* dvarName, int value, int min, int max, unsigned __int16 flags, const char* description);
extern Dvar_RegisterInt_t Dvar_RegisterInt;

typedef dvar_s * (__cdecl * Dvar_RegisterFloat_t)(const char* dvarName, float value, float min, float max, unsigned __int16 flags, const char* description);
extern Dvar_RegisterFloat_t Dvar_RegisterFloat;

typedef dvar_s * (__cdecl * Dvar_RegisterVec2_t)(const char* dvarName, float x, float y, float min, float max, unsigned __int16 flags, const char* description);
extern Dvar_RegisterVec2_t Dvar_RegisterVec2;

typedef dvar_s * (__cdecl * Dvar_RegisterVec3_t)(const char* dvarName, float x, float y, float z, float min, float max, unsigned __int16 flags, const char* description);
extern Dvar_RegisterVec3_t Dvar_RegisterVec3;

typedef dvar_s * (__cdecl * Dvar_RegisterDevTweak_t)(const char* dvarName, float x, float y, float min, float max, unsigned __int16 flags, const char* description);
extern Dvar_RegisterDevTweak_t Dvar_RegisterDevTweak;

typedef dvar_s * (__cdecl * Dvar_RegisterVec4_t)(const char* dvarName, float x, float y, float z, float w, float min, float max, unsigned __int16 flags, const char* description);
extern Dvar_RegisterVec4_t Dvar_RegisterVec4;

typedef dvar_s * (__cdecl * Dvar_RegisterString_t)(const char* dvarName, const char* value, unsigned __int16 flags, const char *description);
extern Dvar_RegisterString_t Dvar_RegisterString;

typedef dvar_s * (__cdecl * Dvar_RegisterEnum_t)(const char* dvarName, const char* * valueList, int defaultIndex, unsigned __int16 flags, const char* description);
extern Dvar_RegisterEnum_t Dvar_RegisterEnum;

typedef dvar_s * (__cdecl * Dvar_RegisterColor_t)(const char* dvarName, float r, float g, float b, float a, unsigned __int16 flags, const char* description);
extern Dvar_RegisterColor_t Dvar_RegisterColor;

typedef void (__cdecl * Dvar_SetBoolFromSource_t)(dvar_s *dvar, bool value, DvarSetSource source);
extern Dvar_SetBoolFromSource_t Dvar_SetBoolFromSource;

typedef void (__cdecl * Dvar_SetIntFromSource_t)(dvar_s *dvar, int value, DvarSetSource source);
extern Dvar_SetIntFromSource_t Dvar_SetIntFromSource;

typedef void (__cdecl * Dvar_SetFloatFromSource_t)(dvar_s *dvar, float value, DvarSetSource source);
extern Dvar_SetFloatFromSource_t Dvar_SetFloatFromSource;

typedef void (__cdecl * Dvar_SetVec2FromSource_t)(dvar_s *dvar, float x, float y, DvarSetSource source);
extern Dvar_SetVec2FromSource_t Dvar_SetVec2FromSource;

typedef void (__cdecl * Dvar_SetVec3FromSource_t)(dvar_s *dvar, float x, float y, float z, DvarSetSource source);
extern Dvar_SetVec3FromSource_t Dvar_SetVec3FromSource;

typedef void (__cdecl * Dvar_SetVec4FromSource_t)(dvar_s *dvar, float x, float y, float z, float w, DvarSetSource source);
extern Dvar_SetVec4FromSource_t Dvar_SetVec4FromSource;

typedef void (__cdecl * Dvar_SetStringFromSource_t)(dvar_s *dvar, const char *string, DvarSetSource source);
extern Dvar_SetStringFromSource_t Dvar_SetStringFromSource;

typedef void (__cdecl * Dvar_SetColorFromSource_t)(dvar_s *dvar, float r, float g, float b, float a, DvarSetSource source);
extern Dvar_SetColorFromSource_t Dvar_SetColorFromSource;

typedef void (__cdecl * Dvar_SetBool_t)(dvar_s * dvar, bool value);
extern Dvar_SetBool_t Dvar_SetBool;

typedef void (__cdecl * Dvar_SetInt_t)(dvar_s * dvar, int value);
extern Dvar_SetInt_t Dvar_SetInt;

typedef void (__cdecl * Dvar_SetFloat_t)(dvar_s * dvar, float value);
extern Dvar_SetFloat_t Dvar_SetFloat;

typedef void (__cdecl * Dvar_SetVec2_t)(dvar_s *dvar, float x, float y);
extern Dvar_SetVec2_t Dvar_SetVec2;

typedef void (__cdecl * Dvar_SetVec3_t)(dvar_s *dvar, float x, float y, float z);
extern Dvar_SetVec3_t Dvar_SetVec3;

typedef void (__cdecl * Dvar_SetVec4_t)(dvar_s *dvar, float x, float y, float z, float w);
extern Dvar_SetVec4_t Dvar_SetVec4;

typedef void (__cdecl * Dvar_SetString_t)(dvar_s *dvar, const char *string);
extern Dvar_SetString_t Dvar_SetString;

typedef void (__cdecl * Dvar_SetColor_t)(dvar_s *dvar, float r, float g, float b, float a);
extern Dvar_SetColor_t Dvar_SetColor;

typedef void (__cdecl * Dvar_SetFromStringFromSource_t)(dvar_s *dvar, const char *string, DvarSetSource source);
extern Dvar_SetFromStringFromSource_t Dvar_SetFromStringFromSource;

typedef void (__cdecl * Dvar_SetFromString_t)(dvar_s *dvar, const char *string);
extern Dvar_SetFromString_t Dvar_SetFromString;

typedef void (__cdecl * Dvar_SetBoolByName_t)(const char *dvarName, bool value);
extern Dvar_SetBoolByName_t Dvar_SetBoolByName;

typedef void (__cdecl * Dvar_SetIntByName_t)(const char *dvarName, int value);
extern Dvar_SetIntByName_t Dvar_SetIntByName;

typedef void (__cdecl * Dvar_SetFloatByName_t)(const char *dvarName, float value);
extern Dvar_SetFloatByName_t Dvar_SetFloatByName;

typedef void (__cdecl * Dvar_SetVec2ByName_t)(const char *dvarName, float x, float y);
extern Dvar_SetVec2ByName_t Dvar_SetVec2ByName;

typedef void (__cdecl * Dvar_SetVec3ByName_t)(const char *dvarName, float x, float y, float z);
extern Dvar_SetVec3ByName_t Dvar_SetVec3ByName;

typedef void (__cdecl * Dvar_SetVec4ByName_t)(const char *dvarName, float x, float y, float z, float w);
extern Dvar_SetVec4ByName_t Dvar_SetVec4ByName;

typedef void (__cdecl * Dvar_SetStringByName_t)(const char *dvarName, const char *string);
extern Dvar_SetStringByName_t Dvar_SetStringByName;

typedef void (__cdecl * Dvar_SetColorByName_t)(const char *dvarName, float r, float g, float b, float a);
extern Dvar_SetColorByName_t Dvar_SetColorByName;

typedef void (__cdecl * Dvar_SetFromStringByNameFromSource_t)(const char *dvarName, const char *string, DvarSetSource source);
extern Dvar_SetFromStringByNameFromSource_t Dvar_SetFromStringByNameFromSource;

typedef void (__cdecl * Dvar_SetFromStringByName_t)(const char *dvarName, const char *string);
extern Dvar_SetFromStringByName_t Dvar_SetFromStringByName;

typedef void (__cdecl * Dvar_AddFlags_t)(dvar_s *dvar, int flags);
extern Dvar_AddFlags_t Dvar_AddFlags;

typedef void (__cdecl * Dvar_Reset_t)(dvar_s *dvar, DvarSetSource setSource);
extern Dvar_Reset_t Dvar_Reset;
