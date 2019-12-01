/*
 * decode AAC-ELD audio data from mac by XBMC, and play it by SDL
 *
 * modify:
 * 2012-10-31   first version (ffmpeg tutorial03.c)
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "../fdk-aac/libAACdec/include/aacdecoder_lib.h"

struct aaceld_context_s {
  int fdk_flags;
  HANDLE_AACDECODER phandle;
  TRANSPORT_TYPE transportFmt;
  unsigned int nrOfLayers;
  CStreamInfo *stream_info;
};

#include "aac_eld.h"

/* period size 480 samples */
#define N_SAMPLE 480

/* ASC config binary data */
unsigned char eld_conf[] = {0xF8, 0xE8, 0x50, 0x00};
unsigned char *conf[] = {eld_conf};
static unsigned int conf_len = sizeof(eld_conf);
static int pcm_pkt_size = 4 * N_SAMPLE;

namespace aps {
/*
 * create aac eld decoder
 */
aaceld_context create_aaceld_decoder() {
  int ret = 0;
  aaceld_context_s *aac;

  aac = (aaceld_context_s *)malloc(sizeof(aaceld_context_s));
  if (!aac)
    return NULL;

  aac->fdk_flags = 0;
  aac->transportFmt = (TRANSPORT_TYPE)0; // raw
  aac->nrOfLayers = 1;                   // 1 layer
  aac->phandle = aacDecoder_Open(aac->transportFmt, aac->nrOfLayers);
  if (aac->phandle == NULL) {
    printf("aacDecoder open faild!\n");
    return NULL;
  }

  printf("conf_len = %d\n", conf_len);
  ret = aacDecoder_ConfigRaw(aac->phandle, conf, &conf_len);
  if (ret != AAC_DEC_OK) {
    fprintf(stderr, "Unable to set configRaw\n");
    return NULL;
  }

  aac->stream_info = aacDecoder_GetStreamInfo(aac->phandle);
  if (aac->stream_info == NULL) {
    printf("aacDecoder_GetStreamInfo failed!\n");
    return NULL;
  }
  printf("> stream info: channel = %d\tsample_rate = %d\tframe_size = %d\taot "
         "= %d\tbitrate = %d\n",
         aac->stream_info->channelConfig, aac->stream_info->aacSampleRate,
         aac->stream_info->aacSamplesPerFrame, aac->stream_info->aot,
         aac->stream_info->bitRate);
  return aac;
}

/*
 * called by external, aac data input queue
 */
void aaceld_decode_frame(aaceld_context aaceld_ctx, unsigned char *inbuffer,
                         int inputsize, void *outbuffer, int *outputsize) {
  int ret = 0;
  aaceld_context_s *aaceld = (aaceld_context_s *)aaceld_ctx;
  UCHAR *input_buf[1] = {inbuffer};
  UINT sizes = inputsize;
  UINT valid_size = inputsize;

  /* step 1 -> fill aac_data_buf to decoder's internal buf */
  ret = aacDecoder_Fill(aaceld->phandle, input_buf, &sizes, &valid_size);
  if (ret != AAC_DEC_OK) {
    fprintf(stderr, "Fill failed: %x\n", ret);
    *outputsize = 0;
    return;
  }

  /* step 2 -> call decoder function */
  ret = aacDecoder_DecodeFrame(aaceld->phandle, (INT_PCM *)outbuffer,
                               pcm_pkt_size, aaceld->fdk_flags);
  if (ret != AAC_DEC_OK) {
    // fprintf(stderr, "aacDecoder_DecodeFrame : 0x%x -- inputsize: %d\n", ret,
    //        inputsize);
    *outputsize = 0;
    return;
  }

  *outputsize = pcm_pkt_size;

  /* TOCHECK: need to check and handle inputsize != valid_size ? */
  // fprintf(stderr, "pcm output %d -- inputsize: %d\n", *outputsize,
  // inputsize);
}

void destroy_aaceld_decoder(aaceld_context aaceld_ctx) {
  aaceld_context_s *aaceld = (aaceld_context_s *)aaceld_ctx;
  if (aaceld) {
    aacDecoder_Close(aaceld->phandle);
    free(aaceld);
  }
}
} // namespace aps
