


#include <chrono>

#include <iostream>
#include <unistd.h>

#include "evt_bus.h"

evt_bus::evt_bus(){
    this->queue = new ReaderWriterQueue<event>(1024);
    this->handler = {};
}

evt_bus::~evt_bus(){
    delete this->queue;
}

void evt_bus::on(std::string topic, callback cb) {

    std::lock_guard<std::shared_mutex> sl(this->rw_lock); //write lock obs

    auto iter = this->handler.find(topic);
    if (iter == this->handler.end()) {
        this->handler[topic] = std::set<observer>();
        iter = this->handler.find(topic);
    }
    iter->second.insert(observer{cb});
}

void evt_bus::emit(std::string topic, std::string content) {

    event evt{topic, content};
    this->queue->enqueue(evt);

    std::unique_lock<std::mutex> ul(this->lock);
    cond.notify_all();
}

void evt_bus::loop(){

    std::chrono::milliseconds ms(1);
    while(true) {
        std::unique_lock<std::mutex> ul(this->lock);
        cond.wait_for(ul, ms);
        this->next_tick();  
    }
}

void evt_bus::next_tick(){

    while (true) {
        
        std::shared_lock<std::shared_mutex> sl(this->rw_lock);//read lock obs

        event evt;
        this->queue->try_dequeue(evt);
        if ( evt.topic == "" ) {
            break;
        }  
        auto iter = this->handler.find(evt.topic);
        if (iter == this->handler.end()) {
            continue;
        }
        for (observer obs : iter->second){
            obs.cb(evt.content);
        }
    }
}