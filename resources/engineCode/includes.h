#ifndef INCLUDES
#define INCLUDES

// stl includes
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

// iostream stuff
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::flush;
using std::string;
using std::unordered_map;

// pi definition - definitely sufficient precision
constexpr double pi = 3.14159265358979323846;

// MSAA count - effects OpenGL geometry evaluation
constexpr int MSAACount = 1;

// tick() / tock()
#include "coreUtils/timer.h"

// vector math library GLM
#define GLM_FORCE_SWIZZLE
#define GLM_SWIZZLE_XYZW
#include "../GLM/glm.hpp"                  //general vector types
#include "../GLM/gtc/matrix_transform.hpp" // for glm::ortho
#include "../GLM/gtc/type_ptr.hpp"         //to send matricies gpu-side
#include "../GLM/gtx/rotate_vector.hpp"
#include "../GLM/gtx/transform.hpp"

// not sure as to the utility of this
#define GLX_GLEXT_PROTOTYPES

// OpenGL Loader
#include "../ImGUI/gl3w.h"

// GUI library (dear ImGUI)
#include "../ImGUI/TextEditor.h"
#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_impl_sdl.h"
#include "../ImGUI/imgui_impl_opengl3.h"

// SDL includes - windowing, gl context, system info
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// structs for menus / operations
#include "coreUtils/dataStructs.h"

// managing bindings of textures to binding points
#include "coreUtils/bindset.h"

// image load/save/resize/access/manipulation wrapper
#include "../ImageHandling/image.h"

// letter stamping
#include "../bitfontCore/letters.h"

// spaceship generator
#include "../bitfontCore/spaceship.h"

// #define WIDTH 640
// #define HEIGHT 480

// #define WIDTH 1280
// #define HEIGHT 720

#define WIDTH 1920
#define HEIGHT 1080

#define STARTONWINDOW 3

#define BLOCKDIM 256
#define SSFACTOR 1.0
#define TILESIZE 64

// wrapper around window management
#include "coreUtils/windowHandler.h"

// orientation widget implementation
#include "coreUtils/trident.h"

// menu entry list + categories
#include "coreUtils/menuEntry.h"

// tracy profiler annotation
#include "../tracy/public/tracy/Tracy.hpp"

// font rendering header
#include "../fonts/fontRenderer/renderer.h"

// wrapper for TinyOBJLoader
#include "../TinyOBJLoader/objLoader.h"

// new shader compilation wrapper
#include "shaders/lib/shaderWrapper.h"

// color codes for CLI output
#include "../fonts/colors.h"

// diamond square heightmap generation
#include "../noise/diamondSquare/diamond_square.h"

// Brent Werness' Voxel Automata Terrain
#include "../noise/VAT/VAT.h"

// more general noise solution
#include "../noise/FastNoise2/include/FastNoise/FastNoise.h"

// old perlin implementation
#include "../noise/perlin.h"

// Niels Lohmann - JSON for Modern C++
#include "../JSON/json.h"
using json = nlohmann::json;


#endif
