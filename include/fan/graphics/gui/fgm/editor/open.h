depth_map.open();
move_offset = 0;
properties_camera = 0;
flags = 0;

original_position.resize(original_position.size() + 1);

pile->editor.selected_type = fan::uninitialized;

builder_viewport_src = constants::builder_viewport_src;
builder_viewport_dst = constants::builder_viewport_dst;
origin_shapes = fan::vec2(builder_viewport_src.x, 0);
origin_properties = fan::vec2(builder_viewport_src.x, 0);

fan::vec2 window_size = pile->window.get_size();

fan::graphics::viewport_t::properties_t vp;

vp.size = fan::vec2(window_size.x - (window_size.x - window_size.x / 2 * origin_properties.x), window_size.y / 2);
vp.position = fan::vec2(window_size.x - window_size.x / 2 * origin_properties.x, 0);

properties_viewport.open(&pile->context);
properties_viewport.set(&pile->context, vp);
properties_viewport.enable(&pile->context);

properties_button.open(&pile->window, &pile->context);
properties_button.enable_draw(&pile->window, &pile->context);
properties_button.bind_matrices(&pile->context, &gui_properties_matrices);
properties_button.set_viewport_collision_offset(origin_properties);

properties_button_text.open(&pile->context);
properties_button_text.enable_draw(&pile->context);
properties_button_text.bind_matrices(&pile->context, &gui_properties_matrices);

vp.position = 0;
vp.size = fan::vec2(window_size.x, window_size.y);
builder_viewport.open(&pile->context);
builder_viewport.set(&pile->context, vp);
builder_viewport.enable(&pile->context);

resize_rectangles.open(&pile->window, &pile->context);
resize_rectangles.enable_draw(&pile->window, &pile->context);
resize_rectangles.bind_matrices(&pile->context, &gui_matrices);

outline.open(&pile->context);
decltype(outline)::properties_t line_p;
line_p.color = fan::colors::white;

line_p.src = constants::builder_viewport_src;
line_p.dst = constants::builder_viewport_dst;
outline.push_back(&pile->context, line_p);
outline.bind_matrices(&pile->context, &gui_matrices);

line_p.src = fan::vec2(constants::builder_viewport_dst.x, 0.5);
line_p.dst = fan::vec2(1, line_p.src.y);
outline.push_back(&pile->context, line_p);
outline.enable_draw(&pile->context);

// builder_viewport top right
fan::vec2 origin = fan::vec2(constants::builder_viewport_src.x, 0);

builder_types.open(&pile->window, &pile->context);
builder_types.enable_draw(&pile->window, &pile->context);

decltype(builder_types)::properties_t builder_types_p;
builder_types_p.font_size = constants::gui_size;
builder_types_p.size = fan::vec2(constants::gui_size * 4, constants::gui_size);
builder_types_p.position = fan::vec2(0.75, -0.8);
builder_types_p.text = "sprite";
builder_types_p.theme = fan_2d::graphics::gui::themes::gray();
builder_types_p.theme.button.outline_thickness = 0.005;
builder_types.push_back(&pile->window, &pile->context, builder_types_p);

original_position[0].push_back(builder_types_p.position);

builder_types_p.position.y += 0.1;
builder_types_p.text = "text";
builder_types.push_back(&pile->window, &pile->context, builder_types_p);
original_position[0].push_back(builder_types_p.position);

builder_types_p.position.y += 0.1;
builder_types_p.text = "hitbox";
builder_types.push_back(&pile->window, &pile->context, builder_types_p);
original_position[0].push_back(builder_types_p.position);

builder_types_p.position.y += 0.1;
builder_types_p.text = "button";
builder_types.push_back(&pile->window, &pile->context, builder_types_p);
original_position[0].push_back(builder_types_p.position);

fan::vec2 old_p = builder_types_p.position;

builder_types_p.position.x -= 0.1;
builder_types_p.position.y += 0.4;
builder_types_p.size.x /= 2;
builder_types_p.size.y /= 1.2;
builder_types_p.text = "export";
builder_types.push_back(&pile->window, &pile->context, builder_types_p);

original_position[0].push_back(builder_types_p.position);

builder_text.open(&pile->context);
builder_text.enable_draw(&pile->context);
builder_text.bind_matrices(&pile->context, &gui_matrices);

decltype(builder_text)::properties_t builder_text_p;

//builder_text_p.position = old_p;
//builder_text_p.position.y += 50;
//builder_text_p.text = "upload image \nto texturepack";
//builder_text_p.font_size = constants::gui_size;
//builder_text_p.text_color = fan::colors::white;
//
//builder_text.push_back(&pile->context, builder_text_p);
//
//builder_types_p.position = old_p;
//builder_types_p.position.y = builder_text_p.position.y + 50;
//
//builder_types_p.text = " ";
//builder_types.push_back(&pile->window, &pile->context, builder_types_p);
//builder_types.m_key_event.allow_input(&pile->window, builder_types.size(&pile->window, &pile->context) - 1, true);

builder_types.bind_matrices(&pile->context, &gui_matrices);