#!/bin/bash

FORMATTER="clang-format-14"

find . -type f \
    -regextype posix-extended -regex '^.*\.(cpp|c\+\+|cxx|cc|h|hh|hpp|h\+\+|hxx)$' \
    -not -path '*3rdparty*' \
    -exec ${FORMATTER} -i -style=file {} \;

git status > /dev/null
git diff-index --exit-code --name-only HEAD
