# System library

[![Build status](https://ci.appveyor.com/api/projects/status/f6xatihwk1kgji85/branch/master?svg=true)](https://ci.appveyor.com/project/m4x1m1l14n/system/branch/master)
[![Build Status](https://travis-ci.org/m4x1m1l14n/System.svg?branch=master)](https://travis-ci.org/m4x1m1l14n/System)

Various C++11 helper classes


### Build
#### Prerequisities

To use this library you will need
* CMake version 2.6 (or higher) installed

#### Windows

To build under Windows OS, after cloning this repository, open command line tool (cmd.exe), navigate to directory, where repository content is cloned, and then type following.

1. Create "build" directory, where CMake output files will be generated

	```shell
	$ mkdir build
	```

2. Switch to newly created output directory

	```shell
    $ cd build
    ```

3. While in build directory, execute CMake, to generate Visual Studio project files.

	> **NOTE:**
	> Code below requests projects for 64 bit architecture to be generated. To generate 32 bit, ommit -A command line argument, which will default to x86 generated output.

	```shell
	$ cmake . -A x64
	```

#### Linux

