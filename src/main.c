
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <locale.h>

#include "grid.h"

static uint8_t keep_alive = 1; 
static Grid grid = {0};

// file local prototypes
static inline void sigint_exit(int sig);
static inline void clear_screen(void);

int main(int argc, char *argv[]) {
  // based on paper at https://arxiv.org/abs/1111.1567

  // prep seed
  // time returns seconds since epoch, updates seed *every second*
  char *__seed_end;
  uint64_t seed = argc > 1
    ? (uint64_t) strtol(argv[1], &__seed_end, 10)
    : (uint64_t) time(NULL);

  if(errno == ERANGE) {
    (void) fprintf(stderr, "Error - invalid unsigned integer seed, please try again\n");
    exit(EXIT_FAILURE);
  }

  // global set up
  srand(seed);
  signal(SIGINT, sigint_exit);

  const uint32_t height = 30;
  const uint32_t width = 30;

  // main logic
  grid_init(&grid, width, height);
  grid_randomize(&grid);

  while(keep_alive) {
    clear_screen();

    grid_step(&grid);
    grid_print(&grid);

    (void) printf("seed number - %ld\n", seed);
    sleep(100);
  }

  grid_free(&grid);


  return 0;
}

// file local functions

static inline void sigint_exit(int sig) {
  (void) sig;

  printf("- removing lifeforms and shutting down...\n");

  keep_alive = 0;
}

static inline void clear_screen(void) {
  printf("\x1B[2J\x1B[1;1H");
  fflush(stdout);
}