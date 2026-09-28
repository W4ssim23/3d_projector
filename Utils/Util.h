// Headers for PBM / PPM image handling */

#ifndef UTIL_H
#define UTIL_H

#include "types.h"
#include <stdio.h>

// TODO: write doc
char pm_getc(FILE *file);

// TODO: write doc
bit pm_getbit(FILE *file);

// TODO: write doc
unsigned char pm_getrawbyte(FILE *file);

// TODO: write doc
int pm_getint(FILE *file);

void pm_error(char *text);

pgm_file read_pgm(char *filename);
void write_pgm(pgm_file image, char *filename);

ppm_file read_ppm(char *filename);
void write_ppm(ppm_file image, char *filename);

// added for pam
void write_pam(ppm_file image, int *tr, char *filename);

#endif