QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH += D:\Programming\C++\vcpkg\installed\x86-windows\include
INCLUDEPATH += .\include

SOURCES += \
    .\src\main.cpp \
    .\src\mainwindow.cpp \
    #.\src\server\server.cpp


HEADERS += \
    .\include\client.hpp \
    .\include\mainwindow.h \
    .\include\chat_message.hpp \
    #.\src\server\server.hpp

FORMS += \
    .\src\qt\mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    CMakeLists.txt
