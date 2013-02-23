#ifdef __APPLE__
  #define HAVE_STRUCT_TIMESPEC 1
#endif

#include <ruby.h>

static char* ruby_code_path = "./code.rb";

static int exit_code = 0;

int main() {
  ruby_init();
  ruby_init_loadpath();
  ruby_script("c.rb");

  int state = 0;
  rb_protect(RUBY_METHOD_FUNC(rb_require), (VALUE) ruby_code_path, &state);

  if (state) { /* We couldn't execute rb_require */
    rb_p(rb_gv_get("$!"));
    exit_code = 1;
  }

  ruby_finalize();
  return exit_code;
}
