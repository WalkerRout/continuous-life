
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grid.h"

#define RADIUS_OUTSIDE 11
#define ALPHA_N 0.028
#define ALPHA_M 0.147

static char cell_states[] = " .,-~=coaA@#"; // implicit null character
static const uint8_t cell_states_count = sizeof(cell_states) / sizeof(cell_states[0]) - 1; // sub 1 for \0

// file local prototypes
static float point_step(Grid *grid, size_t ci, size_t cj);
static inline char float_to_cell_state(float x);
static inline float randf(void);
static inline int32_t emod(int32_t x, int32_t n);
static inline float clampf(float x, float min, float max);

static inline float sigmoid(float x, float a, float alpha);
static inline float sigmoid_n(float x, float a, float b, float alpha_n);
static inline float sigmoid_m(float x, float y, float m, float alpha_m);
static inline float s(float n, float m, CellData *cell_data);

inline void grid_init(Grid *grid, uint32_t width, uint32_t height) {
  grid->width = width;
  grid->height = height;

  CellData cell_data = {
    // values chosen here are identical to the glider in the paper
    .ra = RADIUS_OUTSIDE,
    .alpha_n = ALPHA_N,
    .alpha_m = ALPHA_M,
    .b1 = 0.278f,
    .b2 = 0.365f,
    .d1 = 0.267f,
    .d2 = 0.445f,
    .dt = 0.05f
  };
  memcpy(&grid->cell_data, &cell_data, sizeof(CellData));

  grid->cells = (float*) calloc(width * height, sizeof(float));
}

inline void grid_free(Grid *grid) {
  if(grid->cells) {
    free(grid->cells);
    grid->cells = NULL;
  }
}

void grid_print(Grid *grid) {
  uint32_t width = grid->width;
  uint32_t height = grid->height;
 
  for(size_t i = 0; i < height; ++i) {
    for(size_t j = 0; j < width; ++j) {
      char c = float_to_cell_state(grid->cells[i * width + j]);
      printf(" %c%c ", c, c);
    }
    fputc('\n', stdout);
  }
}

inline void grid_randomize(Grid *grid) {
  for(size_t i = grid->width * grid->height; i != 0; --i)
    grid->cells[i-1] = randf();
}

void grid_step(Grid *grid) {
  const uint32_t width = grid->width;
  const uint32_t height = grid->height;

  float cells_diff[width * height];

  // step through each cell and determine its next value
  for(size_t ci = 0; ci < height; ++ci) {
    for(size_t cj = 0; cj < width; ++cj) {
      cells_diff[ci * width + cj] = point_step(grid, ci, cj);
    }
  }

  // update the current grid with next cell states
  for(size_t i = width * height; i != 0; --i) {
    size_t index = i - 1;
    grid->cells[index] += grid->cell_data.dt * cells_diff[index];
    grid->cells[index] = clampf(grid->cells[index], 0.0f, 1.0f);
  }
}

// file local definitions

static float point_step(Grid *grid, size_t ci, size_t cj) {
  // m and n represent area of 'circle', will need to sum area of circle in order to normalize them
  float M = 0, m = 0;
  float N = 0, n = 0;

  const uint32_t width = grid->width;
  const uint32_t height = grid->height;
  const int32_t ra = grid->cell_data.ra;
  const int32_t ra_sub_one = ra - 1;
  const float ri = ra / 3;
  
  for(int32_t di = -ra_sub_one; di <= ra_sub_one; ++di) {
    for(int32_t dj = -ra_sub_one; dj <= ra_sub_one; ++dj) {
      // bounds checking with wrap around (using euclidean modulo)
      int32_t i = emod(ci + di, height); // height
      int32_t j = emod(cj + dj, width);  // width

      // check inner first (m), then check outer (n)
      if(dj*dj + di*di <= ri*ri) { // use c^2 (ri*ri) instead of sqrt(a^2 + b^2) = c
        m += grid->cells[i * width + j];
        M += 1; // increment count of inner pixels
      } else if (dj*dj + di*di <= ra*ra) {
        n += grid->cells[i * width + j];
        N += 1; // increment count of outer pixels
      } else {}
    }
  }

  // normalization factors
  m /= M; // inner count
  n /= N; // outer count

  return 2 * s(n, m, &grid->cell_data) - 1; // normalize range from [0, 1] to [-1, 1] using `2r-1` formula
}

static inline char float_to_cell_state(float x) {
  // must subtract 1 from cell_states_count because of zero indexing (max is count-1)
  return cell_states[(size_t)(x * (cell_states_count - 1))];
}

static inline float randf(void) {
  return (float) rand() / (float) RAND_MAX;
}

static inline int32_t emod(int32_t x, int32_t n) { 
  return ((x % n) + n) % n; 
}

static inline float clampf(float x, float min, float max) {
  x = x < min ? min : x;    // if x is < lower bound, choose lower bound
  return x > max ? max : x; // if t > higher bound, choose higher bound
}

static inline float sigmoid(float x, float a, float alpha) {
  return 1.0f / (1.0f + expf(-(x - a) * 4 / alpha));
}

static inline float sigmoid_n(float x, float a, float b, float alpha_n) {
  return sigmoid(x, a, alpha_n) * (1 - sigmoid(x, b, alpha_n));
}

static inline float sigmoid_m(float x, float y, float m, float alpha_m) {
  return x * (1 - sigmoid(m, 0.5f, alpha_m)) + y * sigmoid(m, 0.5f, alpha_m);
}

static float s(float n, float m, CellData *cell_data) {
  float alpha_n = cell_data->alpha_n;
  float alpha_m = cell_data->alpha_m;

  float b1 = cell_data->b1;
  float b2 = cell_data->b2;
  float d1 = cell_data->d1;
  float d2 = cell_data->d2;

  float a = sigmoid_m(b1, d1, m, alpha_m);
  float b = sigmoid_m(b2, d2, m, alpha_m);

  return sigmoid_n(n, a, b, alpha_n);
}