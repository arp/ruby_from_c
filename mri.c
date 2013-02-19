#define HAVE_STRUCT_TIMESPEC 1

#include "fcgi_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <fcgiapp.h>
#include <ruby.h>

const char* ruby_callback_path = "./callback.rb";

#define GET_FCGI_PARAM(fcgi_name, envp) rb_str_new2(FCGX_GetParam(fcgi_name, envp));

int main() {
  FCGX_Stream *in, *out, *err;
  FCGX_ParamArray envp;
  int count = 0;

  ruby_init();
  ruby_init_loadpath();

  FILE* ruby_callback_file = fopen(ruby_callback_path, "r");
  char* ruby_callback_code;
  long  numbytes;

  fseek(ruby_callback_file, 0L, SEEK_END);
  numbytes = ftell(ruby_callback_file);
  fseek(ruby_callback_file, 0L, SEEK_SET);
  ruby_callback_code = (char*)calloc(numbytes, sizeof(char));

  fread(ruby_callback_code, sizeof(char), numbytes, ruby_callback_file);
  fclose(ruby_callback_file);

  ruby_script(ruby_callback_path);
  VALUE callback_module = rb_define_module("Callback");
  VALUE callback_class = rb_define_class_under(callback_module, "Handle", rb_cObject);

  int r_err;
  char* errmsg;

  VALUE eval_result = rb_eval_string_protect(ruby_callback_code, &r_err);
  free(ruby_callback_code);

  if (r_err) {
    printf("GOVNII");
    //VALUE exception = rb_get_gv("$!");
    //errmsg = StringValuePtr(rb_obj_as_string(exception));
  } else {
    errmsg = "Ruby initialized successfully\n";
  }
  printf("%s\n", errmsg);

  VALUE handler_object = rb_funcall(callback_class, rb_intern("new"), 0);
  VALUE request = rb_hash_new();
  ID show_method = rb_intern("show");

  while (FCGX_Accept(&in, &out, &err, &envp) >= 0) {
    rb_hash_aset(request, ID2SYM(rb_intern("content_length")), rb_str_new2(FCGX_GetParam("CONTENT_LENGTH", envp)));
    rb_hash_aset(request, ID2SYM(rb_intern("method")), rb_str_new2(FCGX_GetParam("REQUEST_METHOD", envp)));
    rb_hash_aset(request, ID2SYM(rb_intern("uri")), rb_str_new2(FCGX_GetParam("REQUEST_URI", envp)));
    rb_hash_aset(request, ID2SYM(rb_intern("http_user_agent")), rb_str_new2(FCGX_GetParam("HTTP_USER_AGENT", envp)));

    VALUE response_text = rb_funcall(handler_object, show_method, 1, request);

    FCGX_FPrintF(out, StringValueCStr(response_text));
    //FCGX_FPrintF(out, "Content-Type: text/html\r\n\r\nzoo");

  } /* while */

  ruby_finalize();

  return 0;
}
