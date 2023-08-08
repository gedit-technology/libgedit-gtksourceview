unit_tests = [
  'test-buffer',
  'test-buffer-input-stream',
  'test-buffer-output-stream',
  'test-completion-model',
  'test-completion-words',
  'test-encoding',
  'test-file-loader',
  'test-file-saver',
  'test-iter',
  'test-language',
  'test-languagemanager',
  'test-language-specs',
  'test-mark',
  'test-printcompositor',
  'test-regex',
  'test-region',
  'test-search-context',
  'test-space-drawer',
  'test-style',
  'test-stylescheme',
  'test-styleschememanager',
  'test-styleschemeparser',
  'test-undo-manager',
  'test-utils',
  'test-view',
]

unit_tests_c_args = '-DTOP_SRCDIR="@0@"'.format(meson.project_source_root())

unit_tests_env = environment()
unit_tests_env.set('G_TEST_SRCDIR', meson.current_source_dir())

foreach test_name : unit_tests
  test_exe = executable(
    test_name,
    test_name + '.c',
    dependencies: LIBGEDIT_GTKSOURCEVIEW_STATIC_DEP,
    c_args: unit_tests_c_args,
  )

  test(
    test_name,
    test_exe,
    env: unit_tests_env,
  )
endforeach
