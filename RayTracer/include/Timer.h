#pragma once

#include <chrono>
#include <thread>

typedef std::chrono::steady_clock::time_point timePoint;

class Timer
{
	timePoint startTime;

public:
	Timer() : startTime(std::chrono::steady_clock::now()) {};
	~Timer() = default;

	void reset() { startTime = std::chrono::steady_clock::now(); }
	long long  diffMS() { return  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count(); }
	int diffSEC() { return  std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - startTime).count(); }
	int microsecPassed() { return  std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime).count(); }
	int nanosecPassed() { return  std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - startTime).count(); }
};