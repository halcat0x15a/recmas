#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <linux/input.h>

#define EVENT "/dev/input/event0"

struct event {
  useconds_t time;
  struct input_event raw;
};

int main(int argc, char *argv[]) {
  int event_fd = open(EVENT, O_RDWR);
  struct timespec rec;
  int playing = 0;
  switch (argc) {
  case 1:
    for (;;) {
      struct input_event event;
      if (read(event_fd, &event, sizeof(event)) != sizeof(event)) {
	exit(EXIT_FAILURE);
      } else {
	if (rec.tv_sec && rec.tv_nsec) {
	  struct timespec now;
	  clock_gettime(CLOCK_MONOTONIC_RAW, &now);
	  useconds_t delta = (now.tv_sec - rec.tv_sec) * 1000000 + (now.tv_nsec - rec.tv_nsec) / 1000;
	  rec = now;
	  struct event data;
	  data.time = delta;
	  data.raw = event;
	  write(STDOUT_FILENO, &data, sizeof(data));
	} else if (BTN_TOUCH == event.code) {
	  clock_gettime(CLOCK_MONOTONIC_RAW, &rec);
	}
      }
    }
    return 0;
  case 2:
    for (;;) {
      struct input_event event;
      if (!playing) {
	if (read(event_fd, &event, sizeof(event)) != sizeof(event)) {
	  exit(EXIT_FAILURE);
	} else {
	  playing = BTN_TOUCH == event.code;
	}
      } else {
	struct event data;
	if (read(STDIN_FILENO, &data, sizeof(data)) != sizeof(data)) {
	  exit(EXIT_SUCCESS);
	} else {
	  usleep(data.time);
	  write(event_fd, &data.raw, sizeof(data.raw));
	}
      }
    }
    return 0;
  default:
    return 1;
  }
}
