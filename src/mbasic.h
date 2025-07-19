#pragma once

// -----------------------------------------------------------------------------
// Configuration constants
// -----------------------------------------------------------------------------
#define MAX_VARS			64			// Maximum number of user-defined variables
#define VAR_NAME_LEN		8			// Maximum length of each variable name
#define MAX_LINES			200			// Maximum number of program lines
#define LINE_LEN			64			// Maximum length of each program line
#define GOSUB_STACK_SIZE	16			// Depth of GOSUB/RETURN stack

void mbasic_init(void);					// Initialize mbasic.
bool mbasic_load_script(char* path);	// Load a BASIC script into RAM.
void mbasic_exec(void);					// Execute loaded BASIC program.
void mbasic_destroy_prgm(void);			// Destroy the loaded BASIC program from memory and reset GOSUB stack, but does not touch variables.
