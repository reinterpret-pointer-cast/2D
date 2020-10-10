#include <FAN/t.h>

#ifdef REQUIRE_GRAPHICS
#include <GLFW/glfw3.h>

constexpr auto WINDOWSIZE = fan::vec2i(1280, 960);

inline GLFWwindow* window;

inline float delta_time;

inline bool is_colliding;

namespace flags {
	constexpr bool decorated = true;
	constexpr bool disable_mouse = 0;
	constexpr bool antialising = 0;
	constexpr bool full_screen = false;
}

#endif