/*
 * Copyright 2016 Joel Freeman and other contributors
 * Released under the MIT license http://opensource.org/licenses/MIT
 * see LICENSE included with package
 */

#include <string.h>

#include "parse.h"

/**
 * Combines two URI's as specified in RFC3986.
 *
 *\see http://tools.ietf.org/html/rfc3986
 *
 *\param dest The output string where the combined path is set.
 *\param base The Base URI.
 *\param path The path to combine onto base.
 * Within a representation with a well defined base URI of
 *
 *     http://a/b/c/d;p?q
 *
 * a relative reference is transformed to its target URI as follows.
 *
 *    "g:h"           =  "g:h"
 *    "g"             =  "http://a/b/c/g"
 *    "./g"           =  "http://a/b/c/g"
 *    "g/"            =  "http://a/b/c/g/"
 *    "/g"            =  "http://a/g"
 *    "//g"           =  "http://g"
 *    "?y"            =  "http://a/b/c/d;p?y"
 *    "g?y"           =  "http://a/b/c/g?y"
 *    "#s"            =  "http://a/b/c/d;p?q#s"
 *    "g#s"           =  "http://a/b/c/g#s"
 *    "g?y#s"         =  "http://a/b/c/g?y#s"
 *    ";x"            =  "http://a/b/c/;x"
 *    "g;x"           =  "http://a/b/c/g;x"
 *    "g;x?y#s"       =  "http://a/b/c/g;x?y#s"
 *    ""              =  "http://a/b/c/d;p?q"
 *    "."             =  "http://a/b/c/"
 *    "./"            =  "http://a/b/c/"
 *    ".."            =  "http://a/b/"
 *    "../"           =  "http://a/b/"
 *    "../g"          =  "http://a/b/g"
 *    "../.."         =  "http://a/"
 *    "../../"        =  "http://a/"
 *    "../../g"       =  "http://a/g"
 */

char *path_combine(char **dest, const char *base, const char *path) {
  if (!base) {
    if (dest) {
      *dest = (char *)path;
    }
    return dest ? *dest : NULL;
  }

  char *out_dest = dest ? *dest : NULL;

  if (base && base[0] != '\0') {
    if (path && *path != '\0') {
      // if path has a protocol we can return it as is
      const char *protocol = NULL;
      const char *last_sep = NULL;
      const char *domain_end = NULL;
      const char *query = NULL;
      const char *p_base = base;
      const char *path_protocol = strchr(path, ':');

      // find some common characters
      while (*p_base != '\0') {
        switch (*p_base) {
        case ':': {
          protocol = protocol ? protocol : p_base;
        } break;
        case '?': {
          query = p_base;
        } break;
        case '/': {
          last_sep = p_base;
          if (!domain_end && protocol && p_base - protocol > 2) {
            domain_end = last_sep;
          }
        } break;
        }
        // move to next char
        ++p_base;
      }

      if (path_protocol) {
        // if a protocol exists, return the path
        out_dest = str_utils_dup(path);
      } else if (path[0] == '.') {
        // parse all the combinations where the path begins with a dot
        // look for ../ start by counting the number of ../ at the start of path
        const char *tmp_path = path;
        int go_back = 0;
        while (tmp_path[0] == '.' && tmp_path[1] == '.' &&
               (tmp_path[2] == '/' || tmp_path[2] == '\0')) {
          go_back++;
          tmp_path += (tmp_path[2] == '\0' ? 2 : 3);
        }
        // remove the last directories in base
        if (go_back > 0) {
          const char *p_base = last_sep;
          while (go_back > 0 && p_base != base) {
            if (p_base[0] == '/') {
              --go_back;
            }
            --p_base;
          }
          // combine the 2 paths
          out_dest = str_utils_ndup(base, p_base - base);
          if (tmp_path[0] != '\0' &&
              !(tmp_path[0] == '/' && tmp_path[1] == '\0')) {
            out_dest = str_utils_append(out_dest, tmp_path);
          }
        } else if (path[1] == '/') {
          // manage ./
          out_dest = str_utils_ndup(base, last_sep - base);
          out_dest = str_utils_append(out_dest, &path[1]);
        } else if (path[1] == '\0') {
          // manage .
          out_dest = str_utils_ndup(base, last_sep - base + 1);
        }
      } else if (path[0] == '/') {
        if (path[1] == '/') {
          // manage protocol relative path //
          out_dest = str_utils_ndup(base, protocol - base + 1);
          out_dest = str_utils_append(out_dest, path);
        } else {
          // manage domain relative path /
          out_dest = str_utils_ndup(base, domain_end - base);
          out_dest = str_utils_append(out_dest, path);
        }
      } else if (path[0] == '?') {
        // manage ?
        if (query) {
          out_dest = str_utils_ndup(base, query - base);
        } else {
          out_dest = str_utils_dup(base);
        }
        out_dest = str_utils_append(out_dest, path);
      } else if (path[0] == '#') {
        // add the complete base and path together
        out_dest = str_utils_dup(base);
        out_dest = str_utils_append(out_dest, path);
      } else {
        // join the path to the last directory of the base
        out_dest = str_utils_ndup(base, last_sep - base + 1);
        out_dest = str_utils_append(out_dest, path);
      }
    } else {
      // path doesn't exist, return the base
      out_dest = str_utils_dup(base);
    }
  } else if (path) {
    // the base doesn't exist, but the path does, return the path
    out_dest = str_utils_dup(path);
  }

  if (dest) {
    *dest = out_dest;
  }

  return out_dest;
}
