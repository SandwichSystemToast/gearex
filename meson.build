project('gearex', 'cpp', default_options: 'default_library=static')

sdl2_dep = dependency('sdl2')
box2d_dep = dependency('box2d')
imgui_dep = dependency('imgui')
opengl3_dep = dependency('gl')

executable('gearex', 'src/main.cpp', dependencies: [sdl2_dep, box2d_dep, imgui_dep, opengl3_dep])
