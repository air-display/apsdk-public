/*
 * Copyright 2015 Joel Freeman and other contributors
 * Released under the MIT license http://opensource.org/licenses/MIT
 * see LICENSE included with package
 */

#include <memory.h>
#include <string.h>

#include "parse.h"

/**
 * Helper function for initializing a byte_range_t object
 *
 * @param byte_range The byte_range to initialize
 */
void hlsparse_byte_range_init(byte_range_t *byte_range) {
  if (byte_range) {
    byte_range->n = 0;
    byte_range->o = 0;
  }
}

/**
 * Helper function for initializing a ext_inf_t
 *
 * @param ext_inf The ext_inf to initialize
 */
void hlsparse_ext_inf_init(ext_inf_t *ext_inf) {
  if (ext_inf) {
    ext_inf->duration = 0.f;
    ext_inf->title = NULL;
  }
}

/**
 * Helper function for initializing a resolution_t
 *
 * @param resoution The resolution object to initialize
 */
void hlsparse_resolution_init(resolution_t *resolution) {
  if (resolution) {
    resolution->width = 0;
    resolution->height = 0;
  }
}

/**
 * Helper function for initialzing an iframe_stream_inf
 *
 * @param stream_inf The iframe_stream_inf to initialize
 */
void hlsparse_iframe_stream_inf_init(iframe_stream_inf_t *stream_inf) {
  if (stream_inf) {
    memset(stream_inf, 0, sizeof(iframe_stream_inf_t));
  }
}

/**
 * Helper function for initialzing a stream_inf
 *
 * @param stream_inf The stream_inf to initialize
 */
void hlsparse_stream_inf_init(stream_inf_t *stream_inf) {
  if (stream_inf) {
    memset(stream_inf, 0, sizeof(stream_inf_t));
  }
}

/**
 * Helper function for initializing an hls_key_t
 *
 * @param dest The dest to initialize
 */
void hlsparse_key_init(hls_key_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(hls_key_t));
  }
}

/**
 * Helper function for initializing a map_t object
 *
 * @param dest The dest object to initialize
 */
void hlsparse_map_init(map_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(map_t));
  }
}

/**
 * Helper function for initializing a daterange_t object
 *
 * @param dest The dest object to initialize
 */
void hlsparse_daterange_init(daterange_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(daterange_t));
  }
}

/**
 * Helper function for initializing a map_t object
 *
 * @param dest The dest object to initialize
 */
void hlsparse_media_init(media_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(media_t));
  }
}

/**
 * Helper function for initializing a segment_t object
 *
 * @param dest The dest object to initialize
 */
void hlsparse_segment_init(segment_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(segment_t));
  }
}

/**
 * Helper function for initializing a session_data_t object
 *
 * @param dest The dest object to initialize
 */
void hlsparse_session_data_init(session_data_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(session_data_t));
  }
}

/**
 * Helper function for initializing a start_t object
 *
 * @param dest The dest object to initialize
 */
void hlsparse_start_init(start_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(start_t));
  }
}

/**
 * Helper function for initializing a segment_list_t object
 *
 * @param dest The dest object to initialize
 */
void hlsparse_segment_list_init(segment_list_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(segment_list_t));
  }
}

/**
 * Helper function for initializing a session_data_list_t object
 *
 * @param dest The dest object to initialize
 */
void hlsparse_session_data_list_init(session_data_list_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(session_data_list_t));
  }
}

/**
 * Helper function for initializing a key_list_t object
 *
 * @param dest The dest object to initialize
 */
void hlsparse_key_list_init(key_list_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(key_list_t));
  }
}

/**
 * Helper function for initializing a stream_inf_list_t object
 *
 * @param dest The dest object to initialize
 */
void hlsparse_stream_inf_list_init(stream_inf_list_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(stream_inf_list_t));
  }
}

/**
 * Helper function for initializing a iframe_stream_inf_list_t object
 *
 * @param dest The dest object to initialize
 */
void hlsparse_iframe_stream_inf_list_init(iframe_stream_inf_list_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(iframe_stream_inf_list_t));
  }
}

/**
 * Helper function for initializing a media_list_t object
 *
 * @param dest The dest object to initialize
 */
void hlsparse_media_list_init(media_list_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(media_list_t));
  }
}

/**
 * Helper function for initializing a map_list_t object
 *
 * @param dest The dest object to initialize
 */
void hlsparse_map_list_init(map_list_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(map_list_t));
  }
}

/**
 * Helper function for initializing a datrange_list_t object
 *
 * @param dest The dest object to initialize
 */
void hlsparse_daterange_list_init(daterange_list_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(daterange_list_t));
  }
}

/**
 * Helper function for initializing a string_list_t object
 *
 * @param dest The dest object to initialize
 */
void hlsparse_string_list_init(string_list_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(string_list_t));
  }
}

/**
 * Helper function for initializing a param_list_t object
 *
 * @param dest The dest object to initialize
 */
void hlsparse_param_list_init(param_list_t *dest) {
  if (dest) {
    memset(dest, 0, sizeof(param_list_t));
  }
}

/**
 * Helper function for cleanup up tags which have been allocated on the heap
 *
 * @param params The list of parameters than need to be freed if they exist
 * @param size The number of items in params
 */
void hlsparse_param_term(char ***params, int size) {
  for (int i = 0; i < size; ++i) {
    if (params[i] && *params[i]) {
      hls_free(*params[i]);
      *params[i] = NULL;
    }
  }
}

/**
 * Helper function for terminating an ext_inf_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param ext_inf The ext_inf to cleanup
 */
void hlsparse_ext_inf_term(ext_inf_t *ext_inf) {
  if (ext_inf) {
    char **params[] = {&ext_inf->title};
    hlsparse_param_term(params, 1);
  }
}

/**
 * Helper function for terminating an iframe_stream_inf_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param stream_inf The stream_inf to cleanup
 */
void hlsparse_iframe_stream_inf_term(iframe_stream_inf_t *stream_inf) {
  if (stream_inf) {
    char **params[] = {&stream_inf->codecs, &stream_inf->video,
                       &stream_inf->uri};

    hlsparse_param_term(params, 3);
  }
}

/**
 * Helper function for terminating a stream_inf_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param stream_inf The stream_inf to cleanup
 */
void hlsparse_stream_inf_term(stream_inf_t *stream_inf) {
  if (stream_inf) {
    char **params[] = {&stream_inf->codecs,
                       &stream_inf->video,
                       &stream_inf->audio,
                       &stream_inf->subtitles,
                       &stream_inf->closed_captions,
                       &stream_inf->video,
                       &stream_inf->uri};

    hlsparse_param_term(params, 7);
  }
}

/**
 * Helper function for terminating an hls_key_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param dest The object to cleanup
 */
void hlsparse_key_term(hls_key_t *dest) {
  if (dest) {
    char **params[] = {&dest->uri, &dest->iv, &dest->key_format,
                       &dest->key_format_versions};

    hlsparse_param_term(params, 4);
  }
}

/**
 * Helper function for terminating a map_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param dest The object to cleanup
 */
void hlsparse_map_term(map_t *dest) {
  if (dest) {
    char **params[] = {&dest->uri};

    hlsparse_param_term(params, 1);
  }
}

/**
 * Helper function for terminating a daterange_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param dest The object to cleanup
 */
void hlsparse_daterange_term(daterange_t *dest) {
  if (dest) {
    char **params[] = {
        &dest->id,         &dest->klass,     &dest->scte35_cmd,
        &dest->scte35_out, &dest->scte35_in,
    };

    dest->scte35_cmd_size = 0;
    dest->scte35_out_size = 0;
    dest->scte35_in_size = 0;

    hlsparse_param_term(params, 5);
    hlsparse_param_list_term(&dest->client_attributes);
  }
}

/**
 * Helper function for terminating a map_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param dest The object to cleanup
 */
void hlsparse_media_term(media_t *dest) {
  if (dest) {
    char **params[] = {
        &dest->group_id,       &dest->name, &dest->language,
        &dest->assoc_language, &dest->uri,  &dest->characteristics,
        &dest->channels,
    };

    hlsparse_param_term(params, 7);
  }
}

/**
 * Helper function for terminating a segment_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param dest The object to cleanup
 */
void hlsparse_segment_term(segment_t *dest) {
  if (dest) {
    char **params[] = {&dest->title, &dest->uri};

    hlsparse_param_term(params, 2);

    hlsparse_string_list_term(&dest->custom_tags);
  }
}

/**
 * Helper function for terminating a session_data_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param dest The object to cleanup
 */
void hlsparse_session_data_term(session_data_t *dest) {
  if (dest) {
    char **params[] = {&dest->data_id, &dest->uri, &dest->language,
                       &dest->value};

    hlsparse_param_term(params, 4);
  }
}

/**
 * Helper function for terminating a segmentlist_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param dest The object to cleanup
 */
void hlsparse_segment_list_term(segment_list_t *dest) {

  if (dest) {
    if (dest->data) {
      hlsparse_segment_term(dest->data);
      hls_free(dest->data);
      dest->data = NULL;
    }

    if (dest->next) {
      hlsparse_segment_list_term(dest->next);
      hls_free(dest->next);
      dest->next = NULL;
    }
  }
}

/**
 * Helper function for terminating a session_data_list_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param dest The object to cleanup
 */
void hlsparse_session_data_list_term(session_data_list_t *dest) {
  if (dest) {
    if (dest->data) {
      hlsparse_session_data_term(dest->data);
      hls_free(dest->data);
    }

    session_data_list_t *ptr = dest->next;

    if (ptr) {
      hlsparse_session_data_list_term(ptr);
      hls_free(ptr);
    }
  }
}

/**
 * Helper function for terminating a keylist_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param dest The object to cleanup
 */
void hlsparse_key_list_term(key_list_t *dest) {
  if (dest) {
    if (dest->data) {
      hlsparse_key_term(dest->data);
      hls_free(dest->data);
    }

    key_list_t *ptr = dest->next;

    if (ptr) {
      hlsparse_key_list_term(ptr);
      hls_free(ptr);
    }
  }
}

/**
 * Helper function for terminating a media_list_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param dest The object to cleanup
 */
void hlsparse_media_list_term(media_list_t *dest) {
  if (dest) {
    if (dest->data) {
      hlsparse_media_term(dest->data);
      hls_free(dest->data);
    }

    media_list_t *ptr = dest->next;

    if (ptr) {
      hlsparse_media_list_term(ptr);
      hls_free(ptr);
    }
  }
}

/**
 * Helper function for terminating a map_list_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param dest The object to cleanup
 */
void hlsparse_map_list_term(map_list_t *dest) {
  if (dest) {
    if (dest->data) {
      hlsparse_map_term(dest->data);
      hls_free(dest->data);
    }

    map_list_t *ptr = dest->next;

    if (ptr) {
      hlsparse_map_list_term(ptr);
      hls_free(ptr);
    }
  }
}

/**
 * Helper function for terminating a daterange_list_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param dest The object to cleanup
 */
void hlsparse_daterange_list_term(daterange_list_t *dest) {
  if (dest) {
    if (dest->data) {
      hlsparse_daterange_term(dest->data);
      hls_free(dest->data);
    }

    daterange_list_t *ptr = dest->next;

    if (ptr) {
      hlsparse_daterange_list_term(ptr);
      hls_free(ptr);
    }
  }
}

/**
 * Helper function for terminating a iframe_stream_inf_list_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param dest The object to cleanup
 */
void hlsparse_iframe_stream_inf_list_term(iframe_stream_inf_list_t *dest) {

  if (dest) {
    if (dest->data) {
      hlsparse_iframe_stream_inf_term(dest->data);
      hls_free(dest->data);
    }

    iframe_stream_inf_list_t *ptr = dest->next;

    if (ptr) {
      hlsparse_iframe_stream_inf_list_term(ptr);
      hls_free(ptr);
    }
  }
}

/**
 * Helper function for terminating a stream_inf_list_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param dest The object to cleanup
 */
void hlsparse_stream_inf_list_term(stream_inf_list_t *dest) {
  if (dest) {
    if (dest->data) {
      hlsparse_stream_inf_term(dest->data);
      hls_free(dest->data);
    }

    stream_inf_list_t *ptr = dest->next;

    if (ptr) {
      hlsparse_stream_inf_list_term(ptr);
      hls_free(ptr);
    }
  }
}

/**
 * Helper function for terminating a string_list_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param dest The object to cleanup
 */
void hlsparse_string_list_term(string_list_t *dest) {
  if (dest) {
    if (dest->data) {
      hls_free(dest->data);
    }

    string_list_t *ptr = dest->next;

    if (ptr) {
      hlsparse_string_list_term(ptr);
      hls_free(ptr);
    }
  }
}

/**
 * Helper function for terminating a param_list_t.
 * This will free any properties on the object resetting the values, but won't
 * try to free the object itself.
 * To reuse the object call the corresponding init function after termination.
 *
 * @param dest The object to cleanup
 */
void hlsparse_param_list_term(param_list_t *dest) {
  if (dest) {
    if (dest->key) {
      hls_free(dest->key);
      dest->key = NULL;
    }
    if (dest->value.data && (dest->value_type == PARAM_TYPE_DATA ||
                             dest->value_type == PARAM_TYPE_STRING)) {
      hls_free(dest->value.data);
    }
    dest->value.data = NULL;
    dest->value_type = PARAM_TYPE_NONE;
    dest->value_size = 0;

    param_list_t *ptr = dest->next;
    dest->next = NULL;

    if (ptr) {
      hlsparse_param_list_term(ptr);
      hls_free(ptr);
    }
  }
}

/**
 * Parse HLS playlist string data into a supplied byte_range_t object.
 * This function can parse the #EXT-X tag version of a byte range, or the
 * range values itself, e.g. "32@128"
 *
 * @param src The source of the HLS src
 * @param size The length of the src string
 * @param dest The destination object to write the properties to
 */
int parse_byte_range(const char *src, size_t size, byte_range_t *dest) {
  if (!src || !size) {
    return 0;
  }

  int res = 0;
  const char *pt = src;

  // this is dual purpose, it can parse the #EXT-X or a byte-range value 'n@o'
  if (*pt == '#') {
    ++pt;
  }
  if (EQUAL(pt, EXTXBYTERANGE)) {
    // get past the ':'
    ++pt;
  }

  if (src && src[0] != '\0' && dest) {
    int *value = dest ? &dest->n : NULL;
    pt += parse_str_to_int(pt, value, size);
    if (*pt == '@') {
      ++pt;
      value = dest ? &dest->o : NULL;
      pt += parse_str_to_int(pt, value, size - (pt - src));
    }
    res = pt - src;
  }

  return res;
}

/**
 * Parse HLS playlist string data into a supplied iframe_stream_inf_t object.
 *
 * @param src The source of the HLS src
 * @param size The length of the src string
 * @param dest The destination object to write the properties to
 */
int parse_iframe_stream_inf(const char *src, size_t size,
                            iframe_stream_inf_t *dest) {
  int res = 0;

  // make sure we have some data
  if (src && src[0] != '\0') {
    // go through each line parsing the tags
    const char *pt = src;
    int dif;

    while (!(*pt == '\0' || *pt == '\r' || *pt == '\n')) {
      // skip comma characters in between the attributes
      if (*pt == ',' || *pt == '=' || *pt == ':') {
        ++pt;
      } else {
        dif = parse_iframe_stream_inf_tag(pt, size - (pt - src), dest);

        pt += dif > 0 ? dif : 1;
      }
    }

    // return the difference between the two data points
    res = pt - src;
  }

  return res;
}

/**
 * Helper function for parsing HLS tags and setting values on the supplied
 * object
 *
 * @param src The HLS source string
 * @param size The size of the source string
 * @param dest The destination object to write the value to
 */
int parse_iframe_stream_inf_tag(const char *src, size_t size,
                                iframe_stream_inf_t *dest) {
  const char *pt = src;

  if (EQUAL(pt, PROGRAMID)) {
    ++pt; // get past the '=' sign
    pt += parse_str_to_int(pt, &dest->program_id, size - (pt - src));
  } else if (EQUAL(pt, BANDWIDTH)) {
    ++pt; // get past the '=' sign
    pt += parse_str_to_float(pt, &dest->bandwidth, size - (pt - src));
  } else if (EQUAL(pt, AVERAGEBANDWIDTH)) {
    ++pt; // get past the '=' sign
    pt += parse_str_to_float(pt, &dest->avg_bandwidth, size - (pt - src));
  } else if (EQUAL(pt, CODECS)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->codecs, size - (pt - src));
  } else if (EQUAL(pt, RESOLUTION)) {
    ++pt; // get past the '=' sign
    pt += parse_resolution(pt, size - (pt - src), &dest->resolution);
  } else if (EQUAL(pt, VIDEO)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->video, size - (pt - src));
  } else if (EQUAL(pt, URI)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->uri, size - (pt - src));
  } else if (EQUAL(pt, FRAMERATE)) {
    ++pt; // get past the '=' sign
    pt += parse_str_to_float(pt, &dest->frame_rate, size - (pt - src));
  } else if (EQUAL(pt, HDCPLEVEL)) {
    ++pt; // get past the '=' sign
    if (EQUAL(pt, TYPE0)) {
      dest->hdcp_level = HDCP_LEVEL_TYPE0;
    } else if (EQUAL(pt, NONE)) {
      dest->hdcp_level = HDCP_LEVEL_NONE;
    }
  }
  // return the length of characters we have parsed
  return pt - src;
}

/**
 * Parse HLS playlist string data into a supplied resolution_t object.
 *
 * @param src The source of the HLS src
 * @param size The length of the src string
 * @param dest The destination object to write the properties to
 */
int parse_resolution(const char *src, size_t size, resolution_t *dest) {
  if (!src || !size) {
    return 0;
  }

  const char *pt = src;
  int width = 0, height = 0;
  pt += parse_str_to_int(pt, &width, size - (pt - src));

  if (*pt == 'x') {
    ++pt;
    pt += parse_str_to_int(pt, &height, size - (pt - src));
  }

  if (dest) {
    dest->width = width;
    dest->height = height;
  }

  return pt - src;
}

/**
 * Parse HLS playlist string data into a supplied hls_key_t object.
 *
 * @param src The source of the HLS src
 * @param size The length of the src string
 * @param dest The destination object to write the properties to
 */
int parse_key(const char *src, size_t size, hls_key_t *dest) {
  int res = 0;

  if (!src || !size || !dest) {
    return 0;
  }

  // make sure we have some data
  if (src && src[0] != '\0') {
    // go through each line parsing the tags
    const char *pt = src;
    const char *end = &src[size];

    int dif;
    while (!(*pt == '\0' || *pt == '\r' || *pt == '\n' || pt >= end)) {
      // skip comma characters in between the attributes
      if (*pt == ',' || *pt == '=' || *pt == ':') {
        ++pt;
      } else {
        dif = parse_key_tag(pt, size - (pt - src), dest);
        pt += dif > 0 ? dif : 1;
      }
    }

    // return the difference between the 2 data points
    res = pt - src;
  }

  return res;
}

/**
 * Helper function for parsing HLS tags and setting values on the supplied
 * object
 *
 * @param src The HLS source string
 * @param size The size of the source string
 * @param dest The destination object to write the value to
 */
int parse_key_tag(const char *src, size_t size, hls_key_t *dest) {
  const char *pt = src;

  if (EQUAL(pt, METHOD)) {
    // get past the '='
    ++pt;
    if (EQUAL(pt, "NONE")) {
      dest->method = KEY_METHOD_NONE;
    } else if (EQUAL(pt, "AES-128")) {
      dest->method = KEY_METHOD_AES128;
    } else if (EQUAL(pt, "SAMPLE-AES")) {
      dest->method = KEY_METHOD_SAMPLEAES;
    } else {
      dest->method = KEY_METHOD_INVALID;
    }
  } else if (EQUAL(pt, URI)) {
    ++pt;
    pt += parse_attrib_str(pt, &dest->uri, size - (pt - src));
  } else if (EQUAL(pt, KEY_IV)) {
    ++pt;
    pt += parse_attrib_data(pt, &dest->iv, size - (pt - src));
  } else if (EQUAL(pt, KEYFORMATVERSIONS)) {
    ++pt;
    pt += parse_attrib_str(pt, &dest->key_format_versions, size - (pt - src));
  } else if (EQUAL(pt, KEYFORMAT)) {
    ++pt;
    pt += parse_attrib_str(pt, &dest->key_format, size - (pt - src));
  }

  return pt - src;
}

/**
 * Parse HLS playlist string data into a supplied mapt_ object.
 *
 * @param src The source of the HLS src
 * @param size The length of the src string
 * @param dest The destination object to write the properties to
 */
int parse_map(const char *src, size_t size, map_t *dest) {
  int res = 0;

  // make sure we have some data
  if (src && src[0] != '\0' && size > 0) {
    // go through each line parsing the tags
    const char *pt = &src[0];

    int dif;
    while (!(*pt == '\0' || *pt == '\r' || *pt == '\n' || pt >= &src[size])) {
      // skip comma characters in between the attributes
      if (*pt == ',' || *pt == '=' || *pt == ':') {
        ++pt;
      } else {
        dif = parse_map_tag(pt, size - (pt - src), dest);
        pt += dif > 0 ? dif : 1;
      }
    }

    // return the difference between the 2 data points
    res = pt - src;
  }

  return res;
}

/**
 * Helper function for parsing HLS tags and setting values on the supplied
 * object
 *
 * @param src The HLS source string
 * @param size The size of the source string
 * @param dest The destination object to write the value to
 */
int parse_map_tag(const char *src, size_t size, map_t *dest) {
  if (!src || !size || !dest) {
    return 0;
  }

  const char *pt = src;

  if (EQUAL(pt, URI)) {
    ++pt;
    parse_attrib_str(pt, &dest->uri, size - (pt - src));
  } else if (EQUAL(pt, BYTERANGE)) {
    // get past the ="
    pt += 2;
    pt += parse_byte_range(pt, size - (pt - src), &dest->byte_range);
  }

  return pt - src;
}

/**
 * Parse HLS playlist string data into a supplied daterange_t object.
 *
 * @param src The source of the HLS src
 * @param size The length of the src string
 * @param dest The destination object to write the properties to
 */
int parse_daterange(const char *src, size_t size, daterange_t *dest) {
  int res = 0;

  // make sure we have some data
  if (src && src[0] != '\0' && size > 0) {
    // go through each line parsing the tags
    const char *pt = &src[0];

    int dif;
    while (!(*pt == '\0' || *pt == '\r' || *pt == '\n' || pt >= &src[size])) {
      // skip comma characters in between the attributes
      if (*pt == ',' || *pt == '=' || *pt == ':') {
        ++pt;
      } else {
        dif = parse_daterange_tag(pt, size - (pt - src), dest);
        pt += dif > 0 ? dif : 1;
      }
    }

    // return the difference between the 2 data points
    res = pt - src;
  }

  return res;
}

/**
 * Helper function for parsing HLS tags and setting values on the supplied
 * object
 *
 * @param src The HLS source string
 * @param size The size of the source string
 * @param dest The destination object to write the value to
 */
int parse_daterange_tag(const char *src, size_t size, daterange_t *dest) {
  if (!src || !size || !dest) {
    return 0;
  }

  const char *pt = src;

  if (EQUAL(pt, ID)) {
    ++pt;
    pt += parse_attrib_str(pt, &dest->id, size - (pt - src));
  } else if (EQUAL(pt, CLASS)) {
    ++pt;
    pt += parse_attrib_str(pt, &dest->klass, size - (pt - src));
  } else if (EQUAL(pt, STARTDATE)) {
    ++pt; // get past the '=' sign
    pt += parse_date(pt, &dest->start_date, size - (pt - src));
  } else if (EQUAL(pt, ENDDATE)) {
    ++pt; // get past the '=' sign
    pt += parse_date(pt, &dest->end_date, size - (pt - src));
  } else if (EQUAL(pt, DURATION)) {
    ++pt; // get past the '=' sign
    pt += parse_str_to_float(pt, &dest->duration, size - (pt - src));
  } else if (EQUAL(pt, PLANNEDDURATION)) {
    ++pt; // get past the '=' sign
    pt += parse_str_to_float(pt, &dest->planned_duration, size - (pt - src));
  } else if (EQUAL(pt, SCTE35CMD)) {
    ++pt; // get past the '=' sign
    dest->scte35_cmd_size =
        parse_attrib_data(pt, &dest->scte35_cmd, size - (pt - src));
    pt += dest->scte35_cmd_size;
    dest->scte35_cmd_size = (dest->scte35_cmd_size - 2) /
                            2; // minus '0x' / 2 characters per byte e.g. 'AA'
  } else if (EQUAL(pt, SCTE35OUT)) {
    ++pt; // get past the '=' sign
    dest->scte35_out_size =
        parse_attrib_data(pt, &dest->scte35_out, size - (pt - src));
    pt += dest->scte35_out_size;
    dest->scte35_out_size = (dest->scte35_out_size - 2) /
                            2; // minus '0x' / 2 characters per byte e.g. 'AA'
  } else if (EQUAL(pt, SCTE35IN)) {
    ++pt; // get past the '=' sign
    dest->scte35_in_size =
        parse_attrib_data(pt, &dest->scte35_in, size - (pt - src));
    pt += dest->scte35_in_size;
    dest->scte35_in_size = (dest->scte35_in_size - 2) /
                           2; // minus '0x' / 2 characters per byte e.g. 'AA'
  } else if (EQUAL(pt, ENDONNEXT)) {
    ++pt; // get past the '=' sign
    if (EQUAL(pt, YES)) {
      dest->end_on_next = HLS_TRUE;
    } else if (EQUAL(pt, NO)) {
      dest->end_on_next = HLS_FALSE;
    }
  } else if (pt[0] == 'X' && pt[1] == '-') {
    // custom client parameter
    // find the '=' sign so we know where the key ends
    const char *tmp = pt + 1;
    while (*tmp != '=' && *tmp != '\n' && *tmp != '\0') {
      ++tmp;
    }
    // get the key string
    if (tmp - pt > 1) {
      param_list_t *param = NULL;
      // use the base client_attributes if there are no values stored.
      if (dest->client_attributes.value_type == PARAM_TYPE_NONE) {
        param = &dest->client_attributes;
      } else {
        // allocate a new parameter.
        param = (param_list_t *)hls_malloc(sizeof(param_list_t));
        hlsparse_param_list_init(param);
        // find the last value in the link list, we'll attach the new one
        // to this one's "next" property.
        param_list_t *prev = &dest->client_attributes;
        while (prev->next) {
          prev = prev->next;
        }
        // link the param onto the list.
        prev->next = param;
      }
      // duplicate the string to attach the new key
      param->key = str_utils_ndup(pt, tmp - pt);
      // find the new value string
      pt = tmp + 1; // get past the '='
      if (*pt == '"') {
        param->value_type = PARAM_TYPE_STRING;
        param->value_size =
            parse_attrib_str(pt, &param->value.data, size - (pt - src));
        pt += param->value_size;
        param->value_size--; // parse always parses an extra end byte
      } else if (*pt == '0' && (pt[1] == 'x' || pt[1] == 'X')) {
        param->value_type = PARAM_TYPE_DATA;
        param->value_size =
            parse_attrib_data(pt, &param->value.data, size - (pt - src));
        pt += param->value_size;
        param->value_size =
            (param->value_size - 2) / 2; // minus '0x' / 2 bytes per character
      } else {
        param->value_type = PARAM_TYPE_FLOAT;
        pt += parse_str_to_float(pt, &param->value.number, size - (pt - src));
        param->value_size = 0;
      }
    }
  }

  return pt - src;
}

/**
 * Helper function for parsing HLS tags and setting values on the supplied
 * object
 *
 * @param src The HLS source string
 * @param size The size of the source string
 * @param dest The destination object to write the value to
 */
int parse_media_tag(const char *src, size_t size, media_t *dest) {
  if (!src || !size || !dest) {
    return 0;
  }

  const char *pt = src;

  if (EQUAL(pt, TYPE)) {
    ++pt; // get past the '=' sign
    if (EQUAL(pt, VIDEO)) {
      dest->type = MEDIA_TYPE_VIDEO;
    } else if (EQUAL(pt, AUDIO)) {
      dest->type = MEDIA_TYPE_AUDIO;
    } else if (EQUAL(pt, SUBTITLES)) {
      dest->type = MEDIA_TYPE_SUBTITLES;
    } else if (EQUAL(pt, CLOSEDCAPTIONS)) {
      dest->type = MEDIA_TYPE_CLOSEDCAPTIONS;
    } else {
      dest->type = MEDIA_TYPE_INVALID;
    }
  } else if (EQUAL(pt, GROUPID)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->group_id, size - (pt - src));
  } else if (EQUAL(pt, NAME)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->name, size - (pt - src));
  } else if (EQUAL(pt, AUTOSELECT)) {
    ++pt; // get past the '=' sign
    if (EQUAL(pt, YES)) {
      dest->auto_select = HLS_TRUE;
    } else if (EQUAL(pt, NO)) {
      dest->auto_select = HLS_FALSE;
    }
  } else if (EQUAL(pt, FORCED)) {
    ++pt; // get past the '=' sign
    if (EQUAL(pt, YES)) {
      dest->forced = HLS_TRUE;
    } else if (EQUAL(pt, NO)) {
      dest->forced = HLS_FALSE;
    }
  } else if (EQUAL(pt, DEFAULT)) {
    ++pt; // get past the '=' sign
    if (EQUAL(pt, YES)) {
      dest->is_default = HLS_TRUE;
    } else if (EQUAL(pt, NO)) {
      dest->is_default = HLS_FALSE;
    }
  } else if (EQUAL(pt, LANGUAGE)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->language, size - (pt - src));
  } else if (EQUAL(pt, ASSOCLANGUAGE)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->assoc_language, size - (pt - src));
  } else if (EQUAL(pt, URI)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->uri, size - (pt - src));
  } else if (EQUAL(pt, INSTREAMID)) {
    pt += 2; // get past the '=' and the '"'
    if (EQUAL(pt, CC1)) {
      dest->instream_id = MEDIA_INSTREAMID_CC1;
    } else if (EQUAL(pt, CC2)) {
      dest->instream_id = MEDIA_INSTREAMID_CC2;
    } else if (EQUAL(pt, CC3)) {
      dest->instream_id = MEDIA_INSTREAMID_CC3;
    } else if (EQUAL(pt, CC4)) {
      dest->instream_id = MEDIA_INSTREAMID_CC4;
    } else if (EQUAL(pt, SERVICE)) {
      dest->instream_id = MEDIA_INSTREAMID_SERVICE;
      pt += parse_str_to_int(pt, &dest->service_n, size - (pt - src));
    } else {
      dest->instream_id = MEDIA_INSTREAMID_INVALID;
    }
  } else if (EQUAL(pt, CHARACTERISTICS)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->characteristics, size - (pt - src));
  } else if (EQUAL(pt, CHANNELS)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->channels, size - (pt - src));
  } else {
    // custom tags?
  }

  return pt - src;
}

/**
 * Parse HLS playlist string data into a supplied media_t object.
 *
 * @param src The source of the HLS src
 * @param size The length of the src string
 * @param dest The destination object to write the properties to
 */
int parse_media(const char *src, size_t size, media_t *dest) {
  int res = 0;

  // make sure we have some data
  if (src && (src[0] != '\0')) {
    // go through each line parsing the tags
    const char *pt = &src[0];
    int dif;
    while (!(*pt == '\0' || *pt == '\r' || *pt == '\n' || pt >= &src[size])) {
      // skip comma characters in between the attributes
      if (*pt == ',' || *pt == '=' || *pt == ':') {
        ++pt;
      } else {
        dif = parse_media_tag(pt, size - (pt - src), dest);
        pt += dif > 0 ? dif : 1;
      }
    }

    // return the difference between the 2 data points
    res = pt - src;
  }

  return res;
}

/**
 * Parses an HLS EXT-X-I-FRAME-STREAM-INF tag into the specified object.
 *
 * @param src The raw IFrameStreamInf data to parse.
 * @param size The length of src
 */
int parse_segment(const char *src, size_t size, segment_t *dest) {
  int res = 0;

  // make sure we have some data
  if (src && src[0] != '\0') {
    // go through each line parsing the tags
    const char *pt = &src[0];
    int dif;

    while (!(*pt == '\0' || *pt == '\r' || *pt == '\n' || pt >= &src[size])) {
      // skip comma characters in between the attributes
      if (*pt == ',' || *pt == '=' || *pt == '\r' || *pt == '#') {
        ++pt;
      } else {
        dif = parse_segment_tag(pt, size - (pt - src), dest);
        pt += dif > 0 ? dif : 1;
      }
    }

    // return the difference between the 2 data points
    res = pt - src;
  }

  return res;
}

/**
 * Parses an HLS EXT-X-I-FRAME-STREAM-INF tag into the specified object.
 *
 * @param src The raw IFrameStreamInf data to parse.
 * @param size The length of src
 */
int parse_segment_uri(const char *src, size_t size, media_playlist_t *dest) {
  int res = 0;

  segment_t *segment = dest->last_segment;
  // make sure we have some data
  if (segment && src && src[0] != '\0') {
    // parse until the end of the line
    const char *pt = &src[0];
    pt += parse_line_to_str(pt, &segment->uri, size - (pt - src));
    // return the difference between the 2 data points
    res = pt - src;

    if (segment->uri) {
      path_combine(&segment->uri, dest->uri, segment->uri);
    }

    segment->byte_range.n = dest->next_segment_byterange.n;
    segment->byte_range.o = dest->next_segment_byterange.o;
    dest->next_segment_byterange.n = dest->next_segment_byterange.o = 0;

    segment->key_index = dest->nb_keys - 1;
    segment->map_index = dest->nb_maps - 1;
    segment->daterange_index = dest->nb_dateranges - 1;

    segment->custom_tags.data = dest->custom_tags.data;
    segment->custom_tags.next = dest->custom_tags.next;
    dest->custom_tags.data = NULL;
    dest->custom_tags.next = NULL;

    segment->discontinuity = dest->next_segment_discontinuity;
    // reset the discontinuity flag
    dest->next_segment_discontinuity = HLS_FALSE;
  }

  return res;
}

/**
 * Helper function for parsing HLS tags and setting values on the supplied
 * object
 *
 * @param src The HLS source string
 * @param size The size of the source string
 * @param dest The destination object to write the value to
 */
int parse_segment_tag(const char *src, size_t size, segment_t *dest) {
  const char *pt = src;

  if (!src || !size || !dest) {
    return 0;
  }

  // get past the #EXTINF: tag if it is present
  while (!((*pt >= '0' && *pt <= '9') || *pt == '.')) {
    ++pt;
  }

  pt += parse_str_to_float(pt, &dest->duration, size - (pt - src));

  if (*pt == ',') {
    ++pt;
    pt += parse_line_to_str(pt, &dest->title, size - (pt - src));
  }

  return pt - src;
}

/**
 * Parses an HLS session data  tag into the specified object.
 *
 * @param src The raw session data to parse.
 * @param size The length of src
 * @param dest The obeject to write the ouput values into
 */
int parse_session_data(const char *src, size_t size, session_data_t *dest) {
  int res = 0;

  // make sure we have some data
  if (src && src[0] != '\0' && size > 0) {
    // go through each line parsing the tags
    const char *pt = &src[0];
    int dif;
    while (!(*pt == '\0' || *pt == '\r' || *pt == '\n' || pt >= &src[size])) {
      // skip comma characters in between the attributes
      if (*pt == ',' || *pt == '=' || *pt == ':') {
        ++pt;
      } else {
        dif = parse_session_data_tag(pt, size - (pt - src), dest);
        pt += dif > 0 ? dif : 1;
      }
    }

    // return the difference between the 2 data points
    res = pt - src;
  }

  return res;
}

/**
 * Helper function for parsing HLS tags and setting values on the supplied
 * object
 *
 * @param src The HLS source string
 * @param size The size of the source string
 * @param dest The destination object to write the value to
 */
int parse_session_data_tag(const char *src, size_t size, session_data_t *dest) {
  if (!src || !size || !dest) {
    return 0;
  }

  const char *pt = src;

  if (EQUAL(pt, DATAID)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->data_id, size - (pt - src));
  } else if (EQUAL(pt, VALUE)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->value, size - (pt - src));
  } else if (EQUAL(pt, URI)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->uri, size - (pt - src));
  } else if (EQUAL(pt, LANGUAGE)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->language, size - (pt - src));
  } else {
    // custom tags?
  }

  return pt - src;
}

/**
 * Helper function for parsing HLS tags and setting values on the supplied
 * object
 *
 * @param src The HLS source string
 * @param size The size of the source string
 * @param dest The destination object to write the value to
 */
int parse_start(const char *src, size_t size, start_t *dest) {
  if (!src || !size) {
    return 0;
  }

  const char *pt = src;

  while (!(*pt == '\0' || *pt == '\r' || *pt == '\n' || pt >= &src[size])) {
    if (*pt == ':') {
      ++pt;
    }

    if (EQUAL(pt, TIMEOFFSET)) {
      ++pt; // get past the '=' sign
      float *value = dest ? &dest->time_offset : NULL;
      pt += parse_str_to_float(pt, value, size - (pt - src));
    } else if (EQUAL(pt, PRECISE)) {
      ++pt; // get past the '=' sign
      if (EQUAL(pt, YES)) {
        if (dest) {
          dest->precise = HLS_TRUE;
        }
      } else if (EQUAL(pt, NO)) {
        if (dest) {
          dest->precise = HLS_FALSE;
        }
      }
    } else {
      ++pt;
    }
  }

  // return how far we have moved along in the tag
  return pt - src;
}

/**
 * Parses and HLS stream inf tag into a stream_inf object
 *
 * @param src The HLS source string
 * @param size The size of the source string
 * @param dest The destination object to write the value to
 */
int parse_stream_inf(const char *src, size_t size, stream_inf_t *dest) {
  int res = 0;

  // make sure we have some data
  if (src && src[0] != '\0') {
    // go through each line parsing the tags
    const char *pt = src;

    int dif;
    while (!(*pt == '\0' || *pt == '\r' || *pt == '\n' || pt >= &src[size])) {
      // skip comma characters in between the attributes
      if (*pt == ',' || *pt == '=' || *pt == ':') {
        ++pt;
      } else {
        dif = parse_stream_inf_tag(pt, size - (pt - src), dest);
        pt += dif > 0 ? dif : 1;
      }
    }

    // return the difference between the 2 data points
    res = pt - src;
  }

  return res;
}

/**
 * Helper function for parsing HLS tags and setting values on the supplied
 * object
 *
 * @param src The HLS source string
 * @param size The size of the source string
 * @param dest The destination object to write the value to
 */
int parse_stream_inf_tag(const char *src, size_t size, stream_inf_t *dest) {
  if (!src || !size || !dest) {
    return 0;
  }

  const char *pt = src;

  if (EQUAL(pt, PROGRAMID)) {
    ++pt; // get past the '=' sign
    pt += parse_str_to_int(pt, &dest->program_id, size - (pt - src));
  } else if (EQUAL(pt, BANDWIDTH)) {
    ++pt; // get past the '=' sign
    pt += parse_str_to_float(pt, &dest->bandwidth, size - (pt - src));
  } else if (EQUAL(pt, AVERAGEBANDWIDTH)) {
    ++pt; // get past the '=' sign
    pt += parse_str_to_float(pt, &dest->avg_bandwidth, size - (pt - src));
  } else if (EQUAL(pt, CODECS)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->codecs, size - (pt - src));
  } else if (EQUAL(pt, RESOLUTION)) {
    ++pt; // get past the '=' sign
    pt += parse_resolution(pt, size - (pt - src), &dest->resolution);
  } else if (EQUAL(pt, AUDIO)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->audio, size - (pt - src));
  } else if (EQUAL(pt, VIDEO)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->video, size - (pt - src));
  } else if (EQUAL(pt, SUBTITLES)) {
    ++pt; // get past the '=' sign
    pt += parse_attrib_str(pt, &dest->subtitles, size - (pt - src));
  } else if (EQUAL(pt, CLOSEDCAPTIONS)) {
    ++pt; // get past the '=' sign
    if (EQUAL(pt, NONE)) {
      dest->closed_captions = str_utils_dup(NONE);
    } else {
      pt += parse_attrib_str(pt, &dest->closed_captions, size - (pt - src));
    }
  } else if (EQUAL(pt, FRAMERATE)) {
    ++pt; // get past the '=' sign
    pt += parse_str_to_float(pt, &dest->frame_rate, size - (pt - src));
  } else if (EQUAL(pt, HDCPLEVEL)) {
    ++pt; // get past the '=' sign
    if (EQUAL(pt, TYPE0)) {
      dest->hdcp_level = HDCP_LEVEL_TYPE0;
    } else if (EQUAL(pt, NONE)) {
      dest->hdcp_level = HDCP_LEVEL_NONE;
    }
  }

  // return the length of characters we have parsed
  return pt - src;
}
