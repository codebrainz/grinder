//
// FileEvents.h - This file is part of the Grinder library
//
// Copyright (c) 2015 Matthew Brush <mbrush@codebrainz.ca>
// All rights reserved.
//

#ifndef GRINDER_FILEEVENTS_H
#define GRINDER_FILEEVENTS_H

#include <poll.h>

namespace Grinder
{

enum class FileEvents
{
	NONE     = 0,
	INPUT    = POLLIN,
	OUTPUT   = POLLOUT,
	PRIORITY = POLLPRI,
	ERROR    = POLLERR,
	HANGUP   = POLLHUP,
};

} // namespace Grinder

static inline int operator&(Grinder::FileEvents ev1, Grinder::FileEvents ev2)
{
	return static_cast<int>(ev1) & static_cast<int>(ev2);
}

static inline int operator&(int ev1, Grinder::FileEvents ev2)
{
	return ev1 & static_cast<int>(ev2);
}

static inline Grinder::FileEvents operator|(Grinder::FileEvents ev1, Grinder::FileEvents ev2)
{
	return static_cast<Grinder::FileEvents>(static_cast<int>(ev1) | static_cast<int>(ev2));
}

#endif // GRINDER_FILEEVENTS_H

