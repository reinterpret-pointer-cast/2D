case builder_draw_type_t::hitbox: {
  switch (key_state) {
    case fan::key_state::press: {
      pile->editor.flags |= flags_t::moving;
      pile->editor.click_position = pile->window.get_mouse_position();

      fan_2d::graphics::sprite_t::properties_t sprite_p;
      sprite_p.position = pile->window.get_mouse_position();
      sprite_p.size = pile->editor.builder_types.get_size(&pile->window, &pile->context, 0);
      fan::color colors[9];
      colors[0] = fan::colors::gray - fan::color(0, 0, 0, 0.1);
      colors[1] = fan::colors::gray - fan::color(0, 0, 0, 0.1);
      colors[2] = fan::colors::gray - fan::color(0, 0, 0, 0.1);
      colors[3] = fan::colors::gray - fan::color(0, 0, 0, 0.1);
      colors[4] = fan::colors::transparent;
      colors[5] = fan::colors::gray - fan::color(0, 0, 0, 0.1);
      colors[6] = fan::colors::gray - fan::color(0, 0, 0, 0.1);
      colors[7] = fan::colors::gray - fan::color(0, 0, 0, 0.1);
      colors[8] = fan::colors::gray - fan::color(0, 0, 0, 0.1);
      sprite_p.image.load(&pile->context, colors, fan::vec2ui(3, 3));
      pile->builder.hitbox.push_back(&pile->context, sprite_p);

      pile->editor.depth_map_push(pile, builder_draw_type_t::hitbox, pile->builder.hitbox.size(&pile->context) - 1);

      pile->editor.builder_draw_type = editor_t::builder_draw_type_t::hitbox;
      pile->editor.builder_draw_type_index = pile->builder.hitbox.size(&pile->context) - 1;
      pile->editor.selected_type = editor_t::builder_draw_type_t::hitbox;
      pile->editor.selected_type_index = pile->editor.builder_draw_type_index;

      break;
    }
    case fan::key_state::release: {
      pile->editor.flags &= ~flags_t::moving;

      switch (pile->editor.builder_draw_type) {
        case builder_draw_type_t::hitbox: {

          // if object is not within builder_viewport we will delete it
          if (!pile->editor.is_inside_builder_viewport(
            pile, 
            pile->builder.hitbox.get_position(
              &pile->context,
              pile->editor.builder_draw_type_index
            ) +
            pile->builder.hitbox.get_size(
              &pile->context,
              pile->editor.builder_draw_type_index
            )
          ))
          {
            #include "erase_active.h"
          }
          else {
            pile->editor.close_build_properties(pile);
            click_collision_t click_collision;
            click_collision.builder_draw_type = pile->editor.builder_draw_type;
            click_collision.builder_draw_type_index = pile->editor.builder_draw_type_index;
            pile->editor.open_build_properties(pile, click_collision);
          }
          break;
        }
      }
      break;
    }
  }
  break;
}