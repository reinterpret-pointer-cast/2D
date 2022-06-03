R"(
#version 330

in vec2 texture_coordinate;

in vec4 i_color;
in vec2 fragment_position;

out vec4 o_color;

uniform sampler2D texture_sampler;

void main() {

  vec2 flipped_y = vec2(texture_coordinate.x, texture_coordinate.y);

  vec4 texture_color = texture(texture_sampler, flipped_y);

  o_color = texture_color * i_color;
}
)"