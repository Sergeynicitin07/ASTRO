#!/bin/bash
make clean >/dev/null 2>&1
make -j2
echo "Build finished → ./hh_sim"
