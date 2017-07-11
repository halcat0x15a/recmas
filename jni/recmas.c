#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <linux/input.h>

#define EVENT "/dev/input/event0"

struct event {
  useconds_t time;
  struct input_event input;
};

useconds_t delta(struct timespec time) {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC_RAW, &now);
  return (now.tv_sec - time.tv_sec) * 1000000 + (now.tv_nsec - time.tv_nsec) / 1000;
}

int record() {
  int event_fd = open(EVENT, O_RDONLY);
  bool recording = false;
  struct timespec start_time;
  for (;;) {
    struct input_event input;
    if (read(event_fd, &input, sizeof(input)) != sizeof(input)) {
      exit(EXIT_FAILURE);
    }
    if (recording) {
      struct event event;
      event.time = delta(start_time);
      event.input = input;
      write(STDOUT_FILENO, &event, sizeof(event));
    } else if (input.code == BTN_TOUCH) {
      clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);
      recording = true;
    }
  }
}

int play() {
  int event_fd = open(EVENT, O_RDWR);
  bool playing = false;
  struct timespec start_time;
  for (;;) {
    if (playing) {
      struct event event;
      if (read(STDIN_FILENO, &event, sizeof(event)) != sizeof(event)) {
	exit(EXIT_SUCCESS);
      }
      while (delta(start_time) < event.time);
      write(event_fd, &event.input, sizeof(event.input));
    } else {
      struct input_event input;
      if (read(event_fd, &input, sizeof(input)) != sizeof(input)) {
	exit(EXIT_FAILURE);
      }
      if (input.code == BTN_TOUCH) {
	clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);
	playing = true;
      }
    }
  }
}

int print() {
  for (;;) {
    struct event event;
    if (read(STDIN_FILENO, &event, sizeof(event)) != sizeof(event)) {
      exit(EXIT_SUCCESS);
    }
    printf("%llu %d %d %d\n", (uint64_t) event.time, event.input.type, event.input.code, event.input.value);
  }
}

int main(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "rwp")) != -1) {
    switch (opt) {
    case 'r':
      return record();
    case 'w':
      return play();
    case 'p':
      return print();
    default:
      exit(EXIT_FAILURE);
    }
  }
}
