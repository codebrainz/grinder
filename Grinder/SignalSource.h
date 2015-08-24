//
// SignalSource.h - This file is part of the Grinder library
//
// Copyright (c) 2015 Matthew Brush <mbrush@codebrainz.ca>
// All rights reserved.
//

#ifndef GRINDER_SIGNALSOURCE_H
#define GRINDER_SIGNALSOURCE_H

#include <Grinder/FileSource.h>
#include <signal.h>

namespace Grinder
{

class SignalSource : public FileSource
{
public:
	int signo; // when event is dispatched, contains the signal number
	virtual ~SignalSource();
	void add(int signo);
	void remove(int signo);
	bool check() override { return (revents & FileEvents::INPUT); }
	void update();

protected:
	SignalSource(bool manage_proc_mask=false);
	SignalSource(const sigset_t *sigs, bool manage_proc_mask=false);
	virtual void update_signals(const sigset_t *sigs, bool manage_proc_mask) = 0;

private:
	sigset_t m_sigs;
	sigset_t m_old_sigs;
	bool m_manage_proc_mask;
};

} // namespace Grinder

#endif // GRINDER_SIGNALSOURCE_H
