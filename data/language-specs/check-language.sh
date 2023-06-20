#!/bin/sh
# "./check-language.sh files..." will validate files given on command line.
# "./check-language.sh" without arguments will validate all language files
# in the source directory

files=""

if [ $1 ]; then
  files=$@
else
  if [ "$srcdir" ]; then
    cd $srcdir
  fi

  for lang in *.lang; do
    case $lang in
      msil.lang) ;;
      *)
        files="$files $lang"
        ;;
    esac
  done
fi

for file in $files; do
  xmllint --relaxng language2.rng --noout "$file" || exit 1
done
