R"(
#version 330

in vec2 texture_coordinate;

in vec4 instance_color;

out vec4 o_color;

uniform sampler2D _t00;

void main() {
  o_color = vec4(1, 1, 1, 1);
  //if (o_color.a < 0.9) {
  //  discard;
  //}
}
)"