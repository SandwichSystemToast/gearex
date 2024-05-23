#include <spdlog/spdlog.h>

#include <stb_image.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include "engine/assets/assets.hpp"
#include "engine/engine.hpp"
#include "engine/renderer/renderer.hpp"

const char *vertex_shader_source = "#version 330 core\n"
                                   "layout (location = 0) in vec3 aPos;\n"
                                   "layout (location = 1) in vec3 aColor;\n"
                                   "layout (location = 2) in vec2 aTexCoord;\n"
                                   "out vec3 ourColor;\n"
                                   "out vec2 TexCoord;\n"
                                   "void main()\n"
                                   "{\n"
                                   "  gl_Position = vec4(aPos, 1.0);\n"
                                   "  ourColor = aColor;\n"
                                   "  TexCoord = aTexCoord;\n"
                                   "}\0";

const char *fragment_shader_source =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;"
    "in vec2 TexCoord;"
    "uniform sampler2D ourTexture;"
    "void main()\n"
    "{\n"
    "  FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);\n"
    "}\n\0";

using namespace engine;
using namespace engine::renderer;
using namespace engine::assets;

int main(int argc, char *argv[]) {
  auto asset_manager = AssetManager();
  asset_manager.load_from_file("./assets.tar.gz");

  auto engine = Engine();
  auto renderer = Renderer();
  renderer.setup_opengl_debug();

  auto cat_texture = asset_manager.get_image("assets/cat"_hs);
  auto cat_texture_size = cat_texture->dimensions();
  gl texture = renderer.make_texture(cat_texture->raw().data(),
                                     cat_texture_size.x, cat_texture_size.y);

  gl shader = renderer.make_program(vertex_shader_source,
                                    fragment_shader_source, "Demo Shader");

  v3 vertex_positions[4] = {{0.5f, 0.5f, 0.0f},
                    {0.5f, -0.5f, 0.0f},
                    {-0.5f, -0.5f, 0.0f},
                    {-0.5f, 0.5f, 0.0f}};
  v3 colors[4] = {{1., 0., 0.}, {0., 1., 0.}, {0., 0., 1.}, {1., 1., 1.}};
  v2 texture_coordinates[4] = {{0., 0.}, {0., 1.}, {1., 1.}, {1., 0.}};

  u32 indices[6] = {0, 1, 3, 1, 2, 3};

  auto verts = VertexBuilder<v3, v3, v2>();

  // TODO: optimize this
  for (z i = 0; i < 4; i++)
    verts.add_vertex(vertex_positions[i], colors[i], texture_coordinates[i]);

  Mesh mesh = renderer.make_mesh(std::move(verts), std::span(indices, 6),
                                 Topology::TRIANGLES);

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
