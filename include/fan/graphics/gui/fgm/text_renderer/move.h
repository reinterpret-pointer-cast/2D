case builder_draw_type_t::text_renderer: {
  pile->builder.tr.set_position(
    &pile->context,
    pile->editor.selected_type_index,
    position + pile->editor.move_offset
  );

  break;
}