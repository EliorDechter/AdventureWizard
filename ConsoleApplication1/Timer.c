#if 0
/*
 * ===============================================================
 *
 *                          TIMER
 *
 * ===============================================================
 */

typedef struct TimeSystem {
	double total_time_in_seconds;
} TimeSystem;

TimeSystem time_system;

typedef struct Timer {
	double time;
	double time_elapsed;
} Timer;

struct { char* key; Timer value; } *timer_map;

typedef struct TimeInSeconds {
	double time;
} TimeInSeconds;

double ConvertSecondsToMilliseconds(TimeInSeconds time) {
	double result = time.time * 1000;

	return result;
}

#include <stdbool.h>


bool DoTimer(char* name, float time) {
	int i = shgeti(timer_map, name);
	if (i == -1) {
		Timer timer = {
			.time = time
		};

		shput(timer_map, name, timer);

		return true;
	}

	Timer* timer = &timer_map[i].value;
	timer->time_elapsed += 0.016;

	if (timer->time_elapsed <= timer->time) {
		return true;
	}
	else {
		shdel(timer_map, name);
		return false;
	}
}

bool DoTimerSeconds(char* name, TimeInSeconds seconds) {
	DoTimer(name, ConvertSecondsToMilliseconds(seconds));
}

void CountTime() {
	time_system.total_time_in_seconds += 0.016;
	assert(time_system.total_time_in_seconds <= DBL_MAX);
}
#endif