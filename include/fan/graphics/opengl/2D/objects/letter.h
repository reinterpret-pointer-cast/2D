struct letter_t {

  struct vi_t {
    loco_letter_vi_t
  };

  static constexpr uint32_t max_instance_size = fan::min(256ull, 4096 / (sizeof(vi_t) / 4));

  struct bm_properties_t {
    loco_letter_bm_properties_t
  };

  struct cid_t;

  struct ri_t : bm_properties_t {
    loco_letter_ri_t
  };

  struct properties_t : vi_t, ri_t {
    loco_letter_properties_t
  };

  void push_back(fan::graphics::cid_t* cid, properties_t& p) {

    get_key_value(uint16_t) = p.position.z;
    get_key_value(loco_t::camera_list_NodeReference_t) = p.camera;
    get_key_value(fan::graphics::viewport_list_NodeReference_t) = p.viewport;

    loco_t* loco = get_loco();
    fan::font::character_info_t si = loco->font.info.get_letter_info(p.letter_id, p.font_size);

    p.tc_position = si.glyph.position / loco->font.image.size;
    p.tc_size.x = si.glyph.size.x / loco->font.image.size.x;
    p.tc_size.y = si.glyph.size.y / loco->font.image.size.y;

    p.size = si.metrics.size / 2;

    #if defined(loco_vulkan)
      VkDescriptorImageInfo imageInfo{};
      imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      auto& img = loco->image_list[loco->font.image.texture_reference];
      imageInfo.imageView = img.image_view;
      imageInfo.sampler = img.sampler;
      if (img.texture_index.letter == (decltype(img.texture_index.letter))-1) {
        img.texture_index.letter = m_texture_index++;
        if (m_texture_index > fan::vulkan::max_textures) {
          fan::throw_error("too many textures max:" + fan::vulkan::max_textures);
        }
       m_ssbo.m_descriptor.m_properties[2].image_infos[img.texture_index.letter] = imageInfo;
        m_ssbo.m_descriptor.update(loco->get_context(), 1, 2);
      }

      auto& camera = loco->camera_list[p.camera];
      if (camera.camera_index.letter == (decltype(camera.camera_index.letter))-1) {
        camera.camera_index.letter = m_camera_index++;
        m_shader.set_camera(loco, camera.camera_id, camera.camera_index.letter);
      }
    #endif

    sb_push_back(cid, p);
  }
  void erase(fan::graphics::cid_t* cid) {
    sb_erase(cid);
  }

  void draw(bool blending = false) {
    sb_draw(root);
  }

   #if defined(loco_opengl)
    #define sb_shader_vertex_path _FAN_PATH(graphics/glsl/opengl/2D/objects/letter.vs)
    #define sb_shader_fragment_path _FAN_PATH(graphics/glsl/opengl/2D/objects/letter.fs)
  #elif defined(loco_vulkan)
    #define vulkan_buffer_count 5
    #define sb_shader_vertex_path graphics/glsl/vulkan/2D/objects/letter.vert
    #define sb_shader_fragment_path graphics/glsl/vulkan/2D/objects/letter.frag
  #endif

  #define vk_sb_ssbo
  #define vk_sb_vp
  #define vk_sb_image
  #include _FAN_PATH(graphics/shape_builder.h)

  letter_t() {
    sb_open();
  }
  ~letter_t() {
    sb_close();
  }

  void set_camera(fan::graphics::cid_t* cid, loco_t::camera_list_NodeReference_t n) {
    sb_set_key<bm_properties_t::key_t::get_index_with_type<decltype(n)>()>(cid, n);
  }

  void set_viewport(fan::graphics::cid_t* cid, fan::graphics::viewport_list_NodeReference_t n) {
    sb_set_key<bm_properties_t::key_t::get_index_with_type<decltype(n)>()>(cid, n);
  }

  #if defined(loco_vulkan)
    uint32_t m_texture_index = 0;
    uint32_t m_camera_index = 0;
  #endif
};