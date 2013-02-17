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
  rb_define_module("Callback");
  int state;
  VALUE eval_result = rb_eval_string_protect(ruby_callback_code, &state);
  free(ruby_callback_code);

  VALUE handler_object = rb_eval_string_protect("Callback::Handler.new", &state);

  printf("Ruby initialized\n");

  while (FCGX_Accept(&in, &out, &err, &envp) >= 0) {
    char *contentLength = FCGX_GetParam("CONTENT_LENGTH", envp);

    VALUE response_text = rb_funcall(handler_object, rb_intern("show"), 0);

    FCGX_FPrintF(out, StringValueCStr(response_text));

  } /* while */

  ruby_finalize();

  return 0;
}
