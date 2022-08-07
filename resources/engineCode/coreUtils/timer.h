#ifndef TIMER
#define TIMER

#include <chrono>
// no nesting, but makes for a very simple interface
#define NOW std::chrono::high_resolution_clock::now()
static auto tInit = NOW;
static auto t1 = NOW;
static auto t2 = NOW;

// set base time
static void Tick () {
	t1 = NOW;
}
// get difference between base time and current time, return value in useconds
static float Tock () {
	t2 = NOW;
	return std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
}
// getting the time since the engine was started
static float TotalTime () {
	auto tNow = NOW;
	return std::chrono::duration_cast<std::chrono::microseconds>( tNow - tInit ).count();
}

#endif
