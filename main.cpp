#include <bits/types/FILE.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <future>
#include <iostream>
#include <ostream>
#include <string>
#include <unistd.h>

#include "draw.hpp"

#include "event_emitter.h"
#include "evt_bus.h"
#include "fs.h"
#include "compile.h"
#include "vm.h"

extern "C" {

#include <quickjs/quickjs.h>
#include <quickjs/quickjs-libc.h>

}

draw* d_ptr;
void test_c(){
    draw d("test"); // 必须第一位?
    d_ptr = &d;

    std::string body = fs::read("../dist/index.html");
    std::string style = fs::read("../dist/index.css");
    std::string script = fs::read("../dist/index.js");

    std::string content  =  compile::get_html(style, body, script);
    std::cout<<content<<std::endl;

    event_emitter* evt = new evt_bus();

    d.render(content, evt,[&](){
        std::cout<<"render"<<std::endl;
        //d.emit("test","my is native");
        vm* v = new vm(evt);   

        std::string script1= R"(
            print("qjs",__native_handler.length)
            __native_handler("qjs->cxx->wv","my is qjs")
            binder_on("wv->cxx->qjs", function(v){
                console.log("wv->cxx->qjs", v)
            })
        )";
        v->eval(script1);
        v->on("qjs->cxx->wv", [&](std::string content){
            d.emit("qjs->cxx->wv", content);
        });
        d.on("wv->cxx->qjs", [&](std::string content){
           v->emit("wv->cxx->qjs", content);
        });
        d.emit("run", "123");
        evt->loop();
    });
    d.loop();
}

void test_js(){
    evt_bus* ebus = new evt_bus();
    vm v(ebus);
    v.on("test",[](std::string context){
        std::cout<<"native: "<<context<<std::endl;
    });

    std::string script1= R"(
        print(__native_handler.length)
        __native_handler("test","123")
    )";
    v.eval(script1);

    ebus->loop();

    return;
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
  // test_js();
  test_c();

    //test_read();
    std::cout<<"abc123"<<std::endl;
}

/*
draw main // 事件循环, 不退出
worker    // 事件循环, 不退出

draw  => native => js
之间通过事件队列 进行通信
*/