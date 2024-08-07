/* Logger module for JustCoderdev Core library v1
 * */

#include <core.h>
#include <sgr.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


void core_log(CString level, /* CString color, */ CString file, int line, CString module, CString format, ...) {
	va_list args;
	va_start(args, format);

	/* CSI RESET Z BOLD Z FG_WHITE Z "%s" M "%s" */
	/* CSI FG_BR_BLACK M "%s:%d: " */
	fprintf(LOG_STREAM, "%s:%s:%d: ", level, file, line);
	if(module != NULL) fprintf(LOG_STREAM, "[%s] ", module);

	vfprintf(LOG_STREAM, format, args);
	va_end(args);
}
