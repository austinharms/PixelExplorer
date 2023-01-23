# PXEngine
## Basic game engine written in C++  
Renders using OpenGL backend though SDL2 and GLEW supports GUI using ImGui and has real time physics using Nvidia Physx
### Building the Engine
***Note**: These build steps are for Windows with WSL, WSL is required for building the GLEW project source and all projects are built and run using Visual Studio 2022*  
After cloning the repo run `build.bat` this will
- Update git submodules
- Build GLEW source *using WSL*
- Create Physx projects
You should now be able to open the `PixelExplorer.sln` solution file and build/run the projects using Visual Studio
### Examples 
Examples can be found in the PXEngineSamples project 
- *Cube Wall* Renders a wall of cubes
- *Cube Stack* Creates a stack of physics simulated cubes
- *Free Camera* Simple WASD controllable camera implementation
- *Marching Cubes* Primitive implementation of the marching cubes algorithm
