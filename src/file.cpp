/*
 * Reads the whole file into a malloced memory
 */
internal uint8*
LoadEntireFile(char* fileName)
{
  // TODO(mc): probably should be SDL_AllocRW
  // TODO(mc): should it be (void*)
  uint8* fileData = (uint8*)malloc(1<<18);
  SDL_RWops* file = SDL_RWFromFile(fileName, "rb");
  if (file == NULL) {
    SDL_Log("Failed LoadEntireFile(): %s", fileName);
  }
  // TODO(mc): check if opened properly
  uint8 unitSize = 1;
  uint64 unitCount = SDL_RWsize(file);
  SDL_RWread(file, fileData, unitSize, unitCount);
  SDL_RWclose(file);
  return fileData;
}

/*
 * Reads the whole file and saves it into the transient memory.
 */
internal void*
LoadEntireFileToMemory(char* fileName, AssetTable* assetTable, uint32* size)
{
  // TODO(mc): probably should be SDL_AllocRW
  // TODO(mc): should it be (void*)
  void* storagePointer = assetTable->storageMemory->transientTail;
  SDL_RWops* fileHandle = SDL_RWFromFile(fileName, "rb");
  if (fileHandle == NULL) {
    SDL_Log("Failed LoadEntireFile(): %s", fileName);
  }
  // TODO(mc): check if opened properly
  uint8 unitSize = 1;
  uint64 unitCount = SDL_RWsize(fileHandle);
  *size = unitCount;
  SDL_RWread(fileHandle, storagePointer, unitSize, unitCount);
  SDL_RWclose(fileHandle);
  return storagePointer;
}

typedef struct ListDirectoryParams {
  uint32 filesCount;
  char* filesList;
  char filter[64];
  char path[256];
} ListDirectoryParams;

// TODO(mc): port ListDirectory to SDL...?
internal uint32
ListDirectory(char* inPath, char* outFilesList, char* filter)
{
  DIR *dp;
  struct dirent *ep;     
  dp = opendir(inPath);
  uint32 filesCount = 0;

  if (dp != NULL)
  {
    char* copyDestination = outFilesList;

    while (ep = readdir(dp))
    {
      if (!(memcmp(ep->d_name, ".", strlen(ep->d_name))))
      {
        continue;
      }
      else if (!(memcmp(ep->d_name, "..", strlen(ep->d_name))))
      {
        continue;
      }
      else if (strstr(ep->d_name, filter) == NULL)
      {
        continue;
      }
      else
      {
        memcpy(copyDestination, ep->d_name, strlen(ep->d_name));
        memset(copyDestination + strlen(ep->d_name), 0, 1);
        copyDestination += (strlen(ep->d_name) + 1);
        filesCount++;
      }
    }
    (void)closedir(dp);
  }
  else
  {
    fprintf(stderr, " [E] Couldn't open the directory");
  }

  return filesCount;
}

internal uint32
ListDirectoryOnTimer(uint32 interval, void* params)
{
  ListDirectoryParams* _params = (ListDirectoryParams*)params;
  _params->filesCount = ListDirectory(_params->path, _params->filesList, _params->filter);
  return interval;
}
