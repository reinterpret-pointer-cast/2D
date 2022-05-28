#pragma once

#include <fan/types/vector.h>

namespace fan_2d {
  namespace graphics {
    namespace lighting {
      struct light_t {

        enum class light_e {
          light_added,
          light_removed
        };

        enum class light_update_e {
          position,
          radius,
          intensity,
          ambient_strength,
          color,
          rotation_point,
          angle
        };

        typedef void(*light_cb_t)(light_t*, light_e, uint32_t node_reference);
        typedef void(*light_update_cb_t)(light_t*, light_update_e, uint32_t node_reference);

        void open() {
          set_light_cb([](light_t*, light_e, uint32_t) {});
          set_light_update_cb([](light_t*, light_update_e, uint32_t) {});
          m_instance.open();
        }
        void close() {
          m_instance.close();
        }

        void set_light_cb(light_cb_t light_cb)
        {
          m_light_cb = light_cb;
        }
        void set_light_update_cb(light_update_cb_t light_update_cb) {
          m_light_update_cb = light_update_cb;
        }

        struct properties_t {
          fan::vec2 position = 0;
          f32_t radius = 100;
          f32_t intensity = 1;
          f32_t ambient_strength = 0.1;
          fan::color color = fan::colors::white;
          fan::vec2 rotation_point = fan::vec2(0, 0);
          f32_t angle = 0;
          uint8_t type = 0;
        };

        uint32_t push_back(const properties_t& p) {
          uint32_t node_reference = m_instance.push_back(p);
          m_light_cb(this, light_e::light_added, node_reference);
          return node_reference;
        }
        void erase(uint32_t node_reference) {
          m_light_cb(this, light_e::light_removed, node_reference);
          m_instance.erase(node_reference);
        }

        fan::vec2 get_position(uint32_t i) const {
          return m_instance[i].position;
        }
        void set_position(uint32_t i, const fan::vec2& position) {
          m_instance[i].position = position;
          m_light_update_cb(this, light_update_e::position, i);
        }

        f32_t get_radius(uint32_t i) const {
          return m_instance[i].radius;
        }
        void set_radius(uint32_t i, f32_t radius) {
          m_instance[i].radius = radius;
          m_light_update_cb(this, light_update_e::radius, i);
        }

        f32_t get_intensity(uint32_t i) const {
          return m_instance[i].intensity;
        }
        void set_intensity(uint32_t i, f32_t intensity) {
          m_instance[i].intensity = intensity;
          m_light_update_cb(this, light_update_e::intensity, i);
        }

        f32_t get_ambient_strength(uint32_t i) const {
          return m_instance[i].ambient_strength;
        }
        void set_ambient_strength(uint32_t i, f32_t ambient_strength) {
          m_instance[i].ambient_strength = ambient_strength;
          m_light_update_cb(this, light_update_e::ambient_strength, i);
        }

        fan::vec3 get_color(uint32_t i) const {
          return fan::vec3(m_instance[i].color.r, m_instance[i].color.g, m_instance[i].color.b);
        }
        void set_color(uint32_t i, const fan::color& color) {
          m_instance[i].color = color;
          m_light_update_cb(this, light_update_e::color, i);
        }

        fan::vec2 get_rotation_point(uint32_t i) const {
          return m_instance[i].rotation_point;
        }
        void set_rotation_point(uint32_t i, const fan::vec2& rotation_point) {
          m_instance[i].rotation_point = rotation_point;
          m_light_update_cb(this, light_update_e::rotation_point, i);
        }

        f32_t get_angle(uint32_t i) const {
          return m_instance[i].angle;
        }
        void set_angle(uint32_t i, f32_t angle) {
          m_instance[i].angle = angle;
          m_light_update_cb(this, light_update_e::angle, i);
        }

        uint8_t get_type(uint32_t i) const {
          return m_instance[i].type;
        }
        void set_type(uint32_t i, uint8_t type) {
          m_instance[i].type = type;
        }

        uint32_t size() const {
          return m_instance.size();
        }

        fan::vec4 calculate_lighting(
          const uint8_t type,
          const fan::vec2& light_position,
          const fan::vec3& light_color,
          const f32_t intensity,
          const f32_t radius,
          const fan::vec2& fragment_position
        )
        {
          switch (type) {
            case 0: {
              f32_t dscore = radius - (light_position - fragment_position).length();
              dscore = std::max(dscore, 0.f) / radius;
              fan::vec4 r;
              *(fan::vec3*)&r = light_color * dscore;

              return r;
            }
            case 1: {
              f32_t dscore = radius - (light_position - fragment_position).length();
              dscore = std::max(dscore, 0.f) / radius;
              if (fragment_position.y > 0) {
                //dscore /= fragment_position.y;
                f32_t d = fragment_position.y / (64 * 4);
                d = 1.0f - d;
                d = std::max(d, 0.0f);
                dscore = d;
              }
              fan::vec4 r;
              *(fan::vec3*)&r = light_color * dscore;

              return r;
            }
          }
          return 0;
        }

        void calculate_light_map(
          uint8_t* map,
          fan::vec2 camera_position,
          fan::vec2 matrix_size,
          const fan::vec2ui& r
        ) {
          camera_position -= matrix_size;
          matrix_size *= 2;
          for (uint32_t y = 0; y < r.y; y++) {
            for (uint32_t x = 0; x < r.x; x++) {
              fan::vec4 sum(0, 0, 0, 1);
              uint32_t it = m_instance.begin();
              while (it != m_instance.end()) {
                fan::vec4 ret = calculate_lighting(
                 light_t::get_type(it),
                 light_t::get_position(it),
                 light_t::get_color(it),
                 light_t::get_intensity(it),
                 light_t::get_radius(it),
                 (camera_position)+(matrix_size / r) * fan::vec2(x, y)
                );

                *(fan::vec3*)&sum += *(fan::vec3*)&ret;
                sum.w = std::min(sum.w, ret.w);
                it = m_instance.next(it);
              }

              *(fan::vec3*)&sum = *(fan::vec3*)&sum / std::max(fan::vec3(sum).max(), 1.0f);

              map[(y * r.x + x) * 4 + 0] = sum.x * 0xff;
              map[(y * r.x + x) * 4 + 1] = sum.y * 0xff;
              map[(y * r.x + x) * 4 + 2] = sum.z * 0xff;
              map[(y * r.x + x) * 4 + 3] = sum.w * 0xff;
              //fan::print((f32_t)map[y * r.x + x]);
            }
          }
        }


        bll_t<properties_t> m_instance;

      protected:

        light_cb_t m_light_cb;
        light_update_cb_t m_light_update_cb;
      };
    }
  }
}