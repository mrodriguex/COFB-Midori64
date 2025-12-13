/*
 * ============================================================================
 * File: midori.c
 * Purpose: Implementation of Midori-64 lightweight block cipher
 * 
 * Midori-64 is a 64-bit block cipher with 128-bit key that provides:
 * - S-box substitution layer
 * - Cell permutation and shuffling
 * - Key schedule and round key generation
 * - MixColumns diffusion operation
 * 
 * Algorithm Structure:
 *   1. Key expansion: 128-bit key → 16 round keys
 *   2. Initial whitening with XOR
 *   3. 15 rounds of: SubCell → ShuffleCell → MixColumns → KeyAdd
 *   4. Final SubCell and XOR with initial key
 * 
 * Author: COFB-Midori64 Project
 * Date: December 2025
 * ============================================================================
 */

#include"midori.h"

/*
 * Beta Constants Array
 * 
 * Purpose: Round constants used in key schedule generation
 * Size: 16 constants (one per round)
 * Type: 16-bit values (dbyte)
 * 
 * These constants provide cryptographic mixing during key expansion
 * and prevent weak keys or related key attacks
 */
const dbyte  beta[0x10] = {
	0x15b3,	//0
	0x78c0,	//1
	0xa435,	//2
	0x6213,	//3
	0x104f,	//4
	0xd170,	//5
	0x0266,	//6
	0x0bcc,	//7
	0x9481,	//8
	0x40b8,	//9
	0x7197,	//a
	0x228e,	//b
	0x5130,	//c
	0xf8ca,	//d
	0xdf90,	//e
	0x7c81	//f
	};

/*****************************************************************************
 * NIBBLE EXTRACTION AND ASSIGNMENT FUNCTIONS
 * 
 * These functions manipulate 4-bit values (nibbles) within 64-bit blocks
 * Midori-64 uses 4x4 matrix of nibbles as state representation
 *****************************************************************************/

/*
 * Function: obtNibble()
 * 
 * Purpose: Extracts a 4-bit value (nibble) from a 64-bit block at given position
 * 
 * Parameters:
 *   - bloque S: 64-bit source block
 *   - nibble pos: Position of nibble to extract (0-15)
 * 
 * Returns:
 *   - nibble: 4-bit value at specified position
 * 
 * Algorithm:
 *   1. Calculate shift amount: des = (16 - pos - 1) * 2 bits
 *   2. Right-shift S by (des + des) bits to align target nibble to LSB
 *   3. Mask with 0xf to extract only 4 bits
 * 
 * Example: obtNibble(0x0123456789ABCDEF, 0) extracts 0xf (rightmost nibble)
 *          obtNibble(0x0123456789ABCDEF, 15) extracts 0x0 (leftmost nibble)
 */
nibble obtNibble(bloque S, nibble pos)
	{
	byte	des	= (nxn-pos-1)<<1;	// Shift distance: (16-pos-1)*2
	nibble	val	= (S >> des >> des) & 0xf;	// Extract 4-bit value
	return(val);
	}

/*
 * Function: asgNibble()
 * 
 * Purpose: Assigns a 4-bit value (nibble) to a specific position in 64-bit block
 * 
 * Parameters:
 *   - bloque S: Original 64-bit block
 *   - nibble pos: Position where to assign (0-15)
 *   - nibble val: 4-bit value to assign
 * 
 * Returns:
 *   - bloque: Modified block with new nibble value
 * 
 * Algorithm:
 *   1. Calculate shift amount: des = (16 - pos - 1) * 2
 *   2. Create mask isolating target nibble position
 *   3. Clear target nibble in original block
 *   4. XOR with new value
 * 
 * Details: Uses XOR-based manipulation to set value without affecting others
 */
bloque asgNibble(bloque S, nibble pos, nibble val)
	{
	byte	des	= (nxn-pos-1)<<1;	// Calculate shift distance
	bloque	tmp	= (bloque)val << des << des;	// Position new value
	bloque	msk	= (bloque)0x000000000000000f << des << des;	// Create mask
	bloque	mskS	= msk & S;		// Extract current value at position
	return((tmp ^ mskS) ^ S);		// XOR to update value
	}

/*****************************************************************************
 * KEY SCHEDULE FUNCTIONS
 * 
 * Expands 128-bit master key into 16 round keys using beta constants
 *****************************************************************************/

/*
 * Function: keyGen()
 * 
 * Purpose: Generates round keys from 128-bit master key
 * 
 * Parameters:
 *   - bloque RK[r]: Array to store generated round keys (output)
 *   - bloque Ki[2]: Master key split into 2 blocks of 64-bits each
 * 
 * Returns:
 *   - bloque: Whitening key (XOR of both key blocks)
 * 
 * Algorithm:
 *   1. Compute WK = Ki[0] XOR Ki[1] for initial whitening
 *   2. For each round (0 to 14):
 *      - Extract each nibble from alternating key blocks
 *      - XOR with corresponding beta bit
 *      - Assign to round key array
 *   3. Return whitening key
 * 
 * Details:
 *   - Alternates between Ki[0] and Ki[1] using (i & 0x1)
 *   - Beta values provide round constants for variety
 */
bloque keyGen(bloque RK[r], bloque Ki[2])
	{
	nibble i;
	nibble j;
	nibble bit;
	nibble val;
	bloque WK;
	
	// Whitening key is XOR of both key blocks
	WK = Ki[0] ^ Ki[1];
	
	// Generate round keys for all 15 rounds
	for(i=0;i<=r-2;i++)
		{
		// For each nibble in the key
		for(j=0;j<nxn;j++)
			{
			// Extract beta bit for this position
			bit = (beta[i] >> (nxn-j-1)) & 0x1;
			// Extract nibble from alternating key block
			val = obtNibble(Ki[i & 0x1],j);
			// Assign XORed value to round key
			RK[i] = asgNibble(RK[i],j,val^bit);
			}
		}

	return(WK);
	}

/*
 * Function: keyGenInv()
 * 
 * Purpose: Generates inverse round keys for decryption operations
 * 
 * Parameters:
 *   - bloque RK_1[r]: Array to store inverse round keys (output)
 *   - bloque RK[r]: Forward round keys
 * 
 * Returns:
 *   - bloque: Last transformed round key
 * 
 * Algorithm:
 *   1. Iterate rounds in reverse (from round 13 to 0)
 *   2. Apply MixColumns to forward round key
 *   3. Apply inverse ShuffleCell
 *   4. Store as inverse round key
 * 
 * Details: Prepares keys for inverse cipher operations (decryption)
 */
bloque keyGenInv(bloque RK_1[r], bloque RK[r])
	{ 
	bloque tmp;
	nibble i;
	
	// Generate inverse keys by processing forward keys in reverse order
	for(i=0;i<=r-2;i++)
		{
		// Apply MixColumns then inverse ShuffleCell
		tmp = mixColumn(RK[r-2-i]);
		RK_1[i] = shuffleCell(tmp,-1);
		}

	return(tmp);
	}

/*****************************************************************************
 * CIPHER TRANSFORMATION FUNCTIONS
 * 
 * Core operations: SubCell (substitution), ShuffleCell (permutation),
 * MixColumns (diffusion)
 *****************************************************************************/

/*
 * Function: subCell()
 * 
 * Purpose: Applies S-box substitution to all nibbles in block
 * 
 * Parameters:
 *   - bloque S: Input block (64-bit)
 * 
 * Returns:
 *   - bloque: Block with all nibbles substituted via S-box
 * 
 * Algorithm:
 *   1. For each of 16 nibbles in block:
 *      - Extract nibble value (0-15)
 *      - Use as index into S-box (Sb0)
 *      - Assign substituted value to output
 *   2. Return transformed block
 * 
 * Details: Provides non-linear substitution for security
 */
bloque subCell(bloque S)
	{
	nibble	i;
	bloque	SC;
	byte	indiceI;
	byte	indiceJ;
	
	// Apply S-box to all 16 nibbles
	for(i=0;i<nxn;i++)
		{
		indiceI = obtNibble(S,i);		// Extract nibble as S-box index
		indiceJ = obtNibble(Sb0,indiceI);	// Perform substitution
		SC = asgNibble(SC,i,indiceJ);	// Assign substituted value
		}
	return(SC);
	}

/*
 * Function: shuffleCell()
 * 
 * Purpose: Applies cell permutation to reorder nibbles in block
 * 
 * Parameters:
 *   - bloque S: Input block
 *   - byte inv: Operation mode (0=forward, -1=inverse)
 * 
 * Returns:
 *   - bloque: Permuted block
 * 
 * Algorithm:
 *   1. Select permutation table based on inv parameter
 *   2. For each output position i:
 *      - Get source position from permutation table
 *      - Copy nibble from source to destination
 *   3. Return permuted block
 * 
 * Details: Provides positional diffusion (confusion of position)
 */
bloque shuffleCell(bloque S, byte inv)
	{
	nibble	i;
	bloque	SH;
	byte	indiceI;
	byte	indiceJ;
	
	// Select forward or inverse permutation based on inv parameter
	for(i=0;i<nxn;i++)
		{
		// Get permutation index for position i
		indiceI = obtNibble((inv == 0) ? shuffleP : shufflePInv,i);
		// Get nibble from source position
		indiceJ = obtNibble(S,indiceI);
		// Assign to output position
		SH = asgNibble(SH,i,indiceJ);
		}
	
	return(SH);
	}

/*
 * Function: mixColumn()
 * 
 * Purpose: Applies MixColumns transformation for diffusion
 * 
 * Parameters:
 *   - bloque S: Input block
 * 
 * Returns:
 *   - bloque: Block after MixColumns transformation
 * 
 * Algorithm:
 *   1. Process each column (4 nibbles per column, 4 columns total)
 *   2. For each column:
 *      - XOR all 4 nibbles in column (compute column parity)
 *      - For each nibble in column:
 *        * New value = parity XOR original value
 * 
 * Details: Simple but effective diffusion - each nibble becomes combination
 *          of all nibbles in its column, and its own value
 */
bloque mixColumn(bloque S)
	{
	bloque MC;
	bloque tmp;
	nibble i;
	nibble j;
	
	// Process each column (4 nibbles per column, step by 4)
	for(i=0; i<nxn; i+=n)
		{
		j = n;
		tmp = 0;
		// Compute column parity (XOR of all nibbles in column)
		while((j--)>0)
			{
			tmp ^= obtNibble(S,i+j);
			}
		// Apply diffusion: new_value = parity XOR old_value
		for(j=0; j<n; j++)
			{
			MC = asgNibble(MC,i+j, tmp ^ obtNibble(S,i+j));
			}
		}
	
	return(MC);
	}

/*****************************************************************************
 * MAIN CIPHER FUNCTION
 * 
 * Core encryption algorithm implementing the Midori-64 cipher
 *****************************************************************************/

/*
 * Function: midori()
 * 
 * Purpose: Executes Midori-64 block cipher transformation
 * 
 * Parameters:
 *   - bloque S: Input block (plaintext or state)
 *   - bloque Ki[2]: Encryption key (128-bit, split into 2 blocks)
 *   - byte inv: Operation mode (0=encryption, non-zero=other modes)
 * 
 * Returns:
 *   - bloque: Ciphertext/output block
 * 
 * Algorithm Structure:
 *   1. Generate round keys from Ki
 *   2. Initial key addition (whitening): S ⊕ WK
 *   3. Perform 15 rounds, each containing:
 *      - SubCell: S-box substitution
 *      - ShuffleCell: Permutation
 *      - MixColumns: Diffusion
 *      - Key addition: S ⊕ RK[round]
 *   4. Final SubCell substitution
 *   5. Final whitening: S ⊕ WK
 * 
 * Details:
 *   - 16 total rounds (rounds 0-15, where round 15 is final)
 *   - Branch on inv parameter (currently single path)
 *   - Each round operates on full 64-bit state (16 nibbles)
 */
bloque midori(bloque S, bloque Ki[2], byte inv)
	{
	nibble i;
	bloque Y;
	bloque WK;
	bloque RK[r];
	
	// Generate round keys from master key
	WK = keyGen(RK,Ki);
	
	// Initial key addition (whitening)
	S = keyAdd(S,WK);

	// 15 main rounds (0 to 14)
	for(i=0;i<=r-2;i++)
		{
		// SubCell substitution layer
		S = subCell(S);
		
		// Permutation and diffusion layers
		S = (inv == 0) ? shuffleCell(S,0)  : mixColumn(S);
		S = (inv == 0) ? mixColumn(S)      : shuffleCell(S,-1);
		
		// Add round key
		S = keyAdd(S,RK[i]);
		}

	// Final round
	S = subCell(S);
	
	// Final whitening with initial key
	Y = keyAdd(S,WK);
	
	return(Y);
	}
