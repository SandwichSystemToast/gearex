project(
  'gearex',
  'cpp',
  default_options: [
    'default_library=static',
    'c_std=c23',
    'cpp_std=c++2a'
  ]
)

inc = []
dep = []
src = []

inc += include_directories('include', 'ext')

dep += [
  dependency('sdl2'),
  dependency('box2d'),
  dependency('gl'),
  dependency('spdlog'),
  dependency('glm'),
  dependency('libarchive')
]

subdir('ext')
subdir('src')

executable(
  'gearex',
  src,
  dependencies: dep,
  include_directories: inc
)
