//
// SignalFD.h - This file is part of the Grinder library
//
// Copyright (c) 2015 Matthew Brush <mbrush@codebrainz.ca>
// All rights reserved.
//

#ifndef GRINDER_LINUX_SIGNALFD_H
#define GRINDER_LINUX_SIGNALFD_H

#include <Grinder/Platform.h>
#ifdef GRINDER_LINUX

#include <Grinder/SignalSource.h>

namespace Grinder
{

class SignalFD : public SignalSource
{
public:
	SignalFD(bool manage_proc_mask=false);
	SignalFD(const sigset_t *sigs, bool manage_proc_mask=false);

	bool dispatch(EventHandler &func) override;

protected:
	void update_signals(const sigset_t *sigs, bool manage_proc_mask);
};

} // namespace Grinder

#endif // GRINDER_LINUX

#endif // GRINDER_LINUX_SIGNALFD_H
