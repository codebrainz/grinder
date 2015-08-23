#include <Grinder/SignalSource.h>
#include <Grinder/FileSource.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <unistd.h>

using namespace std;

namespace Grinder
{

static const int PIPE_READ = 0;
static const int PIPE_WRITE = 1;
static int pipe_fds[2] = { -1, -1 };

static void signal_handler(int signo)
{
	uint32_t usigno = signo;
	::write(pipe_fds[PIPE_WRITE], &usigno, sizeof(uint32_t));
}

SignalSource::SignalSource(const sigset_t *sigs, bool block_sigs)
	: FileSource(-1, FileEvents::INPUT),
	  signo(0),
	  m_block_sigs(block_sigs)
{
	if (m_block_sigs)
		::sigprocmask(SIG_BLOCK, nullptr, &m_old_sigs);

	if (sigs)
		signals = *sigs;
	else
		::sigemptyset(&signals);

	if (pipe_fds[0] == -1 && pipe_fds[1] == -1)
	{
		if (::pipe(pipe_fds) == 0)
			fd = pipe_fds[PIPE_READ];
	}

	update_signals();
}

SignalSource::~SignalSource()
{
	::close(pipe_fds[0]);
	::close(pipe_fds[1]);
	pipe_fds[0] = pipe_fds[1] = -1;

	if (m_block_sigs)
	{
		sigset_t fullset;
		::sigfillset(&fullset);
		::sigprocmask(SIG_UNBLOCK, &fullset, nullptr);
		::sigprocmask(SIG_BLOCK, &m_old_sigs, nullptr);
	}
}

void SignalSource::add(int signo)
{
	::sigaddset(&signals, signo);
	update_signals();
}

void SignalSource::remove(int signo)
{
	::sigdelset(&signals, signo);
	update_signals();
}

bool SignalSource::check()
{
	return (revents & FileEvents::INPUT);
}

bool SignalSource::dispatch(EventHandler &func)
{
	uint32_t sig = 0;
	if (::read(fd, &sig, sizeof(uint32_t)) == sizeof(uint32_t))
	{
		signo = sig;
		return FileSource::dispatch(func);
	}
	return true;
}

void SignalSource::update_signals()
{
	// fixme: how to iterate a sigset?
	for (int i=0; i < 128 /* FIXME: ugh */; i++)
	{
		int res = ::sigismember(&signals, i);
		if (res > 0)
			::signal(i, signal_handler);
		else if (res == 0)
			::signal(i, SIG_DFL);
	}

	if (m_block_sigs)
		::sigprocmask(SIG_UNBLOCK, &signals, nullptr);
}

} // namespace Grinder
