#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct vc_id {
    uint32_t compressionID;
    int8_t   Name[50];
    int8_t   CompressionFamily[50];
    int8_t   FrameSizeType[50];
    uint32_t bitrate;
    /* 2 bytes padding */
    int16_t  Image_Width;
    int16_t  Image_Height;
    uint8_t  bitdepth;
    int8_t   ScanType[50];
    int8_t   CompressionFactor[25];
    uint32_t CompressedFrameSize;
    uint32_t TargetBitRate;
    uint32_t EncodeType;
    uint32_t InterplolationType[18];
    uint32_t LumaQTable[64];
    uint32_t ChromaQTable[64];
    uint16_t AC_Amp_Decode_2_8bits[256];
    uint16_t AC_Amp_Decode_9_12bits[256];
    uint16_t AC_Amp_Decode_13_16bits[512];
    uint16_t AC_Run_Decode_2_10bits[1024];
    uint32_t VLCAmpLUT[257];
    uint16_t VLCRunLUT[63];
    uint16_t Encode_DC_Code[14];
    uint8_t  Decode_DC_Code[128];
    uint16_t AC_Amp_Decode_2_8bits_limit;
    uint16_t AC_Amp_Decode_9_12bits_limit;
    uint16_t Valid_DC_Code_len;
} vc_id;

int main(int argc, char **argv)
{
    uint8_t ibuf[181192];
    int ilen = fread(ibuf, 1, sizeof(ibuf), stdin);
    int i;

    for (i = 0; i < 29; i++) {
        vc_id cid;
        memcpy(&cid, &ibuf[sizeof(vc_id) * i], sizeof(vc_id));
        fprintf(stdout, "profile %d - cid %u\n", i, cid.compressionID);
    }
}
