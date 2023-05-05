

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include "compile.h"
#include "fs.h"

std::string compile::get_html(std::string style, std::string body, std::string script){
    
    std::string js = compile::get_js(script);
    auto html = fs::read("../templet/index.html");
    size_t len = strlen(html.data()) + strlen(style.data()) + strlen(body.data()) + strlen(js.data());
    char* ret=(char*)malloc(sizeof(char) * len);
    memset(ret, 0, len);
    sprintf(ret, html.data(), style.data(), body.data(), js.data());
    return ret;
}

std::string compile::get_js(std::string script){

    std::string js = fs::read("../templet/index.js");
    size_t len = strlen(js.data())+strlen(script.data()) ;
    char* ret = (char*)malloc(sizeof(char) * len);
    memset(ret, 0, len);
    sprintf(ret, js.data(), script.data());
    return ret;
}