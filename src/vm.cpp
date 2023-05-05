#include "vm.h"
#include "evt_bus.h"
#include "quickjs/quickjs.h"

#include <cstring>
#include <functional>
#include <iostream>
#include <iterator>
#include <mutex>
#include <shared_mutex>
#include <string>


std::map<JSContext*, vhf> vm::global_ctx_map = {};
std::shared_mutex vm::global_rw_lock = {};

vm::vm(event_emitter *evt){
    this->new_runtime();
    this->new_handler();
    this->evt = evt;  
}

vm::~vm(){
    JS_FreeValue(this->ctx, this->global);
    JS_FreeContext(this->ctx);
    JS_FreeRuntime(this->rt);
}

void vm::try_catch(JSValue ret){
    if (!JS_IsException(ret)) {
        return;
    }
    JSValue err = JS_GetException(this->ctx);
    std::string msg= JS_ToCString(this->ctx, err);
    std::cout<<msg<<std::endl;
    JS_FreeValue(this->ctx, err);
}

void vm::new_runtime(){

    this->rt = JS_NewRuntime();
    if (rt == nullptr) {
        throw "vm runtime err";
    }
    this->ctx = JS_NewContext(this->rt);
    if (this->ctx == nullptr) {
        throw "vm ctx err";
    }
    js_std_add_helpers(ctx, 0, nullptr);
    JS_SetRuntimeInfo(this->rt, "vm");
}

void vm::new_handler(){

    this->global = JS_GetGlobalObject(ctx);
    this->native_handler = JS_NewCFunction(ctx, vm::global_handler, "__native_handler", 2);
    JS_SetPropertyStr(ctx, this->global, "__native_handler", native_handler);

    std::lock_guard<std::shared_mutex> lg(vm::global_rw_lock);

    this->f = std::bind(
        &vm::handler,
        this, 
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4);
    vm::global_ctx_map[this->ctx]=this->f;

    std::string init_script = R"(
const __binder_handler={}

function binder_on(topic, cb){
    let handle = __binder_handler[topic] 
    if(handle === undefined){ 
        handle = new Set() 
        __binder_handler[topic] = handle 
    } 
    handle.add(cb) 
}

function binder_emit(topic, content){
    __native_handler(topic, content) 
}

function __js_handler(topic, content){
    let handle = __binder_handler[topic] 
    if(handle === undefined){ 
        return 
    }   
    for(let cb of handle) { 
        cb(content) 
    } 
}
    )";
    this->eval(init_script);

    this->js_handler = JS_GetPropertyStr(this->ctx,this->global, "__js_handler");
}

JSValue vm::global_handler(JSContext* ctx, JSValueConst js_this, int argc, JSValueConst* argv){
    if (argc < 2) {
        return JS_EXCEPTION;
    }
    std::shared_lock<std::shared_mutex> sl(vm::global_rw_lock);
    std::map<JSContext*,vhf>::iterator iter = vm::global_ctx_map.find(ctx);
    if (iter == vm::global_ctx_map.end()) {
        return JS_EXCEPTION;
    }
    return iter->second(ctx, js_this, argc, argv);
}

JSValue vm::handler(JSContext* ctx, JSValueConst js_this, int argc, JSValueConst* argv){

    if (argc < 2) {
        return JS_EXCEPTION;
    }
    std::string topic = JS_ToCString(this->ctx,  argv[0]);
    std::string content = JS_ToCString(this->ctx, argv[1]);

    this->evt->emit(topic, content);
    return JS_UNINITIALIZED;
}

void vm::eval(std::string script){
    JSValue ret = JS_Eval(ctx, script.data(), strlen(script.data()), "<eval>", JS_EVAL_TYPE_GLOBAL);
    this->try_catch(ret);
    JS_FreeValue(ctx, ret);
}

void vm::on(std::string topic, callback cb) {
    this->evt->on(topic, cb);
}

void vm::emit(std::string topic, std::string context) {

    JSValue argv[] = {
        JS_NewString(this->ctx, topic.data()), 
        JS_NewString(this->ctx, context.data()),
    };
    int argc = sizeof(argv)/sizeof(JSValue);
    JSValue ret =  JS_Call(this->ctx, this->js_handler, this->global, 2, argv);
    try_catch(ret);

    for (int i=0;i<argc;i++){
        JS_FreeValue(this->ctx, argv[i]);
    }
    JS_FreeValue(this->ctx, ret);
}
