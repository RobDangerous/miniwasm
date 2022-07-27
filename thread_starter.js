function create_thread(func) {

}

onmessage = function(e) {
  const mod = e.data.mod;
  const memory = e.data.memory;
  const func = e.data.func;

  const importObject = {
    env: { memory },
    imports: {
      imported_func: arg => console.log(arg),
      create_thread
    }
  };

  WebAssembly.instantiate(mod, importObject).then((instance) => {
    //instance.exports[func]();
    instance.exports['thread_start']();
  });
};
