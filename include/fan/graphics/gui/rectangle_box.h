#pragma once

#include _FAN_PATH(graphics/opengl/gl_core.h)
#include _FAN_PATH(graphics/opengl/gl_shader.h)
#include _FAN_PATH(graphics/shared_graphics.h)
#include _FAN_PATH(physics/collision/rectangle.h)

#include _FAN_PATH(graphics/gui/themes.h)

namespace fan_2d {
  namespace graphics {
    namespace gui {
      struct rectangle_box_t {

        using draw_cb_t = void(*)(fan::opengl::context_t* context, rectangle_box_t*, void*);

        struct instance_t {
          fan::vec2 position = 0;
          fan::vec2 size = 0;
          fan::color color = fan::colors::white;
          fan::vec3 rotation_vector = fan::vec3(0, 0, 1);
          f32_t angle = 0;
          fan::color outline_color;
          fan::vec2 rotation_point = 0;
          f32_t outline_size;
          f32_t pad;
        };

        struct properties_t : instance_t {

        protected:
          using instance_t::color;
          using instance_t::outline_color;
          using instance_t::outline_size;
        public:

          fan_2d::graphics::gui::theme theme = fan_2d::graphics::gui::themes::deep_red();

          using type_t = rectangle_box_t;
        };

        static constexpr uint32_t max_instance_size = 256;

        struct id_t {
          id_t(fan::opengl::cid_t* cid) {
            block = cid->id / max_instance_size;
            instance = cid->id % max_instance_size;
          }
          uint32_t block;
          uint32_t instance;
        };

        void open(fan::opengl::context_t* context) {
          m_shader.open(context);

          m_shader.set_vertex(
            context,
#include _FAN_PATH(graphics/glsl/opengl/2D/objects/rectangle_box.vs)
          );

          m_shader.set_fragment(
            context,
#include _FAN_PATH(graphics/glsl/opengl/2D/objects/rectangle_box.fs)
          );

          m_shader.compile(context);

          blocks.open();

          m_draw_node_reference = fan::uninitialized;
          draw_cb = [](fan::opengl::context_t* context, rectangle_box_t*, void*) {};
        }
        void close(fan::opengl::context_t* context) {
          m_shader.close(context);
          for (uint32_t i = 0; i < blocks.size(); i++) {
            blocks[i].uniform_buffer.close(context);
          }
          blocks.close();
        }

        void push_back(fan::opengl::context_t* context, fan::opengl::cid_t* cid, const properties_t& p) {
          instance_t it = p;
          it.color = p.theme.button.color;
          it.outline_color = p.theme.button.outline_color;
          it.outline_size = p.theme.button.outline_size;
          uint32_t i = 0;

          for (; i < blocks.size(); i++) {
            if (blocks[i].uniform_buffer.size() != max_instance_size) {
              break;
            }
          }

          if (i == blocks.size()) {
            blocks.push_back({});
            blocks[i].uniform_buffer.open(context);
            blocks[i].uniform_buffer.bind_uniform_block(context, m_shader.id, "instance_t");
          }

          blocks[i].uniform_buffer.push_ram_instance(context, it);

          const uint32_t instance_id = blocks[i].uniform_buffer.size() - 1;

          blocks[i].cid[instance_id] = cid;

          blocks[i].uniform_buffer.common.edit(
            context,
            instance_id,
            instance_id + 1
          );

          cid->id = i * max_instance_size + instance_id;
        }
        void erase(fan::opengl::context_t* context, fan::opengl::cid_t* cid) {
          uint32_t id = cid->id;
          uint32_t block_id = id / max_instance_size;
          uint32_t instance_id = id % max_instance_size;

#if fan_debug >= fan_debug_medium
          if (block_id >= blocks.size()) {
            fan::throw_error("invalid access");
          }
          if (instance_id >= blocks[block_id].uniform_buffer.size()) {
            fan::throw_error("invalid access");
          }
#endif

          if (block_id == blocks.size() - 1 && instance_id == blocks.ge()->uniform_buffer.size() - 1) {
            blocks[block_id].uniform_buffer.common.m_size -= blocks[block_id].uniform_buffer.common.buffer_bytes_size;
            if (blocks[block_id].uniform_buffer.size() == 0) {
              blocks[block_id].uniform_buffer.close(context);
              blocks.m_size -= 1;
            }
            return;
          }

          uint32_t last_block_id = blocks.size() - 1;
          uint32_t last_instance_id = blocks[last_block_id].uniform_buffer.size() - 1;

          instance_t* last_instance_data = blocks[last_block_id].uniform_buffer.get_instance(context, last_instance_id);

          blocks[block_id].uniform_buffer.edit_ram_instance(
            context,
            instance_id,
            last_instance_data,
            0,
            sizeof(instance_t)
          );

          blocks[last_block_id].uniform_buffer.common.m_size -= blocks[last_block_id].uniform_buffer.common.buffer_bytes_size;

          blocks[block_id].cid[instance_id] = blocks[last_block_id].cid[last_instance_id];
          blocks[block_id].cid[instance_id]->id = block_id * max_instance_size + instance_id;

          if (blocks[last_block_id].uniform_buffer.size() == 0) {
            blocks[last_block_id].uniform_buffer.close(context);
            blocks.m_size -= 1;
          }
        }

        void enable_draw(fan::opengl::context_t* context) {
          this->draw(context);

#if fan_debug >= fan_debug_low
          if (m_draw_node_reference != fan::uninitialized) {
            fan::throw_error("trying to call enable_draw twice");
          }
#endif

          m_draw_node_reference = context->enable_draw(this, [](fan::opengl::context_t* c, void* d) { ((decltype(this))d)->draw(c); });
        }
        void disable_draw(fan::opengl::context_t* context) {
#if fan_debug >= fan_debug_low
          if (m_draw_node_reference == fan::uninitialized) {
            fan::throw_error("trying to disable unenabled draw call");
          }
#endif
          context->disable_draw(m_draw_node_reference);
        }

        void draw(fan::opengl::context_t* context) {
          m_shader.use(context);

          if (draw_cb) {
            draw_cb(context, this, draw_userdata);
          }

          for (uint32_t i = 0; i < blocks.size(); i++) {
            blocks[i].uniform_buffer.bind_buffer_range(context, blocks[i].uniform_buffer.size());

            blocks[i].uniform_buffer.draw(
              context,
              0,
              blocks[i].uniform_buffer.size() * 6
            );
          }
        }

        void bind_matrices(fan::opengl::context_t* context, fan::opengl::matrices_t* matrices) {
          m_shader.bind_matrices(context, matrices);
        }

        template <typename T>
        T get(fan::opengl::context_t* context, const id_t& id, T instance_t::* member) {
          return blocks[id.block].uniform_buffer.get_instance(context, id.instance)->*member;
        }
        template <typename T>
        void set(fan::opengl::context_t* context, const id_t& id, T instance_t::* member, const T& value) {
          blocks[id.block].uniform_buffer.edit_ram_instance(context, id.instance, &value, fan::ofof<instance_t, T>(member), sizeof(T));
        }

        void set_vertex(fan::opengl::context_t* context, const std::string& str) {
          m_shader.set_vertex(context, str);
        }
        void set_fragment(fan::opengl::context_t* context, const std::string& str) {
          m_shader.set_fragment(context, str);
        }
        void compile(fan::opengl::context_t* context) {
          m_shader.compile(context);
        }

        void set_draw_cb(fan::opengl::context_t* context, draw_cb_t draw_cb_, void* userptr = 0) {
          draw_cb = draw_cb_;
          if (userptr != nullptr) {
            draw_userdata = userptr;
          }
        }
        void set_draw_cb_userptr(fan::opengl::context_t* context, void* userptr) {
          draw_userdata = userptr;
        }

        fan::shader_t m_shader;

        struct block_t {
          fan::opengl::core::uniform_block_t<instance_t, max_instance_size> uniform_buffer;
          fan::opengl::cid_t* cid[max_instance_size];
        };
        uint32_t m_draw_node_reference;

        fan::hector_t<block_t> blocks;

        draw_cb_t draw_cb;
        void* draw_userdata;
      };
    }
  }
}