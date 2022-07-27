// clang --target=wasm32 -nostdlib -matomics -mbulk-memory "-Wl,--shared-memory" -o add.wasm add.c

// includes snippets from https://surma.dev/things/c-to-webassembly/

extern unsigned char __heap_base;

unsigned long bump_pointer = (unsigned long)&__heap_base;

__attribute__((export_name("malloc"))) void* malloc(unsigned long n) {
  unsigned int r = bump_pointer;
  bump_pointer += n;
  return (void *)r;
}

__attribute__((export_name("free"))) void free(void* p) {
  // lol
}

__attribute__((import_module("imports"), import_name("imported_func"))) void loggy(int value);
__attribute__((import_module("imports"), import_name("create_thread"))) void create_thread(const char* func);

// function pointers supposedly need a wasm-table
// but from what I figured this func-pointer gets optimized
// away and clang creates the table itself anyway
void (*funcy)(void);

void fp(void (*func)(void)) {
  funcy = func;
}

void ploing(void) {
  malloc(3);
  loggy(18);
}

__attribute__((export_name("add"))) int add(int a, int b) {
  funcy();
  return a*a + b;
}

__attribute__((export_name("sum"))) int sum(int a[], int len) {
  fp(ploing);
  int sum = 0;
  for(int i = 0; i < len; i++) {
    sum += a[i];
  }
  loggy(sum);
  return sum;
}

__attribute__((export_name("thread_start"))) void thread_start(void) {
  for (;;) {
    loggy(3);
  }
}

// This is what clang wants when we do not use --no-entry
// but it doesn't seem to matter otherwise
__attribute__((export_name("_start"))) void _start(void) {
  create_thread("thread_start");
}
