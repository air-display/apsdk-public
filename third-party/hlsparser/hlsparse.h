/*
 * Copyright 2015 Joel Freeman and other contributors
 * Released under the MIT license http://opensource.org/licenses/MIT
 * see LICENSE included with package
 */

#ifndef _HLSPARSE_H
#define _HLSPARSE_H

#include <stdint.h>
#include <stdlib.h>

// HLSCode values
#define HLS_OK (0)
#define HLS_ERROR (1)

// bool_t values
#define HLS_TRUE (1)
#define HLS_FALSE (0)

// HLS tag enums
#define KEY_METHOD_UNDEFINED 0
#define KEY_METHOD_NONE 1
#define KEY_METHOD_AES128 2
#define KEY_METHOD_SAMPLEAES 3
#define KEY_METHOD_INVALID 4

#define MEDIA_TYPE_NONE 0
#define MEDIA_TYPE_VIDEO 1
#define MEDIA_TYPE_AUDIO 2
#define MEDIA_TYPE_SUBTITLES 3
#define MEDIA_TYPE_CLOSEDCAPTIONS 4
#define MEDIA_TYPE_INVALID 5

#define MEDIA_INSTREAMID_NONE 0
#define MEDIA_INSTREAMID_CC1 1
#define MEDIA_INSTREAMID_CC2 2
#define MEDIA_INSTREAMID_CC3 3
#define MEDIA_INSTREAMID_CC4 4
#define MEDIA_INSTREAMID_SERVICE 5
#define MEDIA_INSTREAMID_INVALID 6

#define PLAYLIST_TYPE_VOD 1
#define PLAYLIST_TYPE_EVENT 2
#define PLAYLIST_TYPE_INVALID 0

#define PARAM_TYPE_NONE 0
#define PARAM_TYPE_DATA 1
#define PARAM_TYPE_STRING 2
#define PARAM_TYPE_FLOAT 3

#define HDCP_LEVEL_UNDEFINED 0
#define HDCP_LEVEL_NONE 1
#define HDCP_LEVEL_TYPE0 2

// HLS tags
#define EXTM3U "EXTM3U"
#define EXTXVERSION "EXT-X-VERSION"
#define EXTINF "EXTINF"
#define EXTXBYTERANGE "EXT-X-BYTERANGE"
#define EXTXDISCONTINUITY "EXT-X-DISCONTINUITY"
#define EXTXKEY "EXT-X-KEY"
#define EXTXMAP "EXT-X-MAP"
#define EXTXDATERANGE "EXT-X-DATERANGE"
#define EXTXPROGRAMDATETIME "EXT-X-PROGRAM-DATE-TIME"
#define EXTXTARGETDURATION "EXT-X-TARGETDURATION"
#define EXTXMEDIASEQUENCE "EXT-X-MEDIA-SEQUENCE"
#define EXTXDISCONTINUTITYSEQUENCE "EXT-X-DISCONTINUITY-SEQUENCE"
#define EXTXENDLIST "EXT-X-ENDLIST"
#define EXTXPLAYLISTTYPE "EXT-X-PLAYLIST-TYPE"
#define EXTXIFRAMESONLY "EXT-X-I-FRAMES-ONLY"
#define EXTXMEDIA "EXT-X-MEDIA"
#define EXTXSTREAMINF "EXT-X-STREAM-INF"
#define EXTXIFRAMESTREAMINF "EXT-X-I-FRAME-STREAM-INF"
#define EXTXSESSIONDATA "EXT-X-SESSION-DATA"
#define EXTXSESSIONKEY "EXT-X-SESSION-KEY"
#define EXTXINDEPENDENTSEGMENTS "EXT-X-INDEPENDENT-SEGMENTS"
#define EXTXSTART "EXT-X-START"
#define EXTXALLOWCACHE "EXT-X-ALLOW-CACHE"
#define EXTXDISCONTINUITYSEQ "EXT-X-DISCONTINUITY-SEQUENCE"

// HLS tag properties
#define PROGRAMID "PROGRAM-ID"
#define BANDWIDTH "BANDWIDTH"
#define AVERAGEBANDWIDTH "AVERAGE-BANDWIDTH"
#define CODECS "CODECS"
#define RESOLUTION "RESOLUTION"
#define VIDEO "VIDEO"
#define URI "URI"
#define FRAMERATE "FRAME-RATE"
#define HDCPLEVEL "HDCP-LEVEL"
#define AUDIO "AUDIO"
#define SUBTITLES "SUBTITLES"
#define CLOSEDCAPTIONS "CLOSED-CAPTIONS"
#define METHOD "METHOD"
#define KEY_IV "IV"
#define KEYFORMAT "KEYFORMAT"
#define KEYFORMATVERSIONS "KEYFORMATVERSIONS"
#define BYTERANGE "BYTERANGE"
#define ID "ID"
#define CLASS "CLASS"
#define STARTDATE "START-DATE"
#define ENDDATE "END-DATE"
#define DURATION "DURATION"
#define PLANNEDDURATION "PLANNED-DURATION"
#define XCLIENT "X-"
#define SCTE35CMD "SCTE35-CMD"
#define SCTE35OUT "SCTE35-OUT"
#define SCTE35IN "SCTE35-IN"
#define ENDONNEXT "END-ON-NEXT"
#define TYPE "TYPE"
#define GROUPID "GROUP-ID"
#define NAME "NAME"
#define AUTOSELECT "AUTOSELECT"
#define FORCED "FORCED"
#define DEFAULT "DEFAULT"
#define YES "YES"
#define NO "NO"
#define LANGUAGE "LANGUAGE"
#define ASSOCLANGUAGE "ASSOC-LANGUAGE"
#define INSTREAMID "INSTREAM-ID"
#define CC1 "CC1"
#define CC2 "CC2"
#define CC3 "CC3"
#define CC4 "CC4"
#define SERVICE "SERVICE"
#define CHARACTERISTICS "CHARACTERISTICS"
#define CHANNELS "CHANNELS"
#define DATAID "DATA-ID"
#define VALUE "VALUE"
#define TIMEOFFSET "TIME-OFFSET"
#define PRECISE "PRECISE"
#define VOD "VOD"
#define EVENT "EVENT"
#define TYPE0 "TYPE-0"
#define NONE "NONE"
#define AES128 "AES-128"
#define SAMPLEAES "SAMPLE-AES"

#ifdef __cplusplus
extern "C" {
#endif

typedef int HLSCode;          // HLS_OK or HLS_ERROR
typedef int bool_t;           // HLS_TRUE or HLS_FALSE
typedef uint64_t timestamp_t; // ISO timestamp as an integer

typedef void *(*hlsparse_malloc_callback)(
    size_t); // user memory allocator callback
typedef void (*hlsparse_free_callback)(void *); // user memory free callback

/**
 * Linked List of String values.
 */
typedef struct string_list {
  char *data;
  struct string_list *next;
} string_list_t;

typedef int param_type_t; // type of parameter specified in param_list_t.

/**
 * Linked list of arbitary parameter data.
 */
typedef struct param_list {
  char *key; // name of the a parameter entry.
  union {
    char *data;   // string or byte data value.
    float number; // floating point number value.
  } value;
  param_type_t value_type; // type of content stored in value.
  size_t value_size;       // number of bytes used by value.data.
  struct param_list *next; // next item in linked list
} param_list_t;

///////////////////////////////////////
/// HLS Tag 'C' Structures
///////////////////////////////////////

typedef struct {
  int n, o;
} byte_range_t;

typedef struct {
  float duration;
  char *title;
} ext_inf_t;

typedef struct {
  int width;
  int height;
} resolution_t;

typedef struct {
  int method;
  char *uri;
  char *iv;
  char *key_format;
  char *key_format_versions;
} hls_key_t;

typedef struct {
  int program_id;
  int hdcp_level;
  float bandwidth;
  float avg_bandwidth;
  float frame_rate;
  char *codecs;
  char *video;
  char *uri;
  resolution_t resolution;
} iframe_stream_inf_t;

typedef struct {
  int program_id;
  int hdcp_level;
  float bandwidth;
  float avg_bandwidth;
  float frame_rate;
  char *codecs;
  char *video;
  char *audio;
  char *uri;
  char *subtitles;
  char *closed_captions;
  resolution_t resolution;
} stream_inf_t;

typedef struct {
  char *uri;
  byte_range_t byte_range;
} map_t;

typedef struct {
  timestamp_t
      pdt; // timestamp that the EXT-X-DATERANGE appeared in the playlist
  char *id;
  char *klass;
  timestamp_t start_date;
  timestamp_t end_date;
  float duration;
  float planned_duration;
  param_list_t client_attributes;
  char *scte35_cmd;
  char *scte35_out;
  char *scte35_in;
  size_t scte35_cmd_size;
  size_t scte35_out_size;
  size_t scte35_in_size;
  bool_t end_on_next;
} daterange_t;

typedef struct {
  int type;
  int instream_id;
  int service_n;
  bool_t forced;
  bool_t is_default;
  bool_t auto_select;
  char *name;
  char *group_id;
  char *language;
  char *assoc_language;
  char *uri;
  char *characteristics;
  char *channels;
} media_t;

typedef struct {
  int sequence_num;
  int key_index;
  int map_index;
  int daterange_index;
  float duration;
  char *title;
  char *uri;
  bool_t discontinuity;
  bool_t pdt_discontinuity;
  timestamp_t pdt;
  timestamp_t pdt_end;
  byte_range_t byte_range;
  string_list_t custom_tags; // tags associated to this segment the parser
                             // didn't recognize
} segment_t;

typedef struct {
  char *data_id;
  char *value;
  char *uri;
  char *language;
} session_data_t;

typedef struct {
  float time_offset;
  bool_t precise;
} start_t;

/**
 * Linked list of segments.
 */
typedef struct segment_list {
  segment_t *data;
  struct segment_list *next;
} segment_list_t;

/**
 * Linked list of session data tags.
 */
typedef struct session_data_list {
  session_data_t *data;
  struct session_data_list *next;
} session_data_list_t;

/**
 * Linked list of Keys.
 */
typedef struct key_list {
  hls_key_t *data;
  struct key_list *next;
} key_list_t;

/**
 * Linked list of Stream Infs.
 */
typedef struct stream_inf_list {
  stream_inf_t *data;
  struct stream_inf_list *next;
} stream_inf_list_t;

/**
 * Linked list of iframe stream infs.
 */
typedef struct iframe_stream_inf_list {
  iframe_stream_inf_t *data;
  struct iframe_stream_inf_list *next;
} iframe_stream_inf_list_t;

/**
 * Linked list of Media Tags.
 */
typedef struct media_list {
  media_t *data;
  struct media_list *next;
} media_list_t;

/**
 * Linked list of Map Tags.
 */
typedef struct map_list {
  map_t *data;
  struct map_list *next;
} map_list_t;

/**
 * Linked list of Daterange Tags.
 */
typedef struct daterange_list {
  daterange_t *data;
  struct daterange_list *next;
} daterange_list_t;

/**
 * Master Playlist Structure.
 */
typedef struct {
  int version;
  char *uri;
  bool_t m3u;
  bool_t independent_segments;
  start_t start;
  session_data_list_t session_data;
  media_list_t media;
  stream_inf_list_t stream_infs;
  iframe_stream_inf_list_t iframe_stream_infs;
  string_list_t custom_tags;
  key_list_t session_keys;
  int nb_session_keys;
} master_t;

/**
 * Media Playlist Structure.
 */
typedef struct {
  int version;
  int media_sequence;
  int next_segment_media_sequence;
  int nb_segments;
  int nb_keys;
  int nb_maps;
  int nb_dateranges;
  int nb_custom_tags;
  int playlist_type;
  int discontinuity_sequence;
  float target_duration;
  float duration;
  char *uri;
  bool_t m3u;
  bool_t independent_segments;
  bool_t allow_cache;
  bool_t iframes_only;
  bool_t next_segment_discontinuity;
  bool_t end_list;
  bool_t discontinuity;
  timestamp_t next_segment_pdt;
  start_t start;
  byte_range_t next_segment_byterange;
  segment_list_t segments;
  key_list_t keys;
  map_list_t maps;
  daterange_list_t dateranges;
  string_list_t custom_tags;
  segment_t *last_segment;
} media_playlist_t;

///////////////////////////////////////
/// Parsing and Writing Functions
///////////////////////////////////////

/**
 * Global initialization of the library.
 * This function or hlsparse_global_init_mem(m,f) must be called prior to any
 * other API.
 *
 * @returns HLS_OK on success.
 */
HLSCode hlsparse_global_init(void);

/**
 * Global initialization of the library with a custom memory allocator.
 * This function or hlsparse_global_init(void) must be called prior to any other
 * API.
 *
 * @param m user defined malloc callback.
 * @param f user defined free callback.
 * @returns HLS_OK on success.
 */
HLSCode hlsparse_global_init_mem(hlsparse_malloc_callback m,
                                 hlsparse_free_callback f);

/**
 * Initializes a master_t object
 *
 * @param dest The object to initialize
 * @returns HLS_OK on success.
 */
HLSCode hlsparse_master_init(master_t *dest);

/**
 * Initializes a media_playlist_t object
 *
 * @param dest The object to initialize
 * @returns HLS_OK on success.
 */
HLSCode hlsparse_media_playlist_init(media_playlist_t *dest);

/**
 * Cleans up a master_t object freeing any resources.
 * The master_t object itself is not destroyed in the process.
 * Call parse_master_init to reinitialize the master_t after if has been
 * destroyed. This will free any associated pointers, care must be taken when
 * manipulating or creating playlists from scratch and attempting to terminate
 * them.
 *
 * @param dest The master_t object to destroy
 * @returns HLS_OK on success.
 */
HLSCode hlsparse_master_term(master_t *dest);

/**
 * Cleans up a media_playlist_t object freeing any resources.
 * The media_playlist_t object itself is not destroyed in the process.
 * Call parse_media_playlist_init to reinitialize the media_playlisy_t after it
 * has been destroyed. This will free any associated pointers, care must be
 * taken when manipulating or creating playlists from scratch and attempting to
 * terminate them.
 *
 * @param dest The media_playlist_t object to destroy
 * @returns HLS_OK on success.
 */
HLSCode hlsparse_media_playlist_term(media_playlist_t *dest);

/**
 * parses an HLS string of data into a media_playlist_t struct
 *
 * @param src The raw string of data that represents an HLS master playlist
 * @param size The length of src
 * @param dest The master_t to parse the source text into
 * @returns The number og bytes read.
 */
int hlsparse_master(const char *src, size_t size, master_t *dest);

/**
 * parses an HLS string of data into a media_playlist_t struct
 *
 * @param src The raw string of data that represents an HLS media playlist
 * @param size The length of src
 * @param dest The media_playlist_t to parse the source text into
 * @returns The number og bytes read.
 */
int hlsparse_media_playlist(const char *src, size_t size,
                            media_playlist_t *dest);

/**
 * writes an HLS master playlist from a master_t structure.
 *
 * @param dest A NULL pointer which will be assiged to the output UTF-8 string.
 * @param dest_size The size of the string assigned to 'dest'.
 * @param master The master playlist structure used to write a playlist from.
 * @returns HLS_OK on success.
 */
HLSCode hlswrite_master(char **dest, int *dest_size, master_t *master);

/**
 * writes an HLS media playlist from a media_playlist_t structure.
 *
 * @param dest A NULL pointer which will be assiged to the output UTF-8 string.
 * @param dest_size The size of the string assigned to 'dest'.
 * @param master The media playlist structure used to write a playlist from.
 * @returns HLS_OK on success.
 */
HLSCode hlswrite_media(char **dest, int *dest_size, media_playlist_t *playlist);

///////////////////////////////////////////////////////////////
/// Struct initialization and termination util Functions
///////////////////////////////////////////////////////////////

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

#ifdef __cplusplus
}
#endif

#endif // _HLSPARSE_H
