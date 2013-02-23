#ifdef __APPLE__
  #define HAVE_STRUCT_TIMESPEC 1
#endif

#include <ruby.h>

static VALUE ruby_objects;

void lo(VALUE ruby_object) {
  rb_hash_aset(ruby_objects, rb_obj_id(ruby_object), ruby_object);
}

void uo(VALUE ruby_object) {
  rb_hash_delete(ruby_objects, rb_obj_id(ruby_object));
}

int main() {
  ruby_init();
  ruby_init_loadpath();
  ruby_script("c.rb");

  ruby_objects = rb_hash_new();
  rb_global_variable(&ruby_objects);

  VALUE ruby_string = rb_str_new2("Hi from Ruby!");

  lo(ruby_string);
  rb_p(ruby_string);
  rb_gc();

  rb_p(ruby_objects);
  rb_p(ruby_string);
  uo(ruby_string);
  rb_gc();

  ruby_finalize();
  return 0;
}
