/* String module for JustCoderdev Core library v4
 * */

#include <core.h>

#include <stdarg.h>
#include <stdio.h>
extern int vsnprintf(char* str, size_t size, const char *format, va_list ap);

#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>


/* #define DEBUG_STRING_ENABLE 1 */

void string_new_(String* string, n32 capacity, char* file, int line) {
	/* assert(string->chars == NULL); */
	string->chars = malloc_(capacity, file, line);
	if(string->chars == NULL) {
		printf("ERROR:%s:%d: Couldn't mallocate string, error: %s",
				file, line, strerror(errno));
		exit(failure);
	}

#if DEBUG_STRING_ENABLE
	printf("DEBUG: Mallocated string at %p with size %lu\n",
			string->chars, capacity);
#endif

	string->capacity = capacity;
	string->count = 0;
}

void string_new_from_(String* string, n32 text_len, char* text, char* file, int line) {
	n64 capacity = text_len; /* +1 to nterm */
	string->chars = malloc_(capacity, file, line);
	if(string->chars == NULL) {
		printf("ERROR:%s:%d: Couldn't mallocate string, error: %s",
				file, line, strerror(errno));
		exit(failure);
	}

#if DEBUG_STRING_ENABLE
	printf("DEBUG: Mallocated string at %p with size %lu\n", string->chars, text_len * sizeof(char));
#endif

	string->capacity = capacity;

	strncpy(string->chars, text, text_len);
	string->count = text_len;
}

void string_from_(String* string, n32 text_len, char* text, char* file, int line) {
	if(string->capacity < text_len) {
		n32 capacity = text_len; /* +1 to nterm */
		string->chars = realloc_(string->chars, capacity, file, line);
		if(string->chars == NULL) {
			printf("ERROR:%s:%d: Couldn't reallocate string, error: %s",
					file, line, strerror(errno));
			exit(failure);
		}
		string->capacity = capacity;
	}

	strncpy(string->chars, text, text_len);
	string->count = text_len;
}

void string_nterm_(String* string, char* file, int line) {
	if(string->chars == NULL || string->count == 0
			|| string->chars[string->count - 1] != '\0')
	{
#if DEBUG_STRING_ENABLE
		printf("DEBUG: Null terminating string '"STR_FMT"' at %p\n", STR(*string), string->chars);
#endif

		string_append_(string, '\0', file, line);
	}
}

void string_append_(String* string, char chr, char* file, int line) {
	if(string->chars == NULL) string_new(string, 8);
	if(string->capacity < string->count + 1) {
		string->chars = realloc_(string->chars, (string->count + 1) * 2, file, line);
		if(string->chars == NULL) {
			printf("ERROR:%s:%d: Couldn't resize string, error: %s",
					file, line, strerror(errno));
			exit(failure);
		}
		string->capacity = (string->count + 1) * 2;
	}

	string->chars[string->count] = chr;
	string->count++;
}

void string_free_(String* string, char* file, int line) {
	if(string->chars != NULL) {

#if DEBUG_STRING_ENABLE
		printf("DEBUG: Freeing string at %p ('"STR_FMT"')\n", string->chars, STR(*string));
#endif

		free_(string->chars, file, line);
	}

	string->count = 0;
	string->capacity = 0;
}

void string_clear(String* string) {
	string->count = 0;
}

void string_fmt(String* string, CString format, ...) {
	int size;
	va_list ptr;

	va_start(ptr, format);

	size = vsnprintf(string->chars, string->capacity, format, ptr);
	if(size < 0) {
		printf("ERROR:%s:%d: Couldn't format string \"%s\", error: %s",
				__FILE__, __LINE__, format, strerror(errno));
		exit(failure);
	}

	if(string->capacity < (n32)size) /* size is > 0 */
	{

#if DEBUG_STRING_ENABLE
		printf("DEBUG: Truncated string_fmt result of \"%s\"\n", format);
#endif

		string->count = string->capacity - 1;
	} else {
		string->count = size + 1; /* vsnprintf always null terminates string */
	}

	va_end(ptr);
}

void string_remove(String* string, n32 count) {
	string->count -= pclamp(count, string->count);
}

bool string_equals(String strA, n32 strB_len, const char* strB) {
	n32 i;

	if(strA.count != strB_len) return false;

	for(i = 0; i < strB_len; ++i)
		if(strA.chars[i] != strB[i])
			return false;

	return true;
}

bool string_equallit(String strA, CString strB) {
	return string_equals(strA, strlen(strB), strB);
}
