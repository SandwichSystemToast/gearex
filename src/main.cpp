#include "renderer/mesh.hpp"
#include "renderer/renderer.hpp"
#include <spdlog/spdlog.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include "engine.hpp"
#include "misc.hpp"

const char *vertex_shader_source =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragment_shader_source =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
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
  u32 indices[6] = {0, 1, 3, 1, 2, 3};

  auto builder = MeshBuilder();
  builder.add_attribute(std::span(vertices, 4));
  Mesh mesh = renderer.make_mesh(std::move(builder));

  gl ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  while (!engine.done) {
    engine.process_events();
    engine.window.begin_frame();

    ImGui::ShowDemoWindow(nullptr);

    glUseProgram(shader);
    glBindVertexArray(mesh.vertex_array);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    engine.window.end_frame();
    engine.input.prune();
  }

  return 0;
}
