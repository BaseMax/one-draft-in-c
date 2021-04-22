// library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <limits.h>

// typdef
typedef char* string;
#ifndef bool
typedef enum {
	false,
	true,
} bool;
#endif
#undef TOKEN
#define TOKEN(ident, chars, name) ident,
// #include "token.h"
typedef enum {
	TOKEN_PRINT,//_
	TOKEN_PRINTLN,//__
	TOKEN_IDENT,//ident
	TOKEN_IF,//if
	TOKEN_ELSE,//else
	TOKEN_WHILE,//while
	TOKEN_FOR,//for
	TOKEN_BEGIN,//{}
	TOKEN_END,//}
	TOKEN_TRIM,//space,tab
	TOKEN_TRIM_LINE,//line
	TOKEN_OPERATOR,//op
	TOKEN_VIRGOOL,//,
	TOKEN_SEMICOLON,//;
	TOKEN_STRING_DOUBLE,//"
	TOKEN_STRING_SINGLE,//'
	TOKEN_NUMBER,//number
} TokenType;
typedef enum {
	OPERATOR_ADDITATION,
	OPERATOR_SUBSTRACT,
	OPERATOR_MULTIPLIE,
	OPERATOR_DIVIDE,
	OPERATOR_POWER,
	OPERATOR_LESS,
	OPERATOR_GREATER,
	OPERATOR_EQUAL,
	OPERATOR_BRACKET_BEGIN,
	OPERATOR_BRACKET_END,
	OPERATOR_DOT,
} OperatorType;
typedef struct {
	TokenType type;
	OperatorType operator;
	char value[124];
} Token;
typedef struct {
	string path;
	string fileName;
	size_t	line;
	size_t	indexLine;
	size_t	indexFile;
} File;

// headers
void statement();
void statements();
static void next();
static string tokenName(TokenType tok);
static void error(const string format, ...);
static void memoryFree(void* ptr);
static void* memoryAlloc(unsigned long size);
static string readFile(const string path);
static string getPath();

// global variables
string source;
Token token;
File file;

// functions
static string getPath(string path) {
	char buffer[1024];
	getcwd(buffer, 1024);
	path=buffer;
	// printf("==>%s\n", buffer);
	return path;
}
static void next() {
	checkChar:
	file.indexFile++;
	file.indexLine++;
	switch(*source) {
		case '\0':
			token.type=-1;
		break;
		case ' ':
		case '\t':
		case '\n':
		case '\r': {
			// *source++;
			// goto checkChar;
			bool hasLine=false;
			if(*source == '\n') {
				hasLine=true;
				file.line++;
				file.indexLine=0;
			}
			*source++;
			while(*source && (*source == ' ' || *source == '\t' || *source == '\r' || *source == '\n')) {
				if(*source == '\n') {
					file.line++;
					file.indexLine=0;
					if(hasLine == false) {
						hasLine=true;
					}
				}
				*source++;
			}
			if(hasLine == true) {
				token.type=TOKEN_TRIM_LINE;
			}
			else {
				token.type=TOKEN_TRIM;
			}
		}
		break;
		case '\f':
		case '\v': {
			*source++;
		}
		break;
		case '+': {
			token.type=TOKEN_OPERATOR;
			token.operator=OPERATOR_ADDITATION;
			*source++;
		}
		break;
		case '-': {
			token.type=TOKEN_OPERATOR;
			token.operator=OPERATOR_SUBSTRACT;
			*source++;
		}
		break;
		case '*': {
			token.type=TOKEN_OPERATOR;
			token.operator=OPERATOR_MULTIPLIE;
			*source++;
		}
		break;
		case '/': {
			token.type=TOKEN_OPERATOR;
			token.operator=OPERATOR_DIVIDE;
			*source++;
		}
		break;
		case '^': {
			token.type=TOKEN_OPERATOR;
			token.operator=OPERATOR_POWER;
			*source++;
		}
		break;
		case '>': {
			token.type=TOKEN_OPERATOR;
			token.operator=OPERATOR_LESS;
			*source++;
		}
		break;
		case '<': {
			token.type=TOKEN_OPERATOR;
			token.operator=OPERATOR_GREATER;
			*source++;
		}
		break;
		case '=': {
			token.type=TOKEN_OPERATOR;
			token.operator=OPERATOR_EQUAL;
			*source++;
		}
		break;
		case '.': {
			token.type=TOKEN_OPERATOR;
			token.operator=OPERATOR_DOT;
			*source++;
		}
		break;
		case ',': {
			token.type=TOKEN_VIRGOOL;
			*source++;
		}
		break;
		case ';': {
			token.type=TOKEN_SEMICOLON;
			*source++;
		}
		break;
		case '{': {
			token.type=TOKEN_BEGIN;
			*source++;
		}
		break;
		case '}': {
			token.type=TOKEN_END;
			*source++;
		}
		break;
		case '(': {
			token.type=TOKEN_OPERATOR;
			token.operator=OPERATOR_BRACKET_BEGIN;
			*source++;
		}
		break;
		case ')': {
			token.type=TOKEN_OPERATOR;
			token.operator=OPERATOR_BRACKET_END;
			*source++;
		}
		break;
		case '"': {
			int i=0;
			*source++;
			token.type=TOKEN_STRING_DOUBLE;
			// token.value=memoryAlloc(sizeof(char)*124);
			token.value[i++]=*source;
			// strcpy(token.value, "");
			*source++;
			while(*source && *source != '"') {
				// strcat(token.value, (char[]){(char)*source});
				token.value[i++]=*source;
				*source++;
			}
			token.value[i++]='\0';
			if(*source == '"') {
				*source++;
			}
			else {
				error("except \" token for closing string!");
			}
		}
		break;
		case '\'': {
			int i=0;
			*source++;
			token.type=TOKEN_STRING_SINGLE;
			token.value[i++]=*source;
			// token.value=memoryAlloc(sizeof(char)*124);
			// strcpy(token.value, "");
			*source++;
			while(*source != '\'') {
				token.value[i++]=*source;
				// strcat(token.value, (char[]){(char)*source});
				*source++;
			}
			token.value[i++]='\0';
			if(*source == '\'') {
				*source++;
			}
			else {
				error("except \" token for closing string!");
			}
		}
		break;
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
		case '8': case '9': {
			int i=0;
			token.type=TOKEN_NUMBER;
			token.value[i++]=*source;
			*source++;
			// token.value=memoryAlloc(sizeof(char)*124);
			// strcpy(token.value, "");
			while(*source >= '0' && *source <= '9') {
				token.value[i++]=*source;
				// strcat(token.value, (char[]){(char)*source});
				*source++;
			}
			token.value[i++]='\0';
		}
		break;
		// case '_': {
		// 	*source++;
		// 	if(*source && *source == '_') {
		// 		token.type=TOKEN_PRINTLN;
		// 		*source++;
		// 	}
		// 	else {
		// 		token.type=TOKEN_PRINT;
		// 	}
		// }
		// break;
		case 'a': case 'b': case 'c': case 'd':
		case 'e': case 'f': case 'g': case 'h':
		case 'i': case 'j': case 'k': case 'l':
		case 'm': case 'n': case 'o': case 'p':
		case 'q': case 'r': case 's': case 't':
		case 'u': case 'v': case 'w': case 'x':
		case 'y': case 'z': 
		case 'A': case 'B': case 'C': case 'D':
		case 'E': case 'F': case 'G': case 'H':
		case 'I': case 'J': case 'K': 
		case 'M': case 'N': case 'O': case 'P':
		case 'Q': case 'R': case 'S': case 'T':
		case 'U': case 'V': case 'W': case 'X':
		case 'Y': case 'Z':
		case '_': {
			int i=0;
			// token.value=memoryAlloc(sizeof(char)*124);
			// strcpy(token.value, (char[]){(char)*source});
			token.value[i++]=*source;
			// printf("\t\t%c\n", *source);
			*source++;
			while(
				*source && (
					(*source >= 'a' && *source <= 'z' ) ||
					(*source >= 'A' && *source <= 'Z' ) ||
					(*source >= '0' && *source <= '9' ) ||
					(*source == '_')
				)
			) {
				// printf("\t\t%c\n", *source);
				token.value[i++]=*source;
				// strcat(token.value, (char[]){(char)*source});
				*source++;
			}
			token.value[i++]='\0';
			if(strcmp(token.value, "__") == 0) {
				token.type=TOKEN_PRINTLN;
				return;
			}
			else if(strcmp(token.value, "_") == 0) {
				token.type=TOKEN_PRINT;
				return;
			}
			else if(strcmp(token.value, "if") == 0) {
				token.type=TOKEN_IF;
				return;
			}
			else if(strcmp(token.value, "else") == 0) {
				token.type=TOKEN_ELSE;
				return;
			}
			else if(strcmp(token.value, "while") == 0) {
				token.type=TOKEN_WHILE;
				return;
			}
			else if(strcmp(token.value, "for") == 0) {
				token.type=TOKEN_FOR;
				return;
			}
			// strcat(token.value, "\0");
			// #undef TOKEN
			// #define TOKEN(ident, chars, name) if(*source == name[0]) {token.type=ident;}
			// #include "token.h"
			int s=strlen(token.value)-1;
			file.indexLine+=s;
			file.indexFile+=s;
			// printf("------------------------->>> '%s'\n", token.value);
			token.type=TOKEN_IDENT;
		}
		break;
		default: {
			token.type=*source;
			*source++;
		}
		break;
	}
	// *source++;
	// printf("%c\n", *source);
}

static string operatorName(OperatorType tok) {
	switch(token.operator) {
		case OPERATOR_ADDITATION:
			return "+";
		case OPERATOR_DIVIDE:
			return "/";
		case OPERATOR_MULTIPLIE:
			return "*";
		case OPERATOR_SUBSTRACT:
			return "-";
		case OPERATOR_POWER:
			return "^";
		case OPERATOR_EQUAL:
			return "=";
		case OPERATOR_LESS:
			return "<";
		case OPERATOR_GREATER:
			return ">";
		case OPERATOR_BRACKET_BEGIN:
			return "(";
		case OPERATOR_BRACKET_END:
			return ")";
		default:
			return "unknown";
	}
}

static string tokenName(TokenType tok) {
	switch(tok) {
		// #undef TOKEN
		// #define TOKEN(ident, chars, name) case ident: return name;
		// #include "token.h"
		case TOKEN_VIRGOOL:
			return "virgool";
		case TOKEN_STRING_DOUBLE:
			return "string double";
		case TOKEN_STRING_SINGLE:
			return "string single";
		case TOKEN_PRINT:
			return "print";
		case TOKEN_PRINTLN:
			return "println";
		case TOKEN_NUMBER:
			return "number";
		case TOKEN_IDENT:
			return "ident";
		case TOKEN_IF:
			return "if";
		case TOKEN_ELSE:
			return "else";
		case TOKEN_WHILE:
			return "while";
		case TOKEN_FOR:
			return "for";
		case TOKEN_BEGIN:
			return "begin";
		case TOKEN_END:
			return "end";
		case TOKEN_TRIM:
			return "trim";
		case TOKEN_TRIM_LINE:
			return "trimWithLine";
		case TOKEN_OPERATOR:
			return "operator";
		default:
			return "unknown";
	}
	// return (char[]){(char)tok, '\0'};
}

static void error(const string format, ...) {
	va_list arguments;
	va_start(arguments, format);
	fprintf(stderr, format, arguments);
	fprintf(stderr, "\n");
	// printf(format, arguments);
	exit(-1);
}

static void exceptOperator(OperatorType op) {
	if(token.type == TOKEN_OPERATOR) {
		if(token.operator == op) {
			next();
		}
		else {
			error("1Error: except %s operator but has %s", operatorName(op), tokenName(token.operator));
		}
	}
	else {
		error("2Error: except %s token but has %s", tokenName(TOKEN_OPERATOR), tokenName(token.type));
	}
}

static void except(TokenType tok) {
	if(token.type == tok) {
		next();
	}
	else {
		error("3Error: except %s token but has %s", tokenName(tok), tokenName(token.type));
	}
}

static void skip(TokenType tok) {
	if(token.type == tok) {
		next();
	}
}

static void skip2(TokenType tok1, TokenType tok2) {
	if(token.type == tok1 || token.type == tok2) {
		next();
	}
}

static void trim() {
	skip2(TOKEN_TRIM, TOKEN_TRIM_LINE);
}

void expression() {
	trim();
	if(token.type == TOKEN_OPERATOR) {
		// - <number>
		if(token.operator == OPERATOR_SUBSTRACT) {
			next();
			expression();
		}
		// + <number>
		else if(token.operator == OPERATOR_ADDITATION) {
			next();
			expression();
		}
		// ( <value>
		else if(token.operator == OPERATOR_BRACKET_BEGIN) {
			expression();
			exceptOperator(OPERATOR_BRACKET_END);
		}
		else {
			error("Error token as expression!");
		}
	}
	else if(token.type == TOKEN_STRING_SINGLE) {
		printf("\t\tVALUE:%d,%s\n", token.type, tokenName(token.type));
		next();
	}
	else if(token.type == TOKEN_STRING_DOUBLE) {
		printf("\t\tVALUE:%d,%s\n", token.type, tokenName(token.type));
		next();
	}
	else if(token.type == TOKEN_NUMBER) {
		printf("\t\tVALUE:%d,%s\n", token.type, tokenName(token.type));
		next();
	}
}

void expressions() {
	expression();
	trim();
	if(token.type == TOKEN_VIRGOOL) {
		next();
		expressions();
	}
}

void statement() {
	// ;;;;;;;;;;; as input
	if(token.type == TOKEN_SEMICOLON) {
		next();
		return;
	}
	if(token.type == TOKEN_TRIM || token.type == TOKEN_TRIM_LINE) {
		next();
		return;
	}
	else if(token.type == TOKEN_PRINT) {
		printf("\tPRINT\n");
		next();
		expressions();
		skip(TOKEN_SEMICOLON);
		return;
	}
	else if(token.type == TOKEN_PRINTLN) {
		printf("\tPRINTLN\n");
		next();
		expressions();
		skip(TOKEN_SEMICOLON);
		return;
	}
	else if(token.type == TOKEN_IF) {
		printf("\tIF\n");
		next();
		expressions();
		trim();
		statements();
		return;
	}
	printf("\tSTATEMENT:%d,%s\n", token.type, tokenName(token.type));
	next();
}

void statements() {
	trim();
	if(token.type == TOKEN_BEGIN) {
		except(TOKEN_BEGIN);
		trim();
		while(token.type != TOKEN_END) {
			statement();
		}
		trim();
		except(TOKEN_END);
	}
	else {
		statement();
	}
}
void scanner() {
	for(;;) {
		next();
		if(token.type == -1) {
			break;
		}
		if(token.type == TOKEN_TRIM || token.type == TOKEN_TRIM_LINE) {
			continue;
		}
		// printf("------- ");
		// printf(tokenName(token.type));
		// if(token.type == TOKEN_IDENT) {
		// 	printf(" '%s'", token.value);
		// }
		// else if(token.type == TOKEN_STRING_SINGLE || token.type == TOKEN_STRING_DOUBLE) {
		// 	printf(" '%s'", token.value);
		// }
		// else if(token.type == TOKEN_OPERATOR) {
		// 	printf(" '%s'", operatorName(token.operator));
		// }
		// printf("\n");

		switch(token.type) {
			case TOKEN_IDENT: {
				string ident=token.value;
				next();
				trim();
				if(token.type == TOKEN_OPERATOR) {
					printf("Define function %s\n", ident);
					if(token.type == TOKEN_OPERATOR) {
						exceptOperator(OPERATOR_BRACKET_BEGIN);
						trim();
						while(token.type != TOKEN_OPERATOR && token.operator != OPERATOR_BRACKET_END) {
							string type=token.value;
							except(TOKEN_IDENT);

							// printf("==>ARG2:%d,%s\n", token.type, tokenName(token.type));
							trim();

							// printf("==>ARG3:%d,%s\n", token.type, tokenName(token.type));

							string name=token.value;
							except(TOKEN_IDENT);
							// printf("==>ARG4:%d,%s\n", token.type, tokenName(token.type));
							printf("\t\tARG:variable %s with %s type\n", name, type);
							trim();
							if(token.type != TOKEN_OPERATOR && token.operator != OPERATOR_BRACKET_END) {
								except(TOKEN_VIRGOOL);
								trim();
							}
						}
						exceptOperator(OPERATOR_BRACKET_END);
						trim();
						statements();
					}
				}
				else {
					printf("Define global variable %s\n", ident);
					except(TOKEN_IDENT);
					if(token.type == TOKEN_OPERATOR && token.operator == OPERATOR_EQUAL) {
						next();
						expression();
						trim();
						skip(TOKEN_SEMICOLON);
					}
				}
			}
			break;
		}
	}
	// while(token.type != -1) {
	// 	printf("==>%d\n", token.type);
	// 	puts(tokenName(token));
	// }
	// for(int i = 0; i < 30; i++) {
	// 	next();
	// }
}

static void memoryFree(void* ptr) {
	free(ptr);
}

static void* memoryAlloc(unsigned long size) {
	void *ptr;
	ptr = malloc(size);
	if(!ptr && size)
		error("memory full (malloc)");
	return ptr;
}

static string readFile(const string path) {
	FILE* file = fopen(path, "rb");
	if(file == NULL)
		return NULL;

	// Find out how big the file is.
	fseek(file, 0L, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file);

	// Allocate a buffer for it.
	string buffer = (string)memoryAlloc(fileSize + 1);
	if(buffer == NULL) {
		error("Could not read file \"%s\".", path);
		exit(74);
	}

	// Read the entire file.
	size_t bytesRead = fread(buffer, 1, fileSize, file);
	if(bytesRead < fileSize) {
		error("Could not read file \"%s\".", path);
		exit(74);
	}

	// Terminate the string.
	buffer[bytesRead] = '\0';

	fclose(file);
	return buffer;
}

int main(int argc, string argv[]) {
	if(argc == 2) {
		getPath(file.path);
		file.fileName=argv[1];
		source=readFile(file.fileName);
		if(source == NULL) {
			error("source code not exists!");
		}
		scanner();
		// puts(source);
		return 0;
	}
	puts(
		"One is a tool for managing One source code.\n"
		"Usage:\n"
		"\n"
		"	one <command> [arguments]\n"
		"\n"
		"The commands are:\n"
		"\n"
		"	get         download and install packages and dependencies\n"
		"	install     compile and install packages and dependencies\n"
		"	run         compile and run One program\n"
		"	test        test packages\n"
		"	version     print One version\n"
		"\n"
		"Use \"one help <command>\" for more information about a command.\n"
		"\n"
	);
	return 0;
}
