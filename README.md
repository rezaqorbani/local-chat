# local-chat
A simple cross-platfrom chat application, written in C++, for chatting on a server hosted by users. [Boost Asio](https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio.html) is used for asynchronous communication and [Qt](https://www.qt.io/) is used for GUI. 
## Dependencies
### Boost.Asio
Boost.Asio is included in the boost libraries which can be downloaded from [here](https://www.boost.org/users/history/version_1_76_0.html). However it is easier to use a package manager such as [vcpkg](https://github.com/microsoft/vcpkg). 
Once installed, navigate to the top directory where vcpkg is installed and run the following command
```powershell
.\vcpkg install boost:x86-windows-static
```
this will install the 32 bit static version of boost libraries which was used to create this application. 
### Qt
The client side uses Qt6 for GUI. In this project three Qt modules are used: *widgets*, *gui*  and *core*. Although this could be installed via vcpkg, the easiest way to build the project is to use [Qt Creator](https://www.qt.io/download). 

## Build
### Server
Currently the server is an console application and does not depend on Qt. server can be easily built with 
