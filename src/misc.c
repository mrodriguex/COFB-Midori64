/*
 * ============================================================================
 * File: misc.c
 * Purpose: Miscellaneous utility functions for cryptographic operations
 * 
 * This file implements helper functions for:
 * - Hexadecimal validation
 * - Binary operations and conversions
 * - String manipulation
 * - Input/output processing
 * - Memory management for vectors
 * 
 * Author: COFB-Midori64 Project
 * Date: December 2025
 * ============================================================================
 */

#include"misc.h"

/*
 * Function: esHex()
 * 
 * Purpose: Validates if a character is a valid hexadecimal digit
 * 
 * Parameters:
 *   - char car: Character to validate
 * 
 * Returns:
 *   - 0: Character is valid hexadecimal (0-9, a-f, A-F)
 *   - 1: Character is NOT valid hexadecimal
 * 
 * Details: Used for input validation during hex string parsing
 */
byte esHex(char car)
	{
	if( (car>='0' && car<='9') || (car>='a' && car<='f') || (car>='A' && car<='F') )
		{
		return(0);		// Valid hex character
		}
	else
		{
		return(1);		// Invalid hex character
		}
	}

/*
 * Function: techo()
 * 
 * Purpose: Implements ceiling function for double precision floating point
 * 
 * Parameters:
 *   - double a: Number to compute ceiling for
 * 
 * Returns:
 *   - byte: Ceiling of input value (rounded up to nearest integer)
 * 
 * Algorithm: Adds 0.9 to input and casts to byte, effectively rounding up
 * 
 * Example: techo(2.1) = 3, techo(2.9) = 3
 */
byte techo(double a)
	{		
	return ((byte)(a+(0.9)));	// Ceiling operation via addition and cast
	}

/*
 * Function: impBin()
 * 
 * Purpose: Converts 32-bit number to binary string representation and prints it
 * 
 * Parameters:
 *   - tn2 num: 32-bit number to convert
 * 
 * Returns: void
 * 
 * Output Format: [ bit0, bit1, ..., bit62, bit63 ]
 * - Bits are printed from LSB to MSB
 * - Each bit followed by comma and space (except last bit)
 * - Enclosed in square brackets
 * 
 * Details:
 *   - Buffer size: (64*3)+3 bytes for 64 bits with commas
 *   - Uses bitwise AND to extract LSB
 *   - Right-shifts after each extraction
 */
void impBin(tn2 num)
	{
	byte i;
	byte j=0;
	byte t=(64*3)+3;		// Total buffer size for 64 bits with formatting
	char a[t];
	
	a[j++] = '[';			// Opening bracket
	a[j++] = ' ';
	
	// Extract and print each bit from LSB to MSB
	for(i=0; i<63; i++)
		{
		a[j++] = (num & 0x01) | 0x30;	// Extract LSB, convert to '0' or '1'
		a[j++] = ',';					// Add separator
		a[j++] = ' ';
		num = num>>1;					// Right-shift to next bit
		}
	
	// Print last bit without comma
	a[j++] = (num & 0x01) ^ 0x30;		// Final bit (LSB after 63 shifts)
	a[j++] = ' ';
	a[j++] = ']';						// Closing bracket
	a[j++] = '\0';						// Null terminator
	
	puts(a);
	return;
	}

/*
 * Function: leeBin()
 * 
 * Purpose: Converts binary string representation back to numeric value
 * 
 * Parameters:
 *   - cad a: Binary string (containing '0' and '1' characters)
 * 
 * Returns:
 *   - tn2: 32-bit unsigned integer representation
 * 
 * Algorithm:
 *   1. Start from the end of the string (MSB)
 *   2. For each '0' or '1' character:
 *      - Left-shift accumulated result
 *      - XOR with bit value (0x30 toggle converts '0'/'1' to 0/1)
 *   3. Skip non-binary characters
 */
tn2 leeBin(cad a)
	{
	byte i;
	byte t = strlen(a);		// String length
	tn2 num = 0;			// Accumulator for result
	
	// Iterate backwards from end of string (MSB to LSB)
	for(i=t; i>0; i--)
		{
		if(a[i-1] == '0' || a[i-1]=='1')		// Only process binary digits
			{
			num = num<<1;						// Left-shift for next bit
			num = num | (a[i-1] ^ 0x30);		// Add bit ('0'->0, '1'->1)
			}
		}
	return (num);
	}

/*
 * Function: reverse()
 * 
 * Purpose: Reverses the byte order of a 64-bit block
 * 
 * Parameters:
 *   - bloque a: 64-bit block to reverse
 * 
 * Returns:
 *   - bloque: Byte-reversed version of input
 * 
 * Algorithm:
 *   1. Iterates 8 times (for each byte)
 *   2. Each iteration:
 *      - Left-shifts accumulated result by 8 bits
 *      - Extracts one byte from input via 8 right-shifts
 *      - XORs with accumulated result
 *   3. Result: Original byte order inverted (endianness reversal)
 * 
 * Example: 0x0102030405060708 → 0x0807060504030201
 */
bloque reverse(bloque a)
	{
	bloque b=0x0;				// Accumulator for reversed value
	bloque desp;				// Displacement/shift value
	byte i;						// Loop counter
	
	// Extract each byte and accumulate in reverse order
	for(i=0;i<0x08;i++)
		{
		desp = i;
		// Right-shift by (i*8) bits to extract i-th byte
		// Perform 8 right-shifts of desp value (shifts by 8*desp)
		b = (bloque)(b<<0x08) ^((((((((a >> desp) >> desp) >> desp) >> desp) >> desp) >> desp) >> desp) >> desp) &0xff;
		}
	
	return(b);
	}

/*
 * Function: hayNL()
 * 
 * Purpose: Checks if a string contains a newline character
 * 
 * Parameters:
 *   - cad a: String to search for newline
 * 
 * Returns:
 *   - 1: Newline character found
 *   - 0: No newline character found
 * 
 * Details: Scans entire string until null terminator
 */
byte hayNL(cad a)
	{
	byte t = strlen(a);		// String length
	byte i;
	
	// Iterate through string looking for newline
	for(i=0;i<t;i++)
		{
		if(a[i]=='\n')
			{
			return(1);		// Newline found
			}
		}
	return(0);				// No newline found
	}
