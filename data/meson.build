subdir('style-schemes')

install_subdir(
  'language-specs',
  install_dir:
    get_option('datadir') /
    'libgedit-gtksourceview-' + LIBGEDIT_GTKSOURCEVIEW_API_VERSION,
  exclude_files: [
    'check-language.sh',
    'language-specs.its',
    'language-specs.pot',
    'lang_v1_to_v2.xslt',
    'ue2gsv.pl',
    'update-pot.sh',
  ]
)
