/*
 * mc
 */

#include <GL/glew.h>

#define HAVE_M_PI  // SDL_stdinc.h has M_PI and it collides with Windows M_PI
#define SDL_MAIN_HANDLED
#include "SDL.h"

#include <signal.h>
#include <math.h>
//#include <x86intrin.h>
#include <intrin.h>
#include <dirent.h>

#include "cglm/cglm.h"

#include "meow_intrinsics.h"
#include "meow_hash.h"

#include "shaders.h"

#include "base.h"
#include "storage.cpp"

#include "file.cpp"
#include "img.h"
#include "img.cpp"

#include "render.cpp"
#include "load_asset.cpp"


#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO  // includes stdio.h
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_GLES2_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_sdl_gles2.h"

#include "node_editor.c"

#define NK_MAX_VERTEX_MEMORY    512 * 1024
#define NK_MAX_ELEMENT_MEMORY   128 * 1024


typedef struct Controls
{
  bool32 lmbState;
  bool32 rmbState;
  uint16 mouseX;
  uint16 mouseY;
  float mouseSensitivity;
} Controls;

typedef struct State
{
  bool32 drawGUI;
  GLuint drawMode;
  bool32 running;
} State;


#define INIT_WINDOW_WIDTH    1200
#define INIT_WINDOW_HEIGHT   900

typedef struct WindowParams
{
  uint16 width;
  uint16 height;
} WindowParams;

global_variable Controls controls = {};
global_variable Controls controlsPrev = {};

global_variable uint64 perfCountFrequency;

void
SignalHandler(int signal)
{
  if (signal == SIGINT || SIGTERM) {
    SDL_Log("Received signal %d", signal);
    exit(EXIT_SUCCESS);
  }
}


int
main(int argc, char *argv[])
{
  signal(SIGINT, SignalHandler);
  signal(SIGTERM, SignalHandler);

  Memory mem = {};
  if (!mem.isInitialized)
  {
    mem.isInitialized = true;
    mem.transientMemorySize = Megabytes(8);
    mem.persistentMemorySize = Megabytes(64);
    // TODO(michalc): need a better/cross platform malloc?
    mem.transientMemory = malloc(mem.transientMemorySize);
    mem.transientTail = mem.transientMemory;
    mem.persistentMemory = malloc(mem.persistentMemorySize);
    mem.persistentTail = mem.persistentMemory;
  }
  AssetTable assetTable = {};
  assetTable.storageMemory = &mem;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
    SDL_Log("Failed to SDL_Init");
    exit(EXIT_FAILURE);
  }

  GetDisplayInformation();
  perfCountFrequency = SDL_GetPerformanceFrequency();

  // Need to set some SDL specific things for OpenGL
  if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)) {
    SDL_Log("%s", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2)) {
    SDL_Log("%s", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0)) {
    SDL_Log("%s", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)) {
    SDL_Log("%s", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  if (SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8)) {
    SDL_Log("%s", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  if (SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8)) {
    SDL_Log("%s", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  if (SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8)) {
    SDL_Log("%s", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8)) {
    SDL_Log("%s", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  SDL_Window* window = SDL_CreateWindow(
    "My game",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT,
    SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI
  );

  SDL_GLContext glContext = SDL_GL_CreateContext(window);
  if (glContext == NULL) {
    SDL_Log("%s", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  glViewport(0, 0, INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT);

  glewExperimental = GL_TRUE;
  glewInit();  // glew will look up OpenGL functions
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  //glEnable(GL_CULL_FACE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // NUKLEAR init
  nk_context* nkCtx;
  nkCtx = nk_sdl_init(window);
  nk_font_atlas *atlas;
  nk_sdl_font_stash_begin(&atlas);
  nk_sdl_font_stash_end();

  controls.mouseSensitivity = 0.01f;

  //
  // Set things UP.
  //
  ////////////////////////////////////////
  //
  // Prepare and LOOP.
  //

  uint8 framerateTarget = 60;
  float frametimeTarget = 1.0f/framerateTarget;

  WindowParams windowParameters = {};
  windowParameters.width = INIT_WINDOW_WIDTH;
  windowParameters.height = INIT_WINDOW_HEIGHT;
  State appState = {};
  appState.drawGUI = true;
  appState.drawMode = GL_FILL;
  appState.running = true;
  SDL_Event event;

  int wireframeToggler = 0;

  uint64 lastCounter = SDL_GetPerformanceCounter();
  uint64 lastCycleCount = __rdtsc();


  while (appState.running) {
    nk_input_begin(nkCtx);

    // EVENTS
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT: {
          appState.running = false;
        } break;

        case SDL_WINDOWEVENT: {
        } break;

        case SDL_KEYDOWN: {
          switch (event.key.keysym.sym)
          {
            case SDLK_ESCAPE: {
              appState.running = false;
            } break;

            case SDLK_g: {
              appState.drawGUI = !appState.drawGUI;
            } break;

            case SDLK_w: {
            } break;

            default: break;
          }
        } break;  // SDL_KEYDOWN

        case SDL_MOUSEMOTION: {
        } break;

        case SDL_MOUSEBUTTONDOWN: {
          switch (event.button.button)
          {
            case SDL_BUTTON_LEFT: {
              controls.lmbState = true;
            } break;
            default: break;
          }
        } break;

        case SDL_MOUSEBUTTONUP: {
          switch (event.button.button)
          {
            case SDL_BUTTON_LEFT: {
              controls.lmbState = false;
            } break;
            default: break;
          }
        } break;

        case SDL_MOUSEWHEEL: {
        } break;

        default: break;
      }  // switch (event.type)

      nk_sdl_handle_event(&event);
    }  // while(SDL_PollEvent(&event)

    nk_input_end(nkCtx);


    ////////////////////////////////////////
    //
    // Input
    //

    controlsPrev = controls;

    ////////////////////////////////////////
    //
    // Timing
    //

    // TODO(mc): think which real64 can be changed to real32
    uint64 endCounter = SDL_GetPerformanceCounter();
    uint64 counterElapsed = endCounter - lastCounter;
    real64 msPerFrame = (((1000.0f * (real64)counterElapsed) /
                          (real64)perfCountFrequency));
    real64 fps = (real64)perfCountFrequency / (real64)counterElapsed;
    lastCounter = endCounter;

    uint64 endCycleCount = __rdtsc();
    uint64 cyclesElapsed = endCycleCount - lastCycleCount;
    real64 mcpf = ((real64)cyclesElapsed); // / (1000.0f * 1000.0f));
    lastCycleCount = endCycleCount;


    ////////////////////////////////////////
    //
    // Render
    {
      SDL_GetWindowSize(window,
                        (int*)(&(windowParameters.width)),
                        (int*)(&(windowParameters.height))
                        );
      glViewport(0, 0, windowParameters.width, windowParameters.height);

      glClearColor(0.1, 0.1, 0.1, 1.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      node_editor(nkCtx);
      nk_sdl_render(NK_ANTI_ALIASING_ON, NK_MAX_VERTEX_MEMORY, NK_MAX_ELEMENT_MEMORY);

      SDL_GL_SwapWindow(window);

      SDL_Delay(10);
    }

    // TODO(mc): add sleep for the rest of fixed frame time
    /*
    if (there is more time in this frame computing) {
      sleep
    }
    else {
      drop the target framerate
    }
    */

  }   // <---- END OF running WHILE

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  SDL_Quit();
  return 0;
}
