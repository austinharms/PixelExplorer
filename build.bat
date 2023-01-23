git submodule update --init --recursive
wsl.exe sed -i -e 's/\r$//' ./buildGLEW.sh
wsl.exe ./buildGLEW.sh
./PhysX/physx/generate_projects.bat vc16win64
./PhysX/physx/generate_projects.bat vc16win32


