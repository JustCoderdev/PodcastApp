#ifndef OUTILS_SGR_LIB_H
#define OUTILS_SGR_LIB_H

/***** Select Graphic Rendition
 * Enhance terminal capability
 * */

#define CSI    "\033["
#define M      "m"
#define Z      ";"
#define RESET  "0"

/* Text styles */
#define BOLD       "1"
#define DIM        "2"
#define ITALIC     "3"
#define UNDERLINE  "4"
#define SLW_BLINK  "5"
#define FST_BLINK  "6"
#define SWAP       "7"
#define CONCEAL    "8"
#define CROSS      "9"

/* Text style disabler */
#define NO_BOLD       "21"
#define NO_DIM        "22"
#define NO_ITALIC     "23"
#define NO_UNDERLINE  "24"
#define NO_BLINK      "25"

#define NO_SWAP     "27"
#define NO_CONCEAL  "28"
#define NO_CROSS    "29"

/* Foreground colors 3-bit */
#define FG_BLACK         "30"
#define FG_RED           "31"
#define FG_GREEN         "32"
#define FG_YELLOW        "33"
#define FG_BLUE          "34"
#define FG_MAGENTA       "35"
#define FG_CYAN          "36"
#define FG_WHITE         "37"
#define FG_PAL(CODE)     "38;5;" #CODE
#define FG_RGB(R, G, B)  "38;2;" #R";"#G";"#B
#define FG_DEFAULT       "39"

/* Background colors 3-bit */
#define BG_BLACK         "40"
#define BG_RED           "41"
#define BG_GREEN         "42"
#define BG_YELLOW        "43"
#define BG_BLUE          "44"
#define BG_MAGENTA       "45"
#define BG_CYAN          "46"
#define BG_WHITE         "47"
#define BG_PAL(CODE)     "48;5;" #CODE
#define BG_RGB(R, G, B)  "48;2;" #R";"#G";"#B
#define BG_DEFAULT       "49"

/* Foreground bright colors 3-bit */
#define FG_BR_BLACK  "90"
#define FG_BR_RED    "91"
#define FG_BR_GREEN  "92"
#define FG_BR_YELLO  "93"
#define FG_BR_BLUE   "94"
#define FG_BR_MAGEN  "95"
#define FG_BR_CYAN   "96"
#define FG_BR_WHITE  "97"

/* Background colors 3-bit */
#define BG_BR_BLACK  "100"
#define BG_BR_RED    "101"
#define BG_BR_GREEN  "102"
#define BG_BR_YELLO  "103"
#define BG_BR_BLUE   "104"
#define BG_BR_MAGEN  "105"
#define BG_BR_CYAN   "106"
#define BG_BR_WHITE  "107"

#endif
