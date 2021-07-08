# local-chat
A simple cross-platfrom chat application, written in C++, for chatting on a server hosted by users. [Boost Asio](https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio.html) is used for asynchronous communication and [Qt](https://www.qt.io/) is used for GUI. 
## Dependencies
### Boost.Asio
Boost.Asio is included in the boost libraries which can be downloaded from [here](https://www.boost.org/users/history/version_1_76_0.html). However it is easier to use a package manager such as [vcpkg](https://github.com/microsoft/vcpkg). 
Once installed, navigate to the directory where vcpkg is installed and run the following command:
```
.\vcpkg install boost:x86-windows-static
```
this will install the 32 bit static version of boost libraries which was used to create this application. 
### Qt
The client side uses Qt6 for GUI. In this project three Qt modules are used: *widgets*, *gui*  and *core*. Although this could be installed via vcpkg, the easiest way to build the project is to use [Qt Creator](https://www.qt.io/download). 

## Build
### Server
Currently the server is an console application and does not depend on Qt. server can be easily built with [CMake](https://cmake.org/). Navigate to the projects root directory and run the following command: 
```
cmake -S . -B .\out\build -DCMAKE_TOOLCHAIN_FILE=[VCPKG_DIRECTORY_PATH]/scripts/buildsystems/vcpkg.cmake
```
Replace [VCPKG_DIRECTORY_PATH] with the path to the vcpkg installation directory. This will try to generate build files for the native IDE, for example Visual Studio on windows, according to your platform. You can choose your preferred IDE with the *-G* parameter followed by the name of the IDE of your choice defined [here](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#manual:cmake-generators(7)). Once the build files are generated the project can be built with the command: 
```
cmake --build .\out\build
```
By default, the build type will be Debug, in order to chanage the build type to release just use `--config Release` arugment. 

### Client
The client can be built with Qt Creator. After cloning the project, open the project file, local-chat.pro, and uncomment the following line:
````
INCLUDEPATH += [VCPKG_DIRECTORY_PATH]\installed\x86-windows\include
````
