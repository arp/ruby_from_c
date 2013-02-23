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
  /* FastCGI init */
  FCGX_Stream *in, *out, *err;
  FCGX_ParamArray envp;
  int count = 0;

  /* Ruby init */
  ruby_init();
  ruby_init_loadpath();
  ruby_script(ruby_callback_path);

  /* Defining base classes */
  VALUE callback_module = rb_define_module("Callback");
  VALUE callback_class = rb_define_class_under(callback_module, "Handle", rb_cObject);

  /* Requiring the callback file */
  rb_require(ruby_callback_path);

  int r_err;
  char* errmsg;

  if (r_err) {
    printf("Oops, couldn't evaluate %s.\n", ruby_callback_path);
    //VALUE exception = rb_get_gv("$!");
    //errmsg = StringValuePtr(rb_obj_as_string(exception));
    rb_raise(rb_eStandardError, "Oops, couldn't evaluate.");
  } else {
    errmsg = "Ruby initialized successfully\n";
  }
  printf("%s\n", errmsg);

  VALUE handler_object = rb_funcall(callback_class, rb_intern("new"), 0);
  VALUE request = rb_hash_new();
  ID show_method = rb_intern("show");

  while (FCGX_Accept(&in, &out, &err, &envp) >= 0) {

    /* OPTIMIZE: Replace with methods exposed directly from C to prevent pre-setting the values */
    rb_hash_aset(request, ID2SYM(rb_intern("content_length")), rb_str_new2(FCGX_GetParam("CONTENT_LENGTH", envp)));
    rb_hash_aset(request, ID2SYM(rb_intern("method")), rb_str_new2(FCGX_GetParam("REQUEST_METHOD", envp)));
    rb_hash_aset(request, ID2SYM(rb_intern("uri")), rb_str_new2(FCGX_GetParam("REQUEST_URI", envp)));
    rb_hash_aset(request, ID2SYM(rb_intern("http_user_agent")), rb_str_new2(FCGX_GetParam("HTTP_USER_AGENT", envp)));

    /* Calling Callback::Handle#show */
    VALUE response_text = rb_funcall(handler_object, show_method, 1, request);

    FCGX_FPrintF(out, StringValueCStr(response_text));

  } /* while */

  ruby_finalize();

  return 0;
}
