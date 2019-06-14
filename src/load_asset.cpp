typedef enum OBJ_LINE{
  OBJ_LINE_COMMENT             = 0,
  OBJ_LINE_MATERIAL,
  OBJ_LINE_GROUP,
  OBJ_LINE_VERT_POSITION,
  OBJ_LINE_VERT_INDICES,
  OBJ_LINE_UVS_POSITION,
  OBJ_LINE_VERT_COLORS,
  OBJ_LINE_EOLINE,
  OBJ_LINE_EOFILE
} OBJ_LINE;

global_variable char OBJLINEHEADERS[8][8] = {"#", "mtllib", "o",
                                             "v", "f", "vt"};

internal char*
GoThroughSpaces(char* pointer)
{
  uint8 canary = 0;
  // Go through spaces
  while (*pointer == ' ')
  {
    if ((canary++) > 128)
    {
      return NULL;
    }
    pointer++;
  }

  return pointer;
}

internal void
AddAsset(Asset* asset, AssetTable* assetTable)
{
  assetTable->assetsCount++;

  // TODO(michalc): fix size getting
  uint32 assetSize = sizeof((uint8*)(asset->memory));
  assetTable->storageMemory->transientTail =
    (uint8*)assetTable->storageMemory->transientTail + assetSize;

  // TODO(michalc): always to 0...?
  assetTable->assets[0] = *asset;
}

/*
 * Load file into a transient memory. assetTable is passed because it has a 
 * pointer to the allocated memory (assetTable.storageMemory). asset type is
 * filled in and pushed into assetTable. AddAsset pushes transientTail to the
 * end of the loaded data.
 */
internal void
LoadAsset(char* path, AssetTable* assetTable, AssetType type)
{
  Asset asset = {};
  asset.type = ASSET_MODEL3D_OBJ;
  // Plug the memory pointer to the transient memory and fill in the size.
  asset.memory = LoadEntireFileToMemory(path, assetTable, &asset.size);

  AddAsset(&asset, assetTable);
}

/*
 * Return a pointer to the next line (not the \n).
 */
internal char*
ParseOBJLine(char* lineStart,
             void* values,
             uint8* valuesCount,
             OBJ_LINE* type)
{
  char* cursor = lineStart;
  uint8 bufferIdx = 0;
  char textBuffer[64] = {};

  // Skip spaces if any.
  if (*cursor == ' ')
  {
    cursor = GoThroughSpaces(cursor);
  }

  // Identify the line type.
  while (*cursor != ' ')
  {
    textBuffer[bufferIdx++] = *(cursor++);
  }
  textBuffer[bufferIdx] = 0;

  if (strcmp(textBuffer, OBJLINEHEADERS[OBJ_LINE_COMMENT]) == 0)
  {
    *type = OBJ_LINE_COMMENT;
    while (*(cursor++) != '\n');
    return cursor;
  }
  if (strcmp(textBuffer, OBJLINEHEADERS[OBJ_LINE_MATERIAL]) == 0)
  {
    *type = OBJ_LINE_MATERIAL;
    while (*(cursor++) != '\n');
    return cursor;
  }
  if (strcmp(textBuffer, OBJLINEHEADERS[OBJ_LINE_GROUP]) == 0)
  {
    *type = OBJ_LINE_GROUP;
    while (*(cursor++) != '\n');
    return cursor;
  }
  if (strcmp(textBuffer, OBJLINEHEADERS[OBJ_LINE_VERT_POSITION]) == 0)
  {
    *type = OBJ_LINE_VERT_POSITION;
  }
  if (strcmp(textBuffer, OBJLINEHEADERS[OBJ_LINE_UVS_POSITION]) == 0)
  {
    *type = OBJ_LINE_UVS_POSITION;
  }
  if (strcmp(textBuffer, OBJLINEHEADERS[OBJ_LINE_VERT_COLORS]) == 0)
  {
    *type = OBJ_LINE_VERT_COLORS;
  }
  if (strcmp(textBuffer, OBJLINEHEADERS[OBJ_LINE_VERT_INDICES]) == 0)
  {
    *type = OBJ_LINE_VERT_INDICES;
  }

  // Clean up the textBuffer.
  memset(textBuffer, 0, 64);
  bufferIdx = 0;

  cursor = GoThroughSpaces(++cursor);

  // Float values strategy
  while (*cursor != '\n')
  {
    uint8 idx = 0;

    do
    {
      char character = *(cursor++);
      if (((character >= '0') && (character <= '9')) ||
          (character == '.') || (character == '-') || (character == '/'))
      {
        textBuffer[idx++] = character;
      }
    } while ((*cursor != ' ') && (*cursor != '\n'));
    textBuffer[idx] = 0;

    switch (*type) {
      case OBJ_LINE_VERT_POSITION:
        {
          // TODO(michalc): change the atof to something else (strtof? custom impl?)
          ((float*)values)[bufferIdx++] = atof(textBuffer);
        } break;
      case OBJ_LINE_UVS_POSITION:
        {
          // TODO(michalc): change the atof to something else (strtof? custom impl?)
          ((float*)values)[bufferIdx++] = atof(textBuffer);
        } break;
      case OBJ_LINE_VERT_INDICES: 
        {
          char textBuffer2[16] = {};
          uint8 idx1 = 0;
          uint8 idx2 = 0;
          // Save '1/2/3' indices into the memory
          // TODO(michalc): those indices are actually position/uv/normal indices
          while (true)
          {
            if ((textBuffer[idx1] == '/') || (textBuffer[idx1] == 0))
            {
              textBuffer2[idx2] = 0;
              ((uint32*)values)[bufferIdx++] = atol(textBuffer2);
              memset(textBuffer2, 0, 16);
              idx2 = 0;
              // TODO(michalc): this break below is a dirty hack so only the first
              //                value from X/Y/Z gets extracted
              break;
              // while is being broken here because it needs to handle last index
              // before \n
              if (textBuffer[idx1] == 0)
              {
                break;
              }
            }
            else
            {
              textBuffer2[idx2] = textBuffer[idx1];
              idx2++;
            }
            idx1++;
          }
        } break;
      default:
        {

        } break;
    }

    cursor = GoThroughSpaces(cursor);
  }
  *valuesCount = bufferIdx;

  // ++ to skip last \n
  return (++cursor);
}

/*
 * Parse data from transient memory into the persistent memory.
 */
internal void
RetriveOBJ(uint8 index, AssetTable* assetTable, Model3D* model)
{
  Asset asset = assetTable->assets[index];
  char* assetMemoryPointer = (char*)(asset.memory);
  uint32 assetMemorySize = asset.size;
  // Buffer for 16 4 bytes values
  uint8 values[16 * 4] = {};
  uint8 valuesCount;
  OBJ_LINE lineType = OBJ_LINE_COMMENT;

  do
  {
    uint8* memoryStart = (uint8*)assetMemoryPointer;
    assetMemoryPointer = ParseOBJLine(assetMemoryPointer,
                                      (void*)values, &valuesCount, &lineType);
    uint8* memoryEnd = (uint8*)assetMemoryPointer;
    assetMemorySize -= (uint32)(memoryEnd - memoryStart);

    if ((lineType == OBJ_LINE_VERT_POSITION) ||
        (lineType == OBJ_LINE_UVS_POSITION) ||
        (lineType == OBJ_LINE_VERT_INDICES))
    {
      switch (lineType) {
        case OBJ_LINE_VERT_POSITION:
          {
            if (model->vertices == NULL)
            {
              model->vertices = (float*)assetTable->storageMemory->persistentTail;
            }
            memcpy(assetTable->storageMemory->persistentTail,
                   values, valuesCount*sizeof(float));
            model->verticesSize += valuesCount*sizeof(uint32);
            model->verticesCount += valuesCount;
          } break;
        case OBJ_LINE_UVS_POSITION:
          {
            if (model->uvs == NULL)
            {
              model->uvs = (float*)assetTable->storageMemory->persistentTail;
            }
            memcpy(assetTable->storageMemory->persistentTail,
                   values, valuesCount*sizeof(float));
            model->uvsSize += valuesCount*sizeof(uint32);
          } break;
        case OBJ_LINE_VERT_INDICES: 
          {
            if (model->indices == NULL)
            {
              model->indices = (uint32*)assetTable->storageMemory->persistentTail;
            }
            memcpy(assetTable->storageMemory->persistentTail,
                   values, valuesCount*sizeof(uint32));
            model->indicesSize += valuesCount*sizeof(uint32);
          } break;
        default:
          {

          } break;
      }

      assetTable->storageMemory->persistentTail =
        (float*)assetTable->storageMemory->persistentTail + valuesCount;
    }
  }
  while (assetMemorySize > 0);
}
