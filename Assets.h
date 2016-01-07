#ifndef MW2_ASSETS_H
#define MW2_ASSETS_H

#include "Hooking.h"

enum XAssetType : int
{
	physpreset = 0x00,
	phys_collmap = 0x01,
	xanim = 0x02,
	modelsurfs = 0x03,
	xmodel = 0x04,
	material = 0x05,
	pixelshader = 0x06,
	techset = 0x07,            //On PS3, 0x07 is vertexshader, everything else follows starting with techset at 0x08.
	image = 0x08,
	sound = 0x09,
	sndcurve = 0x0A,
	loaded_sound = 0x0B,
	col_map_sp = 0x0C,
	col_map_mp = 0x0D,
	com_map = 0x0E,
	game_map_sp = 0x0F,
	game_map_mp = 0x10,
	map_ents = 0x11,
	fx_map = 0x12,
	gfx_map = 0x13,
	lightdef = 0x14,
	ui_map = 0x15,              //Not Used
	font = 0x16,
	menufile = 0x17,
	menu = 0x18,
	localize = 0x19,
	weapon = 0x1A,
	snddriverglobals = 0x1B,
	fx = 0x1C,
	impactfx = 0x1D,
	aitype = 0x1E,              //Not Used
	mptype = 0x1F,              //Not Used
	character = 0x20,           //Not Used
	xmodelalias = 0x21,         //Not Used
	rawfile = 0x22,
	stringtable = 0x23,
	leaderboarddef = 0x24,
	structureddatadef = 0x25,
	tracer = 0x26,
	vehicle = 0x27,
	addon_map_ents = 0x28
};

extern const int g_poolSize[0x29];

struct infoParm_t
{
  char *name;
  int clearSolid;
  int surfaceFlags;
  int contents;
  int toolFlags;
};

extern infoParm_t * infoParms;			//Used as bounce type in weapon assets, count 64
extern char * * g_HitLocNames;				//Used as locDamageMultiplier type in weapon assets, count 20

extern const char * (__cdecl ** DB_XAssetGetNameHandler)(void * header);
extern void (__cdecl ** DB_XAssetSetNameHandler)(void * header, char * name);
extern int (__cdecl** DB_GetXAssetSizeHandler)();
extern char * * g_defaultAssetName;

struct Material;
struct Tracer;
struct FxEffectDef;
struct menuDef_t;
struct MaterialTechniqueSet;
struct LoadedSound;
struct SndCurve;
struct GfxImage;
struct MapEnts;

typedef float vec2_t[2];
typedef float vec3_t[3];
typedef float vec4_t[4];

typedef unsigned char cbrushedge_t;
typedef unsigned char UCharVec[3];

typedef unsigned short ScriptString;
typedef unsigned short UShortVec[3];

union GfxColor
{
  unsigned int packed;
  char array[4];
};

struct GfxPlacement
{
  float quat[4];
  float origin[3];
};

struct cplane_s
{
	float normal[3];
	float dist;
	char type;
	char signbits;
	char pad[2];
};

struct cbrushside_t
{
	cplane_s *plane;
	unsigned int materialNum;
};

struct cLeaf_t
{
	unsigned __int16 firstCollAabbIndex;
	unsigned __int16 collAabbCount;
	int brushContents;
	int terrainContents;
	float mins[3];
	float maxs[3];
	int leafBrushNode;
};

struct PhysMass
{
  float centerOfMass[3];
  float momentsOfInertia[3];
  float productsOfInertia[3];
};

#pragma region physpreset (0x0)
extern const AssetEntry PhysPresetEntries[];

//Fully mapped. Thanks IW
#pragma pack(push, 4)
struct PhysPreset
{
  const char *name;
  int type;
  float mass;
  float bounce;
  float friction;
  float bulletForceScale;
  float explosiveForceScale;
  const char *sndAliasPrefix;
  float piecesSpreadFraction;
  float piecesUpwardVelocity;
  bool tempDefaultToCylinder;

  int dumpPhysPreset();
  static void dumpAllPPs();
};
#pragma pack(pop)

extern XAssetPool<PhysPreset, 0x40> * g_physPresetPool;
#pragma endregion

#pragma region phys_collmap (0x1)
struct BrushWrapper
{
	float mins[3];
	float maxs[3];
	short cBrushSideCount;
	short unknown1;
	cbrushside_t * brushSideArray;		//count = cBrushSideCount
	cbrushedge_t * brushEdgeArray;		//Total size = cBrushEdgeCount
	char unknownData2[0x18];
	int cBrushEdgeCount;
	cplane_s * cPlane;					//count - cBrushSideCount //Total size = ((cBrushSideCount << 2) + cBrushSideCount) << 2
};

struct PhysGeomInfo
{
	BrushWrapper * wrapper;
	int type;
	float orientation[3][3];
	float offset[3];
	float halfLengths[3];
};

typedef struct PhysCollMap
{
	char * name;
	int PhysGeomInfoCount;
	PhysGeomInfo * geomInfo;			//count = PhysGeomInfoCount
	char unknownData1[0x3C];
} PhysCollMap;

extern XAssetPool<PhysCollMap, 0x400> * g_physCollMap;
#pragma endregion

#pragma region xanim (0x2)
union XAnimIndices
{
  char *_1;
  unsigned __int16 *_2;
  void *data;
};

#pragma pack(push, 4)
struct XAnimNotifyInfo
{
  unsigned __int16 name;
  float time;
};
#pragma pack(pop)

typedef union XAnimDynamicFrames
{
	UShortVec * varUShortVector;	//count = parent XAnimPartTrans::type * 3
	UCharVec * varCharVector;		//count = parent XAnimPartTrans::type * 3
} XAnimDynamicFrames;

typedef union XAnimDynamicIndicesTrans
{
	short * varUnsignedShort;
	byte * varByte;
} XAnimDynamicIndicesTrans;

struct XAnimPartTransFrames
{
	float mins[3];
	float size[3];
	XAnimDynamicFrames frames;
	/*
	if(parent XAnimPartTrans::subType == 0x0)
		Use XAnimDynamicFrames::varUShortVector
	else
		Use XAnimDynamicFrames::verCharVector
	*/
	XAnimDynamicIndicesTrans indices;
	/*
	if(parent XAnimParts::numFrames >= 0x100)
		Use XAnimDynamicIndicesTrans::varUnsignedShort
	else
		Use XAnimDynamicIndicesTrans::varByte
	*/
};

union XAnimPartTransData
{
	XAnimPartTransFrames frames;
	float frame0[3];
};

typedef struct XAnimPartTrans
{
	short type;
	short subType;
	XAnimPartTransData data;
	/*
	if(type == 0)
		Use XAnimPartTransData::vector
	else
		Use XAnimPartTransData::frame
	*/
} XAnimPartTrans;

union XAnimDynamicIndices
{
  char _1[1];
  unsigned __int16 _2[1];
};

struct XAnimDeltaPartQuatDataFrames
{
	__int16 (*frames)[2];
	XAnimDynamicIndices indices;
};

union XAnimDeltaPartQuatData
{
	XAnimDeltaPartQuatDataFrames frames;
	__int16 frame0[2];
};

struct XAnimDeltaPartQuat
{
	short type;
	short subType;
	XAnimDeltaPartQuatData u;
	/*
	if(type == 0)
		Use XAnimPartTransData::vector
	else
		Use XAnimPartTransData::frame
	*/
};

struct XAnimDeltaPart
{
  XAnimPartTrans *trans;
  XAnimDeltaPartQuat *quat;
  void * unknown;
};

typedef struct XAnimParts
{
	char * name;
	short dataByteCount;
	short dataShortCount;
	short dataIntCount;
	short randomDataByteCount;
	short randomDataIntCount;
	short numframes;
	bool bLoop;
	bool bDelta;
	byte boneCount[12];
	byte notifyCount;
	byte assetType;
	unsigned int randomDataShortCount;
	unsigned int indexCount;
	float framerate;
	float frequency;
	unsigned __int16 * names;	//Count = byte at 0x1C
	byte * dataByte;		//count = dataByteCount
	short * dataShort;		//count = dataShortCount
	int * dataInt;			//count = dataIntCount
	short * randomDataShort;//count = randomDataShortCount
	byte * randomDataByte;	//count = randomDataByteCount
	int * randomDataInt;	//count = randomDataIntCount
	XAnimIndices indices;	/*
	if(numFrames >= 0x100)
		Use XAnimIndice::dataShorts
	else
		Use XAnimIndice::dataBytes

	count = indexCount;
	*/
	XAnimNotifyInfo * notify;	//count = byte at 0x1D
	XAnimDeltaPart * deltaPart;
} XAnimParts;

extern XAssetPool<XAnimParts, 0x1000> * g_xAnimPool;
#pragma endregion

#pragma region modelsurfs (0x3)
typedef unsigned short XBlendInfo;

union PackedTexCoords
{
  unsigned int packed;
};

union PackedUnitVec
{
  unsigned int packed;
};

struct GfxPackedVertex
{
  float origin[3];
  float binormalSign;
  GfxColor color;
  PackedTexCoords texCoord;
  PackedUnitVec normal;
  PackedUnitVec tangent;
};

struct XSurfaceCollisionAabb
{
  unsigned __int16 mins[3];
  unsigned __int16 maxs[3];
};

struct XSurfaceCollisionNode
{
  XSurfaceCollisionAabb aabb;
  unsigned __int16 childBeginIndex;
  unsigned __int16 childCount;
};

struct XSurfaceCollisionLeaf
{
  unsigned __int16 triangleBeginIndex;
};

struct XSurfaceVertexInfo
{
	__int16 vertCount[4];
	unsigned __int16 *vertsBlend;

	/*
	Count... Ok, here we go...

	( 
	( (int)count1 + ( (int)count1 << 1) ) + 
	( ( (int)count3 << 3 ) - (int)count3 ) + 
	( (int)count2 + ( (int)count2 << 2 ) 
	) 
	+ (int)count0) << 1

	ahh... fuck... my head hurts...
	*/
};

struct XSurfaceCollisionTree
{
	float trans[3];
	float scale[3];
	unsigned int nodeCount;
	XSurfaceCollisionNode * nodes;
	unsigned int leafCount;
	XSurfaceCollisionLeaf * leafs;
};

struct XRigidVertList
{
	unsigned __int16 boneOffset;
	unsigned __int16 vertCount;
	unsigned __int16 triOffset;
	unsigned __int16 triCount;
	XSurfaceCollisionTree * collisionTree;
};

typedef struct XSurface
{
	char tileMode;
	bool deformed;
	unsigned __int16 vertCount;
	unsigned __int16 triCount;
	short _padding;
	unsigned __int16 (*triIndices)[3];									//Count = triCount * 3 (groups of 3) WARNING: THIS IS LOADED LAST (Index buffer is actually loaded later, but idk whats in it...
	XSurfaceVertexInfo vertexInfo;
	GfxPackedVertex * packedVerticies;						//Count = vertCount (total size = vertCount << 5)
	IDirect3DVertexBuffer9 vertexBuffer;							//This thing is fuckin weird
	unsigned int vertListCount;
	XRigidVertList * rigidVertLists;						//Count = rigidVertListCount
	IDirect3DIndexBuffer9 indexBuffer;
	char unknownData[0x18];
} XSurface;

typedef struct ModelSurface
{
	char * name;
	XSurface * xSurficies;
	short xSurficiesCount;
	short unknownShort;
	char unknown[0x18];

	int dumpSurface(FILE * writeLog, int verticiesLastSurface = 0, float * offset = 0, char * objectName = 0);
	static void dumpAllSurfaces();
} ModelSurface;

extern XAssetPool<ModelSurface, 0x1000> * g_modelSurfPool;
#pragma endregion

#pragma region xmodel (0x4)
struct DObjAnimMat
{
  float quat[4];
  float trans[3];
  float transWeight;
};

typedef char XModelHighMipBounds[0x24];

struct XBoneInfo
{
  float bounds[2][3];
  float radiusSquared;
};

struct XModelCollSurf
{
	float unknown1;
	long unknown2;
	ModelSurface * collSurf;
	char unknownData[0x1C];
};

struct XModel
{
	char * name;
	char numBones;
	char numRootBones;
	char numsurfs;
	char undefined;
	char unknownData1[0x1C];
	unsigned short * boneNames;				//count = numBones
	unsigned char * parentList;				//size = numBones - numRootBones
	unsigned short * quats;					//size = (numBones - numRootBones) << 3
	float * trans;							//size = (((numBones - numRootBones) << 1) + (numBones - numRootBones)) << 2
	unsigned char * partClassification;		//size = numBones
	DObjAnimMat * baseMat;					//size = numBones << 5
	Material * * materialArray;		//count = numsurfs
	XModelCollSurf collSurfs[4];
	long unknown3;
	XModelHighMipBounds * highMipBounds;				//size = ((unknownDataCount1 << 3) + unknownDataCount1) << 2
	int unknownDataCount1;
	int unknownInt1;
	XBoneInfo * boneInfo;						//size = (numBones * 0x1C)
	byte unknownData3[0x1C];
	short * unknownDataPointer3;				//size = (numsurfs << 1)
	int unknownInt2;
	PhysPreset * physPreset;
	PhysCollMap * physGeoms;
};

extern XAssetPool<XModel, 0x600> * g_xModelPool;
#pragma endregion

#pragma region material (0x5)
struct GfxDrawSurfFields
{
  __int64 _bf0;
};

union GfxDrawSurf
{
  GfxDrawSurfFields fields;
  unsigned __int64 packed;
};

struct __declspec(align(8)) MaterialInfo
{
  const char *name;
  char gameFlags;
  char sortKey;
  char textureAtlasRowCount;
  char textureAtlasColumnCount;
  GfxDrawSurf drawSurf;
  unsigned int surfaceTypeBits;
};

struct WaterWritable
{
  float floatTime;
};

struct water_t
{
	WaterWritable writable;
	float * H0X;					//Total size = (M * N) << 2
	float * H0Y;				//Total size = (M * N) << 2
	float * wTerm;			//Total size = (M * N) << 2
	int M;
	int N;
	float Lx;
	float Lz;
	float gravity;
	float windvel;
	float winddir[2];
	float amplitude;
	float codeConstant[4];
	GfxImage * image;
} ;

union MaterialTextureDefInfo
{
	GfxImage * image;
	water_t * water;
};

struct MaterialTextureDef
{
	unsigned int nameHash;
	char nameStart;
	char nameEnd;
	char samplerState;
	char semantic;
	MaterialTextureDefInfo info;
	//if semantic != 0xB, then use MaterialTextureDefInfo::image
	//else use MaterialTextureDefInfo::water
};

struct MaterialConstantDef
{
  unsigned int nameHash;
  char name[12];
  float literal[4];
};

struct GfxStateBits
{
	unsigned int loadBits[2];
};

struct Material
{
	MaterialInfo info;
	char stateBitsEntry[0x21];
	char textureCount;
	char constantCount;
	char stateBitsCount;
	char stateFlags;
	char cameraRegion;
	short undefined;
	MaterialTechniqueSet * techniqueSet;
	MaterialTextureDef * textureTable;
	MaterialConstantDef * constantTable;
	GfxStateBits * stateBitsTable;
	char ** unknownXStringTable1;
	long unknown;
};

extern XAssetPool<Material, 0x1000> * g_materialPool;
#pragma endregion

#pragma region pixelshader (0x6)
struct GfxPixelShaderLoadDef
{
  char *cachedPart;						//Loaded second
  char *physicalPart;					//Loaded first
  unsigned __int16 cachedPartSize;
  unsigned __int16 physicalPartSize;
};

typedef struct MaterialPixelShader
{
	char * name;
	GfxPixelShaderLoadDef prog;
} MaterialPixelShader;

extern XAssetPool<MaterialPixelShader, 0x1FA0> * g_pixelShaderPool;
#pragma endregion

#pragma region techset (0x7)
struct MaterialStreamRouting
{
  char source;
  char dest;
};

union MaterialVertexStreamRouting
{
  MaterialStreamRouting data[16];
  D3DVertexDeclaration *decl[15];
};

struct MaterialVertexDeclaration
{
  char streamCount;
  bool hasOptionalSource;
  char _padding[2];
  MaterialVertexStreamRouting routing;
};

struct GfxVertexShaderLoadDef
{
  char *cachedPart;
  char *physicalPart;
  unsigned __int16 cachedPartSize;
  unsigned __int16 physicalPartSize;
};

union MaterialVertexShaderProgram
{
  D3DVertexShader *vs;
  GfxVertexShaderLoadDef loadDef;
};

struct MaterialVertexShader
{
  const char *name;
  MaterialVertexShaderProgram prog;
};

struct MaterialArgumentCodeConst
{
  unsigned __int16 index;
  char firstRow;
  char rowCount;
};

union MaterialArgumentDef
{
  const float *literalConst;
  MaterialArgumentCodeConst codeConst;
  unsigned int codeSampler;
  unsigned int nameHash;
};

struct MaterialShaderArgument
{
  unsigned __int16 type;
  unsigned __int16 dest;
  MaterialArgumentDef u;
};

struct MaterialPass
{
	MaterialVertexDeclaration *vertexDecl;
	MaterialVertexShader *vertexShaderArray[15];
	MaterialVertexShader *vertexShader;
	MaterialPixelShader *pixelShader;
	char perPrimArgCount;
	char perObjArgCount;
	char stableArgCount;
	char customSamplerFlags;
	char precompiledIndex;
	MaterialShaderArgument *args;
};

typedef struct MaterialTechnique
{
	const char *name; //Loaded after the passArray
	unsigned __int16 flags;
	unsigned __int16 passCount;
	MaterialPass passArray[1];
} MaterialTechnique;

typedef struct MaterialTechniqueSet
{
	const char *name;
	byte worldVertFormat;
	byte unused[3];
	MaterialTechniqueSet * remappedTechniqueSet;		//only used in mem
	MaterialTechnique * techniques[0x21];
} MaterialTechniqueSet;

extern XAssetPool<MaterialTechniqueSet, 0x300> * g_techSetPool;
#pragma endregion

#pragma region image (0x8)
enum DWFLAGS
{
	DDSD_CAPS = 1,
	DDSD_HEIGHT = 2,
	DDSD_WIDTH = 4,
	DDSD_PITCH = 8,
	DDSD_PIXELFORMAT = 0x1000,
	DDSD_MIPMAPCOUNT = 0x20000,
	DDSD_LINEARSIZE = 0x80000,
	DDSD_DEPTH = 0x800000
};

union GfxTexture
{ //size = 0x34
  D3DBaseTexture basemap;
  D3DTexture map;
  D3DVolumeTexture volmap;
  D3DCubeTexture cubemap;
};
 
struct CardMemory
{
  int platform[1];
};
 
typedef struct GfxImage
{
  GfxTexture texture;
  char unknown[4];
  char mapType;
  char semantic;
  char category;
  char flags;
  CardMemory cardMemory;
  unsigned __int16 width;
  unsigned __int16 height;
  unsigned __int16 depth;
  char mipmap;
  bool streaming;
  char *pixels;
	char unknownData[0x20];
  const char *name;				//LOADED FIRST

	int dumpImage(bool dumpStreamedImages);
	static void dumpAllImages();
} GfxImage;

extern XAssetPool<GfxImage, 0xE00> * g_imagePool;
#pragma endregion

#pragma region sound (0x9)
struct XAUDIOCHANNELMAPENTRY
{
  char InputChannel;
  char OutputChannel;
  char _padding[2];
  float Volume;
};

struct XAUDIOCHANNELMAP
{
	int EntryCount;
	XAUDIOCHANNELMAPENTRY * paEntries; //Total size = entryCount << 3
};

#pragma pack(push, 4)
struct SpeakerMap
{
  bool isDefault;
  const char *name;
  XAUDIOCHANNELMAP channelMaps[2][2];
};
#pragma pack(pop)

typedef struct unknownSoundStruct1
{
	int type;	//If type is 0, then load these strings...
	char * strings[2];
} unknownSoundStruct1;

typedef union unknownSoundUnion
{
	unknownSoundStruct1 unknownStruct;
	LoadedSound * LoadedSoundPtr;
} unknownSoundUnion;

typedef struct SoundFile
{
	byte type;
	byte _undefined[3];
	unknownSoundUnion unknownUnion;
	//If type == 1, use LoadedSoundPtr
	//
} SoundFile;

typedef struct snd_alias_t
{
	char * aliasName;
	char * subtitle;
	char * secondaryAliasName;
	char * chainAliasName;
	char * unknownString4;
	SoundFile * soundFile;
	char unknownData[0x38];
	SndCurve * volumeFalloffCurve;
	float envelopMin;
	float envelopMax;
	float envelopPercentage;
	SpeakerMap * speakerMap;
} snd_alias_t;

typedef struct snd_alias_list_t
{
	char * aliasName;
	snd_alias_t * head;
	int count;
} snd_alias_list_t;

extern XAssetPool<snd_alias_list_t, 0x3E80> * g_soundPool;
#pragma endregion

#pragma region sndcurve (0xA)
typedef struct SndCurve
{
	char * name;
	int knotCount;
	float knots[0x20];
} SndCurve;

extern XAssetPool<SndCurve, 0x40> * g_SNDCurvePool;
#pragma endregion

#pragma region loaded_sound (0xB)
struct XMALOOPREGION
{
  unsigned int LoopStart;
  unsigned int LoopEnd;
  char LoopSubframeEnd;
  char LoopSubframeSkip;
  char _unused[2];
};

struct XAUDIOPACKET_ALIGNED
{
  char *pBuffer;				//count = BufferSize
  unsigned int BufferSize;
  unsigned int LoopCount;
  XMALOOPREGION XMALoop[6];
  char *pContext;				//count = 1
};

typedef struct XaSeekTable
{
	int size;
	unsigned int *data;				//count = count (total size = count << 2)
} XaSeekTable;

struct XAUDIOXMAFORMAT
{
  unsigned int SampleRate;
  char ChannelCount;
  char DecodeBufferSize;
  char _padding[2];
};

union $9D91AD7FD1BA7330B7FA3BAC24377F35
{
  char NumStreams;
  char ChannelCount;
};

union $A2A60DCC2C958E872EECE00B123AF832
{
  XAUDIOXMAFORMAT Stream[6];
  unsigned int SampleRate;
};

struct XAUDIOSOURCEFORMAT
{
  char SampleType;
  __declspec(align(4)) $9D91AD7FD1BA7330B7FA3BAC24377F35 ___u1;
  __declspec(align(4)) $A2A60DCC2C958E872EECE00B123AF832 ___u2;
};

struct XaIwXmaDataInfo
{
  int totalMsec;
};

struct XaSound
{
  XAUDIOPACKET_ALIGNED packet;
  XAUDIOSOURCEFORMAT format;
  XaIwXmaDataInfo xaIwXmaDataInfo;
  XaSeekTable seekTable;
};

typedef struct LoadedSound
{
	char * name;
	XaSound soundData;
} LoadedSound;

extern XAssetPool<LoadedSound, 0x546> * g_loadedSoundPool;
#pragma endregion

#pragma region col_map (0xC && 0xD)
struct dmaterial_t
{
	char * material;
	int surfaceFlags;
	int contentFlags;
};

struct cStaticModel_t
{
	XModel * xmodel;
	float origin[3];
	float invScaledAxis[3][3];
	float absmin[3];
	float absmax[3];
};

struct cNode_t
{
  cplane_s *plane;
  __int16 children[2];
};

struct cLeafBrushNodeLeaf_t
{
  unsigned __int16 *brushes;
};

struct cLeafBrushNodeChildren_t
{
  float dist;
  float range;
  unsigned __int16 childOffset[2];
};

union cLeafBrushNodeData_t
{
  cLeafBrushNodeLeaf_t leaf;
  cLeafBrushNodeChildren_t children;
};

struct cLeafBrushNode_s
{
  char axis;
  __int16 leafBrushCount;
  int contents;
  cLeafBrushNodeData_t data;
};

struct CollisionBorder
{
  float distEq[3];
  float zBase;
  float zSlope;
  float start;
  float length;
};

struct CollisionPartition
{
  char triCount;
  char borderCount;
  int firstTri;
  CollisionBorder *borders;
};

union CollisionAabbTreeIndex
{
  int firstChildIndex;
  int partitionIndex;
};

struct CollisionAabbTree
{
  float origin[3];
  float halfSize[3];
  unsigned __int16 materialIndex;
  unsigned __int16 childCount;
  CollisionAabbTreeIndex u;
};

typedef struct cbrush_t
{
	int contents;
	cbrushside_t * sides;
	cbrushedge_t * edge;						//Total size = 1
	char unknownData[0x18];
} cbrush_t;

struct cmodel_t
{
  float mins[3];
  float maxs[3];
  float radius;
  cLeaf_t leaf;
};

enum DynEntityType : int
{
  DYNENT_TYPE_INVALID = 0x0,
  DYNENT_TYPE_CLUTTER = 0x1,
  DYNENT_TYPE_DESTRUCT = 0x2,
  DYNENT_TYPE_COUNT = 0x3,
};

struct DynEntityDef
{
	DynEntityType type;
	GfxPlacement pose;
	XModel * xModel;
	unsigned __int16 brushModel;
	unsigned __int16 physicsBrushModel;
	FxEffectDef * destroyFx;
	PhysPreset * physPreset;
	int health;
	PhysMass mass;
	int contents;
};

struct DynEntityPose
{
  GfxPlacement pose;
  float radius;
};

struct DynEntityClient
{
  int physObjId;
  unsigned __int16 flags;
  unsigned __int16 lightingHandle;
  int health;
};

struct DynEntityColl
{
  unsigned __int16 sector;
  unsigned __int16 nextEntInSector;
  float linkMins[2];
  float linkMaxs[2];
};

typedef struct clipMap_t
{
	char * name;
	long unknown1;
	int planeCount;
	cplane_s * planes;							//Total size = ((planeCount << 2) + planeCount) << 2
	unsigned int numStaticModels;
	cStaticModel_t * staticModelList;			//count = numStaticModels
	unsigned int numMaterials;
	dmaterial_t * materials;	//count = unknownCount1
	unsigned int numBrushSides;
	cbrushside_t * brushsides;			//count = cBrushSideCount
	unsigned int numBrushEdges;
	cbrushedge_t * brushEdges;					//Total size = cBrushEdgeCount
	unsigned int numNodes;
	cNode_t * nodes;					//count = cNodeCount
	unsigned int numLeafs;
	cLeaf_t * leafs;						//Total size = ((cLeafCount << 2) + cLeafCount) << 3
	unsigned int leafbrushNodesCount;
	cLeafBrushNode_s * leafbrushNodes;	//count = cLeafBrushNodeCount (WARNING: LeafBrushArray is loaded before this!)
	unsigned int numLeafBrushes;
	unsigned __int16 * leafbrushes;					//Total size = LeafBrushCount << 1
	unsigned int numLeafSurfaces;
	unsigned int * leafsurfaces;				//Total size = uIntCount << 2
	unsigned int vertCount;
	float (*verts)[3];						//Total size = ((vec3Count << 1) + vec3Count) << 2
	int triCount;
	unsigned __int16 (*triIndices)[3];			//Total size = ((triCount << 1) + triCount) << 1
	char * triEdgeIsWalkable;						//Total size = ((((triCount << 1) + triCount) + 0x1F) >> 3) << 2 (I think?)
	int borderCount;
	CollisionBorder *borders;			//Total size = collisionBorderCount * 0x1C
	int partitionCount;
	CollisionPartition * partitions;	//count = collisionPartitionCount;
	int aabbTreeCount;
	CollisionAabbTree * aabbTrees;			//Total size = collisionAabbTreeCount << 5
	unsigned int numSubModels;
	cmodel_t * cmodels;					//Total size = unknownCount2 * 0x44
	unsigned __int16 numBrushes;
	short unknownCount;
	cbrush_t * brushes;					//count = cBrushCount
	char * unknownData2;					//Total size = ((cBrushCount << 1) + cBrushCount) << 3
	char * unknownData3;					//Total size = cBrushCount << 2
	MapEnts * mapEnts;						//map_ents pointer (WARNING: unknownData4 is loaded first)
	short unknownCount3;
	short unknownCount4;
	char * unknownData4;					//Total size = unknownCount3 * 0x1C
	unsigned __int16 dynEntCount[2];
	DynEntityDef *dynEntDefList[2];
	DynEntityPose *dynEntPoseList[2];				//Total size = DynEntityDefCount1 << 5
	DynEntityClient *dynEntClientList[2];			//Total size = ((DynEntityDefCount1 << 1) + DynEntityDefCount1) << 2
	DynEntityColl *dynEntCollList[2];				//Total size = ((DynEntityDefCount1 << 2) + DynEntityDefCount1) << 2
	char unknownData5[0x34];

	static int dumpCollisionMap();
} clipMap_t;

extern clipMap_t * g_colMap;
#pragma endregion

#pragma region com_map (0xE)
typedef struct ComPrimaryLight
{
	char type;
	char canUseShadowMap;
	char exponent;
	char unused;
	float color[3];
	float dir[3];
	float origin[3];
	float radius;
	float cosHalfFovOuter;
	float cosHalfFovInner;
	float cosHalfFovExpanded;
	float rotationLimit;
	float translationLimit;
	char *defName;
} ComPrimaryLight;

typedef struct ComWorld
{
	char * name;
	int isInUse;
	unsigned int primaryLightCount;
	ComPrimaryLight * primaryLights;				//Count = ComPrimaryLightCount
} ComWorld;

extern ComWorld * g_comMap;
#pragma endregion

#pragma region map_ents (0x11)
struct unknownInternalMapEnts1
{
	int unknownCount1;
	char * unknownData1;			//size = unknownCount1 << 3
	int unknownCount2;
	char * unknownData2;			//size = unknownCount2 << 5
	int unknownCount3;
	char * unknownData3;			//size = ((unknownCount3 << 2) + unknownCount3) << 2
};

struct Stage
{
	char * stageName;
	float offset[3];
	int flags;
};

struct MapEnts
{
	char * name;
	char * entityString;			//size = numEntityChars
	int numEntityChars;
	unknownInternalMapEnts1 unknownStruct1;
	Stage * stages;
	byte unknownCount;
	byte _padding[3];

	int dumpMapEntities();
	static void dumpAllMapEntities();
};

extern XAssetPool<MapEnts, 2> * g_mapEntsPool;
#pragma endregion

#pragma region fx_map (0x12)
struct UnknownStruct1   // 0x2C
    {
            char unknown1[0x18];
            Material *material1;            // loaded 2nd
            Material *material2;            // loaded 3rd
            PhysPreset *physPreset; // loaded 1st
            char unknown2[8];
    };
     
    struct FxWorldStruct1   // 0x70
    {
            char unknown1[8];
            int unknownCount1;      // 8
            int unknownCount2;      // 0xC
            int unknownCount4;      // 0x10
            int unknwonCount6;      // 0x14
            int unknownCount5;      // 0x18
            char unknown3[0xC];
            int unknownCount3;      // 0x24
            char unknown4[4];
            int unknownCount7;      // 0x2C
            UnknownStruct1 *unknownStruct1; // 0x30 count = unknownCount1;
            char *unknownData2;             // 0x34 size = unknownCount2 << 5 or unknownCount2 * 0x20
            char *unknownData3;             // 0x38 size = unknownCount2 << 5 or unknownCount2 * 0x20
            char *unknownData4;             // 0x3C size = unknownCount2 * 0x24
            char *unknownData5;             // 0x40 size = unknownCount3 << 2 or unknownCount3 * 4
            char *unknownData6;             // 0x44 size = unknownCount4 << 2 or unknownCount4 * 4
            char *unknownData7;             // 0x48 size = (unknownCount4 * unknownCount5) << 2
            char *unknownData8;             // 0x4C size = (unknownCount2 + 0xF)
            char *unknownData9;             // 0x50 size = unknwonCount2 * 0x10
            char *unknownData10;            // 0x54 size = (unknownCount2 + 3) << 2
        __int16 *unknownData11;         // 0x58 size = unknownCount6 << 1
        char *unknownData12;            // 0x5C size = unknownCount6 * 0x34
        char *unknwonData13;            // 0x60 size = unknownCount7 << 2
           
};
     
struct FxWorld  // 0x74
{
        char *name;
        FxWorldStruct1 unknownStruct;
};
#pragma endregion

#pragma region gfx_map (0x13)
struct GfxWorldDpvsPlanes
{
  int cellCount;
  cplane_s *planes;
  unsigned __int16 *nodes;
  unsigned int * raw_uint;			//added from cod4
};

struct GfxReflectionProbe
{
  float origin[3];
  GfxImage *reflectionImage;
};

struct GfxAabbTree
{
  float mins[3];
  float maxs[3];
  unsigned __int16 childCount;
  unsigned __int16 surfaceCount;
  unsigned __int16 startSurfIndex;
  unsigned __int16 smodelIndexCount;
  unsigned __int16 *smodelIndexes;
  int childrenOffset;
};

struct unknownGFXWorldStruct_t
{
	GfxAabbTree * aabbTrees;			//count = i have no fuckin clue
};

struct GfxPortalWritable
{
  bool isQueued;
  bool isAncestor;
  char recursionDepth;
  char hullPointCount;
  float (*hullPoints)[2];
};

struct DpvsPlane
{
  float coeffs[4];
  char side[3];
  char pad;
};

struct GfxPortal
{
  GfxPortalWritable writable;
  DpvsPlane plane;
  float *vertices;
  short unknown;
  char vertexCount;
  char _padding;
  float hullAxis[2][3];
};

struct GfxCell
{
  float mins[3];
  float maxs[3];
  int portalCount;
  GfxPortal *portals;
  int cullGroupCount;
  int *cullGroups;
};

struct GfxLightmapArray
{
  GfxImage *primary;
  GfxImage *secondary;
};

struct GfxWorldVertex
{
  float xyz[3];
  float binormalSign;
  GfxColor color;
  float texCoord[2];
  float lmapCoord[2];
  PackedUnitVec normal;
  PackedUnitVec tangent;
};

struct GfxWorldVertexData
{
  GfxWorldVertex *vertices;
  D3DVertexBuffer worldVb;
};

struct GfxWorldVertexLayerData
{
  char *data;
  D3DVertexBuffer layerVb;
};

struct unknownGFXWorldStruct2_t
{
	int unknownCount1;
	GfxImage * imageArray;			//Count = unknownCount1
	char * unknownData1;			//Size = ((unknownCount1 < 1) + unknownCount1) << 2
	char * unknownData2;			//Size = unknownCount1 * 0x34
	int lightmapArraySize;
	GfxLightmapArray * lightmapArrayArray;
	char * unknownData3;			//Size = lightmapArraySize * 0x34 (same structure as unknownData2)
	char * unknownData4;			//Size = lightmapArraySize * 0x34 (same structure as unknownData2)
	GfxImage * image1;
	GfxImage * image2;
	int vertCount;
	GfxWorldVertexData vertexData;
	long unknownLong2;
	GfxWorldVertexLayerData vertexLayerData;
	int indexCount;
	unsigned __int16 *indices;
	D3DIndexBuffer buffer;
};

struct GfxLightGridEntry
{
  unsigned __int16 colorsIndex;
  char primaryLightIndex;
  char needsTrace;
};

struct GfxLightGridColors
{
  char rgb[56][3];
};

struct GfxLightGrid
{
  bool hasLightRegions;
  char _padding[3];
  unsigned int sunPrimaryLightIndex;
  unsigned __int16 mins[3];
  unsigned __int16 maxs[3];
  unsigned int rowAxis;
  unsigned int colAxis;
  unsigned __int16 *rowDataStart;
  unsigned int rawRowDataSize;
  char *rawRowData;
  unsigned int entryCount;
  GfxLightGridEntry *entries;
  unsigned int colorCount;
  GfxLightGridColors *colors;
};

struct GfxBrushModelWritable
{
  float mins[3];
  float maxs[3];
};

struct GfxBrushModel
{
  GfxBrushModelWritable writable;
  float bounds[2][3];
  unsigned int surfaceCount;
  unsigned int startSurfIndex;
};

struct MaterialMemory
{
  Material *material;
  int memory;
};

struct sunflare_t
{
  bool hasValidData;
  char _padding[3];
  Material *spriteMaterial;
  Material *flareMaterial;
  float spriteSize;
  float flareMinSize;
  float flareMinDot;
  float flareMaxSize;
  float flareMaxDot;
  float flareMaxAlpha;
  int flareFadeInTime;
  int flareFadeOutTime;
  float blindMinDot;
  float blindMaxDot;
  float blindMaxDarken;
  int blindFadeInTime;
  int blindFadeOutTime;
  float glareMinDot;
  float glareMaxDot;
  float glareMaxLighten;
  int glareFadeInTime;
  int glareFadeOutTime;
  float sunFxPosition[3];
};

struct XModelDrawInfo
{
  unsigned __int16 lod;
  unsigned __int16 surfId;
};

struct GfxSceneDynModel
{
  XModelDrawInfo info;
  unsigned __int16 dynEntId;
};

struct BModelDrawInfo
{
  unsigned __int16 surfId;
};

struct GfxSceneDynBrush
{
  BModelDrawInfo info;
  unsigned __int16 dynEntId;
};

struct GfxShadowGeometry
{
  unsigned __int16 surfaceCount;
  unsigned __int16 smodelCount;
  unsigned __int16 *sortedSurfIndex;
  unsigned __int16 *smodelIndex;
};

struct GfxLightRegionAxis
{
  float dir[3];
  float midPoint;
  float halfSize;
};

struct GfxLightRegionHull
{
  float kdopMidPoint[9];
  float kdopHalfSize[9];
  unsigned int axisCount;
  GfxLightRegionAxis *axis;
};

struct GfxLightRegion
{
  unsigned int hullCount;
  GfxLightRegionHull *hulls;
};

struct GfxStaticModelInst
{
  float mins[3];
  float maxs[3];
  GfxColor groundLighting;
  char unknown_data[8];
};

struct GfxSurface
{
  char unknown_data[0x10];
  Material *material;
  char lightmapIndex;
  char reflectionProbeIndex;
  char primaryLightIndex;
  bool castsSunShadow;
};

struct GfxCullGroup
{
  float mins[3];
  float maxs[3];
  int surfaceCount;
  int startSurfIndex;
};

struct GfxPackedPlacement
{
  float origin[3];
  PackedUnitVec axis[3];
  float scale;
};

struct GfxStaticModelDrawInst
{
  GfxPackedPlacement placement;
  XModel *model;
  float cullDist;
  char reflectionProbeIndex;
  char primaryLightIndex;
  unsigned __int16 lightingHandle;
  char flags;
};

struct GfxWorldDpvsStatic
{
  unsigned int smodelCount;
  unsigned int staticSurfaceCount;
  unsigned int litSurfsBegin;
  unsigned int litSurfsEnd;
  unsigned int decalSurfsBegin;
  unsigned int decalSurfsEnd;
  unsigned int emissiveSurfsBegin;
  unsigned int emissiveSurfsEnd;
  char unknown_data[0x10];
  char *smodelVisData[3];
  char *surfaceVisData[3];
  unsigned __int16 *sortedSurfIndex;
  GfxStaticModelInst *smodelInsts;
  GfxSurface *surfaces;
  GfxCullGroup *cullGroups;
  GfxStaticModelDrawInst *smodelDrawInsts;
  GfxDrawSurf *surfaceMaterials;
  unsigned int *surfaceCastsSunShadow;
  volatile int usageCount;
};

struct GfxWorldDpvsDynamic
{
  unsigned int dynEntClientWordCount[2];
  unsigned int dynEntClientCount[2];
  unsigned int *dynEntCellBits[2];
  char *dynEntVisData[2][3];
};

struct GfxWorld
{
	const char * name;
	const char * baseName;
	char unknown_data1[0xC];
	unsigned int reflectionProbeCount;
	GfxReflectionProbe *reflectionProbes;
	char unknown_data2[0x18];
	GfxWorldDpvsPlanes dpvsPlanes;
	char * unknownData1;					//Total size = dpvsPlanes->cellCount << 2
	unknownGFXWorldStruct_t * unknownStruct1;	//count = dpvsPlanes->cellCount
	GfxCell *cells;
	unknownGFXWorldStruct2_t unknownStruct2;
	GfxLightGrid lightGrid;
	int modelCount;
	GfxBrushModel *models;
	float mins[3];
	float maxs[3];
	unsigned int checksum;
	int materialMemoryCount;
	MaterialMemory *materialMemory;
	sunflare_t sun;
	float outdoorLookupMatrix[4][4];
	GfxImage * outdoorImage;
	unsigned int *cellCasterBits;
	unsigned int *sceneEntCellBits;
	GfxSceneDynModel *sceneDynModel;
	GfxSceneDynBrush *sceneDynBrush;
	unsigned int *primaryLightEntityShadowVis;
	unsigned int *primaryLightDynEntShadowVis[2];
	char *primaryLightForModelDynEnt;
	GfxShadowGeometry *shadowGeom;
	GfxLightRegion *lightRegion;
	GfxWorldDpvsStatic dpvs;
	GfxWorldDpvsDynamic dpvsDyn;
	long unknownLong1;
	int unknownCount1;
	char * unknownData2;			//Total size = unknownCount1 * 0x38
	long unknownLong2;

	static int dumpGFXMap();
};

extern GfxWorld * g_GFXMap;
#pragma endregion

#pragma region lightdef (0x14)
#pragma pack(push, 4)
struct GfxLightImage
{
  GfxImage *image;
  char samplerState;
};
#pragma pack(pop)

struct GfxLightDef
{
  const char *name;
  GfxLightImage attenuation;
  int lmapLookupStart;

	int dumpLight();
	static void dumpAllLights();
};

extern XAssetPool<GfxLightDef, 0x20> * g_lightDefPool;
#pragma endregion

#pragma region font (0x16)
struct glyphs_s
{
	unsigned short character;
	unsigned char padLeft;
	unsigned char padTop;
	unsigned char padRight;
	unsigned char width;
	unsigned char height;
	unsigned char const0;
	float uvLeft;
	float uvTop;
	float uvRight;
	float uvBottom;
};

struct Font_s
{
	char * fontName;
	int pixelHeight;
	int glyphCount;
	Material * material;
	Material * glowMaterial;
	glyphs_s * glyphs;
};

extern XAssetPool<Font_s, 0x10> * g_fontPool;
#pragma endregion

#pragma region menufile (0x17)
typedef struct MenuList
{
	char * name;
	long menuCount;
	menuDef_t * * menus;
} MenuList;

extern XAssetPool<MenuList, 0x80> * g_menuListPool;
#pragma endregion

#pragma region menu (0x18)
struct statement_s;
struct itemDef_s;

typedef struct scrollInfo_s {
  float nextScrollTime;
  float nextAdjustTime;
  float adjustValue;
  int scrollKey;
  float xStart;
  float yStart;
  itemDef_s *item;
  int scrollDir;
} scrollInfo_t;

// used in field defs and sliders
typedef struct editFieldDef_s {
  float minVal;
  float maxVal;
  float defVal;
  float range;
  int maxChars;
  int maxPaintChars;
  int paintOffset;
  char _padding[4];  // used in memory, always changes to 0
} editFieldDef_t;

// used in newstickers
struct newsticker_t
{
  int textSource;
  int speed;
  int unknown3;  // might be something to do with borders
  char unknown2[0x10];
};

#define MAX_MULTI_DVARS 32

// used in multi defs
typedef struct multiDef_s {
  char *dvarList[MAX_MULTI_DVARS];
  char *dvarStr[MAX_MULTI_DVARS];
  float dvarValue[MAX_MULTI_DVARS];
  int count;
  int strDef;
} multiDef_t;

// used in listboxes
#define MAX_LB_COLUMNS 16

typedef struct columnInfo_s {
  int pos;
  int width;
  int maxChars;
  int align;
} columnInfo_t;

typedef struct listBoxDef_s {
  char unknown[0x14];
  int cursorPos;
  int unk;
  int startPos;
  int endPos;
  float elementWidth;
  float elementHeight;
  int elementStyle;
  int numColumns;
  columnInfo_t columnInfo[MAX_LB_COLUMNS];
  statement_s* doubleClick;
  int notselectable;
  int noscrollbars;
  int usepaging;
  vec4_t unknown2;
  Material *material;
} listBoxDef_t;

typedef struct {
  int type;
  //If type == 0
  //Then the ID is the value in the enum you just gave me
  //if type == 1
  //ID: 0 = int, 1 = float, 2 = string, 3 = expression
  int Id;
  int data;
} entry_t;

typedef struct expression_s {
  int entryCount;
  entry_t *entries;
  void *unknown;
  char unknown2[0xC];
} expression_s;

typedef struct statementExpression_t {
  expression_s *expression;
  statement_s *statement;
} statementExpression_t;

typedef struct dvarExpression_t {
  char *data;
  expression_s *expression;
} dvarExpression_t;

union entryInternalData
{
	char * string;
	statementExpression_t * StatementExpression;
	statement_s * newStatement;
	dvarExpression_t * dvarExpression;
};

typedef struct expressionEntry {
  entryInternalData data;
  int type;
   /*
     if type == 0x00000000 read string
     if type == 0x01000000 read statementexpression_t	// confirmed
     if type == 0x02000000 read statement_s
     if type == 0x03000000 read dvarexpression_t	// confirmed
     if type == 0x04000000 read dvarexpression_t	// confiremd
     if type == 0x05000000 read dvarexpression_t	// confirmed
     if type == 0x06000000 read dvarexpression_t	// confirmed
   */
} expressionEntry;

typedef struct statement_s {
  int numEntries;
  expressionEntry **entries;
} statement_s;

typedef struct rectDef_s {
  float x;
  float y;
  float w;
  float h;
  char horzAlign;
  char vertAlign;
  char _padding[2];
} rectDef_s;

struct windowDef_t 
{
	char *name;
	rectDef_s rect;
	rectDef_s rectClient;
	char *group;
	int style;
	int border;
	int ownerDraw;
	int ownerDrawFlags;
	float borderSize;
	int staticFlags;
  /*
  decoration: 0x00100000
horizontalScroll: 0x00300000
screenSpace: 0x00500000
autoWrapped: 0x00900000
popup: 0x01000000
outOfBoundsClick: 0x03000000
legacySplitScreenScale: 0x05000000
unknown: 0x09000000
hiddenDuringFlashbang: 0x10000000
hiddenDuringScope: 0x30000000
hiddenDuringUI: 0x50000000
textOnlyFocus: 0x90000000
	*/
  int dynamicFlags[4];
  int nextTime;
  float foreColor[4];
  float backColor[4];
  float borderColor[4];
  float outlineColor[4];
  float disableColor[4];
  Material *background;
};

typedef struct {
  int count;
  expression_s *expression;
} component_t;

typedef struct ItemKeyHandler {
  int key;
  statement_s *action;
  ItemKeyHandler *next;
} ItemKeyHandler;

struct itemDef_s {
  windowDef_t window;
  rectDef_s textRect[4];
  int type;
  int dataType;
  int alignment;
  int fontEnum;
  int textAlignMode;
  float textalignx;
  float textaligny;
  float textscale;
  int textStyle;
  int gameMsgWindowIndex;
  int gameMsgWindowMode;
  char *text;
  int textSavegameInfo;
  menuDef_t *parent;
  statement_s *mouseEnterText;
  statement_s *mouseExitText;
  statement_s *mouseEnter;
  statement_s *mouseExit;
  statement_s *action;
  statement_s *accept;
  statement_s *onFocus;
  statement_s *leaveFocus;
  char *dvar;
  char *dvarTest;
  ItemKeyHandler *execKey;
  char *enableDvar;
  int dvarFlags;
  snd_alias_list_t *focusSound;
  float special;
  int cursorPos[4];
  void* typeData; // data depends on type
    /*
     if type == 0x06
       char unknownBytes1[0x134];
       statement_s statement;
       char unknownBytes2[0x20];
     if type == 0x14
       char unknownBytes1[0x1C];
     if type == 0x15
       void *unknown;
     else
       char unknown[0x20]
    */
  int ImageTrack;
  int numExpressions;
  component_t *componentExpressions; // pointer to an array of component_t whos size is determined by numExpressions;
  expression_s *visibleExp;
  expression_s *disabledExp;
  expression_s *textExp;
  expression_s *materialExp;
  vec4_t glowColor;
  long unknown9;
  int fxBirthTime;
  int fxLetterTime;
  int fxDecayStartTime;
  long unknown10;
  int soundTimeKeeper;

  //My Private Functions
  void SetGlowColor(float * color);
  void SetForeColor(float * color);
  void SetBackColor(float * color);
  void SetBorderColor(float * color);
  void SetOutlineColor(float * color);
  void SetDisableColor(float * color);
  void SetXY(float x, float y);
  float SetY(float y);
  float SetX(float x);
  float SetW(float w);
};

typedef struct {
  void *dvar;
  char *dvarName;
} dvarStruct_t;

typedef struct {
  int expressionCount;
  expression_s **expressions;
  int dvarCount;
  dvarStruct_t **dvarPointers;
  int stringCount;	
  char **stringPointers;
} localItems_t;

typedef struct menuDef_t
{
	windowDef_t window;
	char * font;			//Completely unused
	int fullscreen;
	int itemCount;
	int fontIndex;
	int cursorItems[4];
	int fadeCycle;
	float fadeClamp;
	float fadeAmount;
	float fadeInAmount;
	float blurRadius;
	statement_s *onOpen;
	statement_s *onClose;
	statement_s *onEsc;
	statement_s *onRequestClose;
	ItemKeyHandler *execKeys;
	expression_s *visibleExp;
	char *allowedBinding;
	char *soundName;
	int imageTrack;
	float focusColor[4];
	expression_s *rectXExp;
	expression_s *rectYExp;
	expression_s *rectHExp;
	expression_s *rectWExp;
	itemDef_s **items;
	char unknownBytes[0x1C0];
	localItems_t * localItems;
} menuDef_t;

extern XAssetPool<menuDef_t, 0x264> * g_menuDefPool;
#pragma endregion

#pragma region localize (0x19)
typedef struct LocalizeEntry
{
	char * value;
	char * name;

	static void dumpAllLocalizeEntries();
} LocalizeEntry;

extern XAssetPool<LocalizeEntry, 0x1B58> * g_LocalizeEntryPool;
#pragma endregion

#pragma region weapon (0x1A)
enum weapFireType_t : int
{
	WEAPON_FIRETYPE_FULLAUTO = 0x0,
	WEAPON_FIRETYPE_SINGLESHOT = 0x1,
	WEAPON_FIRETYPE_BURSTFIRE2 = 0x2,
	WEAPON_FIRETYPE_BURSTFIRE3 = 0x3,
	WEAPON_FIRETYPE_BURSTFIRE4 = 0x4,
	WEAPON_FIRETYPE_DOUBLE_BARREL = 0x5,
	WEAPON_FIRETYPE_MAX
};

extern char ** weapFireType_s;

enum weapInventoryType_t : int
{
	WEAPINVENTORY_PRIMARY = 0,
	WEAPINVENTORY_OFFHAND = 1,
	WEAPINVENTORY_ITEM = 2,
	WEAPINVENTORY_ALTMODE = 3,
	WEAPINVENTORY_EXCLUSIVE = 4,
	WEAPINVENTORY_SCAVENGER = 5,
	WEAPINVENTORY_MAX
};

extern char ** weapInventoryType_s;

extern char * * PenetrateType_s;

enum activeReticleType_t : int
{
	VEH_ACTIVE_RETICLE_NONE = 0,
	VEH_ACTIVE_RETICLE_PIP_ON_A_STICK = 1,
	VEH_ACTIVE_RETICLE_BOUNCING_DIAMOND = 2,
	VEH_ACTIVE_RETICLE_MAX
};

extern char * activeReticleType_s[];

enum weapType_t : int
{
	WEAPTYPE_BULLET = 0,
	WEAPTYPE_GRENADE = 1,
	WEAPTYPE_PROJECTILE = 2,
	WEAPTYPE_RIOTSHIELD = 3, //Pre-mw2: WEAPTYPE_BINOCULARS
	WEAPTYPE_MAX
};

extern char * * weaponType_s;

enum weapClass_t : int
{
	WEAPCLASS_RIFLE = 0,
	WEAPCLASS_SNIPER = 1,
	WEAPCLASS_MG = 2,
	WEAPCLASS_SMG = 3,
	WEAPCLASS_SPREAD = 4,
	WEAPCLASS_PISTOL = 5,
	WEAPCLASS_GRENADE = 6,
	WEAPCLASS_ROCKETLAUNCHER = 7,
	WEAPCLASS_TURRET = 8,
	WEAPCLASS_THROWINGKNIFE = 9,
	WEAPCLASS_NON_PLAYER = 10,
	WEAPCLASS_ITEM = 11,
	WEAPCLASS_MAX
};

extern char * * weaponClass_s;

enum OffhandClass : int
{
	OFFHAND_CLASS_NONE = 0,
	OFFHAND_CLASS_FRAG_GRENADE = 1,
	OFFHAND_CLASS_SMOKE_GRENADE = 2,
	OFFHAND_CLASS_FLASH_GRENADE = 3,
	OFFHAND_CLASS_MAX
};

extern char * OffhandClass_s[];

//Note: This enum actually isn't defined in the xex, its defined in a .txt file. This makes it hard to label, so I just copied from there
//That also means the enum name isn't defined by iw, so i made it up
enum playerAnimType_t : int
{
	none = 0x0,
	other = 0x1,
	pistol = 0x2,
	smg = 0x3,
	autorifle = 0x4,
	mg = 0x5,
	sniper = 0x6,
	rocketlauncher = 0x7,
	explosive = 0x8,
	grenade = 0x9,
	turret = 0xA,
	c4 = 0xB, 
	m203 = 0xC, 
	hold = 0xD,
	briefcase = 0xE,
	riotshield = 0xF,
	laptop = 0x10,
	throwingknife = 0x11,
	PLAYER_ANIM_TYPE_MAX
};

extern char * playerAnimType_s[];

enum guidedMissileType_t
{
  MISSILE_GUIDANCE_NONE = 0x0,
  MISSILE_GUIDANCE_SIDEWINDER = 0x1,
  MISSILE_GUIDANCE_HELLFIRE = 0x2,
  MISSILE_GUIDANCE_JAVELIN = 0x3,
  MISSILE_GUIDANCE_MAX
};

extern char * guidedMissileType_s[];

enum weapProjExplosion_t
{
  WEAPPROJEXP_GRENADE = 0x0,
  WEAPPROJEXP_ROCKET = 0x1,
  WEAPPROJEXP_FLASHBANG = 0x2,
  WEAPPROJEXP_NONE = 0x3,
  WEAPPROJEXP_DUD = 0x4,
  WEAPPROJEXP_SMOKE = 0x5,
  WEAPPROJEXP_HEAVY = 0x6,
  WEAPPROJEXP_MAX
};

extern char * weapProjExplosion_s[];

enum WeapStickinessType
{
	WEAP_STICK_DONT = 0,
	WEAP_STICK_ALL,
	WEAP_STICK_ALL_ORIENT,
	WEAP_STICK_GROUND,
	WEAP_STICK_GROUND_YAW,
	WEAP_STICK_KNIFE,
	WEAP_STICK_MAX
};

extern char * WeapStickinessType_s[];

struct weaponDef_s
{
	char * name;						//Exact same as name in the weaponHeader_s. weaponDef_s is what is loaded into the pool, so this is used for finding them...
	XModel * * gunModel;
	XModel * handModel;
	char * * animR;						//0x25 count
	char * * animL;
	char * szModeName;
	ScriptString *noteTrackSoundMap[2];			//0x10
	ScriptString *noteTrackRumbleMap[2];		//0x10
	playerAnimType_t playerAnimType;
	weapType_t weaponType;
	weapClass_t weaponClass;
	PenetrateType penetrateType;
	weapInventoryType_t inventoryType;
	weapFireType_t fireType;
	OffhandClass offhandClass;
	weapStance_t stance;
	FxEffectDef * viewFlashEffect;
	FxEffectDef * worldFlashEffect;
	snd_alias_list_t * pickUpSound;
	snd_alias_list_t * pickUpSoundPlayer;
	snd_alias_list_t * ammoPickupSound;
	snd_alias_list_t * ammoPickupSoundPlayer;
	snd_alias_list_t * projectileSound;
	snd_alias_list_t * pullbackSound;
	snd_alias_list_t * pullbackSoundPlayer;
	snd_alias_list_t * fireSound;
	snd_alias_list_t * fireSoundPlayer;
	snd_alias_list_t * fireSoundPlayerAkimbo;
	snd_alias_list_t * loopFireSound;
	snd_alias_list_t * loopFireSoundPlayer;
	snd_alias_list_t * stopFireSound;
	snd_alias_list_t * stopFireSoundPlayer;
	snd_alias_list_t * lastShotSound;
	snd_alias_list_t * lastShotSoundPlayer;
	snd_alias_list_t * emptyFireSound;
	snd_alias_list_t * emptyFireSoundPlayer;
	snd_alias_list_t * meleeSwipeSound;
	snd_alias_list_t * meleeSwipeSoundPlayer;
	snd_alias_list_t * meleeHitSound;
	snd_alias_list_t * meleeMissSound;
	snd_alias_list_t * rechamberSound;
	snd_alias_list_t * rechamberSoundPlayer;
	snd_alias_list_t * reloadSound;
	snd_alias_list_t * reloadSoundPlayer;
	snd_alias_list_t * reloadEmptySound;
	snd_alias_list_t * reloadEmptySoundPlayer;
	snd_alias_list_t * reloadStartSound;
	snd_alias_list_t * reloadStartSoundPlayer;
	snd_alias_list_t * reloadEndSound;
	snd_alias_list_t * reloadEndSoundPlayer;
	snd_alias_list_t * detonateSound;
	snd_alias_list_t * detonateSoundPlayer;
	snd_alias_list_t * nightVisionWearSound;
	snd_alias_list_t * nightVisionWearSoundPlayer;
	snd_alias_list_t * nightVisionRemoveSound;
	snd_alias_list_t * nightVisionRemoveSoundPlayer;
	snd_alias_list_t * altSwitchSound;
	snd_alias_list_t * altSwitchSoundPlayer;
	snd_alias_list_t * raiseSound;
	snd_alias_list_t * raiseSoundPlayer;
	snd_alias_list_t * firstRaiseSound;
	snd_alias_list_t * firstRaiseSoundPlayer;
	snd_alias_list_t * putawaySound;
	snd_alias_list_t * putawaySoundPlayer;
	snd_alias_list_t * scanSound;
	snd_alias_list_t * * bounceSound;				//0x1F array of char **
	FxEffectDef * viewShellEjectEffect;
	FxEffectDef * worldShellEjectEffect;
	FxEffectDef * viewLastShotEjectEffect;
	FxEffectDef * worldLastShotEjectEffect;
	Material * reticleCenter;
	Material * reticleSide;
	int iReticleCenterSize;
	int iReticleSideSize;
	int iReticleMinOfs;
	activeReticleType_t activeReticleType;
	float vStandMove[3];
	float vStandRot[3];
	float vStrafeMove[3];
	float vStrafeRot[3];
	float vDuckedOfs[3];
	float vDuckedMove[3];
	float vDuckedRot[3];
	float vProneOfs[3];
	float vProneMove[3];
	float vProneRot[3];
	float fPosMoveRate;
	float fPosProneMoveRate;
	float fStandMoveMinSpeed;
	float fDuckedMoveMinSpeed;
	float fProneMoveMinSpeed;
	float fPosRotRate;
	float fPosProneRotRate;
	float fStandRotMinSpeed;
	float fDuckedRotMinSpeed;
	float fProneRotMinSpeed;
	XModel * * worldModel;
	XModel * worldClipModel;
	XModel * rocketModel;
	XModel * knifeModel;
	XModel * worldKnifeModel;
	Material * hudIcon;
	weaponIconRatioType_t hudIconRatio;
	Material * pickupIcon;
	weaponIconRatioType_t pickupIconRatio;
	Material * ammoCounterIcon;
	weaponIconRatioType_t ammoCounterIconRatio;
	ammoCounterClipType_t ammoCounterClip;
	int iStartAmmo;
	const char *szAmmoName;
	int iAmmoIndex;
	const char *szClipName;
	int iClipIndex;
	int iMaxAmmo;
	int shotCount;
	const char *szSharedAmmoCapName;
	int iSharedAmmoCapIndex;
	int iSharedAmmoCap;
	int damage;
	int playerDamage;
	int iMeleeDamage;
	int iDamageType;
	int iFireDelay;
	int iMeleeDelay;
	int meleeChargeDelay;
	int iDetonateDelay;
	int iRechamberTime;
	int iRechamberOneHanded;
	int iRechamberBoltTime;
	int iHoldFireTime;
	int iDetonateTime;
	int iMeleeTime;
	int meleeChargeTime;
	int iReloadTime;
	int reloadShowRocketTime;
	int iReloadEmptyTime;
	int iReloadAddTime;
	int iReloadStartTime;
	int iReloadStartAddTime;
	int iReloadEndTime;
	int iDropTime;
	int iRaiseTime;
	int iAltDropTime;
	int quickDropTime;
	int quickRaiseTime;
	int iFirstRaiseTime;
	int iEmptyRaiseTime;
	int iEmptyDropTime;
	int sprintInTime;
	int sprintLoopTime;
	int sprintOutTime;
	int stunnedTimeBegin;
	int stunnedTimeLoop;
	int stunnedTimeEnd;
	int nightVisionWearTime;
	int nightVisionWearTimeFadeOutEnd;
	int nightVisionWearTimePowerUp;
	int nightVisionRemoveTime;
	int nightVisionRemoveTimePowerDown;
	int nightVisionRemoveTimeFadeInStart;
	int fuseTime;
	int aifuseTime;
	float autoAimRange;
	float aimAssistRange;
	float aimAssistRangeAds;
	float aimPadding;
	float enemyCrosshairRange;
	float moveSpeedScale;
	float adsMoveSpeedScale;
	float sprintDurationScale;
	float adsZoomInFrac;
	float adsZoomOutFrac;
	Material * AdsOverlayShader;
	Material * AdsOverlayShaderLowRes;
	Material * AdsOverlayShaderEMP;
	Material * AdsOverlayShaderEMPLowRes;
	weapOverlayReticle_t adsOverlayReticle;
	weapOverlayInterface_t adsOverlayInterface;
	float adsOverlayWidth;
	float adsOverlayHeight;
	float adsOverlayWidthSplitscreen;
	float adsOverlayHeightSplitscreen;
	float fAdsBobFactor;
	float fAdsViewBobMult;
	float fHipSpreadStandMin;
	float fHipSpreadDuckedMin;
	float fHipSpreadProneMin;
	float hipSpreadStandMax;
	float hipSpreadDuckedMax;
	float hipSpreadProneMax;
	float fHipSpreadDecayRate;
	float fHipSpreadFireAdd;
	float fHipSpreadTurnAdd;
	float fHipSpreadMoveAdd;
	float fHipSpreadDuckedDecay;
	float fHipSpreadProneDecay;
	float fHipReticleSidePos;
	//int iAdsTransInTime;
	//int iAdsTransOutTime;
	float fAdsIdleAmount;
	float fHipIdleAmount;
	float adsIdleSpeed;
	float hipIdleSpeed;
	float fIdleCrouchFactor;
	float fIdleProneFactor;
	float fGunMaxPitch;
	float fGunMaxYaw;
	float swayMaxAngle;
	float swayLerpSpeed;
	float swayPitchScale;
	float swayYawScale;
	float swayHorizScale;
	float swayVertScale;
	float swayShellShockScale;
	float adsSwayMaxAngle;
	float adsSwayLerpSpeed;
	float adsSwayPitchScale;
	float adsSwayYawScale;
	float adsSwayHorizScale;
	float adsSwayVertScale;
	float adsViewErrorMin;
	float adsViewErrorMax;
	PhysCollMap * collisions;
	float dualWieldViewModelOffset;
	weaponIconRatioType_t killIconRatio;
	int iReloadAmmoAdd;
	int iReloadStartAdd;
	int iDropAmmoMin;
	int ammoDropClipPercentMin;
	int ammoDropClipPercentMax;
	int iExplosionRadius;
	int iExplosionRadiusMin;
	int iExplosionInnerDamage;
	int iExplosionOuterDamage;
	float damageConeAngle;
	float bulletExplDmgMult;
	float bulletExplRadiusMult;
	int iProjectileSpeed;
	int iProjectileSpeedUp;
	int iProjectileSpeedForward;
	int iProjectileActivateDist;
	float projLifetime;
	float timeToAccelerate;
	float projectileCurvature;
	XModel * projectileModel;
	weapProjExplosion_t projExplosiveType;
	FxEffectDef * projExplosionEffect;
	FxEffectDef * projDudEffect;
	snd_alias_list_t *projExplosionSound;
	snd_alias_list_t *projDudSound;
	WeapStickinessType stickiness;
	float lowAmmoWarningThreshold;
	float ricochetChance;
	float * parallelBounce;				//Pointer to 31 float array
	float * perpendicularBounce;			//Pointer to 31 float array
	FxEffectDef * projTrailEffect;
	FxEffectDef * projBeaconEffect;
	float vProjectileColor[3];
	guidedMissileType_t guidedMissileType;
	float maxSteeringAccel;
	float projIgnitionDelay;
	FxEffectDef * projIgnitionEffect;
	snd_alias_list_t *projIgnitionSound;			//EHHHH, wrong.
	float fAdsAimPitch;
	float fAdsCrosshairInFrac;
	float fAdsCrosshairOutFrac;
	int adsGunKickReducedKickBullets;
	float adsGunKickReducedKickPercent;
	float fAdsGunKickPitchMin;
	float fAdsGunKickPitchMax;
	float fAdsGunKickYawMin;
	float fAdsGunKickYawMax;
	float fAdsGunKickAccel;
	float fAdsGunKickSpeedMax;
	float fAdsGunKickSpeedDecay;
	float fAdsGunKickStaticDecay;
	float fAdsViewKickPitchMin;
	float fAdsViewKickPitchMax;
	float fAdsViewKickYawMin;
	float fAdsViewKickYawMax;
	float fAdsViewScatterMin;
	float fAdsViewScatterMax;
	float fAdsSpread;
	int hipGunKickReducedKickBullets;
	float hipGunKickReducedKickPercent;
	float fHipGunKickPitchMin;
	float fHipGunKickPitchMax;
	float fHipGunKickYawMin;
	float fHipGunKickYawMax;
	float fHipGunKickAccel;
	float fHipGunKickSpeedMax;
	float fHipGunKickSpeedDecay;
	float fHipGunKickStaticDecay;
	float fHipViewKickPitchMin;
	float fHipViewKickPitchMax;
	float fHipViewKickYawMin;
	float fHipViewKickYawMax;
	float fHipViewScatterMin;
	float fHipViewScatterMax;
	float fightDist;
	float maxDist;
	const char *accuracyGraphName[2];
	vec2_t * accuracyGraphKnots;						//Count = parent weaponHeader_s::unknownCount1
	vec2_t * originalAccuracyGraphKnots;						//Count = parent weaponHeader_s::unknownCount2
	short accuracyGraphKnotCount;
	short originalAccuracyGraphKnotCount;
	int iPositionReloadTransTime;
	float leftArc;
	float rightArc;
	float topArc;
	float bottomArc;
	float accuracy;
	float aiSpread;
	float playerSpread;
	float minVertTurnSpeed;
	float minHorTurnSpeed;
	float maxVertTurnSpeed;
	float maxHorTurnSpeed;
	float pitchConvergenceTime;
	float yawConvergenceTime;
	float suppressTime;
	float maxRange;
	float fAnimHorRotateInc;
	float fPlayerPositionDist;
	const char *szUseHintString;
	const char *dropHintString;
	int iUseHintStringIndex;
	int dropHintStringIndex;
	float horizViewJitter;
	float vertViewJitter;
	float scanSpeed;
	float scanAccel;
	int scanPauseTime;
	const char *szScript;
	float fOOPosAnimLength[2];
	int minDamage;
	int minPlayerDamage;
	float maxDamageRange;
	float minDamageRange;
	float destabilizationRateTime;
	float destabilizationCurvatureMax;
	int destabilizeDistance;
	float * locationDamageMultipliers;	//19
	const char *fireRumble;
	const char *meleeImpactRumble;
	Tracer * tracer;
	int turretScopeZoomRate;
	int turretScopeZoomMin;
	int turretScopeZoomMax;
	int turretOverheatUpRate;
	int turretOverheatDownRate;
	int turretOverheatPenalty;
	snd_alias_list_t *turretOverheatSound;	 // sound
	FxEffectDef *turretOverheatEffect;
	char *turretBarrelSpinRumble;	 // string
	int turretBarrelSpinUpTime;
	int turretBarrelSpinDownTime;
	int turretBarrelSpinSpeed;
	snd_alias_list_t *turretBarrelSpinMaxSnd;
	snd_alias_list_t *turretBarrelSpinUpSnds[4];	// should be 4 sound array here
	snd_alias_list_t *turretBarrelSpinDownSnds[4];
	snd_alias_list_t *missileConeSoundAlias;
	snd_alias_list_t *missileConeSoundAliasAtBase;
	float missileConeSoundRadiusAtTop;
	float missileConeSoundRadiusAtBase;
	float missileConeSoundHeight;
	float missileConeSoundOriginOffset;
	float missileConeSoundVolumescaleAtCore;
	float missileConeSoundVolumescaleAtEdge;
	float missileConeSoundVolumescaleCoreSize;
	float missileConeSoundPitchAtTop;
	float missileConeSoundPitchAtBottom;
	float missileConeSoundPitchTopSize;
	float missileConeSoundPitchBottomSize;
	float missileConeSoundCrossfadeTopSize;
	float missileConeSoundCrossfadeBottomSize;
	bool shareAmmo;
	bool lockonSupported;
	bool requireLockonToFire;
	bool bigExplosion;
	bool noAdsWhenMagEmpty;
	bool avoidDropCleanup;
	bool inheritsPerks;
	bool crosshairColorChange;
	bool rifleBullet;
	bool armorPiercing;
	bool boltAction;
	bool aimDownSight;
	bool rechamberWhileAds;
	bool bBulletExplosiveDamage;
	bool cookOffHold;
	bool clipOnly;
	bool noAmmoPickup;
	bool adsFire;
	bool cancelAutoHolsterWhenEmpty;
	bool disableSwitchToWhenEmpty;
	bool suppressAmmoReserveDisplay;
	bool laserSightDuringNightvision;
	bool markableViewmodel;
	bool noDualWield;
	bool flipKillIcon;
	bool noPartialReload;
	bool segmentedReload;
	bool blocksProne;
	bool silenced;
	bool isRollingGrenade;
	bool projExplosionEffectForceNormalUp;
	bool projImpactExplode;
	bool stickToPlayers;
	bool hasDetonator;
	bool disableFiring;
	bool timedDetonation;
	bool rotate;
	bool holdButtonToThrow;
	bool freezeMovementWhenFiring;
	bool thermalScope;
	bool altModeSameWeapon;
	bool turretBarrelSpinEnabled;
	bool missileConeSoundEnabled;
	bool missileConeSoundPitchshiftEnabled;
	bool missileConeSoundCrossfadeEnabled;
	bool offhandHoldIsCancelable;
	__int16 unused;
};

typedef enum ImpactType
{
	IMPACT_TYPE_NONE = 0,
	IMPACT_TYPE_BULLET_SMALL = 1,
	IMPACT_TYPE_BULLET_LARGE = 2,
	IMPACT_TYPE_BULLET_AP = 3,
	IMPACT_TYPE_SHOTGUN_FMJ = 4,
	IMPACT_TYPE_SHOTGUN = 5,
	IMPACT_TYPE_GRENADE_BOUNCE = 7,
	IMPACT_TYPE_GRENADE_EXPLODE = 8,
	IMPACT_TYPE_ROCKET_EXPLODE = 9,
	IMPACT_TYPE_PROJECTILE_DUD = 10,
	IMPACT_TYPE_MAX
} ImpactType;

//Check these
extern char * ImpactType_s[];

struct weaponVarientDef_s
{
	char * szInternalName;
	weaponDef_s * WeaponDef;
	char * szDisplayName;
	ScriptString * hideTags;		//Count = 0x20
	const char * *szXAnims;						//Pointer to array 37 in size
	float adsZoomFov;
	int adsTransInTime;
	int adsTransOutTime;
	int clipSize;
	ImpactType impactType;
	int fireTime;
	weaponIconRatioType_t dpadIconRatio;
	float penetrateMultiplier;
	float adsViewKickCenterSpeed;
	float hipViewKickCenterSpeed;
	char *szAltWeaponName;
	int altWeaponIndex;
	int altRaiseTime;
	Material * killIcon;				//Embedded material asset
	Material * dpadIcon;				//Embedded material asset
	int unknown8;
	int firstRaiseTime;
	int dropAmmoMax;
	float adsDofStart;
	float adsDofEnd;
	short accuracyGraphKnotCount;
	short originalAccuracyGraphKnotCount;
	vec2_t *accuracyGraphKnots;
	vec2_t *originalAccuracyGraphKnots;						//Count = unknownCount2
	bool motionTracker;
	bool enhanced;
	bool dpadIconShowsAmmo;
	char unused[0x611];

	int dumpWeaponVarient();
	static void dumpAllWeapons();
};

typedef weaponDef_s * (__cdecl * BG_GetWeaponDef_t)(unsigned int weaponIndex);
extern BG_GetWeaponDef_t BG_GetWeaponDef;

typedef weaponVarientDef_s * (__cdecl * BG_GetWeaponVarientDef_t)(unsigned int weaponIndex);
extern BG_GetWeaponVarientDef_t BG_GetWeaponVarientDef;

typedef unsigned int (__cdecl * G_GetWeaponIndexForName_t)(const char * name);
extern G_GetWeaponIndexForName_t G_GetWeaponIndexForName;

typedef int (__cdecl * BG_ClipForWeapon_t)(int weapon);									//Just returns iClipIndex
extern BG_ClipForWeapon_t BG_ClipForWeapon;

typedef void (__cdecl * G_InitializeAmmo_t)(void * pSelf, int weaponIndex, char weaponModel, int hadWeapon);
extern G_InitializeAmmo_t G_InitializeAmmo;

extern XAssetPool<weaponVarientDef_s, 0x578> * g_weaponPool;
#pragma endregion

#pragma region snddriverglobals (0x1B)
struct XAUDIOREVERBSETTINGS
{
  unsigned int ReflectionsDelay;
  char ReverbDelay;
  char RearDelay;
  char PositionLeft;
  char PositionRight;
  char PositionMatrixLeft;
  char PositionMatrixRight;
  char EarlyDiffusion;
  char LateDiffusion;
  char LowEQGain;
  char LowEQCutoff;
  char HighEQGain;
  char HighEQCutoff;
  float RoomFilterFreq;
  float RoomFilterMain;
  float RoomFilterHF;
  float ReflectionsGain;
  float ReverbGain;
  float DecayTime;
  float Density;
  float RoomSize;
};

struct XaReverbSettings
{
  int presetOverridden;
  XAUDIOREVERBSETTINGS reverbSettings;
};

struct SndDriverGlobals
{
	XaReverbSettings * reverbSettings;			//count = 0x1A
	char * name;
};

extern XAssetPool<SndDriverGlobals, 1> * g_sndDriverGlobalsPool;
#pragma endregion

#pragma region fx (0x1C)
struct FxElemVec3Range
{
  float base[3];
  float amplitude[3];
};

struct FxElemVelStateInFrame
{
  FxElemVec3Range velocity;
  FxElemVec3Range totalDelta;
};

const struct FxElemVelStateSample
{
  FxElemVelStateInFrame local;
  FxElemVelStateInFrame world;
};

struct FxElemVisualState
{
  char color[4];
  float rotationDelta;
  float rotationTotal;
  float size[2];
  float scale;
};

/* 6884 */
const struct FxElemVisStateSample
{
  FxElemVisualState base;
  FxElemVisualState amplitude;
};

union FxEffectDefRef
{
  FxEffectDef *handle;
  const char *name;
};

union FxElemVisuals
{
	XModel *model;
	const char *soundName;
	Material * material;
	FxEffectDefRef effectDef;
	const void *anonymous;
};

struct FxElemMarkVisuals
{
	Material *materials[2];
};

union FxElemDefVisuals
{
	FxElemVisuals instance;					//If parent FxElemDef::type == 0x7, use xmodelPtr
											//If parent FxElemDef::type == 0xC, use xstring, and derive a FxEffectDef from it
											//If parent FxElemDef::type == 0xA, use xstring
											//If parent FxElemDef::type != 0x9 && 0x8, use materialHandle

	FxElemVisuals * array;			//Total count = parent FxElemDef::unknownCount2
	FxElemMarkVisuals *	markArray;		//Total count = parent FxElemDef::unknownCount2
};

struct FxTrailVertex
{
  float pos[2];
  float normal[2];
  float texCoord;
};

struct FxTrailDef
{
	char unknownData1[0x14];
	int vertCount;
	FxTrailVertex *verts;			//Total size = ((FxTrailVertexCount << 2) + FxTrailVertexCount) << 2
	int indCount;
	unsigned __int16 *inds;				//count = uShortCount (size = uShortCount << 1)
};

union unknownFXUnion
{
	char * unknownBytes;
	FxTrailDef * TrailDef;
};

struct FxFloatRange
{
	float base;
	float amplitude;
};

struct FxSpawnDefLooping
{
	int intervalMsec;
	int count;
};

struct FxIntRange
{
	int base;
	int amplitude;
};

struct FxSpawnDefOneShot
{
	FxIntRange count;
};

union FxSpawnDef
{
	FxSpawnDefLooping looping;
	FxSpawnDefOneShot oneShot;
};

struct FxElemAtlas
{
  char behavior;
  char index;
  char fps;
  char loopCount; //this one may be diff
};

const struct FxElemDef
{
	int flags;
	FxSpawnDef spawn;
	FxFloatRange spawnRange;
	FxFloatRange fadeInRange;
	FxFloatRange fadeOutRange;
	float spawnFrustumCullRadius;
	FxIntRange spawnDelayMsec;
	FxIntRange lifeSpanMsec;
	FxFloatRange spawnOrigin[3];
	FxFloatRange spawnOffsetRadius;
	FxFloatRange spawnOffsetHeight;
	FxFloatRange spawnAngles[3];
	FxFloatRange angularVelocity[3];
	FxFloatRange initialRotation; // not sure how this works out
	FxFloatRange gravity; // value * 100
	FxFloatRange reflectionFactor;
	FxElemAtlas atlas;
	char elemType;
	char visualCount;
	char velIntervalCount;
	char visStateIntervalCount;
	FxElemVelStateSample * velSamples;	//Count = velIntervalCount + 1 //Data Length = (((velIntervalCount + 1) << 1) + (velIntervalCount + 1)) << 5
	FxElemVisStateSample * visSamples;	//Count = visStateIntervalCount + 1 //Data Length = (((visStateIntervalCount + 1) << 1) + (visStateIntervalCount + 1)) << 4
	FxElemDefVisuals visuals;		
									//If type is 0xB, then use markVisuals
									//If type is not 0xB and visualCount == 1, then use visual
									//If type is not 0xB and visualCount != 1, then use visualsArray
	float collMins[3];
	float collMaxs[3];
	FxEffectDefRef * fxOnImpact;			//derive a FxEffectDef from this
	FxEffectDefRef * fxOnDeath;			//derive a FxEffectDef from this
	FxEffectDefRef * emission;			//derive a FxEffectDef from this
	FxFloatRange emitDist;
	FxFloatRange emitDistVariance;
	unknownFXUnion * trailDef;
									//If type != 3, then use unknownBytes
										//If type != 3 && type != 6, unknownBytes size == 1
										//If type != 3 && type == 6, unknownBytes size == 0x34
									//If type == 3, then use TrailDef
	char sortOrder;
	char lightingFrac;
	char unused[2];
};

struct FxEffectDef
{
	char * name;
	int flags;
	int totalSize;
	int msecLoopingLife;
	int elemDefCountLooping;
	int elemDefCountOneShot;
	int elemDefCountEmission;
	FxElemDef * elemDefs;			//Count = (elemDefCountOneShot + elemDefCountEmission) + elemDefCountLooping
};

extern XAssetPool<FxEffectDef, 0x258> * g_FXPool;
#pragma endregion

#pragma region impactfx (0x1D)
/* All good! */

struct FxImpactEntry
{
	FxEffectDef * nonflesh[0x1F];
	FxEffectDef * flesh[0x4];
};

struct FxImpactTable
{
	char * name;
	FxImpactEntry * table;					//Count = 0xF
};

extern XAssetPool<FxImpactTable, 4> * g_impactFxPool;
#pragma endregion

#pragma region rawfile (0x22)
/* Was done before I even started ;) */

typedef struct Rawfile
{
	char * name;
	int compressedSize;
	int decompressedSize;
	char * data;
} Rawfile;

extern XAssetPool<Rawfile, 0x400> * g_rawfilePool;
#pragma endregion

#pragma region stringtable (0x23)
/* Was done before I even started ;) */

struct StringTableEntry
{
	char * value;
	int hash;

	void UpdateHash();
};

struct StringTable
{
	char * name;
	int columnCount;
	int rowCount;
	StringTableEntry * values;

	int dumpStringTable();
	static void DumpAllStringTables();
};

//To get a certain column/row, do this...
//desiredEntry = columnCount * desiredRow + desiredColumn;
extern XAssetPool<StringTable, 0x190> * g_stringTablePool;
#pragma endregion

#pragma region leaderboarddef (0x24)
/* Structures good... */

struct LeaderboardDefEntry
{
	char * stringPointer1;
	char unknownData1[0xC];
	char * stringPointer2;
    char unknownData2[0xC];
};

struct LeaderboardDef
{
	char * name;
	long unknown1;
	int entryCount;
	long unknown2;
	long unknown3;
	LeaderboardDefEntry * entries;		//count = entryCount
};

extern XAssetPool<LeaderboardDef, 0x64> * g_leaderboardDefPool;
#pragma endregion

#pragma region structureddatadef (0x25)
enum structuredDataType_t
{
	STRUCTURED_DATA_INT = 0,
	STRUCTURED_DATA_BYTE = 1,
	STRUCTURED_DATA_BOOL = 2,
	STRUCTURED_DATA_STRING = 3,
	STRUCTURED_DATA_ENUM = 4,
	STRUCTURED_DATA_STRUCT = 5,
	STRUCTURED_DATA_INDEXEDARR = 6,
	STRUCTURED_DATA_ENUMARR = 7,
	STRUCTURED_DATA_FLOAT = 8,
	STRUCTURED_DATA_SHORT = 9
};

struct structuredDataItem_t
{
	structuredDataType_t type;
	union
	{
		int index;
	};
	int offset;
};

struct enumEntry
{
	char * name;
	short index;
	short _pad;
};

struct definedEnum
{
	int entryCount;
	long unknown4;
	enumEntry * entries;		//Array - Count = unknownCount5;
};

struct structureEntry
{
	char * name;
	structuredDataItem_t item;
};

struct definedStructure
{
	int entryCount;
	structureEntry * entries;
	int size;
	long unknown7;
};

struct structuredDataIndexedArray_t
{
	int numItems;
	structuredDataItem_t item;
};

struct structuredDataEnumArray_t
{
	int enumIndex;
	structuredDataItem_t item;
};

struct StructuredData
{
	int version;
	unsigned int hash;
	int enumCount;
	definedEnum * enums;	//Array - Count = unknownCount1;
	int structureCount;
	definedStructure * structures;	//Array - Count = unknownCount2;
	int indexedArrayCount;
	structuredDataIndexedArray_t * indexedArrays;					//Array of data - Total size = unknownCount3 << 4
	int enumArrayCount;
	structuredDataEnumArray_t * enumArrays;					//Array of data - Total size = unknownCount4 << 4
	structuredDataItem_t rootItem;
};

struct StructuredDataDef
{
	char * name;
	int structureCount;
	StructuredData * internalStructure; //Array-Count = unknownCount1;

	int DumpSDD();
	static void DumpAllSDDs();
};

extern XAssetPool<StructuredDataDef, 0x18> * g_structuredDataDefPool;
#pragma endregion

#pragma region tracer (0x26)
struct Tracer
{
	char * name;
	Material * material;
	int drawInterval;
    float speed;
    float beamLength;
    float beamWidth;
    float screwRadius;
    float screwDist;
	float colors[5][4];

	int dumpTracer();
	static void dumpAllTracers();
};

extern XAssetPool<Tracer, 0x20> * g_tracerPool;
#pragma endregion

#pragma region vehicle (0x27)
enum VehicleAxles {
        VEH_FRONT = 0x0,
        VEH_REAR = 0x1,
        VEH_ALL = 0x2,
		VEH_MAX
};

extern char * VehicleAxles_s[];

struct VehiclePhysics { // sizeof = 0xB4
        int physicsEnabled;
        const char *unknown1;// wasn't there
        PhysPreset *physicsPreset;
        const char *accelerationGraph;
        VehicleAxles steeringAxle;
        VehicleAxles powerAxle;
        VehicleAxles brakingAxle;
        float unknown2;         // wasn't there
        float reverseSpeed;
        float maxVelocity;
        float maxPitch;
        float maxRoll;
        float maxYaw;           // wasn't there, guessed name
        float suspensionTravelRear;
        float suspensionStrengthFront;
        float suspensionDampingFront;
        float suspensionStrengthRear;
        float suspensionDampingRear;
        float frictionBraking;
        float frictionCoasting;
        float frictionTopSpeed;
        float frictionSide;
        float frictionSideRear;
        float velocityDependentSlip;
        float rollStability;
        float rollResistance;
        float pitchResistance;
        float yawResistance;

        float uprightStrengthPitch;
        float uprightStrengthRoll;
        float targetAirPitch;
        float airYawTorque;
        float airPitchTorque;
        float minimumMomentumForCollision;
        float collisionLaunchForceScale;
        float wreckedMassScale;
        float wreckedBodyFriction;
        float minimumJoltForNotify;
        float slipThreshholdFront;
        float slipThreshholdRear;
        float slipFricScaleFront;
        float slipFricScaleRear;
        float slipFricRateFront;
        float slipFricRateRear;
        float slipYawTorque;
};

enum VehicleTypes : int
{
  VEH_WHEELS_4 = 0x0,
  VEH_TANK = 0x1,
  VEH_PLANE = 0x2,
  VEH_BOAT = 0x3,
  VEH_ARTILLERY = 0x4,
  VEH_HELICOPTER = 0x5,
  VEH_SNOWMOBILE = 0x6,
  VEH_TYPE_MAX
};

extern char * * VehicleTypes_s;

struct Vehicle //Total size should be 0x2D0... Never actually found one, so all this is untested
{
	char * name;
	VehicleTypes type;
    const char *useHintString;
    int health;
    int quadBarrel;
	float texureScrollScale;
	float maxSpeed;
	float accel;
	float rotRate;
	float rotAccel;
        float maxBodyPitch;
        float maxBodyRoll;
        float fakeBodyAccelPitch;
        float fakeBodyAccelRoll;
        float fakeBodyVelPitch;
        float fakeBodyVelRoll;
        float fakeBodySideVelPitch;
        float fakeBodySidePitchStrength;
        float fakeBodyRollStrength;
        float fakeBodyPitchDampening;
        float fakeBodyRollDampening;
        float fakeBodyBoatRockingAmplitude;
        float fakeBodyBoatRockingPeriod;
        float fakeBodyBoatRockingRotationPeriod;
        float fakeBodyBoatRockingFadeoutSpeed;
        float boatBouncingMinForce;
        float boatBouncingMaxForce;
        float boatBouncingRate;
        float boatBouncingFadeinSpeed;
        float boatBouncingFadeoutSteeringAngle;
        float collisionDamage;
        float collisionSpeed;
        float killcamZDist;
        float killcamBackDist;
        float killcamUpDist;
        int playerProtected;
        int bulletDamage;
        int armorPiercingDamage;
        int grenadeDamage;
        int projectileDamage;
        int projectileSplashDamage;
        int heavyExplosiveDamage;
        VehiclePhysics vehiclePhysics;
        float boostDuration;
        float boostRechargeTime;
        float boostAcceleration;
        float supensionTravel;
        float maxSteeringAngle;
        float steeringLerp;
        float minSteeringScale;
        float minSteeringSpeed;
        int camLookEnabled;
        float camLerp;
        float camPitchInfluence;
        float camRollInfluence;
        float camFovIncrease;
        float camFovOffset;
        float camFovSpeed;
	char * unknownString2;
	        weaponVarientDef_s *turretWeapon;
        float turretHorizSpanLeft;
        float turretHorizSpanRight;
        float turretVertSpanUp;
        float turretVertSpanDown;
        float turretRotRate;
        snd_alias_list_t *turretSpinSnd;
        snd_alias_list_t *turretStopSnd;
        int trophyEnabled;
        float trophyRadius;
        float trophyInactiveRadius;
        int trophyAmmoCount;
        float trophyReloadTime;
        ScriptString *trophyTags;
        int unknown5;
        Material *compassFriendlyIcon;
        Material *compassEnemyIcon;
        int compassIconWidth;
        int compassIconHeight;
        snd_alias_list_t *lowIdleSound;
        snd_alias_list_t *highIdleSound;
        snd_alias_list_t *lowEngineSound;
        snd_alias_list_t *highEngineSound;			//It looks like this is stored in the ff as a char*, with the name of a sound asset, which is then found and stored in mem
        float engineSndSpeed;
        snd_alias_list_t *engineStartUpSnd;
        int engineStartUpLength;
        snd_alias_list_t *engineShutdownSnd;
        snd_alias_list_t *engineIdleSnd;
        snd_alias_list_t *engineSustainSnd;
        snd_alias_list_t *engineRampUpSnd;
        int engineRampUpLength;
        snd_alias_list_t *engineRampDownSnd;
        int engineRampDownLength;
        snd_alias_list_t *suspensionSoftSnd;
        float suspensionSoftCompression;
        snd_alias_list_t *suspensionHardSnd;
        float suspensionHardConpression;
        snd_alias_list_t *collisionSnd;
        float collisionBlendSpeed;
        snd_alias_list_t *speedSnd;
        float speedSndBlendSpeed;
        char *surfaceSndPrefix;
	snd_alias_list_t * surfaceSounds[0x1F];	//It looks like this is stored in the ff as a char*[0x1F], with each as a
        float surfaceSndBlendSpeed;
        float slideVolume;
        float slideBlendSpeed;
        float inAirPitch;

	int dumpVehicle();
	static void dumpAllVehicles();
};

extern XAssetPool<Vehicle, 0x80> * g_vehiclePool;
#pragma endregion

#pragma region addon_map_ents (0x28)
//I've never found a single one of these
struct AddonMapEnts
{
	char * name;
	char * entityString;						//Size = unknownCount
	int numEntityChars;
	unknownInternalMapEnts1 unknownStruct1;
};
#pragma endregion

 union XAssetHeader
 {
	PhysPreset * physpreset;
	PhysCollMap * phys_collmap;
	XAnimParts * xanim;
	ModelSurface * modelsurfs;
	XModel * xmodel;
	Material * material;
	MaterialPixelShader * pixelshader;
	MaterialTechniqueSet * techset;
	GfxImage * image;
	snd_alias_list_t *sound;
	LoadedSound * loadedSound;
	SndCurve *sndCurve;
	clipMap_t *clipMap;
	MapEnts *mapEnts;
	GfxWorld *gfxWorld;
	GfxLightDef *lightDef;
	Font_s *font;
	MenuList *menuList;
	menuDef_t *menu;
	LocalizeEntry *localize;
	weaponVarientDef_s *weapon;
	SndDriverGlobals *sndDriverGlobals;
	FxEffectDef *fx;
	FxImpactTable *impactFx;
	Rawfile *rawfile;
	StringTable *stringTable;
	LeaderboardDef * leaderboard;
	StructuredDataDef * structuredData;
	Tracer * tracer;
	Vehicle * vehicle;
	AddonMapEnts * addonEnts;
	void * data;
};

typedef XAssetHeader (__cdecl * DB_FindXAssetHeader_t)(XAssetType assetType, char * name);
extern DB_FindXAssetHeader_t DB_FindXAssetHeader;

struct XAsset
{
	XAssetType type;
	XAssetHeader header;
};

struct XAssetEntry
{
	XAsset asset;
	char zoneIndex;
	bool inuse;
	unsigned __int16 nextHash;
	unsigned __int16 nextOverride;
	unsigned __int16 usageFrame;
};

#endif //MW2_ASSETS_H
