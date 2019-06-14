#define ASSETS_MAX_COUNT 128

typedef struct Memory {
  bool32 isInitialized;

  uint64 transientMemorySize;
  void* transientMemory;
  void* transientTail;

  uint64 persistentMemorySize;
  void* persistentMemory;
  void* persistentTail;
} Memory;

typedef enum AssetType {
  ASSET_NONE,
  ASSET_MODEL3D_OBJ,
  ASSET_MODEL3D_GLTF,
  ASSET_FONT_TTF,
} AssetType;

typedef struct Asset {
  AssetType type;
  uint32 size;
  void* memory;
} Asset;

typedef struct AssetTable {
  uint32 assetsCount;
  Asset assets[ASSETS_MAX_COUNT];
  Memory* storageMemory;
} AssetTable;

typedef struct Model3D {
  char* name;
  float* vertices;
  uint32* indices;
  float* normals;
  float* colors;
  float* uvs;

  uint32 verticesCount;
  uint32 verticesSize;
  uint32 indicesSize;
  uint32 normalsSize;
  uint32 colorsSize;
  uint32 uvsSize;
} Model3D;

