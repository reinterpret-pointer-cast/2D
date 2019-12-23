#include "Settings.hpp"
#include <cstdio>
#include <GLFW/glfw3.h>

namespace Settings {
	float deltaTime = 0;
}

namespace FanColors {
	Vec3 White = Vec3(1.0, 1.0, 1.0);
	Vec3 Red(1.0, 0.0, 0.0);
	Vec3 Green(0.0, 1.0, 0.0);
	Vec3 Blue(0.0, 0.0, 1.0);
}

void GetFps(){
	static int fps = 0;
	static double start = glfwGetTime();
	if ((glfwGetTime() - start) > 1.0) {
		printf("%d\n", fps);

		fps = 0;
		start = glfwGetTime();
	}
	fps++;
}