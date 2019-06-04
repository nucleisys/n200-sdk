#include <stdio.h>
#include "cpuidh.h"
#include "config.h"

volatile SPDP  theseSecs = 0.0;
volatile SPDP  startSecs = 0.0;
volatile SPDP  secs;

SPDP time()
{


  return (SPDP) get_timer_value() / get_timer_freq();
}

void getSecs()
{
	theseSecs = time() ;
	return;
}

void start_time()
{
	getSecs();
	startSecs = theseSecs;
	return;
}

void end_time()
{
	getSecs();
	secs = theseSecs - startSecs;
	return;
}
