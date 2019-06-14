typedef enum {
  NONE       = 1 << 0,
  VERTEX_POS = 1 << 1,
  VERTEX_COL = 1 << 2,
  VERTEX_UVS = 1 << 3,
} ATTRIBUTES;


void
GetDisplayInformation()
{
  int8 displayCount = 0, displayIndex = 0, modeIndex = 0;
  SDL_DisplayMode mode = {SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0};
  displayCount = SDL_GetNumVideoDisplays();
  SDL_GetDisplayMode(displayIndex, modeIndex, &mode);
  SDL_Log("Display count: %d\n"
          "      Display refresh rate: %d\n",
          displayCount,
          mode.refresh_rate); 
}

//void
//UpdateVBO()
//{
  //glBindBuffer(GL_ARRAY_BUFFER, vboPlane);
  //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//}


/*
 * SHADERS functionallity
 */

void
CompileShader(const GLchar* const inVS, const GLchar* const inFS,
              GLuint* const outVS, GLuint* const outFS)
{
  int success = 0;
  char infoLog[512];

  *outVS = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(*outVS, 1, &inVS, NULL);
  glCompileShader(*outVS);
  glGetShaderiv(*outVS, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(*outVS, 512, NULL, infoLog);
    SDL_Log("Vertex shader compilation failed: %s", infoLog);
  }

  *outFS = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(*outFS, 1, &inFS, NULL);
  glCompileShader(*outFS);
  glGetShaderiv(*outFS, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(*outFS, 512, NULL, infoLog);
    SDL_Log("Fragment shader compilation failed: %s", infoLog);
  }
}

GLuint
SetShader(GLuint inVS, GLuint inFS)
{
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, inVS);
  glAttachShader(shaderProgram, inFS);
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  return shaderProgram;
}

void
BindShaderLocations(GLint modelLoc,
                    GLint viewLoc,
                    GLint projectionLoc,
                    float* modelData,
                    float* viewData,
                    float* projectionData)
{
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelData);
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, viewData);
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projectionData);
}

GLint
GetUniformLoc(GLuint shaderProgram, const char* name)
{
  GLint planeIndexLoc = glGetUniformLocation(shaderProgram, name);
  char logBuffer[128];
  sprintf(logBuffer, "No '%s' uniform in the shader's code.", name);
  if (planeIndexLoc == -1) {
    SDL_Log(logBuffer);
  }
  return planeIndexLoc;
}
