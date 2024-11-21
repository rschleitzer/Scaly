#!/usr/bin/env bash
set -x
set -e
scalyc scals.scaly scals
clang++ -fcolor-diagnostics -fansi-escape-codes -ferror-limit=5 \
    -g \
    scals.cpp \
    main.cpp \
    -o bin/scals\
    `llvm-config --cxxflags --ldflags --system-libs --libs core`
