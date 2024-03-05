/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// Stubs for PS2SDK, so we don't bring in bulky code
// or have to link in the entirity of it

extern "C" {
int printf(const char* __restrict format, ...);

void _ps2sdk_timezone_update() {
}

void _libcglue_rtc_update() {

}

// HACK(lily): It's techinicially supposed to be possible to override the
// pthreadglue functions to do what we want here, however for some reason,
// that doesn't work here.
//
// ... So I'm doing the very-much-bad-idea of just
// overriding the pthread_* functions to be no-ops. This works perfectly,
// for some reason I'd rather not know.
//
// Before someone asks, yes, this is a hack, and I'm not ashamed of it.

void pthread_init() {

}

void pthread_terminate() {

}

}
