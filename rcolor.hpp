#ifndef R_STD_COLOR_HPP
#define R_STD_COLOR_HPP
#include "rstdafx.hpp"
#include <iostream>
#include <string>
#include <stdexcept>
#include <exception>
using std::string;
using std::basic_ostream;

_recolic_hpp_begin

enum class color_t {color_unset, black, red, green, brown, blue, magenta, cyan, lightgray};
enum class font_t {font_unset = 0, bold = 1, underline = 4, dark = 2, background = 7, striked = 9};

class fontInfo
{
public:
    fontInfo(color_t text_color) : textColor(text_color) {}
    fontInfo(font_t font_type) : fontType(font_type) {}
    fontInfo(color_t text_color, font_t font_type) : textColor(text_color), fontType(font_type) {}
    fontInfo() : textColor(color_t::color_unset), fontType(font_t::font_unset) {}
    color_t textColor;
    font_t fontType;
};

fontInfo clear;
constexpr int _color_to_int(const color_t &_ct)
{
    return static_cast<int>(_ct);
}
constexpr int _font_to_int(const font_t &_ft)
{
    return static_cast<int>(_ft);
}

struct _rosi_font {_rosi_font(const fontInfo &_ref_fi) : _ref_fi(_ref_fi) {} const fontInfo &_ref_fi;};
inline _rosi_font setfont(const fontInfo &__fi) {return _rosi_font(__fi);}

template<typename _CharT, typename _Traits>
    inline basic_ostream<_CharT, _Traits>& 
    operator<<(basic_ostream<_CharT, _Traits>& __os, const fontInfo &__f)
    {
/**
 * Env rewrited...
 * Don't mind codes above.
 * by Recolic
 */
        string formattedStr = "\033[";
        formattedStr += (char)('0' + _font_to_int(__f.fontType));

        char colorCode = '8';
#define _R_SETFONT_SETCASE(color, code) case _color_to_int(color): colorCode = code; break;
        switch(_color_to_int(__f.textColor))
        {
        case _color_to_int(color_t::color_unset): break;
        _R_SETFONT_SETCASE(color_t::black,'0')
        _R_SETFONT_SETCASE(color_t::red,'1')
        _R_SETFONT_SETCASE(color_t::green,'2')
        _R_SETFONT_SETCASE(color_t::brown,'3')
        _R_SETFONT_SETCASE(color_t::blue,'4')
        _R_SETFONT_SETCASE(color_t::magenta,'5')
        _R_SETFONT_SETCASE(color_t::cyan,'6')
        _R_SETFONT_SETCASE(color_t::lightgray,'7')
        default:
            throw std::invalid_argument("Unknown color_t in font_to_set.");
        }
#undef _R_SETFONT_SETCASE
        if(colorCode != '8')
        {
            formattedStr += ";3";
            formattedStr += colorCode;
        }

        formattedStr += 'm';//Done.
        __os << formattedStr;
        return __os;
    }

template<typename _CharT, typename _Traits>
    inline basic_ostream<_CharT, _Traits>& 
    operator<<(basic_ostream<_CharT, _Traits>& __os, _rosi_font __rosi_f)
    {
        const fontInfo &__f = __rosi_f._ref_fi;
        return operator<<<_CharT, _Traits>(__os, __f);
    }

string fontFormat(fontInfo font_to_set, const string &src_str)
{
    
}

_recolic_hpp_end
#endif