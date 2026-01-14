libbase_project = static_library('baseproject',
  sources: [
    'custom_math.c',
    'synthesizer.c'
  ],
  include_directories: inc,
  install: true
)

libbase_project_dep = declare_dependency(
  link_with: libbase_project,
  include_directories: inc
)
