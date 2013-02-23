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

  VALUE test_hash = rb_hash_new();
  lo(test_hash);

  int i;
  for (i = 0; i < 1000000; i++) {
    rb_hash_aset(test_hash, INT2FIX(i), INT2FIX(i+i));
  }

  uo(test_hash);
  rb_gc();

  ruby_finalize();
  return 0;
}
