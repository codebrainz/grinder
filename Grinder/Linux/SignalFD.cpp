//
// SignalFD.cpp - This file is part of the Grinder library
//
// Copyright (c) 2015 Matthew Brush <mbrush@codebrainz.ca>
// All rights reserved.
//

#include <Grinder/Linux/SignalFD.h>
#include <unistd.h>
#include <sys/signalfd.h>

namespace Grinder
{

static inline sigset_t empty_signals()
{
	sigset_t sset;
	::sigemptyset(&sset);
	return sset;
}

SignalFD::SignalFD(bool block_sigs)
	: SignalFD(nullptr, block_sigs)
{
}

SignalFD::SignalFD(const sigset_t *sigs, bool block_sigs)
	: FileSource(-1, FileEvents::INPUT),
	  signals(empty_signals()),
	  m_sigs(sigs ? *sigs : empty_signals()),
	  m_block_sigs(block_sigs)
{
	if (sigs)
		update_signals();
}

void SignalFD::add(int signo)
{
	::sigaddset(&m_sigs, signo);
	update_signals();
}

void SignalFD::remove(int signo)
{
	::sigdelset(&m_sigs, signo);
	update_signals();
}

bool SignalFD::check()
{
	return (revents & FileEvents::INPUT);
}

bool SignalFD::dispatch(EventHandler &func)
{
	signalfd_siginfo info;
	::sigemptyset(&signals);
	while (::read(fd, &info, sizeof(signalfd_siginfo)) == sizeof(signalfd_siginfo))
		::sigaddset(&signals, info.ssi_signo);
	if (! ::sigisemptyset(&signals))
		return FileSource::dispatch(func);
	return true;
}

void SignalFD::update_signals()
{
	fd = signalfd(fd, &m_sigs, SFD_NONBLOCK | SFD_CLOEXEC);
	if (m_block_sigs)
	{
		sigset_t allset;
		::sigfillset(&allset);
		::sigprocmask(SIG_UNBLOCK, &allset, nullptr);
		::sigprocmask(SIG_BLOCK, &m_sigs, nullptr);
	}
}

} // namespace Grinder
