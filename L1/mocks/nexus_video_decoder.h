/**
 * Mock Nexus video decoder header for L1 testing
 */
#ifndef NEXUS_VIDEO_DECODER_H
#define NEXUS_VIDEO_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NEXUS_SecureVideo_eSecure,
    NEXUS_SecureVideo_eUnsecure
} NEXUS_SecureVideo;

typedef struct {
    struct {
        NEXUS_SecureVideo secure;
    } memory[2];
} NEXUS_VideoDecoderCapabilities;

void NEXUS_GetVideoDecoderCapabilities(NEXUS_VideoDecoderCapabilities *pCaps);

#ifdef __cplusplus
}
#endif

#endif // NEXUS_VIDEO_DECODER_H
