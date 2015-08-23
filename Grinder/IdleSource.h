//
// IdleSource.h - This file is part of the Grinder library
//
// Copyright (c) 2015 Matthew Brush <mbrush@codebrainz.ca>
// All rights reserved.
//

#ifndef GRINDER_IDLESOURCE_H
#define GRINDER_IDLESOURCE_H

#include <Grinder/EventSource.h>

namespace Grinder
{

class IdleSource : public EventSource
{
public:
	bool prepare(int&) override final { return true; }
	bool check() override final { return true; }
	bool dispatch(EventHandler &func) override { return func(*this); }
	bool is_idle_source() const override final { return true; }
};

} // namespace Grinder

#endif // GRINDER_IDLESOURCE_H
