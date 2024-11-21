#!/usr/bin/env bash
set -x
set -e
scalyc scalyc.scaly scalyc
clang++ -fcolor-diagnostics -fansi-escape-codes -ferror-limit=5 \
    -g \
    scalyc.cpp \
    ../scaly/scaly.a \
    main.cpp \
    -o bin/scalyc\
    `llvm-config --cxxflags --ldflags --system-libs --libs core`
