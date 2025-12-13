/*
 * ============================================================================
 * File: cofb.c
 * Purpose: COFB (Confidentiality and Authenticity) Authenticated Encryption Mode
 * 
 * COFB mode provides:
 * - Confidentiality: Message encryption
 * - Authenticity: Authentication tag generation and verification
 * 
 * Mode Structure:
 *   1. Derive initial state Y from nonce using Midori cipher
 *   2. Generate mask stream from Y
 *   3. For each message block:
 *      - Apply Galois field masking
 *      - Apply nonlinear transformation via Midori
 *      - Encrypt message block
 *   4. Final state becomes authentication tag
 * 
 * Galois Field Operations:
 *   - Uses GF(2^32) for efficient mask computation
 *   - Doubling (multiply by 2): conditional shift and polynomial XOR
 *   - Tripling (multiply by 3): double + add in GF
 *   - General operations: table-based mask generation
 * 
 * Author: COFB-Midori64 Project
 * Date: December 2025
 * ============================================================================
 */

#include"cofb.h"

/*
 * Function: COFB()
 * 
 * Purpose: Encrypts message and generates authentication tag
 * 
 * Parameters:
 *   - bloques K: 128-bit encryption key (array of 2 blocks)
 *   - bloque N: 64-bit nonce
 * 
 * Returns:
 *   - bloque: Authentication tag T (authentication value)
 * 
 * Algorithm:
 *   1. Y ← Midori(N, K, 0)  [Initialize from nonce]
 *   2. β ← MaskGen(Y)       [Generate base mask]
 *   3. For each plaintext block B:
 *      a. GY ← MulGY(Y)     [Apply G-multiplication]
 *      b. BGY ← B ⊕ GY      [XOR with modified Y]
 *      c. msk ← Mask(β, B, counter)  [Generate block mask]
 *      d. X ← (msk || BGY)  [Combine mask and block]
 *      e. Y ← Midori(X, K, 0)  [Apply cipher]
 *      f. C ← Y ⊕ B         [Produce ciphertext]
 *   4. T ← Y                [Final state is tag]
 * 
 * Input/Output:
 *   - Reads plaintext blocks via stdin in hex format
 *   - Prints ciphertext to stdout
 *   - Returns final authentication tag
 * 
 * Details:
 *   - Block size: 64 bits (8 bytes)
 *   - Mask is 32-bit value (shifted and extended to 64-bit)
 *   - Counter (exp) increments with each message block
 *   - Input parsing handles whitespace and formatting
 */
bloque COFB(bloques K, bloque N)
	{
	byte exp = 1;				// Counter for masking operations
	byte aom = 0;				// Block counter
	bloque Y;					// Cipher state
	bloque beta;				// Base mask value
	bloque B;					// Current plaintext block
	bloque msk;					// Block-specific mask
	bloque GY;					// G-multiplied state
	bloque BGY;					// B XOR GY
	bloque X;					// Input to cipher
	bloque C;					// Ciphertext block
	bloque T;					// Authentication tag

	// Initialize from nonce
	Y = midori(N,K,0);
	// Generate base mask from Y
	beta = maskGen(Y);
	
	// Store initial mask for Galois field operations
	mx2	= beta;
	
	// Clear stdin buffer for hex input parsing
	while(scanf("\n",NULL)!=0);

	char chr;
	scanf("%c",&chr);

	printf("C: \t");
	
	// Process plaintext blocks in a loop
	while(exp<4)
		{
		char blq[17] = {};		// Buffer for hex string
		char bff[16] = {};		// Buffer for partial input
				
		// Read up to 15 characters of hex
		scanf("%15c",bff);
		sprintf(blq,"%c%15s",chr,bff);
		scanf("%c",&chr);
		// Parse hex string to 64-bit block
		sscanf(blq,"%016llx",&B);
			
		// Check for end of input (whitespace or newline)
		if(isblank(chr)!=0 || chr=='\n' || hayNL(blq)!=0)
			{
			exp++;
			aom++;
			while(scanf("\n",NULL)!=0);
			scanf("%c",&chr);
			}
			
		// Generate mask for this block
		msk = mask(beta, B, exp);
		// Compute G-multiplied state
		GY = mulGY(Y);	
		// XOR block with G-multiplied state
		BGY = B ^ GY;
		// Combine mask and BGY
		X = (msk << 32) ^ BGY;

		// Output ciphertext for blocks 3+ (skip initial blocks)
		if(exp > 2)
			{
			C = Y ^ B;
			printf("%016llx",C);
			}

		// Apply cipher to produce next state
		Y = midori(X,K,0);
		
		// Advance counter
		switch(exp)
			{
			case 0:
				exp++;
				break;
			case 2:
				exp++;
				break;
			case 4:
				exp++;
				break;
			}		
		}
	puts("");
	
	// Final tag is final state
	T = Y;
	
	return(T);
	}

/*
 * Function: dCOFB()
 * 
 * Purpose: Decrypts message and verifies authentication tag
 * 
 * Parameters:
 *   - bloques K: 128-bit encryption key
 *   - bloque N: 64-bit nonce
 *   - bloque T: Received authentication tag (for verification)
 * 
 * Returns:
 *   - bloque: Computed authentication tag T_
 * 
 * Algorithm:
 *   Similar to COFB but with ciphertext input:
 *   1. Repeat initialization as COFB
 *   2. For each ciphertext block C:
 *      - Compute Y' = Y ⊕ (BGY ⊕ msk)
 *      - Recover plaintext: M ← Y ⊕ C
 *      - Continue with recovered state
 *   3. Verification: Compare computed T_ with received T
 * 
 * Details:
 *   - Reverses the encryption process
 *   - Accepts ciphertext blocks instead of plaintext
 *   - Produces plaintext as output
 *   - Returns computed tag for verification
 */	
bloque dCOFB(bloques K, bloque N, bloque T)
	{
	byte exp = 1;				// Counter for masking operations
	byte aom = 0;				// Block counter
	bloque Y;					// Cipher state
	bloque beta;				// Base mask value
	bloque B;					// Current ciphertext block
	bloque msk;					// Block-specific mask
	bloque GY;					// G-multiplied state
	bloque BGY;					// B XOR GY
	bloque X;					// Input to cipher
	bloque C;					// Ciphertext block
	bloque T_;					// Computed authentication tag

	// Initialize from nonce (same as encryption)
	Y = midori(N,K,0);
	beta = maskGen(Y);
	
	mx2	= beta;
	
	while(scanf("\n",NULL)!=0);

	char chr;
	scanf("%c",&chr);

	printf("M: \t");
	
	// Process ciphertext blocks
	while(exp<4)
		{
		char blq[17] = {};
		char bff[16] = {};
				
		scanf("%15c",bff);
		sprintf(blq,"%c%15s",chr,bff);
		scanf("%c",&chr);
		sscanf(blq,"%016llx",&B);
			
		if(isblank(chr)!=0 || chr=='\n' || hayNL(blq)!=0)
			{
			exp++;
			aom++;
			while(scanf("\n",NULL)!=0);
			scanf("%c",&chr);
			}
		
		// Generate mask for this block
		msk = mask(beta, B, exp);
		// Compute G-multiplied state
		GY = mulGY(Y);	
		// XOR block with G-multiplied state
		BGY = B ^ GY;
		
		// For decryption, recover plaintext
		if(exp > 2)
			{
			BGY = Y ^ BGY;		// Reverse the XOR
			C = Y ^ B;			// Recover plaintext
			printf("%016llx",C);
			}		
		
		// Combine mask and BGY for next state
		X = (msk << 32) ^ BGY;		
		// Apply cipher to produce next state
		Y = midori(X,K,0);

		// Advance counter
		switch(exp)
			{
			case 0:
				exp++;
				break;
			case 2:
				exp++;
				break;
			case 4:
				exp++;
				break;
			}
		}

	puts("");
	
	// Computed tag
	T_ = Y;

	return(T_);
	}

/*****************************************************************************
 * GALOIS FIELD ARITHMETIC OPERATIONS
 * 
 * Operations in GF(2^32) with polynomial 0x1000000000000001b
 * Used for mask generation and cryptographic mixing
 *****************************************************************************/

/*
 * Function: maskGen()
 * 
 * Purpose: Extracts mask value from cipher output state
 * 
 * Parameters:
 *   - bloque Y0: Output from Midori cipher (64-bit)
 * 
 * Returns:
 *   - bloque: 32-bit mask value (in lower bits of 64-bit block)
 * 
 * Algorithm:
 *   Extracts upper 32 bits of Y0:
 *   β ← Y0[32:63]  (bits 32 to 63 of 64-bit value)
 * 
 * Details: Simple extraction used as seed for mask generation
 */
bloque maskGen(bloque Y0)
	{
	bloque beta;
	// Extract bits 16-47 (32-bit value)
	beta	= (Y0 >> 0x10) & 0xffffffff;
	return (beta);
	}

/*
 * Function: gsuma()
 * 
 * Purpose: Addition in GF(2^32) under polynomial 0x1000000000000001b
 * 
 * Parameters:
 *   - tn2 a, tn2 b: Two 32-bit values to add
 * 
 * Returns:
 *   - tn2: Result of GF addition (XOR operation)
 * 
 * Algorithm:
 *   1. XOR both inputs with polynomial to "normalize"
 *   2. XOR normalized values together
 *   3. Return result
 * 
 * Details: In GF(2), addition is XOR; polynomial transformation
 *          ensures proper field algebra
 */
tn2 gsuma(tn2 a, tn2 b)
	{
	// GF addition: XOR with polynomial transforms, then XOR inputs
	tn2 suma = (a ^ poliN) ^ (b ^ poliN);
	return(suma);
	}

/*
 * Function: gdoble()
 * 
 * Purpose: Doubling in GF(2^32) (multiplication by 2)
 * 
 * Parameters:
 *   - tn2 a: Value to double in Galois field
 * 
 * Returns:
 *   - tn2: Result of doubling (2 * a in GF(2^32))
 * 
 * Algorithm:
 *   1. Check if MSB (bit 31) is set
 *   2. If MSB=1: (a << 1) ⊕ poliN  [left-shift and XOR with polynomial]
 *   3. If MSB=0: a << 1              [simple left-shift]
 * 
 * Details: MSB check prevents overflow in GF arithmetic
 *          Polynomial XOR "reduces" the result back into GF
 */
tn2 gdoble(tn2 a)
	{
	// Check if MSB is set (0x80000000)
	tn2 doble = (a & maxPn) ? (a << 1) ^ poliN : a << 1;
	return(doble);
	}

/*
 * Function: gtriple()
 * 
 * Purpose: Tripling in GF(2^32) (multiplication by 3)
 * 
 * Parameters:
 *   - tn2 a: Value to triple in Galois field
 * 
 * Returns:
 *   - tn2: Result of tripling (3 * a in GF(2^32))
 * 
 * Algorithm:
 *   1. Compute doubled = gdoble(a)   [2 * a]
 *   2. Compute triple = gsuma(a, doubled)  [a + (2 * a) = 3 * a]
 *   3. Return result
 * 
 * Details: Uses GF addition (XOR) to combine a with 2*a
 */

tn2 gtriple(tn2 a)
	{
	tn2 doble = gdoble(a);			// Compute 2 * a
	tn2 triple = gsuma(a, doble);	// Compute a + (2 * a) = 3 * a
	return(triple);
	}

/*
 * Function: goper()
 * 
 * Purpose: Selects and computes Galois field operations based on counter
 * 
 * Parameters:
 *   - byte finB: Counter/mode selector (determines operation)
 *   - tn2 alf: Base value for Galois field operation
 * 
 * Returns:
 *   - tn2: Computed mask value
 * 
 * Operations (by finB value):
 *   - finB=1: mx2 = 2*mx2,   mask = mx2
 *   - finB=2: mx2x3 = 3*mx2, mask = mx2x3
 *   - finB=3: mx2 = 2*mx2, mx2x3 = 3*mx2, mask = mx2x3
 *   - finB=4: mx2x3x3 = 3*3*mx2, mask = mx2x3x3
 * 
 * Details: 
 *   - Maintains state in static variables (mx2, mx2x3, mx2x3x3)
 *   - Updates these powers of 2 and 3 for mask generation
 *   - Different operations per block ensure variety in masks
 */
tn2 goper(byte finB, tn2 alf)
	{
	tn2 mask = 0;
	
	// Select operation based on finB counter
	switch (finB)
		{
		case 1:
			// Double the current mask value
			mx2	= gdoble(mx2);
			mask	= mx2;
			break;
		case 2:
			// Triple the doubled value
			mx2x3	= gtriple(mx2);
			mask	= mx2x3;
			break;
		case 3:
			// Double again and triple the new doubled value
			mx2	= gdoble(mx2);
			mx2x3	= gtriple(mx2);
			mask	= mx2x3;
			break;
		case 4:
			// Triple of triple (9 * original)
			mx2x3x3	= gtriple(gtriple(mx2));
			mask	= mx2x3x3;
		}
	
	return(mask);
	}

/*
 * Function: mask()
 * 
 * Purpose: Generates per-block masking value using Galois field operations
 * 
 * Parameters:
 *   - bloque beta: Base mask derived from nonce
 *   - bloque B: Current plaintext/ciphertext block
 *   - byte finB: Block counter (determines GF operations)
 * 
 * Returns:
 *   - bloque: Computed mask for this block
 * 
 * Algorithm:
 *   1. Call goper(finB, beta) to compute Galois field value
 *   2. Return computed value as mask
 * 
 * Details: Wrapper around goper that applies block-dependent transformations
 */
bloque mask(bloque beta, bloque B, byte finB)
	{
	tn2 enmsk;		// Computed mask value
	byte i;

	// Apply Galois field operation to generate mask
	enmsk = goper(finB, beta);

	return (enmsk);
	}

/*
 * Function: mulGY()
 * 
 * Purpose: Applies G-multiplication to cipher state in Galois field
 * 
 * Parameters:
 *   - bloque Y: Cipher state (64-bit)
 * 
 * Returns:
 *   - bloque: Result of G-multiplication on Y
 * 
 * Algorithm:
 *   1. Left-shift Y by 16 bits: Y << 16
 *   2. Mask result to 64-bit: & 0xffffffffffffffff
 *   3. Right-shift Y by 48 bits: Y >> 48
 *   4. XOR with lower 16 bits of Y
 *   5. Combine shifted and XORed results: (shifted) | (XOR result)
 * 
 * Example: Provides non-linear transformation for mixing
 * 
 * Details: G-multiplication is part of COFB mode design for authentication
 */
bloque mulGY(bloque Y)
	{
	bloque GY;
	
	// Left-shift Y by 16 bits
	GY = (Y << 16) & 0xffffffffffffffff;
	// XOR with right-shifted portion: (Y >> 48) XOR (Y[0:16])
	GY = GY | (((Y >> 16) >> 32) ^ (Y & 0xffff));	
	return(GY);
	}

void Fmt(vect B, vect A, vect M)
	{
	byte i;
	byte j;
	byte a = A->t;
	byte m = M->t;
	byte l = (byte)techo((a+m)/n_8);
	
	memcpy(B->v, A->v, a);
	memcpy((B->v)+a, M->v, m);
		
	return;
	}

void barra(cad A)
	{
	int tA = strlen(A);
	cad B = A + ((tA/n_4)*n_4);
	byte t = strlen(B);
		
	
	if(t == 0)
		{
		if(tA == 0)
			{
			memset(&B[0], '0', 0x20);
			}
		}
	else if(t != 0x20)
		{
		B[t] = '8';
		memset(&B[t+1], '0', 0x20 - (t-1));
		}
	
	B[0x20] = 0;
	return;
	}
	

void xorBloq(vect BGYY, vect BGY, vect Y, byte j)
	{
	byte i;
	
	for(i=0;i<n_8;i++)
		{
		BGYY->v[i+(j*n_8)] = BGY->v[i+(j*n_8)] ^ Y->v[i+((j-1)*n_8)];
		}
	}

void xorBloqBGY(vect A, vect B, vect GY, byte j)
	{
	byte i;
	
	for(i=0;i<n_8;i++)
		{
		A->v[i+(j*n_8)] = B->v[i+(j*n_8)] ^ GY->v[i];
		}
	}

void xorBloqA(vect X, vect BGY, vect msk, byte j)
	{
	byte i;
	
	memcpy((X->v)+(j*n_8),(BGY->v)+(j*n_8),n_8);
	
	for(i=0;i<msk->t;i++)
		{
		X->v[i+(j*n_8)] = BGY->v[i+(j*n_8)] ^ msk->v[i];
		}
	}

void xorBloqB(vect C, vect Y, vect M, byte j)
	{
	byte i;

	for(i=0;i<n_8;i++)
		{
		C->v[i+(j*n_8)] = Y->v[(Y->t-M->t)+(i+((j-1)*n_8))] ^ M->v[i+(j*n_8)];
		}
	}

void xorBloqBd(vect M, vect Y, vect C, byte j)
	{
	byte i;
	byte a = (Y->t - C->t)/n_8;
	
	for(i=0;i<n_8;i++)
		{
		M->v[i+(j*n_8)] = Y->v[(i+((j+a-1)*n_8))] ^ C->v[i+(j*n_8)];
		}
	}

void impVect(vect A)
	{
	byte t = A->t;
	byte i;
	
	for(i=0;i<t;i++)
		{
		printf("%02x",A->v[i]);
		}
	puts("");
	
	}

vect genVect(byte t)
	{
	vect unVect;
	
	unVect	= (vect)malloc(sizeof(vect));
	
	if(unVect==NULL)
		{
		fprintf(stderr,"Error al asignar memoria\n");
		exit(1);
		}
	unVect->v	= (byte *)calloc(t,sizeof(byte));
	if(unVect->v==NULL)
		{
		fprintf(stderr,"Error al asignar memoria\n");
		exit(2);
		}

	unVect->t	= t;
	return(unVect);
	}

cad leerEnt()
	{
	char car;
	char tmp[n_4 + 1] = {};
	int l = 0;
	int t = n_4;
	byte ban;
	cad A = malloc(t * sizeof(char));
	cad B;

	do
		{
		
		if((l & n4m) == 0)
			{
			t = t+l+1;
			A = realloc(A,t * sizeof(char));
			}

		scanf("%c",&car);
		ban = esHex(car);
		A[l] = ban==0 ? car : 0;
		A[l+1] = 0;
		
		l++;
		}
	while(ban==0);

	return(A);
	}

vect cadToVect(cad A)
	{
	int t = (strlen(A)+1)>>1;
	int i;
	int x;
	char hex[0x03] = "  ";
	byte val;
	vect B = genVect(t);
	
	for(i=0; i<t; i++)
		{
		x=i<<1;
		hex[0] = A[x];
		hex[1] = A[x+1];
		val = strtol(hex, NULL, 16);
		B->v[i]=val;
		}	
	
	return(B);
	}

