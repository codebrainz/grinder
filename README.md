Grinder README
==============

Grinder is a simple C++ library providing an "asynchronous" event
loop.

To use Grinder in your project, just copy the .cpp and .h files you
need into your project. The files use standard C++11 as well as some
POSIX-ish APIs. The `Grinder/Linux` directory contains wrappers around
some useful Linux-specific APIs like timerfd and signalfd.

The main.cpp file contains some test/demo code.

Concepts
--------

The main component is the `EventLoop` which controls the processing of
events. It is the heart of the system.

`EventSource` subclasses provide a mechanism for detecting when events
are ready. The `EventLoop` uses the `EventSource` interface to figure
out when the events are ready as well as dispatching events. All of
the `___Source` classes as well as the classes in the `Grinder/Linux`
directory provide implementations of the `EventSource` interface.

The `EventSource` interface is composed of the following pure virtual 
functions:

- `bool prepare(int& max_timeout)`
  This function is called before the event loop polls any file descriptors.
  If your event source knows it's ready now, implementations can return
  `true` and skip polling. The `max_timeout` argument is an output
  parameter that can limit how long the `poll()` timeout is. The value
  is in milliseconds and may be lowered by other event sources.
- `bool check()`
  This function is called after the file descriptors have been polled.
  The `check()` function won't be called if the `prepare()` function
  returned `true` for a particular event loop iteration. Examples of
  using this function are to check if specific `FileEvents` have occured
  or whether a timer has elapsed. Return `true` to have the event handler
  dispatched or false to not dispatch.
- `bool dispatch(EventHandler& handler)`
  This function is called when either the `prepare()` or `check()`
  functions have returned `true` indicating that an event has occurred.
  The `handler` argument provides a reference to the function to be
  called. Implementations of this function might choose to read a file
  descriptor or setup some event source-specific state which might be
  useful to event handlers. When this function returns `false`, the
  event source dispatching will be removed from the event loop and
  deleted. If it returns `true`, events will continue to be checked and
  dispatched.

There are 3 basic event source types, represented by the base classes 
`FileSource`, `IdleSource`, and `TimeoutSource`. Most event sources 
will want to subclass one of these rather than subclassing the 
`EventSource` directly.

The typedef for `EventHandler` provides a function type that should be 
used when setting the callback for event sources. You can use a 
functor, function pointer or anonymous (lambda) function for the event 
handler. If the handler returns `false`, the event source which called 
the handler will be removed from the event loop. If it returns `true`, 
the event source will continue to be checked and dispatched.

Event Sources
-------------

### FileSource

File sources have a file descriptor as well as a set of events to be
watched for on the file descriptor. The `EventLoop` uses the `poll()`
function or similar to check whether any of the file descriptors are
ready. Most event sources will want to subclass this class.

### IdleSource

Idle sources are a special kind of event source which will only be
dispatched when there's nothing else for the event loop to do (ie. no
timers have expired and no file descriptors are ready). Usually you
won't use this class directly but rather add idle event sources to
the event loop using `EventLoop::add_idle()` passing it a function
to be called when the event loop is idle.

### TimeoutSource

Timeout sources show an example of a class which is directly inherited
from `EventSource` as they have no backing file descriptor. Timeouts
are not particularly accurate but provide a convenient mechanism to
call a function at roughly some time in the future. Ususally you won't
use `TimeoutSource` class directly but rather add timeout event sources
to the event loop using `EventLoop::add_timeout()`.

An alternative to `TimeoutSource` for Linux is the `TimerFD` class. It
inherits from `FileSource` as it uses the Linux-specific `timerfd` API.
This should in theory provide better accuracy, but the class is only
designed to handle timers accurate to whole milliseconds.

### SignalSource

Signal sources are designed to wrangle Unix signals into the event
loop. They install a global signal handler which writes to one end of
a pipe, while the `SignalSource`, being derived from `FileSource`,
watches the other end of the pipe in the event loop. The signal handler
writes the signal number as a 32-bit unsigned integer and the `SignalSource`
reads the signal number, sets its `SignalSource::signo` member variable
and then dispatches the event to the handler.

It's important that only one `SignalSource` instance be added to an
event loop. Since the `SignalSource` installs global signal handlers,
and optionally modifies the global process signal mask, adding more
than one `SignalSource` to event loops will cause bad things to happen.

The recommended usage is to create a single `SignalSource` when setting
up the `EventLoop` initially, and adding it to the loop, leaving it to
be freed when the loop cleans up. Keep a pointer the `EventSource` if
you want to add or remove signals to be watched using the
`SignalSource::add()` and `SignalSource::remove()` member functions.
Any other usage is likely to cause problems.

An alternative to `SignalSource` for Linux is the `SignalFD` class
which provides a wrapper around the Linux-specific `signalfd` API. It
derives from `FileSource` and watches a file descriptor that the kernel
will send signal information to.

Building and Embedding
======================

Currently the library is meant to be integrated into other projects
directly. There is a `Makefile` included in the root source directory
which is just meant for testing the build. It doesn't support fancy
stuff like out-of-tree builds or installing the library. The `Makefile`
produces a `libgrinder.so` file and `GrinderTest` program in the root
directory.

There's also a `Grinder.pro` file in the root source directory which is
just enough to compile the code (into a single demo program) using
QtCreator/qmake. This is only meant for use when working on `Grinder`
code in the QtCreator IDE, not for a real build system.

### Using it with your Project

This is the current recommended method of using Grinder in your project:

1. Copy the entire `Grinder` sub-directory into your project's tree.
2. Copy the `License.txt` and optionally the `README.md` files into
   that same directory.
3. If not using the Linux-specific classes, you can delete the
   `Grinder/Linux` directory.
4. Add the .cpp files to be compiled by your build system/C++ compiler.
  - For GCC-like compilers, you should use the `-std=c++11` flag to
    enable the C++11 support which Grinder requires.
  - Add the directory containing the `Grinder` directory to the compiler's
    include search path. This is so your code can use Grinder headers
    like `#include <Grinder/TheHeader.h>`.
5. In the code that uses Grinder, it is recommended to include the
   main `Grinder` header like `#include <Grinder/Grinder>`. That header
   includes all of the other headers, including platform-specific ones
   if supported.

It is up to the project whether to build an actual library. With some
build-systems it's convenient to build "helper" libraries and in other
cases you might want a proper shared library if several programs you
control use Grinder. The optimal method for a single application is to
link the compiled Grinder object files directly into the main binary.

Portability
===========

While Grinder is meant to be cross-platform, at least initially such
support is not widely tested. Primary development happens on Linux as
well as minor testing on OSX.

No testing on Windows has been performed but it is a goal to add both
generic and platform-specific support for Windows. The `EventLoop`,
`EventSource`, `IdleSource`, and `TimeoutSource` should be fine on
Windows out of the box. `SignalSource` and `FileSource`, which use file
descriptors rather than Win32 `HANDLE`s are most likely to require
porting effort. Supporting sockets on Windows should be relatively
painless as it has a file-descriptor like API on Windows.

Class Hierarchy
===============

Below is a little diagram to show the class inheritance in Grinder.

    EventLoop
    EventSource
        IdleSource
        TimeoutSource
        FileSource
            SignalSource
            SignalFD
            TimerFD
