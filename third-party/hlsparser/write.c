/*
 * Copyright 2017 Joel Freeman and other contributors
 * Released under the MIT license http://opensource.org/licenses/MIT
 * see LICENSE included with package
 */

#include <memory.h>
#include <stdio.h>
#include <time.h>

#include "hlsparse.h"
#include "parse.h"
#include "write.h"

#define HLSP_PAGE_SIZE (4096) // 4KB Page size

#define ADD_TAG(tag_name) latest = pgprintf(latest, "#%s\n", tag_name);
#define ADD_TAG_IF_TRUE(tag_name, value)                                       \
  if (value == HLS_TRUE) {                                                     \
    latest = pgprintf(latest, "#%s\n", tag_name);                              \
  }
#define ADD_XSTART_TAG_OPTL(value)                                             \
  if (value.time_offset != 0.f) {                                              \
    latest = pgprintf(latest, "#%s:%s=%.3f,%s=%s\n", EXTXSTART, TIMEOFFSET,    \
                      value.time_offset, PRECISE,                              \
                      (value.precise == HLS_TRUE ? YES : NO));                 \
  }
#define ADD_TAG_INT(tag_name, value)                                           \
  latest = pgprintf(latest, "#%s:%d\n", tag_name, value);
#define ADD_TAG_ENUM(tag_name, value)                                          \
  latest = pgprintf(latest, "#%s:%s\n", tag_name, value);
#define START_TAG_ENUM(tag_name, param_name, value)                            \
  latest = pgprintf(latest, "#%s:%s=%s", tag_name, param_name, value);
#define START_TAG_STR(tag_name, param_name, value)                             \
  latest = pgprintf(latest, "#%s:%s=\"%s\"", tag_name, param_name, value);
#define START_TAG_INT(tag_name, param_name, value)                             \
  latest = pgprintf(latest, "#%s:%s=%d", tag_name, param_name, value);
#define END_TAG() latest = pgprintf(latest, "\n");
#define ADD_PARAM_STR(param_name, value)                                       \
  latest = pgprintf(latest, ",%s=\"%s\"", param_name, value);
#define ADD_PARAM_INT(param_name, value)                                       \
  latest = pgprintf(latest, ",%s=%d", param_name, value);
#define ADD_PARAM_INT_OPTL(param_name, value)                                  \
  if (value > 0) {                                                             \
    latest = pgprintf(latest, ",%s=%d", param_name, value);                    \
  }
#define ADD_PARAM_FLOAT_OPTL(param_name, value)                                \
  if (value > 0.f) {                                                           \
    latest = pgprintf(latest, ",%s=%.3f", param_name, value);                  \
  }
#define ADD_PARAM_STR_OPTL(param_name, value)                                  \
  if (value) {                                                                 \
    latest = pgprintf(latest, ",%s=\"%s\"", param_name, value);                \
  }
#define ADD_PARAM_HEX_OPTL(param_name, value, count)                           \
  if (value) {                                                                 \
    latest = pgprintf(latest, ",%s=0x", param_name);                           \
    int l = 0;                                                                 \
    while (l < count) {                                                        \
      latest = pgprintf(latest, "%02X", (uint8_t)value[l]);                    \
      ++l;                                                                     \
    }                                                                          \
  }
#define ADD_PARAM_BOOL_YES_ONLY(param_name, value)                             \
  if (value == HLS_TRUE) {                                                     \
    latest = pgprintf(latest, ",%s=%s", param_name, YES);                      \
  }
#define ADD_PARAM_RES_OPTL(param_name, value)                                  \
  latest = pgprintf(latest, ",%s=%dx%d", param_name, value.width, value.height);
#define ADD_URI(value) latest = pgprintf(latest, "%s\n", value);

void timestamp_to_iso_date(timestamp_t timestamp, char *date_str, size_t size) {
  int milli = (int)(timestamp % 1000LL);
  time_t time = timestamp / 1000LL;
  struct tm *gmt = gmtime(&time);
  char tmp[30];
  strftime(tmp, 30, "%FT%T", gmt);
  snprintf(date_str, size, "%s.%03dZ", tmp, milli);
}

HLSCode hlswrite_master(char **dest, int *dest_size, master_t *master) {
  if (!dest || dest_size <= 0 || !master) {
    return HLS_ERROR;
  }

  page_t *root = create_page(NULL);
  page_t *latest = root;

  ADD_TAG(EXTM3U);
  if (master->version > 0) {
    ADD_TAG_INT(EXTXVERSION, master->version);
  }
  ADD_TAG_IF_TRUE(EXTXINDEPENDENTSEGMENTS, master->independent_segments);
  ADD_XSTART_TAG_OPTL(master->start);

  media_list_t *media = &master->media;
  while (media && media->data) {
    switch (media->data->type) {
    case MEDIA_TYPE_AUDIO:
      START_TAG_ENUM(EXTXMEDIA, TYPE, AUDIO);
      break;
    case MEDIA_TYPE_VIDEO:
      START_TAG_ENUM(EXTXMEDIA, TYPE, VIDEO);
      break;
    case MEDIA_TYPE_SUBTITLES:
      START_TAG_ENUM(EXTXMEDIA, TYPE, SUBTITLES);
      break;
    case MEDIA_TYPE_CLOSEDCAPTIONS:
      START_TAG_ENUM(EXTXMEDIA, TYPE, CLOSEDCAPTIONS);
      break;
    }

    ADD_PARAM_STR_OPTL(URI, media->data->uri);
    ADD_PARAM_STR(GROUPID, media->data->group_id);
    ADD_PARAM_STR_OPTL(LANGUAGE, media->data->language);
    ADD_PARAM_STR_OPTL(ASSOCLANGUAGE, media->data->language);
    ADD_PARAM_STR_OPTL(ASSOCLANGUAGE, media->data->assoc_language);
    ADD_PARAM_STR(NAME, media->data->name);
    ADD_PARAM_BOOL_YES_ONLY(DEFAULT, media->data->is_default);
    ADD_PARAM_BOOL_YES_ONLY(AUTOSELECT, media->data->auto_select);
    ADD_PARAM_BOOL_YES_ONLY(FORCED, media->data->forced);

    switch (media->data->instream_id) {
    case MEDIA_INSTREAMID_CC1:
      ADD_PARAM_STR(INSTREAMID, CC1);
      break;
    case MEDIA_INSTREAMID_CC2:
      ADD_PARAM_STR(INSTREAMID, CC2);
      break;
    case MEDIA_INSTREAMID_CC3:
      ADD_PARAM_STR(INSTREAMID, CC3);
      break;
    case MEDIA_INSTREAMID_CC4:
      ADD_PARAM_STR(INSTREAMID, CC4);
      break;
    case MEDIA_INSTREAMID_SERVICE: {
      latest = pgprintf(latest, ",%s=\"%s%d\"", INSTREAMID, SERVICE,
                        media->data->service_n);
    } break;
    }

    ADD_PARAM_STR_OPTL(CHARACTERISTICS, media->data->characteristics);
    ADD_PARAM_STR_OPTL(CHANNELS, media->data->channels);
    END_TAG();

    media = media->next;
  }

  // stream infs
  stream_inf_list_t *stream_inf_list = &master->stream_infs;
  while (stream_inf_list && stream_inf_list->data) {
    stream_inf_t *inf = stream_inf_list->data;
    START_TAG_INT(EXTXSTREAMINF, BANDWIDTH, (int)inf->bandwidth);
    ADD_PARAM_INT_OPTL(AVERAGEBANDWIDTH, (int)inf->avg_bandwidth);
    ADD_PARAM_STR_OPTL(CODECS, inf->codecs);
    ADD_PARAM_RES_OPTL(RESOLUTION, inf->resolution);
    ADD_PARAM_FLOAT_OPTL(FRAMERATE, inf->frame_rate);
    switch (inf->hdcp_level) {
    case HDCP_LEVEL_NONE:
      latest = pgprintf(latest, ",%s=%s", HDCPLEVEL, NONE);
      break;
    case HDCP_LEVEL_TYPE0:
      latest = pgprintf(latest, ",%s=%s", HDCPLEVEL, TYPE0);
      break;
    }
    ADD_PARAM_STR_OPTL(AUDIO, inf->audio);
    ADD_PARAM_STR_OPTL(VIDEO, inf->video);
    ADD_PARAM_STR_OPTL(SUBTITLES, inf->subtitles);
    ADD_PARAM_STR_OPTL(CLOSEDCAPTIONS, inf->closed_captions);
    END_TAG();
    // see if we can attach a relative url
    if (master->uri) {
      const char *rel = inf->uri;
      const char *ptr_pl = master->uri;
      const char *ptr_seg = inf->uri;
      while (*ptr_seg == *ptr_pl) {
        if (*ptr_seg == '/') {
          rel = ptr_seg + 1;
        }
        ptr_seg++;
        ptr_pl++;
      }
      ADD_URI(rel);
    } else {
      ADD_URI(inf->uri);
    }
    stream_inf_list = stream_inf_list->next;
  }

  iframe_stream_inf_list_t *if_stream_inf_list = &master->iframe_stream_infs;
  while (if_stream_inf_list && if_stream_inf_list->data) {
    iframe_stream_inf_t *inf = if_stream_inf_list->data;
    START_TAG_INT(EXTXIFRAMESTREAMINF, BANDWIDTH, (int)inf->bandwidth);
    ADD_PARAM_INT_OPTL(AVERAGEBANDWIDTH, (int)inf->avg_bandwidth);
    ADD_PARAM_STR_OPTL(CODECS, inf->codecs);
    ADD_PARAM_RES_OPTL(RESOLUTION, inf->resolution);
    switch (inf->hdcp_level) {
    case HDCP_LEVEL_NONE:
      latest = pgprintf(latest, ",%s=%s", HDCPLEVEL, NONE);
      break;
    case HDCP_LEVEL_TYPE0:
      latest = pgprintf(latest, ",%s=%s", HDCPLEVEL, TYPE0);
      break;
    }
    ADD_PARAM_STR_OPTL(VIDEO, inf->video);
    ADD_PARAM_STR(URI, inf->uri);
    END_TAG();
    if_stream_inf_list = if_stream_inf_list->next;
  }

  session_data_list_t *sess_data = &master->session_data;
  while (sess_data && sess_data->data) {
    session_data_t *sess = sess_data->data;
    START_TAG_STR(EXTXSESSIONDATA, DATAID, sess->data_id);
    ADD_PARAM_STR_OPTL(VALUE, sess->value);
    ADD_PARAM_STR_OPTL(URI, sess->uri);
    ADD_PARAM_STR_OPTL(LANGUAGE, sess->language);
    END_TAG();
    sess_data = sess_data->next;
  }

  key_list_t *key_list = &master->session_keys;
  while (key_list && key_list->data) {
    hls_key_t *key = key_list->data;
    switch (key->method) {
    case KEY_METHOD_NONE:
      START_TAG_ENUM(EXTXSESSIONKEY, METHOD, NONE);
      break;
    case KEY_METHOD_AES128:
      START_TAG_ENUM(EXTXSESSIONKEY, METHOD, AES128);
      break;
    case KEY_METHOD_SAMPLEAES:
      START_TAG_ENUM(EXTXSESSIONKEY, METHOD, SAMPLEAES);
      break;
    }
    ADD_PARAM_STR_OPTL(URI, key->uri);
    ADD_PARAM_HEX_OPTL(KEY_IV, key->iv, 16);
    ADD_PARAM_STR_OPTL(KEYFORMAT, key->key_format);
    ADD_PARAM_STR_OPTL(KEYFORMATVERSIONS, key->key_format_versions);
    END_TAG();
    key_list = key_list->next;
  }

  page_to_str(root, dest, dest_size);

  return HLS_OK;
}

HLSCode hlswrite_media(char **dest, int *dest_size,
                       media_playlist_t *playlist) {
  if (!dest || !dest_size || !playlist) {
    return HLS_ERROR;
  }

  page_t *root = create_page(NULL);
  page_t *latest = root;

  ADD_TAG(EXTM3U);
  ADD_TAG_INT(EXTXVERSION, playlist->version);
  ADD_TAG_INT(EXTXTARGETDURATION, (int)playlist->target_duration);
  ADD_TAG_INT(EXTXMEDIASEQUENCE, playlist->media_sequence);
  ADD_TAG_INT(EXTXDISCONTINUITYSEQ, playlist->discontinuity_sequence);
  switch (playlist->playlist_type) {
  case PLAYLIST_TYPE_EVENT:
    ADD_TAG_ENUM(EXTXPLAYLISTTYPE, EVENT);
    break;
  case PLAYLIST_TYPE_VOD:
    ADD_TAG_ENUM(EXTXPLAYLISTTYPE, VOD);
    break;
  }
  ADD_TAG_IF_TRUE(EXTXIFRAMESONLY, playlist->iframes_only);

  if (playlist->nb_segments > 0) {
    // first PDT
    char buf[30];
    timestamp_to_iso_date(playlist->segments.data->pdt, buf, 30);
    ADD_TAG_ENUM(EXTXPROGRAMDATETIME, buf);
  }

  int i;
  int key_idx = -1; // -1 == no key
  segment_list_t *seg = &playlist->segments;

  for (i = 0; i < playlist->nb_segments; ++i) {
    // new Key index?
    if (seg->data->key_index > key_idx) {
      key_idx = seg->data->key_index;
      // find the key
      int j = 0;
      key_list_t *key_list = &playlist->keys;
      hls_key_t *key = NULL;
      while (key_list && key_list->data && j++ <= key_idx) {
        key = key_list->data;
        key_list = key_list->next;
      }

      // add key tag
      if (key) {
        switch (key->method) {
        case KEY_METHOD_NONE:
          START_TAG_ENUM(EXTXKEY, METHOD, NONE);
          break;
        case KEY_METHOD_AES128:
          START_TAG_ENUM(EXTXKEY, METHOD, AES128);
          break;
        case KEY_METHOD_SAMPLEAES:
          START_TAG_ENUM(EXTXKEY, METHOD, SAMPLEAES);
          break;
        }
        ADD_PARAM_STR_OPTL(URI, key->uri);
        ADD_PARAM_HEX_OPTL(KEY_IV, key->iv, 16);
        ADD_PARAM_STR_OPTL(KEYFORMAT, key->key_format);
        ADD_PARAM_STR_OPTL(KEYFORMATVERSIONS, key->key_format_versions);
        END_TAG();
      }
    }

    string_list_t *ctags = &seg->data->custom_tags;
    while (ctags && ctags->data) {
      ADD_TAG(ctags->data);
      ctags = ctags->next;
    }

    if (seg->data->discontinuity == HLS_TRUE) {
      ADD_TAG(EXTXDISCONTINUITY);
      char buf[30];
      timestamp_to_iso_date(seg->data->pdt, buf, 30);
      ADD_TAG_ENUM(EXTXPROGRAMDATETIME, buf);
    }
    if (seg->data->byte_range.n > 0) {
      if (seg->data->byte_range.o != 0) {
        latest = pgprintf(latest, "#%s:%d@%d\n", EXTXBYTERANGE,
                          seg->data->byte_range.n, seg->data->byte_range.o);
      } else {
        latest = pgprintf(latest, "#%s:%d\n", EXTXBYTERANGE,
                          seg->data->byte_range.n);
      }
    }
    if (seg->data->title) {
      latest = pgprintf(latest, "#%s:%.3f,%s\n", EXTINF, seg->data->duration,
                        seg->data->title);
    } else {
      latest = pgprintf(latest, "#%s:%.3f,\n", EXTINF, seg->data->duration);
    }
    // see if we can attach a relative url
    if (playlist->uri) {
      const char *rel = seg->data->uri;
      const char *ptr_pl = playlist->uri;
      const char *ptr_seg = seg->data->uri;
      while (*ptr_seg == *ptr_pl) {
        if (*ptr_seg == '/') {
          rel = ptr_seg + 1;
        }
        ptr_seg++;
        ptr_pl++;
      }
      ADD_URI(rel);
    } else {
      ADD_URI(seg->data->uri);
    }
    seg = seg->next;
  }

  ADD_TAG_IF_TRUE(EXTXENDLIST, playlist->end_list);
  page_to_str(root, dest, dest_size);

  return HLS_OK;
}

page_t *write_to_page(page_t *page, const char *buffer, int size) {
  while (size > 0) {
    if (page->cur >= page->buffer + page->size) {
      page = create_page(page);
    }
    *page->cur = *buffer;
    page->cur++;
    buffer++;
    --size;
  }
  return page;
}

page_t *create_page(page_t *page) {
  page_t *new_page = (page_t *)hls_malloc(sizeof(page_t));
  memset(new_page, 0, sizeof(page_t));
  new_page->buffer = (char *)hls_malloc(HLSP_PAGE_SIZE);
  new_page->size = HLSP_PAGE_SIZE;
  new_page->cur = new_page->buffer;
  new_page->next = NULL;

  if (page) {
    page->next = new_page;
  }

  return new_page;
}

page_t *pgprintf(page_t *page, const char *format, ...) {
  va_list arg;

  va_start(arg, format);
  char buf[2048];
  int n = vsnprintf(buf, 2048, format, arg);
  page = write_to_page(page, buf, n);
  va_end(arg);

  return page;
}

void page_to_str(page_t *page, char **dest, int *dest_size) {
  int full_size = 0;
  const page_t *pg_ptr = page;

  while (pg_ptr) {
    full_size += pg_ptr->cur - pg_ptr->buffer;
    pg_ptr = pg_ptr->next;
  }

  char *output = (char *)hls_malloc(full_size + 1);
  char *ptr = output;
  pg_ptr = page;
  while (pg_ptr) {
    int pg_size = pg_ptr->cur - pg_ptr->buffer;
    memcpy(ptr, pg_ptr->buffer, pg_size);
    ptr += pg_size;
    pg_ptr = pg_ptr->next;
  }
  *ptr = '\0'; // NULL Pointer

  *dest = output;
  *dest_size = full_size;
}
