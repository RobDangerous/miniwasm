// clang --target=wasm32 -nostdlib -matomics -mbulk-memory "-Wl,--import-memory,--shared-memory" -o add.wasm add.c

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
__attribute__((import_module("imports"), import_name("create_thread"))) void create_thread(void* func);

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

// MT19937

static int MT[624];
static int mermeme_index = 0;

static void generateNumbers() {
  for (int i = 0; i < 624; ++i) {
    int y = (MT[i] & 1) + (MT[(i + 1) % 624]) & 0x7fffffff;
    MT[i] = MT[(i + 397) % 624] ^ (y >> 1);
    if ((y % 2) != 0) MT[i] = MT[i] ^ 0x9908b0df;
  }
}

void kinc_random_init(int seed) {
  MT[0] = seed;
  for (int i = 1; i < 624; ++i) MT[i] = 0x6c078965 * (MT[i - 1] ^ (MT[i - 1] >> 30)) + i;
}

int kinc_random_get() {
  if (mermeme_index == 0) generateNumbers();

  int y = MT[mermeme_index];
  y = y ^ (y >> 11);
  y = y ^ ((y << 7) & (0x9d2c5680));
  y = y ^ ((y << 15) & (0xefc60000));
  y = y ^ (y >> 18);

  mermeme_index = (mermeme_index + 1) % 624;
  return y;
}

int kinc_random_get_max(int max) {
  return kinc_random_get() % (max + 1);
}

__attribute__((export_name("audio_func"))) void audio_func(float *data, int length) {
  loggy(888);
  for (int i = 0; i < length; ++i) {
    data[i] = (kinc_random_get_max(255) / 255.0f) * 2.0f - 1.0f;
  }
}

int data[16];

__attribute__((export_name("thread_start"))) void thread_start(void) {
  loggy(666);
  for (int i = 0; i < 16; ++i) {
    loggy(data[i]);
  }
}

// This is what clang wants when we do not use --no-entry
// but it doesn't seem to matter otherwise
__attribute__((export_name("_start"))) void _start(void) {
  loggy(777);
  for (int i = 0; i < 16; ++i) {
    data[i] = i;
  }
  kinc_random_init(0);
  create_thread("thread_start");
}
