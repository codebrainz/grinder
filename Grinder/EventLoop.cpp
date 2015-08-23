//
// EventLoop.cpp - This file is part of the Grinder library
//
// Copyright (c) 2015 Matthew Brush <mbrush@codebrainz.ca>
// All rights reserved.
//

#include <Grinder/EventLoop.h>
#include <Grinder/EventSource.h>
#include <Grinder/FileSource.h>
#include <Grinder/IdleSource.h>
#include <Grinder/TimeoutSource.h>
#include <algorithm>
#include <unordered_set>
#include <poll.h>
#include <iostream>

using namespace std;

namespace Grinder
{

struct EventLoop::Private
{
	bool quit;
	int exit_code;
	uint64_t time_last;
	vector<EventSource*> sources;
	vector<EventSource*> remove_sources;
	vector<FileSource*> pollable_sources;
	vector<struct pollfd> poll_fds;
	unordered_set<EventSource*> source_set;
	vector<IdleSource*> idle_sources;
	vector<TimeoutSource*> timeout_sources;

	Private() : quit(false), exit_code(0) {}

	~Private()
	{
		for (auto source : sources)
			delete source;
	}
};

EventLoop::EventLoop()
	: impl(make_shared<Private>())
{
}

void EventLoop::iteration()
{
	int max_timeout = -1;
	int n_ready = 0;

	// prepare and check for already ready event sources
	for (auto source : impl->sources)
	{
		if (source->is_idle_source())
			continue;

		int source_timeout = -1;

		if (source->prepare(source_timeout))
		{
			source->loop_data.ready = true;
			n_ready++;
		}
		else if (source_timeout > 0 &&
					(max_timeout == -1 || source_timeout < max_timeout))
		{
			max_timeout = source_timeout;
		}
	}

	// poll all of the pollable event sources
	impl->poll_fds.resize(impl->pollable_sources.size());
	for (size_t i = 0; i < impl->pollable_sources.size(); i++)
	{
		if (! impl->pollable_sources[i]->loop_data.ready &&
				impl->pollable_sources[i]->fd >= 0)
		{
			impl->poll_fds[i].fd = impl->pollable_sources[i]->fd;
			impl->poll_fds[i].events = static_cast<int>(impl->pollable_sources[i]->events);
			impl->poll_fds[i].revents = impl->pollable_sources[i]->revents = 0;
		}
	}

	// if there's nothing else ready and there are idle event sources, make
	// poll() return immediately.
	if (n_ready < 1 && impl->idle_sources.size() > 0)
		max_timeout = 0;

	if (::poll(impl->poll_fds.data(), impl->poll_fds.size(), max_timeout) > 0)
	{
		for (size_t i = 0; i < impl->pollable_sources.size(); i++)
			impl->pollable_sources[i]->revents = impl->poll_fds[i].revents;
	}

	// now check if any more sources are ready after polling
	for (auto source : impl->sources)
	{
		if (source->is_idle_source())
			continue;

		if (! source->loop_data.ready)
		{
			if (source->check())
			{
				source->loop_data.ready = true;
				n_ready++;
			}
		}
	}

	impl->remove_sources.clear();
	if (n_ready > 0)
	{
		// dispatch all ready event sources
		for (auto source : impl->sources)
		{
			if (! source->is_idle_source() &&
					source->loop_data.ready &&
					source->loop_data.handler)
			{
				if (! source->dispatch(source->loop_data.handler))
					impl->remove_sources.emplace_back(source);
				source->loop_data.ready = false;
			}
		}
	}
	else
	{
		// nothing else ready, dispatch idle event sources
		for (auto source : impl->idle_sources)
		{
			if (source->loop_data.handler &&
					! source->dispatch(source->loop_data.handler))
			{
				impl->remove_sources.emplace_back(source);
			}
		}
	}

	// prune event sources which requested to be removed
	for (auto source : impl->remove_sources)
		remove_event_source(source);
}

bool EventLoop::contains_source(EventSource *source) const
{
	return (impl->source_set.count(source) > 0);
}

int EventLoop::run()
{
	impl->quit = false;
	impl->exit_code = 0;
	while (! impl->quit)
		iteration();
	return impl->exit_code;
}

void EventLoop::quit(int exit_code)
{
	impl->exit_code = exit_code;
	impl->quit = true;
}

bool EventLoop::add_event_source(EventSource *source, EventHandler handler)
{
	if (contains_source(source))
		return false;

	source->loop_data.ready = false;
	source->loop_data.handler = handler ? move(handler) : nullptr;
	impl->sources.emplace_back(source);
	impl->source_set.emplace(source);

	if (source->is_idle_source())
		impl->idle_sources.emplace_back(dynamic_cast<IdleSource*>(source));
	else if (source->is_timeout_source())
		impl->timeout_sources.emplace_back(dynamic_cast<TimeoutSource*>(source));
	else if (source->is_file_source())
		impl->pollable_sources.emplace_back(dynamic_cast<FileSource*>(source));

	return true;
}

bool EventLoop::set_source_handler(EventSource *source, EventHandler handler)
{
	if (contains_source(source))
		source->loop_data.handler = handler;
	return false;
}

bool EventLoop::remove_event_source(EventSource *source)
{
	if (! contains_source(source))
		return false;

	if (source->is_idle_source())
	{
		impl->idle_sources.erase(remove(begin(impl->idle_sources),
										end(impl->idle_sources), source),
								 end(impl->idle_sources));
	}
	else if (source->is_timeout_source())
	{
		impl->timeout_sources.erase(remove(begin(impl->timeout_sources),
										   end(impl->timeout_sources), source),
									end(impl->timeout_sources));
	}
	else if (source->is_file_source())
	{
		impl->pollable_sources.erase(remove(begin(impl->pollable_sources),
											end(impl->pollable_sources), source),
									 end(impl->pollable_sources));
	}

	impl->sources.erase(remove(begin(impl->sources),
							   end(impl->sources), source),
						end(impl->sources));

	impl->source_set.erase(source);

	delete source;

	return true;
}

EventSource *EventLoop::add_file(int fd, FileEvents events, EventHandler handler)
{
	auto source = new FileSource(fd, events);
	if (! add_event_source(source, handler))
	{
		delete source;
		return nullptr;
	}
	return source;
}

EventSource *EventLoop::add_idle(EventHandler handler)
{
	auto source = new IdleSource;
	if (! add_event_source(source, handler))
	{
		delete source;
		return nullptr;
	}
	return source;
}

EventSource *EventLoop::add_timeout(int timeout_ms, EventHandler handler)
{
	auto source = new TimeoutSource(timeout_ms);
	if (! add_event_source(source, handler))
	{
		delete source;
		return nullptr;
	}
	return source;
}

} // namespace Grinder
