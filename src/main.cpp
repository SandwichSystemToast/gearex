#include <span>

#include <spdlog/spdlog.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include "engine.hpp"
#include "misc.hpp"

#include "renderer/renderer.hpp"
#include "renderer/vertex.hpp"

const char *vertex_shader_source = "#version 330 core\n"
                                   "layout (location = 0) in vec3 aPos;\n"
                                   "layout (location = 1) in vec3 aColor;\n"
                                   "out vec3 ourColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   gl_Position = vec4(aPos, 1.0);\n"
                                   "   ourColor = aColor;\n"
                                   "}\0";

const char *fragment_shader_source = "#version 330 core\n"
                                     "out vec4 FragColor;\n"
                                     "in vec3 ourColor;"
                                     "void main()\n"
                                     "{\n"
                                     "   FragColor = vec4(ourColor, 1.0);\n"
                                     "}\n\0";

int main(int argc, char *argv[]) {
  auto engine = Engine();
  auto renderer = Renderer();
  renderer.setup_opengl_debug();

  gl shader = renderer.make_program(vertex_shader_source,
                                    fragment_shader_source, "Demo Shader");

  v3 vertices[4] = {{0.5f, 0.5f, 0.0f},
                    {0.5f, -0.5f, 0.0f},
                    {-0.5f, -0.5f, 0.0f},
                    {-0.5f, 0.5f, 0.0f}};

  v3 colors[4] = {{1., 0., 0.}, {0., 1., 0.}, {0., 0., 1.}, {1., 1., 1.}};

  u32 indices[6] = {0, 1, 3, 1, 2, 3};

  auto verts = VertexBuilder<v3, v3>();

  // TODO: optimize this
  for (z i = 0; i < 4; i++)
    verts.add_vertex(std::move(vertices[i]), std::move(colors[i]));

  Mesh mesh = renderer.make_mesh(std::move(verts), std::span(indices, 6));

  while (!engine.done) {
    engine.process_events();
    engine.window.begin_frame();

    ImGui::ShowDemoWindow(nullptr);

    glUseProgram(shader);
    glBindVertexArray(mesh.vertex_array);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_buffer);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    engine.window.end_frame();
    engine.input.prune();
  }

  return 0;
}
