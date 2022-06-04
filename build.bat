wsl.exe git submodule init
wsl.exe git submodule update
wsl.exe ./buildGLEW.sh
rm -rf "./glfw/build"
mkdir "./glfw/build"
mkdir "./glfw/build/Win32"
mkdir "./glfw/build/x64"
cmake -G "Visual Studio 17 2022" -A Win32 -B ./glfw/build/Win32 -S ./glfw
cmake -G "Visual Studio 17 2022" -A x64 -B ./glfw/build/x64 -S ./glfw
mv "./glfw/build/Win32/src/glfw.vcxproj" "./glfw/build/Win32/src/glfw-Win32.vcxproj"
mv "./glfw/build/Win32/src/glfw.vcxproj.filters" "./glfw/build/Win32/src/glfw-Win32.vcxproj.filters"
mv "./glfw/build/x64/src/glfw.vcxproj" "./glfw/build/x64/src/glfw-x64.vcxproj"
mv "./glfw/build/x64/src/glfw.vcxproj.filters" "./glfw/build/x64/src/glfw-x64.vcxproj.filters"