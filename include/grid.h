#ifndef GRID_H
#define GRID_H

#include "common.h"

typedef struct CellData {
  const uint32_t ra;
  const float alpha_n;
  const float alpha_m;
  const float b1;
  const float b2;
  const float d1;
  const float d2;
  const float dt;
} CellData;

typedef struct Grid {
  uint32_t height; // i
  uint32_t width;  // j
  CellData cell_data; // alpha, b, d, etc
  float *cells; // perform single initial heap allocation
} Grid;

void grid_init(Grid *grid, uint32_t width, uint32_t height);
void grid_free(Grid *grid);
void grid_print(Grid *grid);
void grid_randomize(Grid *grid);
void grid_step(Grid *grid);

#endif // GRID_H