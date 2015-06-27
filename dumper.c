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

int dump(vc_id *cid)
{
    int i;

    fprintf(stdout, "    { %d, %d, %d, %d, %d, %d, %d, %d,\n",
            cid->compressionID, cid->Image_Width, cid->Image_Height,
            !!strcmp((char *)cid->ScanType, "Progressive"),
            cid->CompressedFrameSize, cid->CompressedFrameSize,
            cid->bitdepth == 8 ? 4 : 6, cid->bitdepth);
    fprintf(stdout, "      dnxhd_%d_luma_weight, dnxhd_%d_chroma_weight,\n",
            cid->compressionID, cid->compressionID);
    fprintf(stdout, "      dnxhd_%d_dc_codes, dnxhd_%d_dc_bits,\n",
            cid->compressionID, cid->compressionID);
    fprintf(stdout, "      dnxhd_%d_ac_codes, dnxhd_%d_ac_bits, dnxhd_%d_ac_level\n",
            cid->compressionID, cid->compressionID, cid->compressionID);
    fprintf(stdout, "      dnxhd_%d_ac_run_flag, dnxhd_%d_ac_index_flag\n",
            cid->compressionID, cid->compressionID);
    fprintf(stdout, "      dnxhd_%d_run_codes, dnxhd_%d_run_bit, dnxhd_%d_run\n",
            cid->compressionID, cid->compressionID, cid->compressionID);
    fprintf(stdout, "      { FILL, ME, WITH, SENSE } },\n\n");

    fprintf(stdout, "static const uint8_t dnxhd_%d_luma_weight[] = {\n",
            cid->compressionID);
    for (i = 0; i < 64; i += 8) {
        fprintf(stdout, "    %d, %d, %d, %d, %d, %d, %d, %d,\n",
                cid->LumaQTable[i + 0], cid->LumaQTable[i + 1],
                cid->LumaQTable[i + 2], cid->LumaQTable[i + 3],
                cid->LumaQTable[i + 4], cid->LumaQTable[i + 5],
                cid->LumaQTable[i + 6], cid->LumaQTable[i + 7]);
    }
    fprintf(stdout, "};\n\n");
    fprintf(stdout, "static const uint8_t dnxhd_%d_chroma_weight[] = {\n",
            cid->compressionID);
    for (i = 0; i < 64; i += 8) {
        fprintf(stdout, "    %d, %d, %d, %d, %d, %d, %d, %d,\n",
                cid->ChromaQTable[i + 0], cid->ChromaQTable[i + 1],
                cid->ChromaQTable[i + 2], cid->ChromaQTable[i + 3],
                cid->ChromaQTable[i + 4], cid->ChromaQTable[i + 5],
                cid->ChromaQTable[i + 6], cid->ChromaQTable[i + 7]);
    }
    fprintf(stdout, "};\n\n");

    fprintf(stdout, "static const uint8_t dnxhd_%d_dc_codes[] = {\n    ",
            cid->compressionID);
    for (i = 0; i < cid->bitdepth + 4; i++)
        fprintf(stdout, "%d, ", cid->Encode_DC_Code[i] >> 8);
    fprintf(stdout, "\n};\n\n");

    fprintf(stdout, "static const uint8_t dnxhd_%d_dc_bits[] = {\n    ",
            cid->compressionID);
    for (i = 0; i < cid->bitdepth + 4; i++)
        fprintf(stdout, "%d, ", cid->Encode_DC_Code[i] & 0xFF);
    fprintf(stdout, "\n};\n\n");


    uint8_t repeated[65536] = { 0 };
    uint8_t acbits[1024];
    uint32_t accodes[1024];
    uint8_t acrun[1024];
    uint8_t acindex[1024];
    uint8_t aclevel[1024];

    int lastval = -1;
    int k = 0;
    for (i = 0; i <= cid->AC_Amp_Decode_2_8bits_limit; i++) {
        int val = cid->AC_Amp_Decode_2_8bits[i];
        int sym = val >> 9;
        int code = val & 0x1F;
        if (code < 0xf) {
            if (!repeated[val]) {
                repeated[val] = 1;

                lastval = val;
                acbits[k]  = code;
                acrun[k]   = !!(val & 0x020);
                acindex[k] = !!(val & 0x040);
                aclevel[k] = !(val & 0x080) ? sym : 0;
                accodes[k] = i >> (8 - code);
                k++;
            }
        }
    }

    for (i = 0; i <= cid->AC_Amp_Decode_9_12bits_limit; i++) {
        int val = cid->AC_Amp_Decode_9_12bits[i & 0xFF];
        int sym = val >> 9;
        int code = val & 0x1F;
        if (code < 0xf) {
            if (!repeated[val]) {
                repeated[val] = 1;

                lastval = val;
                acbits[k]  = code;
                acrun[k]   = !!(val & 0x020);
                acindex[k] = !!(val & 0x040);
                aclevel[k] = !(val & 0x080) ? sym : 0;
                accodes[k] = (0xf00 +i) >> (12 - code);
                k++;
            }
        }
    }

    for (i = 0; i <= 512; i++) {
        int val = cid->AC_Amp_Decode_13_16bits[i & 0x1FF];
        int sym = val >> 9;
        int code = val & 0x1F;
        if (code <= 0x10) {
            if (!repeated[val]) {
                repeated[val] = 1;

                lastval = val;
                acbits[k]  = code;
                acrun[k]   = !!(val & 0x020);
                acindex[k] = !!(val & 0x040);
                aclevel[k] = !(val & 0x080) ? sym : 0;
                accodes[k] = (0xfe00 + i) >> (16 - code);
                k++;
            }
        }
    }

    fprintf(stdout, "static const uint16_t dnxhd_%d_ac_codes[257] = {\n   ",
            cid->compressionID);
    for (i = 0; i < k; i++) {
        fprintf(stdout, " %5d,", accodes[i]);
        if ((i&0x7)==0x7)
            fprintf(stdout, "\n   ");
    }
    fprintf(stdout, "\n};\n\n");

    fprintf(stdout, "static const uint16_t dnxhd_%d_ac_bits[257] = {\n   ",
            cid->compressionID);
    for (i = 0; i < k; i++) {
        fprintf(stdout, " %2d,", acbits[i]);
        if ((i&0xF)==0xf)
            fprintf(stdout, "\n   ");
    }
    fprintf(stdout, "\n};\n\n");

    fprintf(stdout, "static const uint16_t dnxhd_%d_ac_level[257] = {\n   ",
            cid->compressionID);
    for (i = 0; i < k; i++) {
        fprintf(stdout, " %2d,", aclevel[i]);
        if ((i&0xF)==0xf)
            fprintf(stdout, "\n   ");
    }
    fprintf(stdout, "\n};\n\n");

    fprintf(stdout, "static const uint16_t dnxhd_%d_ac_run_flag[257] = {\n   ",
            cid->compressionID);
    for (i = 0; i < k; i++) {
        fprintf(stdout, " %2d,", acrun[i]);
        if ((i&0xF)==0xf)
            fprintf(stdout, "\n   ");
    }
    fprintf(stdout, "\n};\n\n");

    fprintf(stdout, "static const uint16_t dnxhd_%d_ac_index_flag[257] = {\n   ",
            cid->compressionID);
    for (i = 0; i < k; i++) {
        fprintf(stdout, " %2d,", acindex[i]);
        if ((i&0xF)==0xf)
            fprintf(stdout, "\n   ");
    }
    fprintf(stdout, "\n};\n\n");

    uint32_t runcodes[1024];
    uint8_t runbits[1024];
    uint8_t run[1024];

    lastval = -1;
    k = 0;
    for (i = 0; i < 1024; i++) {
        int val = cid->AC_Run_Decode_2_10bits[i];
        if (lastval != val) {
            lastval = val;
            runbits[k] = val;
            run[k] = val >> 8;
            runcodes[k] = i >> (10 - runbits[k]);
            k++;
        }
    }

    fprintf(stdout, "static const uint16_t dnxhd_%d_run_codes[%d] = {\n   ",
            cid->compressionID, k);
    for (i = 0; i < k; i++) {
        fprintf(stdout, " %4d,", runcodes[i]);
        if ((i&0x7)==0x7)
            fprintf(stdout, "\n   ");
    }
    fprintf(stdout, "\n};\n\n");

    fprintf(stdout, "static const uint16_t dnxhd_%d_run_bits[%d] = {\n   ",
            cid->compressionID, k);
    for (i = 0; i < k; i++) {
        fprintf(stdout, " %2d,", runbits[i]);
        if ((i&0xF)==0xf)
            fprintf(stdout, "\n   ");
    }
    fprintf(stdout, "\n};\n\n");

    fprintf(stdout, "static const uint16_t dnxhd_%d_run[%d] = {\n   ",
            cid->compressionID, k);
    for (i = 0; i < k; i++) {
        fprintf(stdout, " %2d,", run[i]);
        if ((i&0xF)==0xf)
            fprintf(stdout, "\n   ");
    }
    fprintf(stdout, "\n};\n\n");


    return 0;
}

int main(int argc, char **argv)
{
    uint8_t ibuf[181192];
    int ilen = fread(ibuf, 1, sizeof(ibuf), stdin);
    int i, prof = -1;

    if (argc == 2)
        prof = atoi(argv[1]);

    for (i = 0; i < 29; i++) {
        vc_id cid;
        memcpy(&cid, &ibuf[sizeof(vc_id) * i], sizeof(vc_id));

        if (argc == 2) {
            if (cid.compressionID != prof)
                continue;
            dump(&cid);
            break;
        }

        fprintf(stderr, "profile cid %u\n", cid.compressionID);
        fprintf(stderr, "  name %s - family %s (%s)\n",
                cid.Name, cid.CompressionFamily, cid.FrameSizeType);
        fprintf(stderr, "  bitrate %d - size %dx%d @ %d\n",
                cid.bitrate, cid.Image_Width, cid.Image_Height, cid.bitdepth);
        fprintf(stderr, "  scantype %s - factor %s\n",
                cid.ScanType, cid.CompressionFactor);
    }

    return 0;
}
