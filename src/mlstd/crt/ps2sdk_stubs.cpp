/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// Stubs for PS2SDK

extern "C" {
int printf(const char* __restrict format, ...);

// void _ps2sdk_args_parse(int argc, char ** argv) {
//	(void)argc;
//	(void)argv;
// }

// Remove code relating to setting up the time zone,
// we don't need it

void _ps2sdk_libc_init() {
	printf("libc init\n");
}

void _ps2sdk_libc_deinit() {
	printf("libc deinit\n");
}

void _ps2sdk_timezone_update() {
}

void __libpthreadglue_init() {

}

void __libpthreadglue_deinit() {

}

}