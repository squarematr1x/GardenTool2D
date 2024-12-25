# GardenTool2D

A 2D game engine based on entity component system (ECS) architecture.

![image](https://github.com/squarematr1x/GardenTool2D/blob/main/demo/test.png)

## Features

* ECS
* Level editor GUI for editing/creating entities inside levels
* Support for different scenes (e.g. menu, topdown RPG or sidescroller)
* Utilities such as profiler, logger and timer
* In-built math (2D vector) and physics libraries
* Particle system
* Parallax scrolling
* Texture atlas

## Libraries

* SFML for rendering, sound, i/o and window system
* ImGui for editor

## Build and run

### Windows
_TBA_

### Debian (Ubuntu or similar)
_TBA_

### macOS
Install SFML using [Homebrew](https://brew.sh/):

```
brew install sfml@2
```

Clone this repository:

```
git clone https://github.com/squarematr1x/GardenTool2D.git
```

Navigate to directory:

```
cd GardenTool2D
```

Compile and run:

```
make run
```