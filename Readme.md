# Odometry-Visualization

> Visualization part for the [Direct Stereo Semi-dense Visual Odometry and 3D Reconstruction Project](https://github.com/WangYuTum/odometry)

## Install as Library

To use this as a library in another CMake project, do the following

    git submodule add https://github.com/whateverforever/Odometry-Vis.git
    git submodule update --init --recursive

## Install Standalone

To get to run the example (main.cpp), clone this repository with its submodules:

    git clone --recurse-submodules https://github.com/whateverforever/Odometry-Vis.git
    cd Odometry-Vis
    ./buildrun.sh

`buildrun.sh` is used to keep the top level clean, all build artifacts will be built in `build`.

## Current State

![Latest screenshot](docs/latest.png)
