project(
  'gearex',
  'cpp',
  default_options: [
    'default_library=static',
    'c_std=c23',
    'cpp_std=c++2a'
  ]
)

arg = []
inc = []
dep = []
src = []

arg += '-DGLM_ENABLE_EXPERIMENTAL'
arg += '-DGL_EXT_PROTOTYPES'

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
  dependencies : dep,
  include_directories : inc,
  cpp_args : arg,
  install : true
)

run_command('tar', '-czvf', 'assets.tar.gz', 'assets')

tar = find_program('tar')

assets = custom_target(
  'assets',
  output : 'assets.tar.gz',
  command : [ tar, '-czvf', 'assets', '@OUTPUT@' ],
  install : false
)

install_data(
  sources : 'assets.tar.gz',
  install_dir : get_option('datadir') / meson.project_name()
)
