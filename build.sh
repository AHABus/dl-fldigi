#!/usr/bin/env bash
CC=gcc-5 CXX=g++-5 PKG_CONFIG_PATH=/usr/local/opt/curl/lib/pkgconfig ./configure --with-jpeg=/usr/local/include/ --disable-flarq
