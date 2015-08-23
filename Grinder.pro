TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle qt

SOURCES += \
    main.cpp \
    Grinder/EventLoop.cpp \
    Grinder/TimeoutSource.cpp \
    Grinder/Linux/TimerFD.cpp \
    Grinder/Linux/SignalFD.cpp

HEADERS += \
    Grinder/EventLoop.h \
    Grinder/EventSource.h \
    Grinder/FileEvents.h \
    Grinder/FileSource.h \
    Grinder/IdleSource.h \
    Grinder/TimeoutSource.h \
    Grinder/Utility.h \
    Grinder/Grinder \
    Grinder/Linux/TimerFD.h \
    Grinder/Linux/SignalFD.h \
    Grinder/Platform.h

