//
// EventSource.h - This file is part of the Grinder library
//
// Copyright (c) 2015 Matthew Brush <mbrush@codebrainz.ca>
// All rights reserved.
//

#ifndef GRINDER_EVENTSOURCE_H
#define GRINDER_EVENTSOURCE_H

#include <functional>

namespace Grinder
{

class EventLoop;
class EventSource;

typedef std::function<bool(EventSource&)> EventHandler;

class EventSource
{
public:
	virtual ~EventSource() {}
	virtual bool prepare(int& max_timeout) { max_timeout = -1; return false; }
	virtual bool check() = 0;
	virtual bool dispatch(EventHandler& func) = 0;
	virtual bool is_file_source() const { return false; }
	virtual bool is_timeout_source() const { return false; }
	virtual bool is_idle_source() const { return false; }

private:
	friend class EventLoop;
	struct {
		bool ready;
		EventHandler handler;
	} loop_data;
};

} // namespace Grinder

#endif // GRINDER_EVENTSOURCE_H
