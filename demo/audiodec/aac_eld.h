#ifndef __AAC__ELD_DECOMP_H
#define __AAC__ELD_DECOMP_H

namespace aps {
/// <summary>
/// 
/// </summary>
typedef void * aaceld_context;

/// <summary>
/// 
/// </summary>
aaceld_context create_aaceld_decoder();

/// <summary>
/// 
/// </summary>
void aaceld_decode_frame(aaceld_context aaceld_ctx, unsigned char *inbuffer,
  int inputsize, void *outbuffer, int *outputsize);

/// <summary>
/// 
/// </summary>
void destroy_aaceld_decoder(aaceld_context aaceld_ctx);
}

#endif /* __ALAC__DECOMP_H */
