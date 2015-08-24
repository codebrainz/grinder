cxxflags = $(CPPFLAGS) $(CXXFLAGS) -I. \
	-Wall -Wextra -Werror -std=c++11 -fPIC
ldflags = $(LDFLAGS)

sources = \
	Grinder/EventLoop.cpp \
	Grinder/GenericSignalSource.cpp \
	Grinder/SignalSource.cpp \
	Grinder/TimeoutSource.cpp \
	Grinder/Linux/SignalFD.cpp \
	Grinder/Linux/TimerFD.cpp

objects = $(sources:.cpp=.o)

ifeq ($(V),1)
	VCXX   = $(CXX) -c
	VCXXLD = $(CXX)
	VDEPS  = $(CXX) -MM
else
	VCXX   = @echo "  COMPILE $@" && $(CXX) -c
	VCXXLD = @echo "  LINK $@" && $(CXX)
	VDEPS  = @echo "  DEPENDS $@" && $(CXX) -MM
endif

all: GrinderTest

GrinderTest: libgrinder.so main.cpp
	$(VCXXLD) $(strip $(cxxflags) -o $@ main.cpp $(ldflags) -L. -lgrinder)

libgrinder.so: $(objects)
	$(VCXXLD) $(strip -shared $(cxxflags) -o $@ $(objects) $(ldflags))

.cpp.o:
	$(VCXX) $(strip $(cxxflags) -o $@ $<)

-include Makefile.deps
Makefile.deps: $(sources) main.cpp
	$(VDEPS) $(strip $(cxxflags) $(sources) main.cpp > $@)

clean:
	$(RM) *.o Grinder/*.o Grinder/Linux/*.o *.so GrinderTest

.PHONY: all clean
