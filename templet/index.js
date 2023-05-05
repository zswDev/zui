
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
    __native_handler(topic,content) 
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

%s