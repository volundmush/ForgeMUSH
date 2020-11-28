//
// Created by volund on 10/29/20.
//

#include "../../include/forgemush/ansi.h"

AnsiMarkup::AnsiMarkup(size_t in_idx, size_t in_parent, size_t in_start, size_t in_depth, char in_type) {
    idx = in_idx;
    parentIdx = in_parent;
    start = in_start;
    type = in_type;
    depth = in_depth;
}

void AnsiMarkup::auto_close(size_t pos) {
    end = pos;
    // TODO: Whatever auto-logic is necessary for terminating an improperly ended AnsiMarkup section.
}

bool AnsiString::has_markup(std::string *str) {
    return str->find_first_of(MARKUP_ANY) != std::string::npos;
}

AnsiString::AnsiString(const char *src) : source(src), text() {
    // If there isn't any markup, there's no need to go further.

    if(!AnsiString::has_markup(&source)) {
        return;
    }

    // String has markup. nuts.
    std::vector<size_t> mstack;
    mstack.reserve(5);
    char tag = 0;
    size_t state = 0, pos = 0, index = 0;
    markup.emplace_back(0, 0, 0, 0, 'z');

    for(auto s: source) {
        switch(state) {
            case 0: // We are not inside of a tag opener or closer.
                if(s == TAG_START) {
                    state = 1;
                } else {
                    text.push_back(s);
                    idx.push_back(index);
                    pos++;
                }
                break;
            case 1: // we encountered a TAG START... what is the tag type?
                tag = s;
                state = 2;
                break;
            case 2: // we are just inside a tag. if it begins with / this is a closing. else, opening.
                if(s == '/') {
                    // yup, it's closing.
                    state = 4;
                } else {
                    state = 3;
                    // This is an opening tag!
                    if(mstack.size()) {
                        markup.emplace_back(markup.size(), mstack.back(), pos, mstack.size()-1, tag);
                    } else {
                        markup.emplace_back(markup.size(), 0, pos, mstack.size()-1, tag);
                    }
                    index = markup.size()-1;
                    mstack.push_back(index);
                    markup[mstack.back()].start_text.push_back(s);
                }
                break;
            case 3: // we are inside an opening tag, gathering text. continue until TAG_END.
                if(s == TAG_END) {
                    state = 0;
                } else {
                    markup[mstack.back()].start_text.push_back(s);
                }
                break;
            case 4: // we are inside a closing tag, gathering text. continue until TAG_END.
                if(s == TAG_END) {
                    // pop up a depth level.
                    auto m = markup[mstack.back()];
                    m.end = pos;
                    index = m.parentIdx;
                    mstack.pop_back();
                    state = 0;
                } else {
                    markup[mstack.back()].end_text.push_back(s);
                }
                break;
            default:
                break;
        }
    }

    // We have finished iterating over the provided string. check for unclosed tags!
    for(auto i = mstack.rbegin(); i != mstack.rend(); ++i) {
        markup[*i].auto_close(pos);
    }
}

void AnsiString::reverse() {
    std::reverse(text.begin(), text.end());
    std::reverse(idx.begin(), idx.end());
}