#!/usr/bin/env bash
set -x
/opt/homebrew/opt/llvm/bin/clang++ -fcolor-diagnostics -fansi-escape-codes -ferror-limit=5 \
    -g \
    scals.cpp \
    main.cpp \
    -o bin/scals\
    `llvm-config --cxxflags --ldflags --system-libs --libs core`
