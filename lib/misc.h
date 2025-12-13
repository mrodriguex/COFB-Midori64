#ifndef MISC_H
#define MISC_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>

typedef uint8_t 	nibble;
typedef uint64_t 	bloque;
typedef bloque * 	bloques;
typedef uint8_t		byte;
typedef uint32_t	tn2;
typedef char * cad;

typedef struct VecS{
	byte *v;
	byte t;
	} *vect;

byte hayNL(cad a);
byte esHex(char car);
byte techo(double a);
void impBin(tn2 num);
tn2 leeBin(cad a);
bloque reverse(bloque a);

#endif
