#pragma once

#include _FAN_PATH(types/types.h)

#include _FAN_PATH(types/color.h)

#include _FAN_PATH(graphics/gui/button_event.h)
#include _FAN_PATH(graphics/gui/types.h)

namespace fan_2d {
	namespace graphics {
		namespace gui {

			struct theme {

				theme() = default;

				struct button {

					button() = default;

					typedef void(*click_callback_t)(fan::window_t *window, uint32_t index, uint16_t key, fan::key_state key_state, mouse_stage mouse_stage, void* user_ptr);
					typedef void(*hover_callback_t)(fan::window_t *window, uint32_t index, mouse_stage mouse_stage, void* user_ptr);

					click_callback_t m_click_callback = 0;
					hover_callback_t m_hover_callback = 0;

					enum class states_e {
						outside,
						hover,
						click
					};

					fan::color color;
					fan::color outline_color;
					fan::color text_color;
					fan::color text_outline_color;
					f32_t text_outline_size;

					fan::color hover_color;
					fan::color hover_outline_color;

					fan::color click_color;
					fan::color click_outline_color;

					f32_t outline_thickness;

				}button;

				struct checkbox {

					fan::color color;
					fan::color text_color;

					fan::color hover_color;

					fan::color click_color;

					fan::color check_color;

				}checkbox;

			};

			using theme_ptr_t = fan::ptr_maker_t<theme>;

			namespace themes {

				struct empty : public fan_2d::graphics::gui::theme {

					empty() {

						button.color = fan::color(0, 0, 0);
						button.outline_color = fan::color(0, 0, 0);
						button.text_color = fan_2d::graphics::gui::defaults::text_color;
						button.text_outline_color = fan::colors::black;
						button.text_outline_size = fan_2d::graphics::gui::defaults::text_renderer_outline_size;
						button.outline_thickness = 0; // px

						button.hover_color = button.color + 0.1;
						button.hover_outline_color = button.outline_color + 0.1;

						button.click_color = button.hover_color + 0.05;
						button.click_outline_color = button.click_color + 0.05;

						checkbox.color = button.color;
						checkbox.text_color = button.text_color;
						checkbox.hover_color = button.hover_color;
						checkbox.click_color = button.click_color;
						checkbox.check_color = fan::color(1, 1, 1, 0);

					}

				};	

				struct deep_blue : public fan_2d::graphics::gui::theme {

					deep_blue(f32_t intensity = 1) {

						button.color = fan::color(0, 0, 0.3) * intensity;
						button.outline_color = fan::color(0, 0, 0.5) * intensity;
						button.text_color = fan_2d::graphics::gui::defaults::text_color;
						button.text_outline_color = fan::colors::black * intensity;
						button.text_outline_size = fan_2d::graphics::gui::defaults::text_renderer_outline_size * intensity;
						button.outline_thickness = 2; // px
						
						button.hover_color = (button.color + 0.1) * intensity;
						button.hover_outline_color = (button.outline_color + 0.1) * intensity;
						
						button.click_color = (button.hover_color + 0.05) * intensity;
						button.click_outline_color = (button.click_color + 0.05) * intensity;

						checkbox.color = button.color * intensity;
						checkbox.text_color = button.text_color * intensity;
						checkbox.hover_color = button.hover_color * intensity;
						checkbox.click_color = button.click_color * intensity;
						checkbox.check_color = fan::color(1, 1, 1) * intensity;

					}

				};	

				struct deep_red : public fan_2d::graphics::gui::theme {

					deep_red() {

						button.color = fan::color(0.3, 0, 0);
						button.outline_color = fan::color(0.5, 0, 0);
						button.text_color = fan_2d::graphics::gui::defaults::text_color;
						button.text_outline_color = fan::colors::black;
						button.text_outline_size = fan_2d::graphics::gui::defaults::text_renderer_outline_size;
						button.outline_thickness = 2; // px

						button.hover_color = button.color + 0.1;
						button.hover_outline_color = button.outline_color + 0.1;

						button.click_color = button.hover_color + 0.05;
						button.click_outline_color = button.click_color + 0.05;

						checkbox.color = button.color;
						checkbox.text_color = button.text_color;
						checkbox.hover_color = button.hover_color;
						checkbox.click_color = button.click_color;
						checkbox.check_color = fan::color(0.2, 0, 0);

					}

				};

				struct white : public fan_2d::graphics::gui::theme {

					white() {

						button.color = fan::color(0.8, 0.8, 0.8);
						button.outline_color = fan::color(0.9, 0.9, 0.9);
						button.text_color = fan_2d::graphics::gui::defaults::text_color;
						button.text_outline_color = fan::colors::black;
						button.text_outline_size = fan_2d::graphics::gui::defaults::text_renderer_outline_size;
						button.outline_thickness = 2; // px

						button.hover_color = button.color + 0.1;
						button.hover_outline_color = button.outline_color + 0.1;

						button.click_color = button.hover_color + 0.05;
						button.click_outline_color = button.click_color + 0.05;

						checkbox.color = button.color;
						checkbox.text_color = button.text_color;
						checkbox.hover_color = button.hover_color;
						checkbox.click_color = button.click_color;
						checkbox.check_color = fan::color(0.5, 0, 0);

					}

				};	

				struct locked : public fan_2d::graphics::gui::theme {

					locked() {

						button.color = fan::color(0.2, 0.2, 0.2, 0.8);
						button.outline_color = fan::color(0.3, 0.3, 0.3, 0.8);
						button.text_color = fan_2d::graphics::gui::defaults::text_color;
						button.text_outline_color = fan::colors::black;
						button.text_outline_size = fan_2d::graphics::gui::defaults::text_renderer_outline_size;
						button.outline_thickness = 2; // px

						button.hover_color = button.color + 0.1;
						button.hover_outline_color = button.outline_color + 0.1;

						button.click_color = button.hover_color + 0.05;
						button.click_outline_color = button.click_color + 0.05;

						checkbox.color = button.color;
						checkbox.text_color = button.text_color;
						checkbox.hover_color = button.hover_color;
						checkbox.click_color = button.click_color;
						checkbox.check_color = fan::color(0.5, 0, 0);

					}

				};	

				struct hidden : public fan_2d::graphics::gui::theme {

					hidden() {

						button.color = fan::color(0.0, 0.0, 0.0, 0.3);
						button.outline_color = fan::color(0.0, 0.0, 0.0, 0.3);
						button.text_color = fan_2d::graphics::gui::defaults::text_color;
						button.text_outline_color = fan::colors::black;
						button.text_outline_size = fan_2d::graphics::gui::defaults::text_renderer_outline_size;
						button.outline_thickness = 2; // px

						button.hover_color = fan::color(0.0, 0.0, 0.0, 0.15);
						button.hover_outline_color = fan::color(0.0, 0.0, 0.0, 0.15);

						button.click_color = fan::color(0.0, 0.0, 0.0, 0.0);
						button.click_outline_color = button.click_color + 0.0;

						checkbox.color = button.color;
						checkbox.text_color = button.text_color;
						checkbox.hover_color = button.hover_color;
						checkbox.click_color = button.click_color;
						checkbox.check_color = fan::color(0.0, 0.0, 0.0, 0.0);

					}

				};

				struct transparent : public fan_2d::graphics::gui::theme {

					transparent(f32_t intensity = 0.3) {

						button.color = fan::color(intensity, intensity, intensity, intensity);
						button.outline_color = fan::color(intensity + 0.2, intensity + 0.2, intensity + 0.2, intensity + 0.2);

						button.hover_color = button.color + 0.1;
						button.hover_outline_color = button.outline_color + 0.1;

						button.click_color = button.hover_color + 0.1;
						button.click_outline_color = button.hover_outline_color + 0.1;

						button.text_color = fan::colors::white;
						button.text_outline_color = fan::colors::black;
						button.text_outline_size = fan_2d::graphics::gui::defaults::text_renderer_outline_size;

						checkbox.color = button.color;
						checkbox.text_color = button.text_color;
						checkbox.hover_color = button.hover_color;
						checkbox.click_color = button.click_color;
						checkbox.check_color = button.click_color + intensity * 2;

					}

				};

				struct gray : public fan_2d::graphics::gui::theme {
					gray(f32_t intensity = 1) {

						button.color = fan::color(0.2, 0.2, 0.2, 1) * intensity;
						button.outline_color = fan::color(0.3, 0.3, 0.3, 1) * intensity;
						button.hover_color = fan::color(0.3, 0.3, 0.3, 1) * intensity;
						button.hover_outline_color = fan::color(0.4, 0.4, 0.4, 1) * intensity;
						button.click_color = fan::color(0.5, 0.5, 0.5, 1) * intensity;
						button.click_outline_color = fan::color(0.6, 0.6, 0.6, 1) * intensity;

						button.text_color = fan_2d::graphics::gui::defaults::text_color;
						button.text_outline_color = fan::colors::black;
						button.text_outline_size = fan_2d::graphics::gui::defaults::text_renderer_outline_size;
						button.outline_thickness = 2; // px

					}
				};

				struct custom : public fan_2d::graphics::gui::theme {

					struct properties_t {

						fan::color color;
						fan::color outline_color;
						fan::color hover_color;
						fan::color hover_outline_color;
						fan::color click_color;
						fan::color click_outline_color;
					};

					custom(const properties_t& properties) {

						button.color = properties.color;
						button.outline_color = properties.outline_color;
						button.text_color = fan_2d::graphics::gui::defaults::text_color;
						button.text_outline_color = fan::colors::black;
						button.text_outline_size = fan_2d::graphics::gui::defaults::text_renderer_outline_size;
						button.outline_thickness = 2; // px

						button.hover_color = properties.hover_color;
						button.hover_outline_color = properties.outline_color;

						button.click_color = properties.click_color;
						button.click_outline_color = properties.click_outline_color;

						checkbox.color = button.color;
						checkbox.text_color = button.text_color;
						checkbox.hover_color = button.hover_color;
						checkbox.click_color = button.click_color;
						checkbox.check_color = fan::color(0.0, 0.0, 0.0, 0.0);

					}
				};
			}
		}
	}
}