#include "renderer/command.hpp"
#include <span>

#include <spdlog/spdlog.h>

#include <stb_image.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include "engine.hpp"
#include "misc.hpp"

#include "entt/entt.hpp"
#include "renderer/renderer.hpp"
#include "renderer/vertex.hpp"

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

int main(int argc, char *argv[]) {
  auto engine = Engine();
  auto renderer = Renderer();
  renderer.setup_opengl_debug();

  i32 width, height, channels;
  u8 *cat_data = stbi_load("cat.png", &width, &height, &channels, 0);
  gl texture = renderer.make_texture(cat_data, width, height);
  stbi_image_free(cat_data);

  gl shader = renderer.make_program(vertex_shader_source,
                                    fragment_shader_source, "Demo Shader");

  v3 vertices[4] = {{0.5f, 0.5f, 0.0f},
                    {0.5f, -0.5f, 0.0f},
                    {-0.5f, -0.5f, 0.0f},
                    {-0.5f, 0.5f, 0.0f}};
  v3 colors[4] = {{1., 0., 0.}, {0., 1., 0.}, {0., 0., 1.}, {1., 1., 1.}};
  v2 texture_coordinates[4] = {{0., 0.}, {1., 0.}, {1., 1.}, {0., 1.}};

  u32 indices[6] = {0, 1, 3, 1, 2, 3};

  auto verts = VertexBuilder<v3, v3, v2>();

  // TODO: optimize this
  for (z i = 0; i < 4; i++)
    verts.add_vertex(vertices[i], colors[i], texture_coordinates[i]);

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
