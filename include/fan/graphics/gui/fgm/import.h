#include _FAN_PATH(graphics/gui/gui.h)
#include _FAN_PATH(graphics/gui/be.h)

namespace fan_2d {
  namespace graphics {
    namespace gui {
      namespace fgm {

        struct load_t {

          typedef void(*on_input_cb)(const std::string& id, be_t*, uint32_t index, uint16_t key, fan::key_state key_state, fan_2d::graphics::gui::mouse_stage mouse_stage);
          typedef void(*on_mouse_move_cb)(const std::string& id, be_t*, uint32_t index, mouse_stage mouse_stage);

          void open(fan::window_t* window, fan::opengl::context_t* context_) {

            on_input_function = [](const std::string& id, be_t*, uint32_t index, uint16_t key, fan::key_state key_state, fan_2d::graphics::gui::mouse_stage mouse_stage) {};
            on_mouse_event_function = [](const std::string& id, be_t*, uint32_t index, mouse_stage mouse_stage) {};

            context = context_;
            sprite.open(context);
            tr.open(context);

            button_ids = new std::vector<std::string>();

            fan::vec2 window_size = window->get_size();

            matrices.open();
            matrices.set_ortho(context, fan::vec2(0, window_size.x), fan::vec2(0, window_size.y));

            sprite.bind_matrices(context, &matrices);
            tr.bind_matrices(context, &matrices);

            window->add_resize_callback(this, [](fan::window_t* window, const fan::vec2i& size, void* userptr) {
              load_t* pile = (load_t*)userptr;

              pile->context->set_viewport(0, size);

              fan::vec2 window_size = window->get_size();
              pile->matrices.set_ortho(pile->context, fan::vec2(0, window_size.x), fan::vec2(0, window_size.y));
            });
            be.open();
          }
          void close(fan::window_t* window, fan::opengl::context_t* context) {
            sprite.close(context);
            tr.close(context);
            be.close();
            matrices.close();
            delete button_ids;
          }

          void load(fan::window_t* window, fan::opengl::context_t* context, const char* path, fan::opengl::texturepack* tp) {
            FILE* f = fopen(path, "r+b");
            if (!f) {
              fan::throw_error("failed to open file stream");
            }

            sprite.write_in_texturepack(context, f, tp, 0);
            tr.write_in(context, f);
            be.write_in(context, f);

            uint32_t count;
            fread(&count, sizeof(count), 1, f);
            button_ids->resize(count);
            for (uint32_t i = 0; i < count; i++) {
              uint32_t s;
              fread(&s, sizeof(s), 1, f);
              (*button_ids)[i].resize(s);
              fread((*button_ids)[i].data(), s, 1, f);
            }

            fclose(f);
            be.set_userptr(this);
            be.set_on_input([](fan_2d::graphics::gui::be_t* be, uint32_t index, uint16_t key, fan::key_state 
              key_state, fan_2d::graphics::gui::mouse_stage mouse_stage) {
              load_t* load = (load_t*)be->get_userptr();
              load->on_input_function((*load->button_ids)[index - 1], be, index, key, key_state, mouse_stage);
            });
            be.set_on_mouse_event([](be_t* be, uint32_t index, mouse_stage mouse_stage) {
              load_t* load = (load_t*)be->get_userptr();
              load->on_mouse_event_function((*load->button_ids)[index - 1], be, index, mouse_stage);
            });
            be.bind_to_window(window);
          }

          void* get_userptr() {
            return userptr;
          }
          void set_userptr(void* userptr_) {
            userptr = userptr_;
          }

          void set_on_input(on_input_cb function) {
            on_input_function = function;
          }

          void set_on_mouse_event(on_mouse_move_cb function) {
            on_mouse_event_function = function;
          }

          void enable_draw(fan::window_t* window, fan::opengl::context_t* context) {
            sprite.enable_draw(context);
            tr.enable_draw(context);
          }

          fan_2d::graphics::sprite_t sprite;
          fan_2d::graphics::gui::text_renderer_t tr;

          fan::opengl::matrices_t matrices;
          fan::opengl::context_t* context;
          fan_2d::graphics::gui::be_t be;
          std::vector<std::string>* button_ids;

          on_input_cb on_input_function;
          on_mouse_move_cb on_mouse_event_function;
          
          void* userptr;
        };
      }
    }
  }
}