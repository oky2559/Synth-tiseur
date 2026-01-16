project(
  'isen-base-project',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3'],
)

cc = meson.get_compiler('c')
inc = include_directories('include')

subdir('src')

if get_option('build_app')
  subdir('app')
endif

if get_option('tests')
  subdir('tests')
endif
