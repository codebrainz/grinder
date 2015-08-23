//
// main.cpp - This file is part of the Grinder library
//
// Copyright (c) 2015 Matthew Brush <mbrush@codebrainz.ca>
// All rights reserved.
//

#include <Grinder/Grinder>
#include <iostream>

using namespace std;
using namespace Grinder;

int main()
{
	EventLoop loop;

	//loop.add_idle([&](EventSource&) {
	//	cout << ".";
	//	return true;
	//});

	loop.add_timeout(1000, [&](EventSource&) {
		cout << "timer expired" << endl;
		return true;
	});

	loop.add_file(0, FileEvents::INPUT, [&](EventSource&) {
		string text;
		cin >> text;
		if (text.length() > 0)
			cout << "file text:" << text << endl;
		return true;
	});

#ifdef GRINDER_LINUX

	loop.add_event_source(new TimerFD(250), [&](EventSource&) {
		cout << "TimerFD expired" << endl;
		return true;
	});

	sigset_t ss;
	::sigemptyset(&ss);
	::sigaddset(&ss, SIGINT);
	::sigaddset(&ss, SIGTERM);
	loop.add_event_source(new SignalFD(&ss), [&](EventSource&) {
		cout << "Received signal, quitting" << endl;
		loop.quit();
		return true;
	});

#endif

	return loop.run();
}
