#!/bin/bash

set -x

# run in subshell, don't wanna pollute our precious terminal
(
cd build && \
cmake .. && \
make && \
./src/main
)