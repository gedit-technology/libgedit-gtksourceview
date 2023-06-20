words_completion_provider_public_headers = [
  'gtksourcecompletionwords.h',
]

words_completion_provider_public_c_files = [
  'gtksourcecompletionwords.c',
]

WORDS_COMPLETION_PROVIDER_PRIVATE_HEADERS = [
  'gtksourcecompletionwordsbuffer.h',
  'gtksourcecompletionwordslibrary.h',
  'gtksourcecompletionwordsproposal.h',
  'gtksourcecompletionwordsutils.h',
]

words_completion_provider_private_c_files = [
  'gtksourcecompletionwordsbuffer.c',
  'gtksourcecompletionwordslibrary.c',
  'gtksourcecompletionwordsproposal.c',
  'gtksourcecompletionwordsutils.c',
]

words_completion_provider_headers_install_dir = get_option('includedir') / 'libgedit-gtksourceview-' + LIBGEDIT_GTKSOURCEVIEW_API_VERSION / 'gtksourceview/completion-providers/words'
install_headers(
  words_completion_provider_public_headers,
  install_dir: words_completion_provider_headers_install_dir
)

WORDS_COMPLETION_PROVIDER_STATIC_LIB = static_library(
  'words-completion-provider-static',
  [words_completion_provider_public_headers,
   words_completion_provider_public_c_files,
   WORDS_COMPLETION_PROVIDER_PRIVATE_HEADERS,
   words_completion_provider_private_c_files],
  pic: true, # this static lib is potentially linked in a shared library.
  include_directories: ROOT_INCLUDE_DIR,
  dependencies: LIBGEDIT_GTKSOURCEVIEW_ALL_DEPS,
  c_args: [
    '-DGTK_SOURCE_COMPILATION',
    '-DHAVE_CONFIG_H',
  ]
)
