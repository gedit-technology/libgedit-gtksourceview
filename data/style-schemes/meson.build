install_subdir(
  'default-style-schemes',
  install_dir:
    get_option('datadir') /
    'libgedit-gtksourceview-' + LIBGEDIT_GTKSOURCEVIEW_API_VERSION /
    'styles',
  strip_directory: true
)

if get_option('tests_relying_on_external_programs')
  test('test-style-schemes', files('test.sh')[0])
endif
