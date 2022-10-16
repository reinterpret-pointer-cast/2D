#pragma once

#include _FAN_PATH(window/window.h)
#include _FAN_PATH(physics/collision/rectangle.h)
#include _FAN_PATH(graphics/camera.h)
#include _FAN_PATH(types/masterpiece.h)

#if defined(loco_opengl)
	#include _FAN_PATH(graphics/opengl/gl_graphics.h)
#elif defined(loco_vulkan)
	#include _FAN_PATH(graphics/vulkan/vk_graphics.h)
#endif

namespace fan {
	namespace graphics {

		#if defined(loco_opengl)
			using fan::opengl::context_t;
			using fan::opengl::matrices_t;
			using fan::opengl::viewport_t;
			using fan::opengl::viewport_list_NodeReference_t;
			using fan::opengl::matrices_list_NodeReference_t;
			using fan::opengl::open_matrices;
			using fan::opengl::cid_t;
			using fan::opengl::shader_t;
			using fan::opengl::textureid_t;
		#elif defined(loco_vulkan)
			using fan::vulkan::context_t;
			using fan::vulkan::matrices_t;
			using fan::vulkan::viewport_t;
			using fan::vulkan::viewport_list_NodeReference_t;
			using fan::vulkan::matrices_list_NodeReference_t;
			using fan::vulkan::open_matrices;
			using fan::vulkan::cid_t;
			using fan::vulkan::shader_t;
		#endif

		namespace core {
			#if defined(loco_opengl)
				using fan::opengl::core::uniform_write_queue_t;
				using fan::opengl::core::uniform_block_t;
			#elif defined(loco_vulkan)
				using fan::vulkan::core::uniform_write_queue_t;
				using fan::vulkan::core::uniform_block_t;
			#endif
		}

	}
}