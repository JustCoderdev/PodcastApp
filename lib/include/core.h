/* JustCoderdev's Core library v7
 * */

#ifndef CORE_H_
#define CORE_H_

/* #define DEBUG_ENABLE 1 */
/* #define DEBUG_STRING_ENABLE 1 */
/* #define DEBUG_MEMDEB_ENABLE 1 */

#include <sgr.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>

/* Types */
typedef unsigned char uchar;

typedef unsigned char     n8;
typedef unsigned short    n16;
typedef unsigned int      n32;
typedef unsigned long int n64;

typedef signed char     i8;
typedef signed short    i16;
typedef signed int      i32;
typedef signed long int i64;

#define RL_BOOL_TYPE
typedef enum bool {
	true = (1 == 1),
	false = (1 != 1)
} bool;

typedef enum error {
	success = false,
	failure = true
} error;

typedef int Errno;

/* Strings */
typedef const char* CString;
typedef struct String {
	n32 count, capacity;
	char* chars;
} String;

#define STR_FMT "%.*s"
#define STR(STRING) (STRING).count, (STRING).chars

#define string_new(STRING, CAPACITY) string_new_(STRING, CAPACITY, __FILE__, __LINE__)
#define string_new_from(STRING, TEXT_LEN, TEXT) string_new_from_(STRING, TEXT_LEN, TEXT, __FILE__, __LINE__)
#define string_from(STRING, TEXT_LEN, TEXT) string_from_(STRING, TEXT_LEN, TEXT, __FILE__, __LINE__)
#define string_cpy(STRING, TEXT_LEN, TEXT) string_cpy_(STRING, TEXT_LEN, TEXT, __FILE__, __LINE__)
#define string_nterm(STRING) string_nterm_(STRING, __FILE__, __LINE__)
#define string_append(STRING, CHR) string_append_(STRING, CHR, __FILE__, __LINE__)
#define string_free(STRING) string_free_(STRING, __FILE__, __LINE__)

/* TODO: use int instead of n64 and swap  char* with size */
extern void string_new_(String* string, n32 capacity, char* file, int line);
extern void string_new_from_(String* string, n32 text_len, char* text, char* file, int line);
extern void string_from_(String* string, n32 text_len, char* text, char* file, int line);
extern void string_nterm_(String* string, char* file, int line); /* Null terminate a string */
extern void string_append_(String* string, char chr, char* file, int line);
extern void string_free_(String* string, char* file, int line);

extern void string_clear(String* string);
extern void string_fmt(String* string, CString format, ...);
extern void string_remove(String* string, n32 count);

extern bool string_equals(String strA, n32 strB_len, const char* strB);
extern bool string_equallit(String strA, CString strB);

/* Memory debug */
#define dmalloc(SIZE)        malloc_(SIZE, __FILE__, __LINE__)
#define dfree(PTR)           free_(PTR, __FILE__, __LINE__)
#define dcalloc(NMEMB, SIZE) calloc_(NMEMB, SIZE, __FILE__, __LINE__)
#define drealloc(PTR, SIZE)  realloc_(PTR, SIZE, __FILE__, __LINE__)

extern void *malloc_(size_t size, char* file, int line);
extern void free_(void* ptr, char* file, int line);
extern void *calloc_(size_t nmemb, size_t size, char* file, int line);
extern void *realloc_(void* ptr, size_t size, char* file, int line);

/* Logger */
#ifndef LOG_MODULE
#define LOG_MODULE NULL
#endif

#ifndef LOG_STREAM
#define LOG_STREAM stdout
#endif

#define CORE_INFO   "INFO",   /*  FG_BLUE,    */ __FILE__, __LINE__, LOG_MODULE
#define CORE_DEBUG  "DEBUG",  /*  FG_GREEN,   */ __FILE__, __LINE__, LOG_MODULE
#define CORE_WARN   "WARN",   /*  FG_YELLOW,  */ __FILE__, __LINE__, LOG_MODULE
#define CORE_ERROR  "ERROR",  /*  FG_RED,     */ __FILE__, __LINE__, LOG_MODULE
extern void core_log(CString level, /* CString color, */ CString file,
		int line, CString module, CString format, ...);

/* Macros */
#ifndef min /* clamp(0, x, y) = min(x, y) if x is unsigned */
#define min(A, B) ((A) > (B) ? (B) : (A))
#endif

#ifndef max
#define max(A, B) ((A) > (B) ? (A) : (B))
#endif

#ifndef clamp
#define clamp(MIN, VAL, MAX) ((VAL) <= (MIN) ? (MIN) : ((VAL) >= (MAX) ? (MAX) : (VAL)))
#endif

#ifndef pclamp /* positive clamp */
#define pclamp(VAL, MAX) ((VAL) > (MAX) ? (MAX) : (VAL))
#endif

#ifndef oclamp /* overflow clamp */
#define oclamp(MIN, VAL, MAX) ((VAL) < (MIN) ? (MAX) : ((VAL) > (MAX) ? (MIN) : (VAL)))
#endif

#ifndef range_out
#define range_out(LB, VAL, HB) ((VAL) < (LB) || (VAL) > (HB))
#endif

#ifndef range_in
#define range_in(LB, VAL, HB) ((VAL) > (LB) && (VAL) < (HB))
#endif


/* Bit level stuff */
extern void printb(n8 byte);
extern void printw(n32 word);

extern void savebuff(FILE *file, char *buffer, n64 buff_len);


/* Net */
/* typedef union ip4_addr { */
	/* struct in_addr addr; */
	/* struct { n8 D, C, B, A; }; */
	/* n8 bytes_rev[4]; */
/* } in_addr; */

/* Convert decimal ipv4 address (192.168.42.069) to `struct in_addr` */
extern struct in_addr addr_to_bytes(n8 A, n8 B, n8 C, n8 D);

/* Given the hostname the function will try to resolve it and fill the
 * `address` field */
extern error hostname_resolve(const char *hostname, struct in_addr *address);


/* Buffer */
/* Return the index of the chr or -1 */
extern i64 buffer_find_chr(char chr, char *buffer, n64 len);

/* Return the index of the first char that is not a delimiter or -1 */
extern i64 buffer_skip_str(char *del, n64 del_len, char *buffer, n64 buff_len);

/* Copy from src to dest buffers until the delimiter is reached */
extern n64 buffer_copy_until_chr(char delimiter,
            char *src_buffer, n64 src_len,
            char *dest_buffer, n64 dest_len);

/* Copy from src to dest buffers until the delimiter is reached */
extern n64 buffer_copy_until_str(char *delimiter, n64 del_len,
            char *src_buffer, n64 src_len,
            char *dest_buffer, n64 dest_len);


/* Dinamic array fast implementation --------------------------- */

/* typedef struct VoidArray { */
/* 	char* items; */
/* 	char item_size; */
/* 	n64 count; */
/* 	n64 capacity; */
/* } VoidArray; */

#define arr_new(arr, cap) \
	do { \
		if((arr)->capacity < (cap)) { \
			(arr)->items = drealloc((arr)->items, (cap) * sizeof(*(arr)->items)); \
			if((arr)->items == NULL) { \
				printf("ERROR:%s:%d: Couldn't resize array, message: %s", \
						__FILE__, __LINE__, strerror(errno)); \
				exit(failure); \
			} \
			(arr)->capacity = (cap); \
		} \
		(arr)->count = 0; \
	} while(0)

#define arr_from(arr, src, src_len) do { \
	if((arr)->capacity < (src_len)) { \
		(arr)->items = drealloc((arr)->items, (src_len) * sizeof(*(arr)->items)); \
		if((arr)->items == NULL) { \
			printf("ERROR:%s:%d: Couldn't resize array, message: %s", \
					__FILE__, __LINE__, strerror(errno)); \
			exit(failure); \
		} \
		(arr)->capacity = (src_len); \
	} \
	(void)memcpy((arr)->items, (src)->items, (src_len)); \
	(arr)->count = (src_len); \
} while(0)

#define arr_at_end(arr, off) ((arr)->items[(arr)->count - 1 - (off)])

#define arr_append(arr, item) do { \
	if((arr)->capacity < (arr)->count + 1) { \
		(arr)->items = drealloc((arr)->items, ((arr)->count + 1) * 2 * sizeof(*(arr)->items)); \
		if((arr)->items == NULL) { \
			printf("ERROR:%s:%d: Couldn't resize array, message: %s", \
					__FILE__, __LINE__, strerror(errno)); \
			exit(failure); \
		} \
		(arr)->capacity = ((arr)->count + 1) * 2; \
	} \
	(arr)->items[(arr)->count] = (item); \
	(arr)->count++; \
} while(0)
/* Dinamic array fast implementation END ----------------------- */

#endif /* CORE_ */
