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

  VALUE str = rb_str_new2("Some string");
  VALUE reg = rb_reg_new("string", 6, 0);
  rb_funcall(str, rb_intern("gsub!"), 2, reg, rb_str_new2("thing"));
  rb_p(str);

  ruby_finalize();
  return 0;
}
