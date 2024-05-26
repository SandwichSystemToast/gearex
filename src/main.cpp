#include <spdlog/spdlog.h>

#define  STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include "engine/assets/assets.hpp"
#include "engine/engine.hpp"
#include "engine/renderer/geometry.hpp"
#include "engine/renderer/mesh.hpp"
#include "engine/renderer/renderer.hpp"

using namespace engine;
using namespace engine::renderer;
using namespace engine::assets;

int main(int argc, char *argv[]) {
  auto asset_manager = AssetManager();
  asset_manager.register_asset_type<ImageAsset>();
  asset_manager.register_asset_type<ShaderSourceAsset>();

  asset_manager.load_from_file("./assets.tar.gz");

  auto engine = Engine();
  auto renderer = Renderer();
  renderer.setup_opengl_debug();

  auto cat_texture = asset_manager.get_asset<ImageAsset>("assets/cat"_hs);
  gl texture =
      renderer.make_texture(cat_texture->raw.data(), cat_texture->dimensions.x,
                            cat_texture->dimensions.y);

  auto vertex_shader =
      asset_manager.get_asset<ShaderSourceAsset>("assets/shaders/vertex"_hs);
  auto fragment_shader =
      asset_manager.get_asset<ShaderSourceAsset>("assets/shaders/fragment"_hs);

  gl shader =
      renderer.make_program(vertex_shader->source.c_str(),
                            fragment_shader->source.c_str(), "Demo Shader");

  v3 vertex_positions[4] = {{0.5f, 0.5f, 0.0f},
                            {0.5f, -0.5f, 0.0f},
                            {-0.5f, -0.5f, 0.0f},
                            {-0.5f, 0.5f, 0.0f}};
  v3 colors[4] = {{1., 0., 0.}, {0., 1., 0.}, {0., 0., 1.}, {1., 1., 1.}};
  v2 texture_coordinates[4] = {{0., 0.}, {0., 1.}, {1., 1.}, {1., 0.}};

  u32 indices[6] = {0, 1, 3, 1, 2, 3};

  auto geometry_builder = GeometryBuilder<v3, v3, v2>();

  // TODO: optimize this
  for (z i = 0; i < 4; i++)
    geometry_builder.add_vertex(vertex_positions[i], colors[i],
                                texture_coordinates[i]);

  auto geometry =
      geometry_builder.stitch(GeometryPrimitive::TRIANGLES, indices);
  auto mesh = asset_manager.add_asset(
      "mesh"_hs, new Mesh(renderer.make_mesh(geometry)));

  while (!engine.done) {
    engine.process_events();
    engine.window.begin_frame();
    auto queue = CommandQueue();

    ImGui::ShowDemoWindow(nullptr);

    queue.draw_mesh(mesh, shader);
    glBindTexture(GL_TEXTURE_2D, texture);
    renderer.submit_queue(queue);

    engine.window.end_frame();
    engine.input.prune();
  }

  return 0;
}
