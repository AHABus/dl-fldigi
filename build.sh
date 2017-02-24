#!/usr/bin/env bash
autoreconf -fi
CXXFLAGS="-stdlib=libstdc++" PKG_CONFIG_PATH=/usr/local/opt/curl/lib/pkgconfig ./configure --with-jpeg=/usr/local/include/ --disable-flarq --enable-optimizations=native
make
