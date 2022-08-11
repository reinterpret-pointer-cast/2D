struct fgm_t {
  struct viewport_area{
    static constexpr uint32_t editor = 0;
    static constexpr uint32_t types = 1;
  };
  struct shapes {
    static constexpr uint32_t button = 0;
  };

  fan::vec2 editor_size;

  fan::vec2 translate_viewport_position(loco_t* loco, const fan::vec2& value) {
    fan::vec2 window_size = loco->get_window()->get_size();
    return (value + 1) / 2 * window_size;
  }

  void open(loco_t* loco) {

    selected_type = fan::uninitialized;
    theme = fan_2d::graphics::gui::themes::gray();
    theme.open(loco->get_context());

    loco->get_window()->add_resize_callback(this, [](fan::window_t* window, const fan::vec2i& ws, void* userptr) {
      /*fan::vec2 window_size = window->get_size();
      fan::vec2 ratio = window_size / window_size.max();
      std::swap(ratio.x, ratio.y);
      fgm_t* fgm = (fgm_t*)userptr;
      fgm->matrices.set_ortho(
        fan::vec2(-1, 1) * ratio.x, 
        fan::vec2(-1, 1) * ratio.y
      );
      pile_t* pile = OFFSETLESS(fgm, pile_t, fgm);
      fgm->viewport[0].set_viewport(pile->loco.get_context(), 0, fgm->get_viewport_size(&pile->loco));*/
    });

    // half size
    editor_size = fan::vec2(0.5, 1);

    fan::graphics::open_matrices(
      loco->get_context(),
      &matrices,
      loco->get_window()->get_size(),
      fan::vec2(-1, 1),
      fan::vec2(-1, 1)
    );
    viewport[viewport_area::editor].open(
      loco->get_context(), 
      0, 
      translate_viewport_position(loco, editor_size)
    );
    {
      fan::vec2 top_viewport = translate_viewport_position(loco, editor_size);
      fan::vec2 bottom_viewport = translate_viewport_position(loco, fan::vec2(-0.5, 0));
      top_viewport.y = bottom_viewport.y;
      viewport[viewport_area::types].open(
        loco->get_context(),
        top_viewport,
        bottom_viewport
      );
    }

    line.open(loco);
    editor_button.open(loco);
    builder_button.open(loco);

    line_t::properties_t lp;
    lp.viewport = &viewport[viewport_area::editor];
    lp.matrices = &matrices;
    lp.src = fan::vec2(-1.0 , -1.0);
    lp.dst = fan::vec2(+1.0, -1.0);
    lp.color = fan::colors::white;
    line.push_back(loco, lp);
    lp.src = lp.dst;
    lp.dst.y += +2.0;
    line.push_back(loco, lp);
    lp.src = fan::vec2(-1, 1);
    lp.dst = fan::vec2(1, 1);
    lp.viewport = &viewport[viewport_area::types];
    line.push_back(loco, lp);

    editor_button_t::properties_t ebp;
    ebp.matrices = &matrices;
    ebp.viewport = &viewport[viewport_area::types];
    ebp.position = fan::vec2(0, -0.8);
    ebp.size = fan::vec2(0.5, 0.1);
    ebp.theme = &theme;
    ebp.text = "button";
    ebp.font_size = 0.2;
    ebp.mouse_input_cb = [](const loco_t::mouse_input_data_t& ii_d) -> uint8_t {
      if (ii_d.key != fan::mouse_left) {
        return 1;
      }
      if (ii_d.key_state != fan::key_state::press) {
        return 1;
      }
      pile_t* pile = OFFSETLESS(ii_d.userptr, pile_t, fgm);
      builder_button_t::properties_t bbp;
      bbp.matrices = &pile->fgm.matrices;
      bbp.viewport = &pile->fgm.viewport[viewport_area::types];
      bbp.position = pile->loco.get_mouse_position(
        pile->fgm.viewport[viewport_area::types].get_viewport_position(),
        pile->fgm.viewport[viewport_area::types].get_viewport_size()
      );
      bbp.size = fan::vec2(0.5, 0.1);
      bbp.theme = &pile->fgm.theme;
      bbp.text = "button";
      bbp.font_size = 0.2;
      pile->fgm.selected_type = shapes::button;
      pile->fgm.selected_index = (void*)pile->fgm.builder_button.push_back(&pile->loco, bbp);
      return 1;
    };
    ebp.mouse_move_cb = [](const loco_t::mouse_move_data_t& ii_d) -> uint8_t {
      pile_t* pile = OFFSETLESS(ii_d.userptr, pile_t, fgm);
      switch(pile->fgm.selected_type) {
        case shapes::button: {
          pile->loco.button.set_button(
            &pile->loco,
            (fan::opengl::cid_t*)pile->fgm.selected_index,
            &loco_t::button_t::instance_t::position,
            pile->loco.get_mouse_position(
              pile->fgm.viewport[viewport_area::types].get_viewport_position(),
              pile->fgm.viewport[viewport_area::types].get_viewport_size()
            )
          );
          pile->loco.button.set_text(
            &pile->loco,
            (fan::opengl::cid_t*)pile->fgm.selected_index,
            &loco_t::letter_t::instance_t::position,
            pile->loco.get_mouse_position(
              pile->fgm.viewport[viewport_area::types].get_viewport_position(),
              pile->fgm.viewport[viewport_area::types].get_viewport_size()
            )
          );
          break;
        }
      }
      return 1;
    };
    ebp.userptr = this;
    editor_button.push_back(loco, ebp);
    //loco->button
  }
  void close(loco_t* loco) {
    line.close(loco);
    editor_button.close(loco);
    builder_button.close(loco);
  }

  struct line_t {
    using properties_t = loco_t::line_t::properties_t;

    void open(loco_t* loco) {
      cids.open();
    }
    void close(loco_t* loco) {
      cids.close();
    }
    void push_back(loco_t* loco, properties_t& p) {
      cids.resize(cids.size() + 1);
      loco->line.push_back(loco, &cids[cids.size() - 1], p);
    }
    fan::hector_t<fan::opengl::cid_t> cids;
  }line;

  struct editor_button_t {
    using properties_t = loco_t::button_t::properties_t;

    void open(loco_t* loco) {
      cids.open();
    }
    void close(loco_t* loco) {
      cids.close();
    }
    void push_back(loco_t* loco, properties_t& p) {
      cids.resize(cids.size() + 1);
      loco->button.push_back(loco, 0, &cids[cids.size() - 1], p);
    }
    fan::hector_t<fan::opengl::cid_t> cids;
  }editor_button;

  struct builder_button_t {
    using properties_t = loco_t::button_t::properties_t;

    void open(loco_t* loco) {
      cids.open();
    }
    void close(loco_t* loco) {
      cids.close();
    }
    fan::opengl::cid_t* push_back(loco_t* loco, properties_t& p) {
      cids.resize(cids.size() + 1);
      loco->button.push_back(loco, 0, &cids[cids.size() - 1], p);
      return &cids[cids.size() - 1];
    }
    fan::hector_t<fan::opengl::cid_t> cids;
  }builder_button;

  fan::opengl::matrices_t matrices;
  fan::opengl::viewport_t viewport[3];

  fan_2d::graphics::gui::theme_t theme;
  uint32_t selected_type;
  void* selected_index;
};