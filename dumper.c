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
    int i, j;

    for (i = 0; i < 29; i++) {
        vc_id cid;
        memcpy(&cid, &ibuf[sizeof(vc_id) * i], sizeof(vc_id));
        fprintf(stderr, "profile %d - cid %u\n", i, cid.compressionID);
        fprintf(stderr, "  name %s - family %s (%s)\n",
                cid.Name, cid.CompressionFamily, cid.FrameSizeType);
        fprintf(stderr,  "  bitrate %d - size %dx%d @ %d\n",
                cid.bitrate, cid.Image_Width, cid.Image_Height, cid.bitdepth);
        fprintf(stderr, "  scantype %s - factor %s\n",
                cid.ScanType, cid.CompressionFactor);

        fprintf(stdout, "    { %d, %d, %d, %d, %d, %d, %d, %d,\n",
                cid.compressionID, cid.Image_Width, cid.Image_Height,
                !!strcmp((char *)cid.ScanType, "Progressive"),
                cid.CompressedFrameSize, cid.CompressedFrameSize,
                cid.bitdepth == 8 ? 4 : 6, cid.bitdepth);
        fprintf(stdout, "      dnxhd_%d_luma_weight, dnxhd_%d_chroma_weight,\n",
                cid.compressionID, cid.compressionID);
        fprintf(stdout, "      dnxhd_%d_dc_codes, dnxhd_%d_dc_bits,\n",
                cid.compressionID, cid.compressionID);
        fprintf(stdout, "      dnxhd_%d_ac_codes, dnxhd_%d_ac_bits, dnxhd_%d_ac_level\n",
                cid.compressionID, cid.compressionID, cid.compressionID);
        fprintf(stdout, "      dnxhd_%d_ac_run_flag, dnxhd_%d_ac_index_flag\n",
                cid.compressionID, cid.compressionID);
        fprintf(stdout, "      dnxhd_%d_run_codes, dnxhd_%d_run_bit, dnxhd_%d_run\n",
                cid.compressionID, cid.compressionID, cid.compressionID);
        fprintf(stdout, "      { FILL, ME, WITH, SENSE } },\n\n");

        fprintf(stdout, "static const uint8_t dnxhd_%d_luma_weight[] = {\n",
                cid.compressionID);
        for (j = 0; j < 64; j += 8) {
            fprintf(stdout, "     %d, %d, %d, %d, %d, %d, %d, %d,\n",
                    cid.LumaQTable[j + 0], cid.LumaQTable[j + 1],
                    cid.LumaQTable[j + 2], cid.LumaQTable[j + 3],
                    cid.LumaQTable[j + 4], cid.LumaQTable[j + 5],
                    cid.LumaQTable[j + 6], cid.LumaQTable[j + 7]);
        }
        fprintf(stdout, "};\n\n");
        fprintf(stdout, "static const uint8_t dnxhd_%d_chroma_weight[] = {\n",
                cid.compressionID);
        for (j = 0; j < 64; j += 8) {
            fprintf(stdout, "     %d, %d, %d, %d, %d, %d, %d, %d,\n",
                    cid.ChromaQTable[j + 0], cid.ChromaQTable[j + 1],
                    cid.ChromaQTable[j + 2], cid.ChromaQTable[j + 3],
                    cid.ChromaQTable[j + 4], cid.ChromaQTable[j + 5],
                    cid.ChromaQTable[j + 6], cid.ChromaQTable[j + 7]);
        }
        fprintf(stdout, "};\n\n");
    }
}
