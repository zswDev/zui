
// binder_on("test", function(v){
//     console.log("on2", v)
// })
// binder_emit("test","my is master")

binder_on("qjs->cxx->wv", function(v){
    console.log("qjs->cxx->wv", v)
})

binder_on("run",function(v){
    binder_emit("wv->cxx->qjs","my is wv")
})

