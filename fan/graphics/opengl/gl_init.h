#pragma once

#include <fan/types/print.h>
#if defined(fan_platform_windows)
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_NATIVE_INCLUDE_NONE
#endif
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <utility>

#ifndef debug_glcall_timings
  #define debug_glcall_timings
#endif
#if defined(debug_glcall_timings)
  #include <fan/time/timer.h>
#endif

#include <fan/graphics/types.h>

#define fan_opengl_call(func) \
  [&]() { \
    struct measure_func_t { \
      measure_func_t() { \
        c.start(); \
      }\
      ~measure_func_t() { \
        glFlush(); \
        glFinish(); \
        if (c.elapsed() / 1e+9 > 0.01) {\
          std::string func_call = #func; \
          std::string func_name = func_call.substr(0, func_call.find('(')); \
          fan::printclnnh(fan::graphics::highlight_e::text, func_name + ":"); \
          fan::printclh(fan::graphics::highlight_e::warning,  fan::to_string(c.elapsed() / 1e+6) + "ms"); \
        }\
      } \
      fan::time::clock c; \
    }mf; \
    return func; \
  }()

namespace fan {
  namespace opengl {
    struct opengl_t {
      int major = -1;
      int minor = -1;
      void open() {
        static uint8_t init = 1;
        if (init == 0) {
          return;
        }
        init = 0;
        if (GLenum err = glewInit() != GLEW_OK) {
          fan::throw_error(std::string("glew init error:") + (const char*)glewGetErrorString(err));
        }
      }
    };
  }

}