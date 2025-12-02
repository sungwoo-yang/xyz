# CS200 Computer Graphics I

This project is a 2D rendering engine demo for the CS200 Computer Graphics I course. It implements and compares multiple 2D rendering techniques using C++ and OpenGL.

The engine supports three distinct renderers: **Immediate Mode**, **Batch Mode**, and **Instanced Mode**. You can switch between them in real-time via ImGui controls to visually compare their performance by adjusting the number of sprites on screen.

## Project DemosThis project includes two main demo scenes. The default starting scene is `DemoPerformance`.

### 1. DemoPerformance (Default)

The performance demo tests the efficiency of the three rendering pipelines by drawing thousands of sprites.

* **Immediate Renderer**: Issues one `glDrawElements` call for every single sprite. This is simple for a few objects but suffers from severe CPU overhead as the count grows.
* **Batch Renderer**: Batches vertex data for many sprites (that share textures) into one large buffer, minimizing `glDrawElements` calls to one per frame.
* **Instanced Renderer**: Tells the GPU to draw a single quad mesh thousands of times, using a separate buffer for per-instance data (transform, color, UV offset). This is achieved with a single `glDrawElementsInstanced` call.

**ImGui Controls:**
* Switch renderers in real-time using the `Immediate`, `Batch`, and `Instanced` radio buttons.
* Adjust the number of sprites rendered using the `Sprite Count` buttons.
* Monitor the `Draw Calls` counter to see how many draw commands each renderer issues per frame.

### 2. DemoCreativeScene

A simple 2.5D parallax scrolling demo utilizing the `BatchRenderer2D`.

* Features a background (planets), mid-ground (stars), and foreground (platforms) scrolling at different speeds to create a sense of depth.
* Thousands of star particles are drawn efficiently using the batch renderer.

**ImGui Controls:**
* Click the `Switch to Performance Demo` button to return to the `DemoPerformance` scene.

## How to Build and Run

First Setup your [Development Environment](docs/DevEnvironment.md)

### Command Line Build

**Windows**

```sh
# configure / generate build files
cmake --preset windows-debug
cmake --preset windows-developer-release
cmake --preset windows-release
cmake --preset web-debug-on-windows

# Build exe
cmake --build --preset windows-debug
cmake --build --preset windows-developer-release
cmake --build --preset windows-release
cmake --build --preset web-debug-on-windows
```

**Linux**
```sh
# configure / generate build files
cmake --preset linux-debug
cmake --preset linux-developer-release
cmake --preset linux-release
cmake --preset web-debug
cmake --preset web-developer-release
cmake --preset web-release

# Build exe
cmake --build --preset linux-debug
cmake --build --preset linux-developer-release
cmake --build --preset linux-release
cmake --build --preset web-debug
cmake --build --preset web-developer-release
cmake --build --preset web-release
```

### Automated Build Script

For convenience, use the automated build script that handles all configurations:

```sh
python3 scripts/scan_build_project.py
```

See the script help for filtering options:
```sh
python3 scripts/scan_build_project.py --help
```

## Build Configurations

This project supports three distinct build configurations, each designed for different stages of development and deployment:

### **Debug Configuration**
- **Purpose**: Active development and debugging
- **Features**: 
  - Allows setting breakpoints to step through code line-by-line
  - No compiler optimizations (preserves variable values for debugging)
  - Console window for viewing debug output
  - All developer helper features enabled
- **Performance**: Slower execution, larger executable size
- **When to use**: When you need to debug issues or understand code flow

### **Developer-Release Configuration**  
- **Purpose**: Testing with optimized performance while keeping developer tools
- **Features**:
  - Compiler optimizations enabled (faster execution)
  - Cannot set breakpoints (optimizations interfere with debugging)
  - Console window for viewing output
  - Verbose logging and error checking enabled
  - Developer assertions active
- **Performance**: Fast execution, optimized code
- **When to use**: Performance testing while retaining diagnostic capabilities

### **Release Configuration**
- **Purpose**: Final builds for end users
- **Features**:
  - Full compiler optimizations
  - No console window (clean user experience)
  - Minimal logging (only critical errors)
  - No developer features or extra error checking
  - Smallest executable size
- **Performance**: Maximum speed, production-ready
- **When to use**: Final builds for distribution to users

## CMake Presets

CMake presets simplify the build process by pre-configuring complex settings. Instead of manually specifying:

- Compiler flags and optimization levels
- Output directories and build paths  
- Platform-specific configurations
- Dependency locations
- Debug/release-specific settings

The presets handle all of this automatically. For example, `cmake --preset linux-debug` is equivalent to running:

```sh
cmake -S . -B build/linux-debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DDEVELOPER_VERSION=ON \
  [... many more flags ...]
```

This saves time and reduces configuration errors, especially when working across different platforms and build types.
