#pragma once

#include <fan/graphics/opengl/gl_defines.h>

#include <fan/math/random.h>
#include <fan/time/time.h>

#if defined(fan_platform_windows)
  #include <Windows.h>
  #pragma comment(lib, "User32.lib")
  #pragma comment(lib, "Gdi32.lib")

#elif defined(fan_platform_unix)
#endif

#include <unordered_map>

#include <fan/window/window.h>

namespace fan {

  namespace opengl {
    struct opengl_t {
      int major = -1;
      int minor = -1;

      static void* get_proc_address_(const char* name)
      {
        return (void*)glfwGetProcAddress(name);
      }


    #if fan_debug >= fan_debug_high
      // todo implement debug
      #define get_proc_address(type, name) \
        type name = (type)get_proc_address_(#name)
      std::unordered_map<void*, fan::string> function_map;
    #else
      #define get_proc_address(type, name, internal_) type name = (type)get_proc_address_(#name, internal_);
    #endif
      
      #if fan_debug >= fan_debug_high

      fan::time::clock c;

      opengl_t() {

      }

      void execute_before(const fan::string& function_name) {
        c.start();
      }

      // TODO if function empty probably some WGL/GLX function, initialized in bind window
      void execute_after(const fan::string& function_name) {
        glFlush();
        glFinish();
        auto elapsed = c.elapsed();
        if (elapsed > 1000000) {
          fan::print_no_space("slow function call ns:", elapsed, ", function::" + function_name);
        }
      }
      
      //efine call_opengl

      template <typename T, typename... T2>
      constexpr auto call(const T& t, T2&&... args) {
          if constexpr (std::is_same<std::invoke_result_t<T, T2...>, void>::value) {
              t(std::forward<T2>(args)...);
          } else {
              return t(std::forward<T2>(args)...);
          }
      }
      #else

      template <typename T, typename ...T2>
      constexpr auto call(const T& t, T2&&... args) {
        if constexpr (std::is_same<fan::return_type_of_t<T>, void>::value) {
          t(args...);
        }
        else {
          return t(args...);
        }
      }

      #endif

      GLFWwindow* init__ = [] {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

        GLFWwindow* dummy_window = glfwCreateWindow(640, 400, "dummy", nullptr, nullptr);
        if (dummy_window == nullptr) {
          fan::throw_error("failed to open dummy window");
        }
        glfwMakeContextCurrent(dummy_window);
        return dummy_window;
      }();
      get_proc_address(PFNGLGETSTRINGPROC, glGetString);
      get_proc_address(PFNGLVIEWPORTPROC, glViewport);
      get_proc_address(PFNGLBLENDFUNCPROC, glBlendFunc);
      get_proc_address(PFNGLCREATEVERTEXARRAYSPROC, glGenVertexArrays);
      get_proc_address(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
      get_proc_address(PFNGLGENBUFFERSPROC, glGenBuffers);
      get_proc_address(PFNGLBINDBUFFERPROC, glBindBuffer);
      get_proc_address(PFNGLBUFFERDATAPROC, glBufferData);
      get_proc_address(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
      get_proc_address(PFNGLBUFFERSUBDATAPROC, glBufferSubData);
      get_proc_address(PFNGLCLEARPROC, glClear);
      get_proc_address(PFNGLCLEARCOLORPROC, glClearColor);
      get_proc_address(PFNGLDEBUGMESSAGECALLBACKPROC, glDebugMessageCallback);
      get_proc_address(PFNGLENABLEPROC, glEnable);
      get_proc_address(PFNGLDISABLEPROC, glDisable);
      get_proc_address(PFNGLDRAWARRAYSPROC, glDrawArrays);
      get_proc_address(PFNGLDRAWARRAYSINSTANCEDPROC, glDrawArraysInstanced);
      get_proc_address(PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC, glDrawArraysInstancedBaseInstance);
      get_proc_address(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
      get_proc_address(PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation);
      get_proc_address(PFNGLGETBUFFERPARAMETERIVPROC, glGetBufferParameteriv);
      get_proc_address(PFNGLGETINTEGERVPROC, glGetIntegerv);
      get_proc_address(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
      get_proc_address(PFNGLGENTEXTURESPROC, glGenTextures);
      get_proc_address(PFNGLDELETETEXTURESPROC, glDeleteTextures);
      get_proc_address(PFNGLBINDTEXTUREPROC, glBindTexture);
      get_proc_address(PFNGLGETTEXIMAGEPROC, glGetTexImage);
      get_proc_address(PFNGLTEXIMAGE2DPROC, glTexImage2D);
      get_proc_address(PFNGLTEXPARAMETERIPROC, glTexParameteri);
      get_proc_address(PFNGLACTIVETEXTUREPROC, glActiveTexture);
      get_proc_address(PFNGLATTACHSHADERPROC, glAttachShader);
      get_proc_address(PFNGLCREATESHADERPROC, glCreateShader);
      get_proc_address(PFNGLDELETESHADERPROC, glDeleteShader);
      get_proc_address(PFNGLCOMPILESHADERPROC, glCompileShader);
      get_proc_address(PFNGLCREATEPROGRAMPROC, glCreateProgram);
      get_proc_address(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
      get_proc_address(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap);
      get_proc_address(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
      get_proc_address(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
      get_proc_address(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
      get_proc_address(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
      get_proc_address(PFNGLLINKPROGRAMPROC, glLinkProgram);
      get_proc_address(PFNGLSHADERSOURCEPROC, glShaderSource);
      get_proc_address(PFNGLUNIFORM1DPROC, glUniform1d);
      get_proc_address(PFNGLUNIFORM1FPROC, glUniform1f);
      get_proc_address(PFNGLUNIFORM1IPROC, glUniform1i);
      get_proc_address(PFNGLUNIFORM1IVPROC, glUniform1iv);
      get_proc_address(PFNGLUNIFORM1UIPROC, glUniform1ui);
      get_proc_address(PFNGLUNIFORM2DPROC, glUniform2d);
      get_proc_address(PFNGLUNIFORM2DVPROC, glUniform2dv);
      get_proc_address(PFNGLUNIFORM2FPROC, glUniform2f);
      get_proc_address(PFNGLUNIFORM2FVPROC, glUniform2fv);
      get_proc_address(PFNGLUNIFORM3DPROC, glUniform3d);
      get_proc_address(PFNGLUNIFORM3FPROC, glUniform3f);
      get_proc_address(PFNGLUNIFORM3FVPROC, glUniform3fv);
      get_proc_address(PFNGLUNIFORM4DPROC, glUniform4d);
      get_proc_address(PFNGLUNIFORM4FPROC, glUniform4f);
      get_proc_address(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
      get_proc_address(PFNGLUNIFORMMATRIX4DVPROC, glUniformMatrix4dv);
      get_proc_address(PFNGLUSEPROGRAMPROC, glUseProgram);
      get_proc_address(PFNGLVALIDATEPROGRAMPROC, glValidateProgram);
      get_proc_address(PFNGLGETSHADERIVPROC, glGetShaderiv);
      get_proc_address(PFNGLDEPTHFUNCPROC, glDepthFunc);
      get_proc_address(PFNGLPOLYGONMODEPROC, glPolygonMode);
      get_proc_address(PFNGLUNIFORM1UIVPROC, glUniform1uiv);
      get_proc_address(PFNGLUNIFORM4FVPROC, glUniform4fv);
      get_proc_address(PFNGLMATRIXMODEPROC, glMatrixMode);
      get_proc_address(PFNGLLOADIDENTITYPROC, glLoadIdentity);
      get_proc_address(PFNGLORTHOPROC, glOrtho);
      get_proc_address(PFNGLHINTPROC, glHint);
      get_proc_address(PFNGLFLUSHPROC, glFlush);
      get_proc_address(PFNGLFINISHPROC, glFinish);
      get_proc_address(PFNGLGETTEXTURELEVELPARAMETERIVPROC, glGetTexLevelParameteriv);
      get_proc_address(PFNGLGETUNIFORMBLOCKINDEXPROC, glGetUniformBlockIndex);
      get_proc_address(PFNGLUNIFORMBLOCKBINDINGPROC, glUniformBlockBinding);
      get_proc_address(PFNGLBINDBUFFERRANGEPROC, glBindBufferRange);
      get_proc_address(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
      get_proc_address(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers);
      get_proc_address(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer);
      get_proc_address(PFNGLFRAMEBUFFERTEXTUREPROC , glFramebufferTexture);
      get_proc_address(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D);
      get_proc_address(PFNGLGENRENDERBUFFERSPROC, glGenRenderbuffers);
      get_proc_address(PFNGLDELETERENDERBUFFERSPROC, glDeleteRenderbuffers);
      get_proc_address(PFNGLBINDRENDERBUFFERPROC, glBindRenderbuffer);
      get_proc_address(PFNGLRENDERBUFFERSTORAGEPROC, glRenderbufferStorage);
      get_proc_address(PFNGLFRAMEBUFFERRENDERBUFFERPROC, glFramebufferRenderbuffer);
      get_proc_address(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus);
      get_proc_address(PFNGLDEPTHMASKPROC, glDepthMask);
      get_proc_address(PFNGLCULLFACEPROC, glCullFace);
      get_proc_address(PFNGLFRONTFACEPROC, glFrontFace);
      get_proc_address(PFNGLBLENDEQUATIONPROC, glBlendEquation);
      get_proc_address(PFNGLALPHAFUNCPROC, glAlphaFunc);
      get_proc_address(PFNGLTEXPARAMETERFPROC, glTexParameterf);
      get_proc_address(PFNGLDRAWBUFFERSPROC, glDrawBuffers);
      get_proc_address(PFNGLCLEARBUFFERFVPROC, glClearBufferfv);
      get_proc_address(PFNGLLINEWIDTHPROC, glLineWidth);
      get_proc_address(PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC, glNamedFramebufferDrawBuffers);
      get_proc_address(PFNGLDRAWBUFFERPROC, glDrawBuffer);
      get_proc_address(PFNGLGETERRORPROC, glGetError);
      get_proc_address(PFNGLPIXELSTOREIPROC, glPixelStorei);
      get_proc_address(PFNGLGETBUFFERSUBDATAPROC, glGetBufferSubData);
      get_proc_address(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);
      get_proc_address(PFNGLUNIFORM1FVPROC, glUniform1fv);
      get_proc_address(PFNGLGETSTRINGIPROC, glGetStringi);
      get_proc_address(PFNGLBLENDFUNCSEPARATEPROC, glBlendFuncSeparate);
      get_proc_address(PFNGLSCISSORPROC, glScissor);
      get_proc_address(PFNGLDRAWELEMENTSPROC, glDrawElements);
      get_proc_address(PFNGLDETACHSHADERPROC, glDetachShader);
      get_proc_address(PFNGLBLENDEQUATIONSEPARATEPROC, glBlendEquationSeparate);
      get_proc_address(PFNGLISENABLEDPROC, glIsEnabled);
      get_proc_address(PFNGLISPROGRAMPROC, glIsProgram);
      get_proc_address(PFNGLREADPIXELSPROC, glReadPixels);
      get_proc_address(PFNGLREADBUFFERPROC, glReadBuffer);
      get_proc_address(PFNGLDRAWPIXELSPROC, glDrawPixels);
      get_proc_address(PFNGLBLITFRAMEBUFFERPROC, glBlitFramebuffer);
      get_proc_address(PFNGLVERTEXATTRIBDIVISORPROC, glVertexAttribDivisor);
      get_proc_address(PFNGLSTENCILOPPROC, glStencilOp);
      get_proc_address(PFNGLSTENCILFUNCPROC, glStencilFunc);
      get_proc_address(PFNGLSTENCILMASKPROC, glStencilMask);
      get_proc_address(PFNGLMAPBUFFERPROC, glMapBuffer);
      get_proc_address(PFNGLUNMAPBUFFERPROC, glUnmapBuffer);
      bool end_of_init = [this] {
        if (major == -1 || minor == -1) {
          const char* gl_version = (const char*)glGetString(GL_VERSION);
          sscanf(gl_version, "%d.%d", &major, &minor);
        }
        glfwDestroyWindow(init__);
        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
        return 1;
      }();
    };
    #undef get_proc_address


  }

}