struct rectangle_t {

  struct instance_t {
    fan::vec3 position = 0;
  private:
    f32_t pad[1];
  public:
    fan::vec2 size = 0;
    fan::vec2 rotation_point = 0;
    fan::color color = fan::colors::white;
    fan::vec3 rotation_vector = fan::vec3(0, 0, 1);
    f32_t angle = 0;
  };

  #define hardcode0_t fan::graphics::matrices_list_NodeReference_t
  #define hardcode0_n matrices
  #define hardcode1_t fan::graphics::viewport_list_NodeReference_t
  #define hardcode1_n viewport
  #include _FAN_PATH(graphics/opengl/2D/objects/hardcode_open.h)

  struct instance_properties_t {
    struct key_t : parsed_masterpiece_t {}key;
    expand_get_functions
  };
      
  struct properties_t : instance_t, instance_properties_t {
    properties_t() = default;
    properties_t(const instance_t& i) : instance_t(i) {}
    properties_t(const instance_properties_t& p) : instance_properties_t(p) {}
  };
  
  void push_back(fan::graphics::cid_t* cid, properties_t& p) {
    sb_push_back(cid, p);
  }
  void erase(fan::graphics::cid_t* cid) {
    sb_erase(cid);
  }

  void draw() {
    sb_draw();
  }

  static constexpr uint32_t max_instance_size = fan::min(256, 4096 / (sizeof(instance_t) / 4));
  #if defined(loco_opengl)
    #define sb_shader_vertex_path _FAN_PATH(graphics/glsl/opengl/2D/objects/rectangle.vs)
    #define sb_shader_fragment_path _FAN_PATH(graphics/glsl/opengl/2D/objects/rectangle.fs)
  #elif defined(loco_vulkan)
    #define vulkan_buffer_count 2
    #define sb_shader_vertex_path _FAN_PATH_QUOTE(graphics/glsl/vulkan/2D/objects/rectangle.vert.spv)
    #define sb_shader_fragment_path _FAN_PATH_QUOTE(graphics/glsl/vulkan/2D/objects/rectangle.frag.spv)
  #endif
  #include _FAN_PATH(graphics/shape_builder.h)

  rectangle_t() {
    #if defined(loco_opengl)
      sb_open();
    #elif defined(loco_vulkan)
      std::array<fan::vulkan::write_descriptor_set_t, vulkan_buffer_count> ds_properties;

      ds_properties[0].binding = 0;
      ds_properties[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
      ds_properties[0].flags = VK_SHADER_STAGE_VERTEX_BIT;
      ds_properties[0].range = VK_WHOLE_SIZE;
      ds_properties[0].common = &m_ssbo.common;
      ds_properties[0].dst_binding = 0;

      ds_properties[1].binding = 1;
      ds_properties[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      ds_properties[1].flags = VK_SHADER_STAGE_VERTEX_BIT;
      ds_properties[1].common = &m_shader.projection_view_block.common;
      ds_properties[1].range = sizeof(fan::mat4) * 2;
      ds_properties[1].dst_binding = 1;

      sb_open(ds_properties);
    #endif
  }
  ~rectangle_t() {
    sb_close();
  }

  void set_matrices(fan::graphics::cid_t* cid, fan::graphics::matrices_list_NodeReference_t n) {
    sb_set_key<instance_properties_t::key_t::get_index_with_type<decltype(n)>()>(cid, n);
  }

  void set_viewport(fan::graphics::cid_t* cid, fan::graphics::viewport_list_NodeReference_t n) {
    sb_set_key<instance_properties_t::key_t::get_index_with_type<decltype(n)>()>(cid, n);
  }
};

#include _FAN_PATH(graphics/opengl/2D/objects/hardcode_close.h)
#undef vulkan_buffer_count