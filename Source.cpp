// Creates window, opengl context

#include 	<string.h>
#include <tchar.h>
#define _INCLUDE_TOKEN(p0, p1) <p0/p1>

#define FAN_INCLUDE_PATH C:/libs/fan/include
#define fan_debug 0
#include _INCLUDE_TOKEN(FAN_INCLUDE_PATH, fan/types/types.h)
#include _FAN_PATH(window/window.h)

int main() {
  fan::string str("test");

  return 0;
}