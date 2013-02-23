#ifndef TARGET_OS_MAC
  #define HAVE_STRUCT_TIMESPEC 1
#endif

#include <ruby.h>

int main() {
  ruby_init();
  ruby_init_loadpath();
  ruby_script("c.rb");

  VALUE ruby_string = rb_str_new2("Hi from Ruby!");
  rb_p(ruby_string);

  ruby_finalize();
  return 0;
}
