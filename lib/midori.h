#ifndef MIDORI_H
#define MIDORI_H

#include <misc.h>
//#include <immintrin.h>
//#include <wmmintrin.h>

//typedef __m128i	m128;
#define n	0x04	//4
#define nxn	0x10	//16
#define r	0x10	//16

//#define n 4
#define n_1	0x40	//numero de bits en una palabra
#define n_2	0x20	//numero de bits en media palabra
#define n_4	0x10	//numero de caracteres por bloque de longitud n
#define n_8	0x08	//numero de bytes por bloque de longitud n
#define n_10	0x04	//numero de bytes por bloque de longitud n/2
#define n_20	0x02	//numero de bytes por bloque de longitud n/4

#define maxPn 0x80000000	//ultimo bit de la media palabra
#define poliN 0x10000001b	//polinomio de la media palabra 0x1000000000000001b

#define n4m 0x07	//mascara para n_4
#define mskN4 0xffff	//mascara para n/4

#define Sb0		0xcad3ebf789150246
#define shuffleP	0x0a5fe4b193c67d28
#define shufflePInv	0x07e952bcf816ad43

#define keyAdd(A,B)(A^B)

typedef byte bloq[n_1];

typedef nibble *	nibbles;
typedef byte   *	bytes;

//#define obtNibble(S, pos)((S >> ((nxn-pos-1)<<1) >> ((nxn-pos-1)<<1)) & 0xf)

typedef uint16_t 	dbyte;
typedef uint8_t 	nibble;
typedef uint8_t 	byte;	

nibble obtNibble(bloque S, nibble pos);
bloque asgNibble(bloque S, byte pos, nibble val);
bloque keyGen(bloque RK[r], bloque Ki[2]);
bloque keyGenInv(bloque RK_1[r], bloque RK[r]);
bloque subCell(bloque S);
bloque shuffleCell(bloque S, byte inv);
bloque mixColumn(bloque S);
bloque midori(bloque S, bloque Ki[2], byte inv);

#endif
