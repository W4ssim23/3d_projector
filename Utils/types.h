#ifndef TYPES_H
#define TYPES_H

// A gray byte in an image, represents an intensity.
typedef unsigned char gray;
typedef unsigned char bit;

// A PGM file, containing a gray map and its metadata
typedef struct
{
  gray *graymap;
  char magic_number;
  int rows;
  int cols;
  int maxval;
} pgm_file;

// A pixel in an image, represents its 3 channels R, G and B
typedef struct
{
  bit red;
  bit green;
  bit blue;
} pixel;

// A PPM file, containing a pixel map and its metadata
typedef struct
{
  pixel *pixmap;
  char magic_number;
  int rows;
  int cols;
  int maxval;
} ppm_file;

#endif
