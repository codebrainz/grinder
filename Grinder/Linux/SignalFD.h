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

#include <Grinder/FileSource.h>
#include <signal.h>

namespace Grinder
{

class SignalFD : public FileSource
{
public:
	sigset_t signals;

	SignalFD(bool block_sigs=true);
	SignalFD(const sigset_t *sigs, bool block_sigs=true);

	void add(int signo);
	void remove(int signo);

	bool check() override;
	bool dispatch(EventHandler &func) override;

private:
	sigset_t m_sigs;
	bool m_block_sigs;
	void update_signals();
};

} // namespace Grinder

#endif // GRINDER_LINUX

#endif // GRINDER_LINUX_SIGNALFD_H
