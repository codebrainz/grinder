//
// EventLoop.h - This file is part of the Grinder library
//
// Copyright (c) 2015 Matthew Brush <mbrush@codebrainz.ca>
// All rights reserved.
//

#ifndef GRINDER_EVENTLOOP_H
#define GRINDER_EVENTLOOP_H

#include <Grinder/EventSource.h>
#include <Grinder/FileSource.h>
#include <Grinder/IdleSource.h>
#include <Grinder/TimeoutSource.h>

#include <memory>
#include <vector>

namespace Grinder
{

class EventLoop
{
public:
	EventLoop();
	int run();
	void quit(int exit_code=0);

	template< class T, class... Args >
	T *emplace_event_source(Args&&... args)
	{
		auto source = new T(std::forward<Args&&>(args)...);
		if (! add_event_source(source))
		{
			delete source;
			return nullptr;
		}
		return source;
	}

	bool add_event_source(EventSource *source, EventHandler handler=nullptr);
	bool set_source_handler(EventSource *source, EventHandler handler);
	bool remove_event_source(EventSource *source);
	bool clear_source_handler(EventSource *source) {
		return set_source_handler(source, nullptr);
	}

	EventSource *add_file(int fd, FileEvents events, EventHandler handler=nullptr);
	EventSource *add_idle(EventHandler handler=nullptr);
	EventSource *add_timeout(int timeout_ms, EventHandler handler=nullptr);

private:
	struct Private;
	std::shared_ptr<Private> impl;
	void iteration();
	bool contains_source(EventSource *source) const;
};

} // namespace Grinder

#endif // GRINDER_EVENTLOOP_H
