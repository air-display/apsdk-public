cmake_minimum_required(VERSION 3.0)
project(fdk-aac)

set(FDK_AAC_INCLUDES
	libAACdec/include
	libAACenc/include
	libSBRdec/include
	libSBRenc/include
	libMpegTPDec/include
	libMpegTPEnc/include
	libSYS/include
	libFDK/include
	libPCMutils/include
)
	
set(AACDEC_SRC
	libAACdec/src/aacdec_drc.cpp
	libAACdec/src/aacdec_hcr.cpp
	libAACdec/src/aacdecoder.cpp
	libAACdec/src/aacdec_pns.cpp
	libAACdec/src/aac_ram.cpp
	libAACdec/src/block.cpp
	libAACdec/src/channelinfo.cpp
	libAACdec/src/ldfiltbank.cpp
	libAACdec/src/rvlcbit.cpp
	libAACdec/src/rvlc.cpp
	libAACdec/src/aacdec_hcr_bit.cpp
	libAACdec/src/aacdec_hcrs.cpp
	libAACdec/src/aacdecoder_lib.cpp
	libAACdec/src/aacdec_tns.cpp
	libAACdec/src/aac_rom.cpp
	libAACdec/src/channel.cpp
	libAACdec/src/conceal.cpp
	libAACdec/src/pulsedata.cpp
	libAACdec/src/rvlcconceal.cpp
	libAACdec/src/stereo.cpp
)
#add_library(AACdec ${AACDEC_SRC})
#target_include_directories (AACdec PUBLIC ${FDK_AAC_INCLUDES})


set(AACENC_SRC
    libAACenc/src/aacenc.cpp
    libAACenc/src/aacEnc_ram.cpp
    libAACenc/src/band_nrg.cpp
    libAACenc/src/block_switch.cpp
    libAACenc/src/grp_data.cpp
    libAACenc/src/metadata_main.cpp
    libAACenc/src/pre_echo_control.cpp
    libAACenc/src/quantize.cpp
    libAACenc/src/tonality.cpp
    libAACenc/src/aacEnc_rom.cpp
    libAACenc/src/bandwidth.cpp
    libAACenc/src/channel_map.cpp
    libAACenc/src/intensity.cpp
    libAACenc/src/ms_stereo.cpp
    libAACenc/src/psy_configuration.cpp
    libAACenc/src/sf_estim.cpp
    libAACenc/src/transform.cpp
    libAACenc/src/aacenc_lib.cpp
    libAACenc/src/aacenc_tns.cpp
    libAACenc/src/bit_cnt.cpp
    libAACenc/src/chaosmeasure.cpp
    libAACenc/src/line_pe.cpp
    libAACenc/src/noisedet.cpp
    libAACenc/src/psy_main.cpp
    libAACenc/src/spreading.cpp
    libAACenc/src/aacenc_pns.cpp
    libAACenc/src/adj_thr.cpp
    libAACenc/src/bitenc.cpp
    libAACenc/src/dyn_bits.cpp
    libAACenc/src/metadata_compressor.cpp
    libAACenc/src/pnsparam.cpp
    libAACenc/src/qc_main.cpp
)
#add_library(AACenc ${AACENC_SRC})
#target_include_directories (AACenc PUBLIC ${FDK_AAC_INCLUDES})

set(FDK_SRC
    libFDK/src/autocorr2nd.cpp
    libFDK/src/dct.cpp
    libFDK/src/FDK_bitbuffer.cpp
    libFDK/src/FDK_core.cpp
    libFDK/src/FDK_crc.cpp
    libFDK/src/FDK_hybrid.cpp
    libFDK/src/FDK_tools_rom.cpp
    libFDK/src/FDK_trigFcts.cpp
    libFDK/src/fft.cpp
    libFDK/src/fft_rad2.cpp
    libFDK/src/fixpoint_math.cpp
    libFDK/src/mdct.cpp
    libFDK/src/qmf.cpp
    libFDK/src/scale.cpp
)
#add_library(FDK ${FDK_SRC})
#target_include_directories (FDK PUBLIC ${FDK_AAC_INCLUDES})

set(MPEGTPDEC_SRC
    libMpegTPDec/src/tpdec_adif.cpp
    libMpegTPDec/src/tpdec_adts.cpp
    libMpegTPDec/src/tpdec_asc.cpp
    libMpegTPDec/src/tpdec_drm.cpp
    libMpegTPDec/src/tpdec_latm.cpp
    libMpegTPDec/src/tpdec_lib.cpp
)
#add_library(MpegTPDec ${MPEGTPDEC_SRC})
#target_include_directories (MpegTPDec PUBLIC ${FDK_AAC_INCLUDES})

set(MPEGTPENC_SRC
    libMpegTPEnc/src/tpenc_adif.cpp
    libMpegTPEnc/src/tpenc_adts.cpp
    libMpegTPEnc/src/tpenc_asc.cpp
    libMpegTPEnc/src/tpenc_latm.cpp
    libMpegTPEnc/src/tpenc_lib.cpp
)
#add_library(MpegTPEnc ${MPEGTPENC_SRC})
#target_include_directories (MpegTPEnc PUBLIC ${FDK_AAC_INCLUDES})

set(PCMUTILS_SRC
    libPCMutils/src/limiter.cpp
    libPCMutils/src/pcmutils_lib.cpp
)
#add_library(PCMutils ${PCMUTILS_SRC})
#target_include_directories (PCMutils PUBLIC ${FDK_AAC_INCLUDES})

set(SBRDEC_SRC
    libSBRdec/src/env_calc.cpp
    libSBRdec/src/env_dec.cpp
    libSBRdec/src/env_extr.cpp
    libSBRdec/src/huff_dec.cpp
    libSBRdec/src/lpp_tran.cpp
    libSBRdec/src/psbitdec.cpp
    libSBRdec/src/psdec.cpp
    libSBRdec/src/psdec_hybrid.cpp
    libSBRdec/src/sbr_crc.cpp
    libSBRdec/src/sbr_deb.cpp
    libSBRdec/src/sbr_dec.cpp
    libSBRdec/src/sbrdec_drc.cpp
    libSBRdec/src/sbrdec_freq_sca.cpp
    libSBRdec/src/sbrdecoder.cpp
    libSBRdec/src/sbr_ram.cpp
    libSBRdec/src/sbr_rom.cpp
)
#add_library(SBRdec ${SBRDEC_SRC})
#target_include_directories (SBRdec PUBLIC ${FDK_AAC_INCLUDES})

set(SBRENC_SRC
    libSBRenc/src/bit_sbr.cpp
    libSBRenc/src/env_bit.cpp
    libSBRenc/src/fram_gen.cpp
    libSBRenc/src/mh_det.cpp
    libSBRenc/src/ps_bitenc.cpp
    libSBRenc/src/ps_encode.cpp
    libSBRenc/src/resampler.cpp
    libSBRenc/src/sbr_encoder.cpp
    libSBRenc/src/sbr_ram.cpp
    libSBRenc/src/ton_corr.cpp
    libSBRenc/src/code_env.cpp
    libSBRenc/src/env_est.cpp
    libSBRenc/src/invf_est.cpp
    libSBRenc/src/nf_est.cpp
    libSBRenc/src/ps_main.cpp
    libSBRenc/src/sbrenc_freq_sca.cpp
    libSBRenc/src/sbr_misc.cpp
    libSBRenc/src/sbr_rom.cpp
    libSBRenc/src/tran_det.cpp
)
#add_library(SBRenc ${SBRENC_SRC})
#target_include_directories (SBRenc PUBLIC ${FDK_AAC_INCLUDES})

set(SYS_SRC
    libSYS/src/cmdl_parser.cpp
    libSYS/src/conv_string.cpp
    libSYS/src/genericStds.cpp
    libSYS/src/wav_file.cpp
)
#add_library(SYS ${SYS_SRC})
#target_include_directories (SYS PUBLIC ${FDK_AAC_INCLUDES})

set(libfdk_aac_SOURCES
    ${AACDEC_SRC} 
    ${AACENC_SRC}
    ${MPEGTPDEC_SRC} 
    ${MPEGTPENC_SRC}
    ${SBRDEC_SRC} 
    ${SBRENC_SRC}
    ${PCMUTILS_SRC} 
    ${FDK_SRC} 
    ${SYS_SRC}
)
add_library(fdk-aac ${libfdk_aac_SOURCES})
target_include_directories (fdk-aac PUBLIC ${FDK_AAC_INCLUDES})

if(WIN32)
	add_compile_options(/EHsc /Dinline=__inline)
	list(APPEND FDK_AAC_INCLUDES win32)
endif(WIN32)

list(APPEND FDK_AAC_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})

#set(aac_enc_SOURCES aac-enc.c wavreader.c)
#add_executable(aac-enc ${libfdk_aac_SOURCES} ${aac_enc_SOURCES})
#target_include_directories (aac-enc PUBLIC ${FDK_AAC_INCLUDES})
#set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT aac-enc)
