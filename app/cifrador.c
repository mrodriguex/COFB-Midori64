/*
 * ============================================================================
 * File: cifrador.c
 * Purpose: Main application/CLI interface for COFB-Midori64 encryption
 * 
 * This is the entry point for the encryption/decryption application.
 * 
 * Functionality:
 * 1. Reads 128-bit encryption key (two 64-bit hex values)
 * 2. Reads 64-bit nonce/IV (one 64-bit hex value)
 * 3. Encrypts message using COFB mode
 * 4. Decrypts and verifies authentication tag
 * 5. Displays results to stdout
 * 
 * Input Format:
 *   - Line 1: Two 64-bit hex values (K0 and K1) for 128-bit key
 *   - Line 2: One 64-bit hex value (N) for nonce
 *   - Subsequently: Message blocks via COFB function stdin
 * 
 * Output Format:
 *   - K:  [128-bit key in hex]
 *   - N:  [64-bit nonce in hex]
 *   - C:  [Ciphertext blocks]
 *   - T:  [Authentication tag]
 *   - T_: [Verification tag]
 * 
 * Author: COFB-Midori64 Project
 * Date: December 2025
 * ============================================================================
 */

#include"cofb.h"

/*
 * Function: main()
 * 
 * Purpose: Initializes cryptographic system and performs encryption/decryption
 * 
 * Returns:
 *   - int: 0 on successful execution
 * 
 * Algorithm:
 *   1. Allocate memory for key, nonce, and ciphertext
 *   2. Read 128-bit key (two 64-bit hex values)
 *   3. Read 64-bit nonce (one 64-bit hex value)
 *   4. Display key and nonce
 *   5. Encrypt: Call COFB(K, N) to get authentication tag T
 *   6. Decrypt: Call dCOFB(K, N, T) to recover plaintext and verify
 *   7. Display tags
 *   8. Return success
 * 
 * Variables:
 *   - K[2]: 128-bit key split into two 64-bit blocks
 *   - N: 64-bit nonce
 *   - T: Authentication tag from encryption
 *   - T_: Authentication tag from decryption
 *   - Various legacy vectors (commented out)
 */
int main()
	{
	// Deprecated: Legacy string variables for user input
	//cad cadK;
	//cad cadN;
	//cad cadA;
	//cad cadM;
	
	// Deprecated: Legacy vector structures for comprehensive messaging
	//vect llave	= genVect(n);
	//vect Kinv	= genVect(n);
	//vect cifrado	= genVect(n_8);
	
	// 128-bit encryption key (two 64-bit blocks)
	bloque K[2];
	// 64-bit nonce/initialization vector
	bloque N;
	// Authentication tags
	bloque T;	// Tag from encryption
	bloque T_;	// Tag from decryption/verification
	
	// ========================================================================
	// KEY AND NONCE INPUT
	// ========================================================================
	
	// Read key: Two 64-bit hex values (128 bits total)
	// Format: %016llx = read up to 16 hex digits as 64-bit unsigned long long
	scanf("%016llx%016llx",&K[0],&K[1]);
	
	// Read nonce: One 64-bit hex value (64 bits)
	scanf("%08llx",&N);

	// ========================================================================
	// DISPLAY INPUT PARAMETERS
	// ========================================================================
	
	// Print received key
	printf("K: \t%016llx%016llx\n",K[0],K[1]);
	// Print received nonce
	printf("N: \t%016llx\n",N);
	
	// ========================================================================
	// ENCRYPTION PHASE
	// ========================================================================
	
	// Call COFB encryption mode
	// Returns authentication tag T
	T = COFB(K,N);
	
	// Display authentication tag from encryption
	printf("T: \t%016llx\n",T);
	
	// ========================================================================
	// DECRYPTION PHASE (Verification)
	// ========================================================================
	
	// Call COFB decryption mode
	// Returns computed authentication tag T_ for verification
	T_ = dCOFB(K,N,T);
	
	// Display computed tag from decryption
	printf("T_: \t%016llx\n",T_);
	
	// ========================================================================
	// LEGACY CODE (COMMENTED OUT)
	// ========================================================================
	
	// The following code demonstrates more comprehensive message processing
	// with associated data support, but is currently disabled:
	
	//printf("K: \t");
	//cadK = leerEnt();
	//K = cadToVect(cadK);
	//impVect(K);
	//printf("----------------------\n");
	
	//printf("N: \t");
	//cadN = leerEnt();	// Nonce should be 64 bits
	//N = cadToVect(cadN);
	//impVect(N);
	//printf("----------------------\n");

	//printf("D: \t");		// Associated Data (A)
	//cadA = leerEnt();
	//barra(cadA);
	//A = cadToVect(cadA);
	//impVect(A);
	//printf("----------------------\n");

	//printf("M: \t");		// Message (M)
	//cadM = leerEnt();
	//barra(cadM);
	//M = cadToVect(cadM);
	//impVect(M);
	//printf("----------------------\n");

	//printf("C: \t");		// Ciphertext (C)
	//C	= genVect(M->t);
	//T = COFB(K,N);
	//impVect(C);
	//printf("----------------------\n");

	//printf("T: \t");		// Authentication tag
	//impVect(T);
	//printf("----------------------\n");

	//printf("Mensaje descifrado: \n");		// Decrypted message
	//D	= genVect(C->t);
	//dCOFB(D,N,A,C,T_,K);
	//impVect(D);
	//printf("----------------------\n");

	//printf("Tag(T'): \t\t");			// Verification tag
	//impVect(T_);
	//printf("----------------------\n");
	
	return(0);
	}

