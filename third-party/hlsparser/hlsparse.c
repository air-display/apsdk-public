#include <memory.h>

#include "parse.h"

hlsparse_malloc_callback hls_malloc = (hlsparse_malloc_callback)malloc;
hlsparse_free_callback hls_free = (hlsparse_free_callback)free;

HLSCode hlsparse_global_init(void) {
  hls_malloc = (hlsparse_malloc_callback)malloc;
  hls_free = (hlsparse_free_callback)free;

  return HLS_OK;
}

HLSCode hlsparse_global_init_mem(hlsparse_malloc_callback m,
                                 hlsparse_free_callback f) {
  if (!m || !f) {
    return HLS_ERROR;
  }

  hls_malloc = m;
  hls_free = f;
  return HLS_OK;
}

HLSCode hlsparse_master_init(master_t *dest) {
  if (!dest) {
    return HLS_ERROR;
  }

  memset(dest, 0, sizeof(master_t));
  return HLS_OK;
}

HLSCode hlsparse_media_playlist_init(media_playlist_t *dest) {
  if (!dest) {
    return HLS_ERROR;
  }

  memset(dest, 0, sizeof(media_playlist_t));
  return HLS_OK;
}

HLSCode hlsparse_master_term(master_t *dest) {
  if (!dest) {
    return HLS_ERROR;
  }

  char **params[] = {&dest->uri};

  hlsparse_param_term(params, 1);
  hlsparse_string_list_term(&dest->custom_tags);
  hlsparse_session_data_list_term(&dest->session_data);
  hlsparse_media_list_term(&dest->media);
  hlsparse_stream_inf_list_term(&dest->stream_infs);
  hlsparse_iframe_stream_inf_list_term(&dest->iframe_stream_infs);

  return HLS_OK;
}

HLSCode hlsparse_media_playlist_term(media_playlist_t *dest) {
  if (!dest) {
    return HLS_ERROR;
  }
  char **params[] = {&dest->uri};

  hlsparse_param_term(params, 1);
  hlsparse_string_list_term(&dest->custom_tags);
  hlsparse_segment_list_term(&dest->segments);
  hlsparse_map_list_term(&dest->maps);
  hlsparse_daterange_list_term(&dest->dateranges);

  return HLS_OK;
}

int hlsparse_master(const char *src, size_t size, master_t *dest) {
  int res = 0;

  // make sure we have some data
  if (src && *src != '\0' && src < &src[size]) {
    // go through each line parsing the tags
    const char *end = &src[size];
    const char *pt = src;
    // loop until we find a null terminator or hit the end of the data
    while (*pt != '\0' && pt < end) {
      if (*pt == '#') {
        ++pt;
        pt += parse_master_tag(pt, size - (pt - src), dest);
      } else {
        ++pt;
      }
    }

    res = pt - src;
  }

  return res;
}

int hlsparse_media_playlist(const char *src, size_t size,
                            media_playlist_t *dest) {
  int res = 0;

  if (dest) {
    // reset the duration
    dest->duration = 0;
    // reset the segment byte range
    dest->next_segment_byterange.n = dest->next_segment_byterange.o = 0;
  }

  // make sure we have some data
  if (src && (src[0] != '\0') && size > 0) {
    // go through each line parsing the tags
    const char *pt = &src[0];
    while (*pt != '\0' && pt < &src[size]) {
      if (*pt == '#') {
        ++pt;
        pt += parse_media_playlist_tag(pt, size - (pt - src), dest);
      } else if (*pt == '\n' && pt[1] != '#') {
        ++pt;
        if (dest->last_segment) {
          pt += parse_segment_uri(pt, size - (pt - src), dest);
        }
      } else {
        ++pt;
      }
    }

    res = pt - src;
  }

  return res;
}
