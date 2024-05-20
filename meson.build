project('gearex', 'cpp', default_options: 'default_library=static', default_options : ['c_std=c23', 'cpp_std=c++2a'])

inc = include_directories('include')

sdl2_dep = dependency('sdl2')
box2d_dep = dependency('box2d')
imgui_dep = dependency('imgui')
opengl3_dep = dependency('gl')
spdlog_dep = dependency('spdlog')
glm_dep = dependency('glm')

executable('gearex', 'src/main.cpp', dependencies: [sdl2_dep, box2d_dep, imgui_dep, opengl3_dep, spdlog_dep, glm_dep], include_directories: inc)
