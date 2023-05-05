#ifndef __draw_h__
#define __draw_h__

#include "event_emitter.h"
#include <iostream>
#include <functional>
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>
#include <string>

#include <readerwriterqueue/readerwriterqueue.h>
#include <thread>
#include <set>
#include <map>
#include <nlohmann/json.hpp>
#include <unistd.h>

#include <webview/webview.h>

using namespace moodycamel;

using json = nlohmann::json;



class draw{
private:
    webview::webview* wv;
 
    std::thread worker;

    std::string content;
    event_emitter* evt;
    std::function<void()> job;
public:
    draw()=default;
    draw(std::string title){

        this->wv = new webview::webview(true, nullptr);
        this->wv->set_title(title);
        this->wv->set_size(480, 320, WEBVIEW_HINT_NONE);
    }
    ~draw(){
        delete this->wv;
    }
    void render(std::string content, event_emitter* evt, std::function<void()> job){
        this->content = content;
        this->evt = evt;
        this->job = job;
    }

    void on(std::string topic, callback cb){
        this->evt->on(topic, cb);
    }
    void emit(std::string topic, std::string data){   
        this->wv->dispatch([=](){
            std::string js = "__js_handler(\""+topic+"\",\""+data+"\")";
            this->wv->eval(js);
        });
    }
    void loop(){
        this->wv->bind("__native_handler", [&](std::string data)->std::string{
            json arr = json::parse(data);
            std::string topic = arr[0].get<std::string>();
            std::string content = arr[1].get<std::string>();
            this->evt->emit(topic,content);
            this->evt->next_tick();
            return "";
        });
        this->wv->set_html(this->content);
        this->run(this->job);
        this->wv->run();
    }
    void run(std::function<void()> job){
        this->worker = std::thread([&](){
            usleep(1000*500);
            job();
        });
        this->worker.detach();
    }
};


#endif