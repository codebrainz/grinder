//
// TimeoutSource.h - This file is part of the Grinder library
//
// Copyright (c) 2015 Matthew Brush <mbrush@codebrainz.ca>
// All rights reserved.
//

#ifndef GRINDER_TIMEOUTSOURCE_H
#define GRINDER_TIMEOUTSOURCE_H

#include <Grinder/EventSource.h>
#include <cstdint>

namespace Grinder
{

class TimeoutSource : public EventSource
{
public:
	TimeoutSource(int timeout=-1);
	bool prepare(int &max_timeout) override;
	bool check() override;
	bool dispatch(EventHandler &func) override { return func(*this); }
	bool is_timeout_source() const override final { return true; }

private:
	int m_timeout;
	std::uint64_t m_next_expiry;
	bool is_ready(int& max_timeout);
};

} // namespace Grinder

#endif // GRINDER_TIMEOUTSOURCE_H
