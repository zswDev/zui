#ifndef __compile_h__
#define __compile_h__

#include <string>

class compile {
public:
    static std::string get_html(std::string style, std::string body, std::string script);
    static std::string get_js(std::string script);
};

#endif