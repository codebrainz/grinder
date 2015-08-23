cxxflags = $(CPPFLAGS) $(CXXFLAGS) -I. \
	-Wall -Wextra -Werror -std=c++11 -fPIC
ldflags = $(LDFLAGS)

sources = \
	Grinder/EventLoop.cpp \
	Grinder/TimeoutSource.cpp

objects = $(sources:.o=.cpp)

all: GrinderTest

GrinderTest: libgrinder.so main.cpp
	$(CXX) $(strip $(cxxflags) -o $@ main.cpp $(ldflags) -L. -lgrinder)

libgrinder.so: $(objects)
	$(CXX) $(strip -shared $(cxxflags) -o $@ $(objects) $(ldflags))

.cpp.o:
	$(CXX) $(strip $(cxxflags) -o $@ $<)

-include Makefile.deps
Makefile.deps: $(sources) main.cpp
	$(CXX) $(strip -MM $(cxxflags) $(sources) main.cpp > $@)

clean:
	$(RM) *.o Grinder/*.o *.so GrinderTest

.PHONY: all clean
