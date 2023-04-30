struct stage {

  struct stage0_t : stage_common_t_t<stage0_t> {
    using stage_common_t_t::stage_common_t_t;
    static constexpr auto stage_name = "stage0";
    #include _PATH_QUOTE(stage_loader_path/stages_compile/stage0.h)
  };

  struct stage1_t : stage_common_t_t<stage1_t> {
    using stage_common_t_t::stage_common_t_t;
    static constexpr auto stage_name = "stage1";
    #include _PATH_QUOTE(stage_loader_path/stages_compile/stage1.h)
  };

  using variant_t = std::variant<stage0_t*,stage1_t*>;
};