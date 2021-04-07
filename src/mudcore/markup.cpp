//
// Created by volund on 10/29/20.
//

#include "mudcore/markup.h"


namespace mudcore::markup {

    bool AnsiGround::process(std::string &mode, std::smatch &sm, bool background) {
        if(mode == "numbers") {
            auto data = sm[1];
            mode = Numbers;
        } else if (mode == "rgb") {
            mode = Rgb;
            auto red = sm[1];
            auto green = sm[2];
            auto blue = sm[3];
        } else if (mode == "hex1" || mode == "hex2") {

            if(mode == "hex1") {
                mode = Hex1;
            } else {
                mode = Hex2;
            }
            auto data = sm[1];

        } else if (mode == "name") {
            auto data = sm[1];
        }
        return true;
    }


    bool AnsiData::parse_markup(std::string &src, std::string &errmsg) {
        std::string codes(src);
        std::smatch sm;
        bool val = false;

        while(codes.length()) {
            switch(codes[0]) {
                case '/':
                case '!':
                    codes = codes.substr(1);
                    if(codes.length() == 0) {
                        return true;
                    }
                    if(codes[0] == '/' || codes[0] == '!' || codes[0] == ' ') {
                        continue;
                    }
                    val = false;
                    for(auto r : get_match()) {
                        auto mode = std::get<std::string>(r);
                        auto reg = std::get<std::regex>(r);
                        if(std::regex_match(codes, sm, reg)) {

                            if(bg.process(mode, sm, true)) {
                                codes = codes.substr(sm.length(0));
                                val = true;
                                break;
                            }
                            else {
                                errmsg = "#-1 INVALID ANSI DEFINITION: " + src;
                                return false;
                            }
                        }
                    }
                    if(!val) {
                        errmsg = "#-1 INVALID ANSI DEFINITION: " + src;
                        return false;
                    }
                    break;
                case ' ':
                    codes = codes.substr(1);
                    continue;
                    break;
                default:
                    val = false;
                    for(auto r : get_match()) {
                        auto mode = std::get<std::string>(r);
                        auto reg = std::get<std::regex>(r);
                        if(std::regex_match(codes, sm, reg)) {

                            if(fg.process(mode, sm, true)) {
                                codes = codes.substr(sm.length(0));
                                val = true;
                                break;
                            }
                            else {
                                errmsg = "#-1 INVALID ANSI DEFINITION: " + src;
                                return false;
                            }
                        }
                    }
                    if(!val) {
                        errmsg = "#-1 INVALID ANSI DEFINITION: " + src;
                        return false;
                    }
            }
        }
        return true;
    }

    Markup::Markup(char type) {
        switch(type) {
            case 'p':
                this->type = MXP;
                break;
            case 'c':
                this->type = Color;
                break;
            default:
                this->type = Color;
                break;
        }
        switch(this->type) {
            case Color:
                this->ansi.emplace();
        }
    }

    void Markup::set_parent(const ShMarkup &mark) {
        this->parent.emplace(mark);
    }

    bool Markup::validate(std::string &errmsg) {
        switch(type) {
            case Color:
                return ansi.value()->parse_markup(start_text, errmsg);
                break;
            case MXP:
                return true;
                break;
            default:
                return false;
                break;
        }
    }

    MarkupString::MarkupString() {

    }

    MarkupString::MarkupString(const MarkupString &src) : text(src.text), source(src.source), markup(src.markup), idx(src.idx) {

    }

    MarkupString::MarkupString(const std::string &src) : text(src), source(src) {
        clear_idx();
    }

    void MarkupString::clear_idx() {
        idx.clear();
        for(auto c : text) {
            idx.emplace_back(OpMarkup(), c);
        }
    }

    void MarkupString::regen_text() {
        text.clear();
        for(auto i : idx) {
            char c = std::get<char>(i);
            text.push_back(c);
        }
    }

    bool MarkupString::decode_markup(const char *src) {
        auto s = std::string(src);
        return decode_markup(s);
    }

    bool MarkupString::decode_markup(std::string &src) {
        if(!idx.empty()) {
            idx.clear();
            markup.clear();
            text.clear();
        }
        source = src;
        idx.reserve(source.length());
        std::vector<ShMarkup> mstack;
        char tag;
        uint8_t state = 0;
        OpMarkup current;

        for(auto s: source) {
            switch(state) {
                case 0: // We are not inside of a tag opener or closer.
                    if(s == TAG_START) {
                        state = 1;
                    } else {
                        text.push_back(s);
                        idx.emplace_back(MarkupIdx(OpMarkup(), s));
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
                        auto mk = std::make_shared<Markup>(tag);
                        markup.emplace_back(mk);
                        if(current.has_value()) {
                            mk->set_parent(current.value());
                        }
                        current = markup.back();
                        mstack.push_back(mk);
                        mk->start_text.push_back(s);
                    }
                    break;
                case 3: // we are inside an opening tag, gathering text. continue until TAG_END.
                    if(s == TAG_END) {
                        state = 0;
                    } else {
                        markup.back()->start_text.push_back(s);
                    }
                    break;
                case 4: // we are inside a closing tag, gathering text. continue until TAG_END.
                    if(s == TAG_END) {
                        // pop up a depth level.
                        std::string errmsg;
                        if(!current.value()->validate(errmsg)) {
                            idx.clear();
                            mstack.clear();
                            markup.clear();
                            text = errmsg;
                            for (auto s2 : errmsg) {
                                idx.emplace_back(MarkupIdx(OpMarkup(), s2));
                            }
                            return false;
                        }
                        current = std::make_optional(mstack.back());
                        mstack.pop_back();
                        state = 0;
                    } else {
                        mstack.back()->end_text.push_back(s);
                    }
                    break;
                default:
                    break;
            }
        }
        return true;
    }

    void MarkupString::reverse() {
        std::reverse(text.begin(), text.end());
        std::reverse(idx.begin(), idx.end());
    }

    MarkupString MarkupString::operator+(const MarkupString &other) {
        auto clone = MarkupString(*this);
        return clone += other;
    }

    MarkupString MarkupString::operator+(const char *other) {
        auto clone = MarkupString(*this);
        auto s = std::string(other);
        return clone += s;
    }

    MarkupString MarkupString::operator+(const char other) {
        auto clone = MarkupString(*this);
        return clone += other;
    }

    MarkupString MarkupString::operator+(const std::string &other) {
        auto clone = MarkupString(*this);
        return clone += other;
    }

    MarkupString &MarkupString::operator+=(const MarkupString &other) {
        text += other.text;
        source += other.source;
        markup.insert(markup.end(), other.markup.begin(), other.markup.end());
        idx.insert(idx.end(), other.idx.begin(), other.idx.end());
        return *this;
    }

    MarkupString& MarkupString::operator+=(const char *other) {
        auto s = std::string(other);
        return *this += s;
    }

    MarkupString& MarkupString::operator+=(const std::string &other) {
        for(auto c : other) {
            this->idx.emplace_back(OpMarkup(), c);
        }
        this->text += other;
        return *this;
    }

    MarkupString& MarkupString::operator+=(const char other) {
        this->idx.emplace_back(OpMarkup(), other);
        this->text.push_back(other);
        return *this;
    }

    std::vector<std::tuple<std::string, std::regex>> AnsiData::ansi_codes;

    std::vector<std::tuple<std::string, std::regex>> &AnsiData::get_match() {
        if (ansi_codes.empty()) {
            ansi_codes.reserve(6);
            ansi_codes.emplace_back("letters", std::regex("^([a-z ]+)\\b"));
            ansi_codes.emplace_back("numbers", std::regex("^(\\d+)\\b"));
            ansi_codes.emplace_back("rgb", std::regex(R"(^<(\d{1,3})\s+(\d{1,3})\s+(\d{1,3})>(\b)?)"));
            ansi_codes.emplace_back("hex1", std::regex("^#([0-9A-F]{6})\\b"));
            ansi_codes.emplace_back("hex2", std::regex("^<#([0-9A-F]{6})>(\\b)?"));
            ansi_codes.emplace_back("name", std::regex(R"(^\+(\w+)\b)"));
        }
        return ansi_codes;
    }

    size_t MarkupString::size() const {
        return this->text.size();
    }

    size_t MarkupString::length() const {
        return this->text.length();
    }

    size_t MarkupString::max_size() const {
        return this->text.max_size();
    }

    void MarkupString::clear() {
        this->text.clear();
        this->source.clear();
        this->markup.clear();
        this->idx.clear();
    }

    bool MarkupString::empty() const {
        return this->text.empty();
    }

    char& MarkupString::operator[](size_t pos) {
        return this->text[pos];
    }

    const char& MarkupString::operator[](size_t pos) const {
        return this->text[pos];
    }

    const char& MarkupString::front() const {
        return this->text.front();
    }

    const char& MarkupString::back() const {
        return this->text.back();
    }

    const char& MarkupString::at(size_t pos) const {
        return this->text.at(pos);
    }

    void MarkupString::push_back(char c) {
        *this += c;
    }



}
