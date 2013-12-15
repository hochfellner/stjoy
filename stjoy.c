/*
 *   stjoy - cheat joystick driver
 *   Copyright 2013 Pau Oliva Fora <pof@eslack.org>
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation; either version 2
 *   of the License, or (at your option) any later version.
 *
 *   based on:
 *    https://github.com/poliva/opengalax
 *
 */

#include "stjoy.h"

#define DEBUG 0

#define VERSION "0.1"

void usage() {
	printf("stjoy v%s - (c)2013 Pau Oliva Fora <pof@eslack.org>\n", VERSION);
	printf("Usage: stjoy [options]\n");
	printf("	-f                   : run in foreground (do not daemonize)\n");
	printf("	-s <joystick-device> : default=/dev/input/js0\n");
	printf("	-u <uinput-device>   : default=/dev/uinput\n");
	exit (1);
}


int main (int argc, char *argv[]) {

	int foreground = 0;
	int opt;

	pid_t pid;
	ssize_t res;

	unsigned char buffer[8];

	struct input_event ev[6];
	struct input_event ev_button[22];
	struct input_event ev_sync;

	conf_data conf;
	conf = config_parse();

	while ((opt = getopt(argc, argv, "hfs:u:?")) != EOF) {
		switch (opt) {
			case 'h':
				usage();
				break;
			case 'f':
				foreground=1;
				break;
			case 's':
				sprintf(conf.joystick_device, "%s", optarg);
				break;
			case 'u':
				sprintf(conf.uinput_device, "%s", optarg);
				break;
			default:
				usage();
				break;
		}
	}

	if (!running_as_root()) {
		fprintf(stderr,"this program must be run as root user\n");
		exit (-1);
	}

	printf("stjoy v%s ", VERSION);
	fflush(stdout);

	if (!foreground) {

		if ((pid = fork()) < 0)
			exit(1);
		else
			if (pid != 0)
			exit(0);

		/* daemon running here */
		setsid();
		if (chdir("/") != 0) 
			die("Could not chdir");
		umask(0);
		printf("forked into background\n");
	} else
		printf("\n");

	/* create pid file */
	if (!create_pid_file())
		exit(-1);

	if (foreground) {
		printf ("\nConfiguration data:\n");
		printf ("\tjoystick_device=%s\n",conf.joystick_device);
		printf ("\tuinput_device=%s\n",conf.uinput_device);
	}

	// Open joystick port
	open_joystick_port(conf.joystick_device);

	// configure uinput
	setup_uinput_dev(conf.uinput_device);

	// handle signals
	signal_installer();

	// input sync signal:
	memset (&ev_sync, 0, sizeof (struct input_event));
	ev_sync.type = EV_SYN;
	ev_sync.code = 0;
	ev_sync.value = 0;

	// button press signals:
	memset (&ev_button, 0, sizeof (ev_button));

	ev_button[0].type = EV_KEY;
	ev_button[0].code = BTN_A;
	ev_button[0].value = 0;
	ev_button[1].type = EV_KEY;
	ev_button[1].code = BTN_A;
	ev_button[1].value = 1;

	ev_button[2].type = EV_KEY;
	ev_button[2].code = BTN_B;
	ev_button[2].value = 0;
	ev_button[3].type = EV_KEY;
	ev_button[3].code = BTN_B;
	ev_button[3].value = 1;

	ev_button[4].type = EV_KEY;
	ev_button[4].code = BTN_X;
	ev_button[4].value = 0;
	ev_button[5].type = EV_KEY;
	ev_button[5].code = BTN_X;
	ev_button[5].value = 1;

	ev_button[6].type = EV_KEY;
	ev_button[6].code = BTN_Y;
	ev_button[6].value = 0;
	ev_button[7].type = EV_KEY;
	ev_button[7].code = BTN_Y;
	ev_button[7].value = 1;

	ev_button[8].type = EV_KEY;
	ev_button[8].code = BTN_TL;
	ev_button[8].value = 0;
	ev_button[9].type = EV_KEY;
	ev_button[9].code = BTN_TL;
	ev_button[9].value = 1;

	ev_button[10].type = EV_KEY;
	ev_button[10].code = BTN_TR;
	ev_button[10].value = 0;
	ev_button[11].type = EV_KEY;
	ev_button[11].code = BTN_TR;
	ev_button[11].value = 1;

	ev_button[12].type = EV_KEY;
	ev_button[12].code = BTN_SELECT;
	ev_button[12].value = 0;
	ev_button[13].type = EV_KEY;
	ev_button[13].code = BTN_SELECT;
	ev_button[13].value = 1;

	ev_button[14].type = EV_KEY;
	ev_button[14].code = BTN_START;
	ev_button[14].value = 0;
	ev_button[15].type = EV_KEY;
	ev_button[15].code = BTN_START;
	ev_button[15].value = 1;

	ev_button[16].type = EV_KEY;
	ev_button[16].code = BTN_MODE;
	ev_button[16].value = 0;
	ev_button[17].type = EV_KEY;
	ev_button[17].code = BTN_MODE;
	ev_button[17].value = 1;

/*
	ev_button[18].type = EV_ABS;
	ev_button[18].code = ABS_RZ;
	ev_button[18].value = 0;
	ev_button[19].type = EV_ABS;
	ev_button[19].code = ABS_RZ;
	ev_button[19].value = 255;

	ev_button[20].type = EV_ABS;
	ev_button[20].code = ABS_Z;
	ev_button[20].value = 0;
	ev_button[21].type = EV_ABS;
	ev_button[21].code = ABS_Z;
	ev_button[21].value = 255;
*/

	ev_button[18].type = EV_KEY;
	ev_button[18].code = BTN_THUMBL;
	ev_button[18].value = 0;
	ev_button[19].type = EV_KEY;
	ev_button[19].code = BTN_THUMBL;
	ev_button[19].value = 1;

	ev_button[20].type = EV_KEY;
	ev_button[20].code = BTN_THUMBR;
	ev_button[20].value = 0;
	ev_button[21].type = EV_KEY;
	ev_button[21].code = BTN_THUMBR;
	ev_button[21].value = 1;

	// load X,Y into input_events
	memset (ev, 0, sizeof (ev));	//resets object

	ev[0].type = EV_ABS;
	ev[0].code = ABS_HAT0X;
	ev[0].value = 0;
	ev[1].type = EV_ABS;
	ev[1].code = ABS_HAT0X;
	ev[1].value = 1;
	ev[2].type = EV_ABS;
	ev[2].code = ABS_HAT0X;
	ev[2].value = -1;

	ev[3].type = EV_ABS;
	ev[3].code = ABS_HAT0Y;
	ev[3].value = 0;
	ev[4].type = EV_ABS;
	ev[4].code = ABS_HAT0Y;
	ev[4].value = 1;
	ev[5].type = EV_ABS;
	ev[5].code = ABS_HAT0Y;
	ev[5].value = -1;



	while (1) {


		memset (buffer, 0, sizeof (buffer));
		res = read (fd_joystick, &buffer, sizeof (buffer));
		if (res < 0)
			die ("error reading from joystick port");

		if (DEBUG || foreground)
			fprintf (stderr,"PDU:  %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7] );

		// A
		if (buffer[6]==1 && buffer[7]==0) {
			if (buffer[4]==0 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[0], sizeof (struct input_event)) < 0) die ("error: write");
			if (buffer[4]==1 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[1], sizeof (struct input_event)) < 0) die ("error: write");
		}

		// B
		if (buffer[6]==1 && buffer[7]==1) {
			if (buffer[4]==0 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[2], sizeof (struct input_event)) < 0) die ("error: write");
			if (buffer[4]==1 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[3], sizeof (struct input_event)) < 0) die ("error: write");
		}

		// X
		if (buffer[6]==1 && buffer[7]==2) {
			if (buffer[4]==0 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[4], sizeof (struct input_event)) < 0) die ("error: write");
			if (buffer[4]==1 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[5], sizeof (struct input_event)) < 0) die ("error: write");
		}

		// Y
		if (buffer[6]==1 && buffer[7]==3) {
			if (buffer[4]==0 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[6], sizeof (struct input_event)) < 0) die ("error: write");
			if (buffer[4]==1 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[7], sizeof (struct input_event)) < 0) die ("error: write");
		}

		// LB <---- cheat!
		if (buffer[6]==1 && buffer[7]==4) {
/*
			if (buffer[4]==0 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[8], sizeof (struct input_event)) < 0) die ("error: write");
			if (buffer[4]==1 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[9], sizeof (struct input_event)) < 0) die ("error: write");
*/
			// DP left on release, DP Right on press
			if (buffer[4]==0 && buffer[5]==0) {
				// x/neutral
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(500);
				// y/neutral
				if (write (fd_uinput, &ev[3], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(500);
				// x/left
				if (write (fd_uinput, &ev[2], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(20000);
				// x/neutral
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(27);
				// y/down
				if (write (fd_uinput, &ev[4], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(20000);
				// x/neutral
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(15000);
				// x/left
				if (write (fd_uinput, &ev[2], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(27);
				// press X
				if (write (fd_uinput, &ev_button[5], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(20000);
				// y/neutral
				if (write (fd_uinput, &ev[3], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(15000);
				// release X
				if (write (fd_uinput, &ev_button[4], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(15000);
				// x/neutral x
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(500);
			}

			if (buffer[4]==1 && buffer[5]==0) {
				// x/neutral
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(500);
				// y/neutral
				if (write (fd_uinput, &ev[3], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(500);
				// x/right
				if (write (fd_uinput, &ev[1], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(20000);
				// x/neutral
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(27);
				// y/down
				if (write (fd_uinput, &ev[4], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(20000);
				// x/neutral
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(15000);
				// x/right
				if (write (fd_uinput, &ev[1], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(27);
				// press X
				if (write (fd_uinput, &ev_button[5], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(20000);
				// y/neutral
				if (write (fd_uinput, &ev[3], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(15000);
				// release X
				if (write (fd_uinput, &ev_button[4], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(15000);
				// x/neutral x
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(500);

			}
		}

		// RB
		if (buffer[6]==1 && buffer[7]==5) {
			if (buffer[4]==0 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[10], sizeof (struct input_event)) < 0) die ("error: write");
			if (buffer[4]==1 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[11], sizeof (struct input_event)) < 0) die ("error: write");
		}

		if (buffer[6]==1 && buffer[7]==6) {
			if (buffer[4]==0 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[12], sizeof (struct input_event)) < 0) die ("error: write");
			if (buffer[4]==1 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[13], sizeof (struct input_event)) < 0) die ("error: write");
		}

		if (buffer[6]==1 && buffer[7]==7) {
			if (buffer[4]==0 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[14], sizeof (struct input_event)) < 0) die ("error: write");
			if (buffer[4]==1 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[15], sizeof (struct input_event)) < 0) die ("error: write");
		}

		if (buffer[6]==1 && buffer[7]==8) {
			if (buffer[4]==0 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[16], sizeof (struct input_event)) < 0) die ("error: write");
			if (buffer[4]==1 && buffer[5]==0)
				if (write (fd_uinput, &ev_button[17], sizeof (struct input_event)) < 0) die ("error: write");
		}

		// RT
		if (buffer[6]==2 && buffer[7]==5) {
			if (buffer[4]==1 && buffer[5]==0x80)
				if (write (fd_uinput, &ev_button[18], sizeof (struct input_event)) < 0) die ("error: write");
			if (buffer[4]==0xff && buffer[5]==0x7f)
				if (write (fd_uinput, &ev_button[19], sizeof (struct input_event)) < 0) die ("error: write");
		}

		// LT <---- cheat!

/* dragon punch */
#if 0
Event: time 1387128747.109831, type 3 (EV_ABS), code 16 (ABS_HAT0X), value 1      <---- x/right
Event: time 1387128747.197553, type 3 (EV_ABS), code 16 (ABS_HAT0X), value 0      <---- x/neutral
Event: time 1387128747.197580, type 3 (EV_ABS), code 17 (ABS_HAT0Y), value 1      <---- y/down
Event: time 1387128747.217594, type 3 (EV_ABS), code 16 (ABS_HAT0X), value -1     <---- x/left
Event: time 1387128747.277551, type 3 (EV_ABS), code 16 (ABS_HAT0X), value 0      <---- x/neutral
Event: time 1387128747.293556, type 3 (EV_ABS), code 16 (ABS_HAT0X), value 1      <---- x/right
Event: time 1387128747.293583, type 1 (EV_KEY), code 307 (BTN_X), value 1         <---- press X
Event: time 1387128747.345564, type 3 (EV_ABS), code 17 (ABS_HAT0Y), value 0      <---- y/neutral
Event: time 1387128747.365552, type 1 (EV_KEY), code 307 (BTN_X), value 0         <---- release X
Event: time 1387128747.381499, type 3 (EV_ABS), code 16 (ABS_HAT0X), value 0      <---- x/neutral
#endif



		if (buffer[6]==2 && buffer[7]==2) {
			/*
			if (buffer[4]==1 && buffer[5]==0x80)
				if (write (fd_uinput, &ev_button[20], sizeof (struct input_event)) < 0) die ("error: write");
			if (buffer[4]==0xff && buffer[5]==0x7f)
				if (write (fd_uinput, &ev_button[21], sizeof (struct input_event)) < 0) die ("error: write");
			*/

			// DP left on press, DP right on release

			if (buffer[4]==0xff && buffer[5]==0x7f) {
				// x/neutral
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(500);
				// y/neutral
				if (write (fd_uinput, &ev[3], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(500);
				// x/left
				if (write (fd_uinput, &ev[2], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(20000);
				// x/neutral
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(27);
				// y/down
				if (write (fd_uinput, &ev[4], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(20000);
				// x/neutral
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(15000);
				// x/left
				if (write (fd_uinput, &ev[2], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(27);
				// press X
				if (write (fd_uinput, &ev_button[5], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(20000);
				// y/neutral
				if (write (fd_uinput, &ev[3], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(15000);
				// release X
				if (write (fd_uinput, &ev_button[4], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(15000);
				// x/neutral x
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(500);
			}

			if (buffer[4]==1 && buffer[5]==0x80) {
/*
				// x/neutral
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(500);
				// y/neutral
				if (write (fd_uinput, &ev[3], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(500);
*/
#if 0
				// x/right
				if (write (fd_uinput, &ev[1], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(87722);
				// x/neutral
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(27);
				// y/down
				if (write (fd_uinput, &ev[4], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(20014);
				// x/left
				if (write (fd_uinput, &ev[2], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(59957);
				// x/neutral
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(16005);
				// x/right
				if (write (fd_uinput, &ev[1], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(27);
				// press X
				if (write (fd_uinput, &ev_button[5], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(51981);
				// y/neutral
				if (write (fd_uinput, &ev[3], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(19988);
				// release X
				if (write (fd_uinput, &ev_button[4], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(15947);
				// x/neutral x
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(500);
#endif
				// x/neutral
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(500);
				// y/neutral
				if (write (fd_uinput, &ev[3], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(500);
				// x/right
				if (write (fd_uinput, &ev[1], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(20000);
				// x/neutral
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(27);
				// y/down
				if (write (fd_uinput, &ev[4], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(20000);
				// x/neutral
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(15000);
				// x/right
				if (write (fd_uinput, &ev[1], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(27);
				// press X
				if (write (fd_uinput, &ev_button[5], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(20000);
				// y/neutral
				if (write (fd_uinput, &ev[3], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(15000);
				// release X
				if (write (fd_uinput, &ev_button[4], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(15000);
				// x/neutral x
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
				if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");
				usleep(500);

			}

		}



		if (buffer[6]==2 && buffer[7]==6) {
			if (buffer[4]==0 && buffer[5]==0)
				if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");
			if (buffer[4]==0xff && buffer[5]==0x7f)
				if (write (fd_uinput, &ev[1], sizeof (struct input_event)) < 0) die ("error: write");
			if (buffer[4]==1 && buffer[5]==0x80)
				if (write (fd_uinput, &ev[2], sizeof (struct input_event)) < 0) die ("error: write");

		}

		if (buffer[6]==2 && buffer[7]==7) {
			if (buffer[4]==0 && buffer[5]==0)
				if (write (fd_uinput, &ev[3], sizeof (struct input_event)) < 0) die ("error: write");
			if (buffer[4]==0xff && buffer[5]==0x7f)
				if (write (fd_uinput, &ev[4], sizeof (struct input_event)) < 0) die ("error: write");
			if (buffer[4]==1 && buffer[5]==0x80)
				if (write (fd_uinput, &ev[5], sizeof (struct input_event)) < 0) die ("error: write");

		}



		// Sync
		if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");



	}

	if (ioctl (fd_uinput, UI_DEV_DESTROY) < 0)
		die ("error: ioctl");

	close (fd_uinput);

	return 0;
}
