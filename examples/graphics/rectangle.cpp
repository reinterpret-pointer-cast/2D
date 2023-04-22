// Creates window, opengl context and renders a rectangle

#define _INCLUDE_TOKEN(p0, p1) <p0/p1>

#ifndef FAN_INCLUDE_PATH
  #define FAN_INCLUDE_PATH C:/libs/fan/include
#endif
#define fan_debug 0
#include _INCLUDE_TOKEN(FAN_INCLUDE_PATH, fan/types/types.h)
#define loco_window
#define loco_context

#define loco_rectangle
#include _FAN_PATH(graphics/loco.h)

struct pile_t {

  static constexpr fan::vec2 ortho_x = fan::vec2(-1, 1);
  static constexpr fan::vec2 ortho_y = fan::vec2(-1, 1);

  pile_t() {
    loco.open_camera(
      &camera,
      ortho_x,
      ortho_y
    );
    loco.get_window()->add_resize_callback([&](const fan::window_t::resize_cb_data_t& d) {
      viewport.set(loco.get_context(), 0, d.size, d.size);
    });
    viewport.open(loco.get_context());
    viewport.set(loco.get_context(), 0, loco.get_window()->get_size(), loco.get_window()->get_size());
  }

  loco_t loco;
  loco_t::camera_t camera;
  loco_t::viewport_t viewport;
};

int main() {
  pile_t* pile = new pile_t;

  loco_t::rectangle_t::properties_t p;
  p.camera = &pile->camera;
  p.viewport = &pile->viewport;

  p.size = fan::vec2(0.05);

  p.color = fan::colors::red;
  p.color.a = 1;

  p.position = fan::vec3(0, 0, 0);
  loco_t::shape_t r0 = p;
  p.position = fan::vec3(0.1, 0, 1);
  p.color = fan::colors::blue;
  p.color.a = 1;
  loco_t::shape_t r1 = p;

  pile->loco.set_vsync(false);
  
  pile->loco.loop([&] {

    r0.set_position(pile->loco.get_mouse_position(pile->camera, pile->viewport));

    pile->loco.get_fps();

  });

  return 0;
}