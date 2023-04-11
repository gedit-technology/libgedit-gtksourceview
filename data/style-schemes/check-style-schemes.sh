#!/bin/sh

# Usage: ./check-style-schemes.sh XML-FILE(S)...

xmllint --relaxng styles.rng --noout "$@"
