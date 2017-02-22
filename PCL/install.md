- Hardware (VirtualMachine)
HardDisk/RAM: 32GB/8GB (as opposed to 8GB/0.5GB for OpenCV)
 
- Compiler
sudo apt-get install build-essential

- CMake (and ...)
sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev

- CMake-GUI
sudo apt install cmake-qt-gui

- OpenGL
sudo apt-get install libgl1-mesa-dev
sudo apt-get install libglew-dev
sudo apt-get install freeglut3 freeglut3-dev
sudo apt-get install binutils-gold
sudo apt-get install libglm-dev
sudo apt install mesa-utils

- Boost
sudo apt-get install libboost-all-dev

- Eigen
https://eigen.tuxfamily.org/dox/GettingStarted.html

- VTK
http://www.vtk.org/download/

- HDF5
sudo apt-get install libhdf5-openmpi-dev

- FLANN
Compile <b>without HDF5!</b> (in CMake settings)
http://www.cs.ubc.ca/research/flann/uploads/FLANN/flann-1.8.4-src.zip

- PCL
https://github.com/PointCloudLibrary/pcl/releases
