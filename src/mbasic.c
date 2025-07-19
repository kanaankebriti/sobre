/**
 * @file basic_interpreter.c
 * @brief A minimal, embeddable BASIC interpreter in C (~200 LOC).
 *
 * This interpreter supports:
 *   - Line-numbered program storage (up to MAX_LINES lines, LINE_LEN each).
 *   - Variables (up to MAX_VARS) and dynamic variable naming.
 *   - Commands: PRINT, INPUT, VAR (assignment), IF, GOTO, GOSUB, RETURN, END.
 *   - Integer math with operators: +, -, *, /, %, &, |, >, <, ==, !=
 *   - GOSUB/RETURN stack for subroutines.
 *   - Error reporting with line numbers.
 *
 * Licensed under the WTFPL.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "mbasic.h"

// -----------------------------------------------------------------------------
// Utility functions
// -----------------------------------------------------------------------------

/**
 * @brief Compute length of a null-terminated string.
 * @param s Pointer to string.
 * @return Number of characters before '\0'.
 */
static int slen(const char* s) {
	int i = 0;
	while (s[i]) ++i;
	return i;
}

/**
 * @brief Compare two null-terminated strings for equality.
 * @param s1 First string.
 * @param s2 Second string.
 * @return 1 if equal, 0 otherwise.
 */
static int scmp(const char* s1, const char* s2) {
	if (slen(s1) != slen(s2)) return 0;
	for (int i = 0; s1[i]; ++i)
		if (s1[i] != s2[i]) return 0;
	return 1;
}

/**
 * @brief Copy source string to destination buffer.
 * @param d Destination buffer (must be large enough).
 * @param s Source null-terminated string.
 */
static void scpy(char* d, const char* s) {
	int i = 0;
	while (s[i]) {
		d[i] = s[i];
		++i;
	}
	d[i] = '\0';
}

/**
 * @brief Check if character is whitespace.
 * @param c Input character.
 * @return Non-zero if space, newline, or tab.
 */
static int isspc(char c) {
	return (c == ' ' || c == '\n' || c == '\t');
}

/**
 * @brief Check if character is a digit.
 * @param c Input character.
 * @return Non-zero if between '0' and '9'.
 */
static int isdg(char c) {
	return (c >= '0' && c <= '9');
}

/**
 * @brief Convert initial numeric substring to integer.
 * @param s Null-terminated string of digits.
 * @return Parsed integer value.
 */
static int str_to_int(const char* s) {
	int v = 0;
	for (; *s && isdg(*s); ++s)
		v = v * 10 + (*s - '0');
	return v;
}

// -----------------------------------------------------------------------------
// Tokenization functions
// -----------------------------------------------------------------------------

static char* _CURTOK = NULL; // Internal pointer for token functions */

/**
 * @brief Tokenize string on whitespace.
 * @param s Input string (or NULL to continue).
 * @return Next token or NULL if none.
 */
static char* stoken(char* s) {
	if (s) _CURTOK = s;
	if (!_CURTOK || !*_CURTOK) return NULL;
	while (isspc(*_CURTOK)) ++_CURTOK;
	char* start = _CURTOK;
	while (*_CURTOK && !isspc(*_CURTOK)) ++_CURTOK;
	if (*_CURTOK) *(_CURTOK++) = '\0';
	return start;
}

/**
 * @brief Tokenize string, respecting quoted segments.
 * @param s Input string (or NULL to continue).
 * @return Next token or NULL if none.
 */
static char* sstrtoken(char* s) {
	int in_quote = 0;
	if (s) _CURTOK = s;
	if (!_CURTOK || !*_CURTOK) return NULL;
	while (isspc(*_CURTOK)) ++_CURTOK;
	char* start = _CURTOK;
	while (*_CURTOK && (in_quote || !isspc(*_CURTOK))) {
		if (*_CURTOK == '"') in_quote = !in_quote;
		++_CURTOK;
	}
	if (*_CURTOK) *(_CURTOK++) = '\0';
	return start;
}

// -----------------------------------------------------------------------------
// Variable management
// -----------------------------------------------------------------------------

static char varnames[MAX_VARS][VAR_NAME_LEN]; // Array of variable names
static int  varcontent[MAX_VARS];			  // Corresponding variable values

/**
 * @brief Initialize variables and seed random.
 */
static void init_vars(void) {
	srand((unsigned int)time(NULL));
	for (int i = 0; i < MAX_VARS; ++i) varnames[i][0] = '\0';
}

/**
 * @brief Retrieve variable value or RANDOM.
 * @param name Variable name.
 * @return Value or random if name is "RANDOM".
 */
static int get_var(const char* name) {
	if (scmp(name, "RANDOM")) return rand();
	for (int i = 0; i < MAX_VARS; ++i)
		if (scmp(name, varnames[i])) return varcontent[i];
	return 0;
}

/**
 * @brief Set variable value, registering new if needed.
 * @param name Variable name.
 * @param value Integer value to assign.
 * @return 1 on success, 0 if no free slot.
 */
static int set_var(const char* name, int value) {
	for (int i = 0; i < MAX_VARS; ++i)
		if (scmp(name, varnames[i])) {
			varcontent[i] = value;
			return 1;
		}
	for (int i = 0; i < MAX_VARS; ++i)
		if (!varnames[i][0]) {
			scpy(varnames[i], name);
			varcontent[i] = value;
			return 1;
		}
	return 0;
}

// -----------------------------------------------------------------------------
// Program storage and GOSUB stack
// -----------------------------------------------------------------------------

static char prgm[MAX_LINES][LINE_LEN]; // Program lines indexed by line number
static int  _linestack[GOSUB_STACK_SIZE]; // Stack for GOSUB line numbers
static int  _linestackpos = 0;			// Current stack position

/**
 * @brief Initialize program memory and GOSUB stack.
 */
static void init_prgm(void) {
	for (int i = 0; i < MAX_LINES; i++) prgm[i][0] = '\0';
	_linestackpos = 0;
}

/**
 * @brief Push current line onto GOSUB stack.
 * @param line Line index to push.
 */
static void push_line(int line) {
	_linestack[_linestackpos++] = line;
}

/**
 * @brief Pop line from GOSUB stack for RETURN.
 * @return Line index.
 */
static int pop_line(void) {
	return _linestack[--_linestackpos];
}

// -----------------------------------------------------------------------------
// Error handling
// -----------------------------------------------------------------------------

/**
 * @brief Report error and terminate.
 * @param linenum Line number of error or -1 for general.
 * @param msg Error message.
 */
static void report_error(int linenum, const char* msg) {
	if (linenum < 0)
		printf("ERROR: %s\n", msg);
	else
		printf("ERROR AT %d: %s\n", linenum, msg);
}

// -----------------------------------------------------------------------------
// Forward declarations
// -----------------------------------------------------------------------------
static int runcmd(int ln, char* line);
static int emath(char* s);

// -----------------------------------------------------------------------------
// Commands and execution
// -----------------------------------------------------------------------------
typedef enum {
	CMD_PRINT, CMD_INPUT, CMD_VAR, CMD_IF,
	CMD_GOTO, CMD_GOSUB, CMD_RET, CMD_END,
	CMD_CLEAR,
	NUM_CMDS
} BasicCmdID;

static const char* cmd_names[NUM_CMDS] = {
	"PRINT","INPUT","VAR","IF",
	"GOTO","GOSUB","RETURN","END",
	"CLEAR"
};

typedef int (*CmdFunc)(int, char*); // Function pointer for commands
static CmdFunc cmd_funcs[NUM_CMDS]; // Mapping of cmd ID to function

/**
 * @brief Get command ID from token.
 * @param s Token string.
 * @return Command ID or -1 if not found.
 */
static int get_cmd_id(const char* s) {
	for (int i = 0; i < NUM_CMDS; ++i)
		if (scmp(s, cmd_names[i])) return i;
	return -1;
}

/**
 * @brief Implementation of PRINT command.
 * @param ln Current line index.
 * @param args Argument string (expressions or quoted text).
 * @return Next line index (same ln for PRINT).
 */
static int cmd_print(int ln, char* args) {
	char* tok = sstrtoken(args);
	while (tok && *tok) {
		if (tok[0] == '"')	// string
			for (int i = 1; tok[i] && tok[i] != '"'; i++)
				putchar(tok[i]);
		else				// variable
			printf("%d", emath(tok));
		tok = sstrtoken(NULL);
	}
	putchar('\n');
	return ln;
}

/**
 * @brief Implementation of INPUT command.
 * @param ln Current line index.
 * @param args Variable name to input into.
 * @return Same line index for INPUT.
 */
static int cmd_input(int ln, char* args) {
	char name[16], buf[16];
	char* tok = stoken(args);
	if (!tok) report_error(ln, "INPUT requires variable");
	scpy(name, tok);
	printf("%s? ", name);
	fgets(buf, sizeof(buf), stdin);
	set_var(name, str_to_int(buf));
	return ln;
}

/**
 * @brief Implementation of VAR assignment command.
 * @param ln Current line index.
 * @param args "name value"
 * @return Same line index.
 */
static int cmd_var(int ln, char* args) {
	char name[16];
	char* tok = stoken(args);
	if (!tok) report_error(ln, "VAR requires name");
	scpy(name, tok);
	tok = stoken(NULL);
	if (!tok) report_error(ln, "VAR requires value");
	set_var(name, emath(tok));
	return ln;
}

/**
 * @brief Implementation of IF command.
 * @param ln Current line index.
 * @param args "expr then-statement"
 * @return New line index based on THEN result.
 */
static int cmd_if(int ln, char* args) {
	char* tok = stoken(args);
	if (!tok || !_CURTOK) report_error(ln, "Invalid IF");
	return emath(tok) ? runcmd(ln, _CURTOK) : ln;
}

/**
 * @brief Implementation of GOTO command.
 * @param ln Current line index.
 * @param args Line number string.
 * @return Target line index -1 (to compensate loop increment).
 */
static int cmd_goto(int ln, char* args) {
	char* tok = stoken(args);
	if (!tok) report_error(ln, "GOTO needs line");
	return emath(tok) - 1;
}

/**
 * @brief Implementation of GOSUB command.
 * @param ln Current line index.
 * @param args Line number string.
 * @return Target line index -1.
 */
static int cmd_gosub(int ln, char* args) {
	char* tok = stoken(args);
	if (!tok) report_error(ln, "GOSUB needs line");
	push_line(ln);
	return emath(tok) - 1;
}

/**
 * @brief Implementation of RETURN command.
 * @param ln Current line index.
 * @return Line index popped from GOSUB stack.
 */
static int cmd_ret(int ln, char* args) {
	(void)ln; (void)args;
	return pop_line();
}

/**
 * @brief Implementation of END command (terminate).
 */
static int cmd_end(int ln, char* args) {
	(void)ln; (void)args;
	exit(0);
}

/**
 * @brief Resets all variables.
 */
static int cmd_clear(int ln, char* args) {
	(void)ln; (void)args;
	for (int i = 0; i < MAX_VARS; ++i) {
		varnames[i][0] = '\0';
		varcontent[i] = 0;
	}
	return ln;
}

// -----------------------------------------------------------------------------
// Math expression evaluator
// -----------------------------------------------------------------------------
static const char* mathops = "&|><~=%*/+-";

static int math_and(int a,int b){return a & b;}   static int math_or(int a,int b){return a | b;}
static int math_gt(int a,int b){return a > b;}	static int math_lt(int a,int b){return a < b;}
static int math_ne(int a,int b){return a != b;}  static int math_eq(int a,int b){return a == b;}
static int math_mod(int a,int b){return a % b;}  static int math_mul(int a,int b){return a * b;}
static int math_div(int a,int b){return a / b;}  static int math_add(int a,int b){return a + b;}
static int math_sub(int a,int b){return a - b;}

typedef int(*MathFunc)(int,int);
static MathFunc mathfuncs[] = {math_and, math_or, math_gt, math_lt, math_ne, math_eq,
								math_mod, math_mul, math_div, math_add, math_sub};

/**
 * @brief Evaluate integer expression recursively.
 * @param s Expression string (will be modified).
 * @return Computed integer result.
 */
static int emath(char* s) {
	if (!*s) return 0;
	for (int i = 0; mathops[i]; i++)
		for (int j = 0; s[j]; j++)
			if (s[j] == mathops[i]) {
				s[j] = '\0';
				int lhs = emath(s);
				int rhs = emath(s + j + 1);
				return mathfuncs[i](lhs, rhs);
			}
	return isdg(*s) ? str_to_int(s) : get_var(s);
}

// -----------------------------------------------------------------------------
// Command dispatcher
// -----------------------------------------------------------------------------
/**
 * @brief Execute a single program line.
 * @param ln Current line index.
 * @param line Content of the line.
 * @return Next line index.
 */
static int runcmd(int ln, char* line) {
	char buf[LINE_LEN]; scpy(buf, line);
	char* tok = sstrtoken(buf);
	if (!tok) return ln;
	int cid = get_cmd_id(tok);
	if (cid < 0) report_error(ln, "Unknown command");
	return cmd_funcs[cid](ln, _CURTOK);
}

// -----------------------------------------------------------------------------
// Program loading and execution
// -----------------------------------------------------------------------------
/**
 * @brief Read BASIC program from file into memory.
 * @param f File pointer (opened for reading).
 */
static void read_program(FILE* f) {
	char buffer[LINE_LEN];
	int lineno = 0;
	while (fgets(buffer, LINE_LEN, f)) {
		lineno++;
		char* ptr = buffer;
		while (*ptr && isspc(*ptr)) ptr++;
		if (!*ptr || *ptr == '\'') continue;
		if (!isdg(*ptr)) report_error(lineno, "Missing line number");
		char* numtok = stoken(ptr);
		int pln = str_to_int(numtok);
		scpy(prgm[pln], _CURTOK);
	}
}

/**
 * @brief Execute loaded BASIC program.
 */
void mbasic_exec() {
	for (int i = 0; i < MAX_LINES; i++)
		i = runcmd(i, prgm[i]);
}

/**
 * @brief Initialize mbasic.
 */
void mbasic_init() {
	init_prgm();
	init_vars();

	// Bind command IDs to functions
	cmd_funcs[CMD_PRINT] 	= cmd_print;
	cmd_funcs[CMD_INPUT] 	= cmd_input;
	cmd_funcs[CMD_VAR]		= cmd_var;
	cmd_funcs[CMD_IF]		= cmd_if;
	cmd_funcs[CMD_GOTO]		= cmd_goto;
	cmd_funcs[CMD_GOSUB]	= cmd_gosub;
	cmd_funcs[CMD_RET]		= cmd_ret;
	cmd_funcs[CMD_END]		= cmd_end;
	cmd_funcs[CMD_CLEAR]	= cmd_clear;
}

/**
 * @brief Load a BASIC script into RAM.
 */
bool mbasic_load_script(char* path) {
	FILE* script_file_ptr;
	script_file_ptr = fopen(path, "r");
	if (!script_file_ptr) {
		report_error(-1, "Cannot open file");
		return false;
	}
	read_program(script_file_ptr);
	fclose(script_file_ptr);
	return true;
}
