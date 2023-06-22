#ifndef LIGHTCACHE_H
#define LIGHTCACHE_H

#include "mathlib/vector.h"
#include "basetypes.h"
#include "ivmodelrender.h"
#include "datacache/idatacache.h"

#define MAXLOCALLIGHTS	4
#define	MAX_OSPATH		260			// max length of a filesystem pathname

struct dworldlight_t;
struct worldbrushdata_t;

class IClientRenderable;
class CEngineSprite;

//typedef unsigned short ModelInstanceHandle_t;

FORWARD_DECLARE_HANDLE(LightCacheHandle_t);
FORWARD_DECLARE_HANDLE(memhandle_t);

// DECLARE_POINTER_HANDLE(StudioDecalHandle_t);
//DECLARE_POINTER_HANDLE(LightCacheHandle_t);

typedef memhandle_t DataCacheHandle_t;
typedef void* FileNameHandle_t;
typedef unsigned short MDLHandle_t;

struct brushdata_t
{
	worldbrushdata_t* pShared;
	int			firstmodelsurface, nummodelsurfaces;

	unsigned short	renderHandle;
	unsigned short	firstnode;
};

struct spritedata_t
{
	int				numframes;
	int				width;
	int				height;
	CEngineSprite* sprite;
};

enum modtype_t
{
	mod_bad = 0,
	mod_brush,
	mod_sprite,
	mod_studio
};

// Taken from the source of the game tf2 ('...\engine\lightcache.h'). Looks like the structure is the same.
// size 92
struct LightingState_t
{
	Vector		r_boxcolor[6];		// ambient, and lights that aren't in locallight[]
	int			numlights;
	dworldlight_t* locallight[MAXLOCALLIGHTS];
};

// Taken from the source of the game tf2 ('...\engine\l_studio.h'). Looks like the structure is the same.
// size 260
// 'CUtlLinkedList< ModelInstance_t, ModelInstanceHandle_t > &m_ModelInstances' - tells us that the size is 260 
// (it seems structure 'CUtlLinkedList' itself adds 4 more bytes)
struct ModelInstance_t
{
	IClientRenderable* m_pRenderable; // 0

	// Need to store off the model. When it changes, we lose all instance data..
	// Offset 4. We use this. Offset is inside function 'CColorMeshData* CModelRender::FindOrCreateStaticPropColorData(ModelInstanceHandle_t handle)'.
	model_t* m_pModel; // 4

	StudioDecalHandle_t	m_DecalHandle; // 8

	// Stores off the current lighting state
	LightingState_t m_CurrentLightingState; // 12 
	LightingState_t	m_AmbientLightingState; // 104
	Vector m_flLightIntensity[MAXLOCALLIGHTS]; // 196 (size 48)
	float m_flLightingTime; // 244

	// First shadow projected onto the model
	unsigned short m_FirstShadow; // 248
	unsigned short m_nFlags; // 250

	// Static lighting
	LightCacheHandle_t m_LightCacheHandle; // 256

	// Color mesh managed by cache
	// Offset 256. We use this. Offset is inside function 'CColorMeshData* CModelRender::FindOrCreateStaticPropColorData(ModelInstanceHandle_t handle)'.
	DataCacheHandle_t m_ColorMeshHandle; // 256
};

class CModelRender /* :
	public IVModelRender,
	public CManagedDataCacheClient< CColorMeshData, colormeshparams_t >*/
{
public:
	void* vtable; // 0 
	void* vtable2; // 4

	// Offset 8. We use this. Offset is inside function 'CColorMeshData* CModelRender::FindOrCreateStaticPropColorData(ModelInstanceHandle_t handle)'.
	// Is inside the inherited class 'CManagedDataCacheClient'.
	// From '...\public\datacache\idatacache.h'
	IDataCacheSection* m_pCache; // 8

	// Offset 12. We use this. Offset is inside function 'CColorMeshData* CModelRender::FindOrCreateStaticPropColorData(ModelInstanceHandle_t handle)'.
	CUtlLinkedList< ModelInstance_t, ModelInstanceHandle_t > m_ModelInstances; // 12
};

struct model_t
{
	FileNameHandle_t	fnHandle; // 0
	char				szPathName[MAX_OSPATH]; // 4

	int					nLoadFlags;		// 264, mark loaded/not loaded
	int					nServerCount;	// 268, marked at load
	
	modtype_t			type;
	int					flags;			// MODELFLAG_???

	// volume occupied by the model graphics	
	Vector				mins, maxs;
	float				radius;

	union
	{
		brushdata_t		brush;
		MDLHandle_t		studio;
		spritedata_t	sprite;
	};
};

#endif // LIGHTCACHE_H
