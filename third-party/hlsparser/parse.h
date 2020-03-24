/*
 * Copyright 2015 Joel Freeman and other contributors
 * Released under the MIT license http://opensource.org/licenses/MIT
 * see LICENSE included with package
 */

#ifndef _PARSER_TAGS_H
#define _PARSER_TAGS_H

#include "hlsparse.h"

// compares a char* with a string literal HLS tag increasing the ptr by the
// length of the tag if successful
#define EQUAL(a, b)                                                            \
  (0 == strncmp((a), (b), sizeof(b) - 1) && (a += sizeof(b) - 1))

#ifdef __cplusplus
extern "C" {
#endif

// Memory Allocator
extern hlsparse_malloc_callback hls_malloc;
extern hlsparse_free_callback hls_free;

// Utils
char *str_utils_dup(const char *str);
char *str_utils_ndup(const char *str, size_t size);
char *str_utils_append(char *str, const char *append);
char *str_utils_nappend(char *str, const char *append, size_t size);
char *str_utils_join(const char *str, const char *join);
char *str_utils_njoin(const char *str, const char *join, size_t size);
char *path_combine(char **dest, const char *base, const char *path);

// Tag parsing
int parse_line_to_str(const char *src, char **dest, size_t size);
int parse_str_to_int(const char *src, int *dest, size_t size);
int parse_str_to_float(const char *str, float *dest, size_t size);
int parse_date(const char *src, uint64_t *dest, size_t size);
int parse_attrib_str(const char *src, char **dest, size_t size);
int parse_attrib_data(const char *src, char **dest, size_t size);
int parse_master_tag(const char *src, size_t size, master_t *dest);
int parse_media_playlist_tag(const char *src, size_t size,
                             media_playlist_t *dest);
void hlsparse_byte_range_init(byte_range_t *byte_range);
void hlsparse_ext_inf_init(ext_inf_t *ext_inf);
void hlsparse_resolution_init(resolution_t *resolution);
void hlsparse_iframe_stream_inf_init(iframe_stream_inf_t *stream_inf);
void hlsparse_stream_inf_init(stream_inf_t *stream_inf);
void hlsparse_key_init(hls_key_t *key);
void hlsparse_map_init(map_t *map);
void hlsparse_daterange_init(daterange_t *daterange);
void hlsparse_media_init(media_t *media);
void hlsparse_segment_init(segment_t *segment);
void hlsparse_session_data_init(session_data_t *session_data);
void hlsparse_start_init(start_t *start);
void hlsparse_segment_list_init(segment_list_t *list);
void hlsparse_session_data_list_init(session_data_list_t *list);
void hlsparse_key_list_init(key_list_t *list);
void hlsparse_stream_inf_list_init(stream_inf_list_t *list);
void hlsparse_iframe_stream_inf_list_init(iframe_stream_inf_list_t *list);
void hlsparse_media_list_init(media_list_t *list);
void hlsparse_map_list_init(map_list_t *list);
void hlsparse_string_list_init(string_list_t *list);
void hlsparse_daterange_list_init(daterange_list_t *list);
void hlsparse_param_list_init(param_list_t *list);
void hlsparse_param_term(char ***params, int size);
void hlsparse_ext_inf_term(ext_inf_t *ext_inf);
void hlsparse_iframe_stream_inf_term(iframe_stream_inf_t *stream_inf);
void hlsparse_stream_inf_term(stream_inf_t *stream_inf);
void hlsparse_key_term(hls_key_t *key);
void hlsparse_map_term(map_t *map);
void hlsparse_daterange_term(daterange_t *daterange);
void hlsparse_media_term(media_t *media);
void hlsparse_segment_term(segment_t *segment);
void hlsparse_session_data_term(session_data_t *session_data);
void hlsparse_segment_list_term(segment_list_t *list);
void hlsparse_session_data_list_term(session_data_list_t *list);
void hlsparse_key_list_term(key_list_t *list);
void hlsparse_media_list_term(media_list_t *list);
void hlsparse_map_list_term(map_list_t *list);
void hlsparse_daterange_list_term(daterange_list_t *list);
void hlsparse_iframe_stream_inf_list_term(iframe_stream_inf_list_t *list);
void hlsparse_stream_inf_list_term(stream_inf_list_t *list);
void hlsparse_string_list_term(string_list_t *list);
void hlsparse_param_list_term(param_list_t *list);
int parse_byte_range(const char *src, size_t size, byte_range_t *dest);
int parse_iframe_stream_inf(const char *src, size_t size,
                            iframe_stream_inf_t *dest);
int parse_iframe_stream_inf_tag(const char *src, size_t size,
                                iframe_stream_inf_t *dest);
int parse_stream_inf(const char *src, size_t size, stream_inf_t *dest);
int parse_stream_inf_tag(const char *src, size_t size, stream_inf_t *dest);
int parse_resolution(const char *src, size_t size, resolution_t *dest);
int parse_key(const char *src, size_t size, hls_key_t *key);
int parse_key_tag(const char *src, size_t size, hls_key_t *key);
int parse_map(const char *src, size_t size, map_t *map);
int parse_daterange_tag(const char *src, size_t size, daterange_t *daterange);
int parse_daterange(const char *src, size_t size, daterange_t *daterange);
int parse_map_tag(const char *src, size_t size, map_t *map);
int parse_media(const char *src, size_t size, media_t *media);
int parse_media_tag(const char *src, size_t size, media_t *media);
int parse_segment(const char *src, size_t size, segment_t *segment);
int parse_segment_tag(const char *src, size_t size, segment_t *segment);
int parse_segment_uri(const char *src, size_t size, media_playlist_t *dest);
int parse_session_data(const char *src, size_t size,
                       session_data_t *session_data);
int parse_session_data_tag(const char *src, size_t size,
                           session_data_t *session_data);
int parse_start(const char *src, size_t size, start_t *start);

#ifdef __cplusplus
}
#endif

#endif // _PARSER_TAGS_H
