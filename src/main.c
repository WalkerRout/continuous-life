
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <locale.h>

#include "grid.h"

#define WIDTH  30
#define HEIGHT 30

static uint8_t keep_alive = 1; 
static Grid grid = {0};

// file local prototypes
static inline void sigint_exit(int sig);
static inline void clear_screen(void);

int main(int argc, char *argv[]) {
  // based on paper at https://arxiv.org/abs/1111.1567

  // prep seed
  // time returns seconds since epoch, updates seed *every second*
  char *__strtol_end;
  uint64_t seed = argc > 1
    ? (uint64_t) strtol(argv[1], &__strtol_end, 10)
    : (uint64_t) time(NULL);

  if(seed == 0) seed = (uint64_t) time(NULL);

  if(errno == ERANGE) {
    (void) fprintf(stderr, "Error - invalid unsigned integer seed, please try again\n");
    exit(EXIT_FAILURE);
  }

  const uint32_t width = argc > 2
    ? (uint32_t) strtol(argv[2], &__strtol_end, 10)
    : (uint32_t) WIDTH;

  if(errno == ERANGE) {
    (void) fprintf(stderr, "Error - invalid unsigned integer width, please try again\n");
    exit(EXIT_FAILURE);
  }

  const uint32_t height = argc > 3
    ? (uint32_t) strtol(argv[3], &__strtol_end, 10)
    : (uint32_t) HEIGHT;

  if(errno == ERANGE) {
    (void) fprintf(stderr, "Error - invalid unsigned integer height, please try again\n");
    exit(EXIT_FAILURE);
  }

  // global set up
  srand(seed);
  signal(SIGINT, sigint_exit);

  // main logic
  grid_init(&grid, width, height);
  grid_randomize(&grid);

  while(keep_alive) {
    clear_screen();

    grid_step(&grid);
    grid_print(&grid);

    // TODO: figure out how to print uint64_t in ISO C???
    (void) fprintf(stdout, "seed number - %lu\n", (unsigned long) seed);
    sleep(16);
  }

  grid_free(&grid);
  
  (void) fprintf(stdout, "\033[1;1H""\033[2J");

  return 0;
}

// file local functions

static inline void sigint_exit(int sig) {
  (void) sig;

  (void) fprintf(stdout, "- removing lifeforms and shutting down...\n");
  keep_alive = 0;
}

static inline void clear_screen(void) {
  (void) fprintf(stdout, "\033[1;1H");
}
