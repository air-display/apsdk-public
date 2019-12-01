#ifndef __ALAC__DECOMP_H
#define __ALAC__DECOMP_H

namespace aps {
/// <summary>
/// 
/// </summary>
typedef void * alac_context;

/// <summary>
/// 
/// </summary>
alac_context create_alac_decoder();

/// <summary>
/// 
/// </summary>
void alac_decode_frame(alac_context alac_ctx, unsigned char *inbuffer,
                       void *outbuffer, int *outputsize);

/// /// <summary>
/// 
/// </summary>
void destory_alac_decoder(alac_context alac_ctx);
}

#endif /* __ALAC__DECOMP_H */
