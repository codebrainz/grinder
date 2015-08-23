#ifndef SIGNALSOURCE_H
#define SIGNALSOURCE_H

#include <Grinder/FileSource.h>
#include <signal.h>

namespace Grinder
{

class SignalSource : public FileSource
{
public:
	sigset_t signals;
	int signo;
	SignalSource(const sigset_t *sigs=nullptr, bool block_sigs=true);
	~SignalSource();
	void add(int signo);
	void remove(int signo);
	bool check() override;
	bool dispatch(EventHandler &func) override;

private:
	bool m_block_sigs;
	sigset_t m_old_sigs;
	void update_signals();
};

} // namespace Grinder

#endif // SIGNALSOURCE_H
