//
// Created by volund on 10/29/20.
//

#ifndef FORGEMUSH_ANSI_H
#define FORGEMUSH_ANSI_H

#include <vector>
#include <string>
#include <cstdint>

#define BEEP_CHAR '\a'
#define ESC_CHAR '\x1B'

#define ANSI_RAW_NORMAL "\x1B[0m"

#define TAG_START '\002'
#define TAG_END '\003'
#define MARKUP_START "\002"
#define MARKUP_END "\003"

#define MARKUP_ANY "\x1b\002\003"

#define ANSI_HILITE MARKUP_START "ch" MARKUP_END
#define ANSI_INVERSE MARKUP_START "ci" MARKUP_END
#define ANSI_BLINK MARKUP_START "cf" MARKUP_END
#define ANSI_UNDERSCORE MARKUP_START "cu" MARKUP_END

#define ANSI_INV_BLINK MARKUP_START "cfi" MARKUP_END
#define ANSI_INV_HILITE MARKUP_START "chi" MARKUP_END
#define ANSI_BLINK_HILITE MARKUP_START "cfh" MARKUP_END
#define ANSI_INV_BLINK_HILITE MARKUP_START "cifh" MARKUP_END

/* Foreground colors */

#define ANSI_PLAIN MARKUP_START "n" MARKUP_END
#define ANSI_BLACK MARKUP_START "cx" MARKUP_END
#define ANSI_RED MARKUP_START "cr" MARKUP_END
#define ANSI_GREEN MARKUP_START "cg" MARKUP_END
#define ANSI_YELLOW MARKUP_START "cy" MARKUP_END
#define ANSI_BLUE MARKUP_START "cb" MARKUP_END
#define ANSI_MAGENTA MARKUP_START "cm" MARKUP_END
#define ANSI_CYAN MARKUP_START "cc" MARKUP_END
#define ANSI_WHITE MARKUP_START "cw" MARKUP_END

#define ANSI_HIBLACK MARKUP_START "chx" MARKUP_END
#define ANSI_HIRED MARKUP_START "chr" MARKUP_END
#define ANSI_HIGREEN MARKUP_START "chg" MARKUP_END
#define ANSI_HIYELLOW MARKUP_START "chy" MARKUP_END
#define ANSI_HIBLUE MARKUP_START "chb" MARKUP_END
#define ANSI_HIMAGENTA MARKUP_START "chm" MARKUP_END
#define ANSI_HICYAN MARKUP_START "chc" MARKUP_END
#define ANSI_HIWHITE MARKUP_START "chw" MARKUP_END

/* Background colors */

#define ANSI_BBLACK MARKUP_START "cX" MARKUP_END
#define ANSI_BRED MARKUP_START "cR" MARKUP_END
#define ANSI_BGREEN MARKUP_START "cG" MARKUP_END
#define ANSI_BYELLOW MARKUP_START "cY" MARKUP_END
#define ANSI_BBLUE MARKUP_START "cB" MARKUP_END
#define ANSI_BMAGENTA MARKUP_START "cM" MARKUP_END
#define ANSI_BCYAN MARKUP_START "cC" MARKUP_END
#define ANSI_BWHITE MARKUP_START "cW" MARKUP_END

#define ANSI_END MARKUP_START "c/" MARKUP_END
#define ANSI_ENDALL MARKUP_START "c/a" MARKUP_END

#define ANSI_NORMAL ANSI_ENDALL


/* Different ways of handling ANSI colors */
#define ANSI_FORMAT_NONE 0 /**< Strip all colors */
#define ANSI_FORMAT_HILITE                                                     \
  1 /**< Only show ANSI highlight, no colors/underline/etc */
#define ANSI_FORMAT_16COLOR                                                    \
  2 /**< Show the full basic ANSI palette, including highlight, underline, etc \
     */
#define ANSI_FORMAT_XTERM256 3 /**< Use the 256 color XTERM palette */
#define ANSI_FORMAT_HTML                                                       \
  4 /**< Show colors as HTML tags. Not currently used.                         \
     */

#define MARKUP_COLOR 'c'
#define MARKUP_COLOR_STR "c"
#define MARKUP_HTML 'p'
#define MARKUP_HTML_STR "p"
#define MARKUP_OLDANSI 'o'
#define MARKUP_OLDANSI_STR "o"

#define MARKUP_WS 'w'
#define MARKUP_WS_ALT 'W'
#define MARKUP_WS_ALT_END 'M'

#define NOMARKUP (-1) /**< Character has no markup */

class AnsiMarkup {
public:
    size_t idx = 0, parentIdx = 0, start = 0, end = 0, depth = 0;
    char type = 0;
    bool standalone = false;
    std::string start_text, end_text;
    AnsiMarkup(size_t in_idx, size_t in_parent, size_t in_start, size_t in_depth, char in_type);
    void auto_close(size_t pos);
};

class AnsiString {
public:
    std::string text, source;
    unsigned int flags;
    std::vector<AnsiMarkup> markup;
    std::vector<size_t> idx;

    AnsiString(const char *src);
    static bool has_markup(std::string *str);
    void reverse();

};


#endif //FORGEMUSH_ANSI_H
