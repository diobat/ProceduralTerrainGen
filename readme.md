

# FluxLumina-based Procedural terrain generator
<div align="center">
<img width="1457" alt="PTG_snow" src="https://github.com/diobat/ProceduralTerrainGen/assets/25584192/925adb97-4afd-4914-9bf9-9343791e3981">
</div>  

## Overview
Welcome to ProceduralTerrainGen, a FluxLumina Example Project! 

This project generates geometry and renders contiguous infinite chunks of terrain. It makes use of Perlin noise and Hydraulic Erosion to generate realistic landmasses with visually appealing geographical features.

It also serves as a template to demonstrate the capabilities of [FluxLumina](https://github.com/diobat/FluxLumina), an OpenGL-based rendering engine static library. 

## Getting Started

### Prerequisites
- openGL (Modern OSes already come with it)
- CMake (version 3.23 or higher)
- A C++ compiler

### Usage

```bash
# Clone the repository
git clone https://github.com/diobat/FluxLumina-template.git

# Go to the project directory
cd FluxLumina-template

# Create a build directory
mkdir build

# Configure the build
cmake -S . -B build

# Build target
cmake --build build --config Release

# Go into target folder
cd build/Release

# Run the target
./FL_PCG.exe
```
