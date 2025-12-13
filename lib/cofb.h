#ifndef COFB_H
#define COFB_H

#include <midori.h>

static tn2 mx2;
static tn2 mx2x3;
static tn2 mx2x3x3;

bloque COFB(bloques K, bloque N);
bloque dCOFB(bloques K, bloque N, bloque T);
bloque maskGen(bloque Y0);
tn2 gsuma(tn2 a, tn2 b);
tn2 gdoble(tn2 a);
tn2 gtriple(tn2 a);
tn2 goper(byte finB, tn2 alf);
bloque mask(bloque beta, bloque B, byte finB);
bloque mulGY(bloque Y);
void Fmt(vect B, vect A, vect M);
void barra(cad B);
void xorBloq(vect BGYY, vect BGY, vect Y, byte j);
void xorBloqBGY(vect A, vect B, vect GY, byte j);
void xorBloqA(vect X, vect BGY, vect msk, byte j);
void xorBloqB(vect C, vect Y, vect M, byte j);
void xorBloqBd(vect M, vect Y, vect C, byte j);
void impVect(vect A);
vect genVect(byte t);
cad leerEnt();
vect cadToVect(cad A);

#endif
