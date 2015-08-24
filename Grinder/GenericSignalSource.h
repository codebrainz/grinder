#ifndef GRINDER_GENERICSIGNALSOURCE_H
#define GRINDER_GENERICSIGNALSOURCE_H

#include <Grinder/SignalSource.h>

namespace Grinder
{

class GenericSignalSource : public SignalSource
{
public:
	GenericSignalSource(bool manage_proc_mask=false);
	GenericSignalSource(const sigset_t *sigs=nullptr, bool manage_proc_mask=false);
	~GenericSignalSource();
	bool dispatch(EventHandler &func) override;

protected:
	void update_signals(const sigset_t *sigs, bool manage_proc_mask);
};

} // namespace Grinder

#endif // GRINDER_GENERICSIGNALSOURCE_H
