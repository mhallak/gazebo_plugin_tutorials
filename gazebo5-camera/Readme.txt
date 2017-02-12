
Compiling Plugin
===============
Add needed lines in CMakeLists.txt; 
for example:

          cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

	  find_package(gazebo REQUIRED)
	  include_directories(${GAZEBO_INCLUDE_DIRS})
	  link_directories(${GAZEBO_LIBRARY_DIRS})
	  list(APPEND CMAKE_CXX_FLAGS "${GAZEBO_CXX_FLAGS}")

	  add_library(model_push SHARED hello_world.cc)
	  target_link_libraries(model_push ${GAZEBO_LIBRARIES})

For gazebo6, add:
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GAZEBO_CXX_FLAGS}")

Create and go to build directory:

	$ mkdir ~/gazebo5-camera/build
	$ cd ~/gazebo5-camera/build

Compile:
	$ cmake ../
	$ make

Source this.env

Using Plugin: see camera.world file

Running:
      cd ~/gazebo_plugin_tutorial/gazebo5-camera
1. 
      gazebo camera.world
2.
      gzserver -u camera.world
      somewhere else run gzclient
        (will print what images are kept)
3.    
      roslaunch camera_world.launch

=======================================================================
Notes about plugins
============
1. The name of the plugin mentioned in the world file is not important:
      <plugin name="model_push" filename="libmodel_push.so"/>
or       <plugin name="model_push2" filename="libmodel_push.so"/>
2. Load is called before Init method
3. World file: when saving from gazebo, it becomes an sdf file - regarding clip: we can save from gazebo and then play with the near and far clp of the camera.

