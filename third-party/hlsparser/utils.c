/*
 * Copyright 2015 Joel Freeman and other contributors
 * Released under the MIT license http://opensource.org/licenses/MIT
 * see LICENSE included with package
 */

#include <string.h>

#include "parse.h"

char *str_utils_dup(const char *str) {
  char *ret = NULL;
  if (str) {
    size_t len = strlen(str);
    ret = hls_malloc(len + 1);
    strncpy(ret, str, len + 1);
  }
  return ret;
}

char *str_utils_ndup(const char *str, size_t size) {
  char *ret = NULL;
  if (str) {
    ret = hls_malloc(size + 1);
    memcpy(ret, str, size);
    ret[size] = '\0';
  }
  return ret;
}

char *str_utils_append(char *str, const char *append) {
  return str_utils_nappend(str, append, append ? strlen(append) : 0);
}

char *str_utils_nappend(char *str, const char *append, size_t append_size) {
  char *out = str;

  if (str) {
    size_t len = strlen(str);
    size_t full_len = len + append_size;
    out = hls_malloc(full_len + 1);
    if (out) {
      if (len) {
        memcpy(out, str, len);
      }
      if (append_size) {
        memcpy(&out[len], append, append_size);
      }
      out[full_len] = '\0';
      hls_free(str);
    }
  }

  return out;
}

char *str_utils_join(const char *str, const char *join) {
  if (!join) {
    return NULL;
  }

  return str_utils_njoin(str, join, strlen(join));
}

char *str_utils_njoin(const char *str, const char *join, size_t size) {
  char *out = NULL;

  if (str) {
    size_t len = strlen(str);
    size_t full_len = len + size;
    out = hls_malloc(full_len + 1);
    if (out) {
      if (len) {
        memcpy(out, str, len);
      }
      if (size) {
        memcpy(&out[len], join, size);
      }
      out[full_len] = '\0';
    }
  }

  return out;
}
