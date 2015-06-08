#include "y2r.h"

#include <memory>
#include <cstdio>
#include <cstring>

#include <3ds.h>
#include "y2r_u.h"

#include "output.h"
#include "common/lodepng.h"
#include "common/string_funcs.h"
#include "yuv_random_bin.h"

#include "mandrill_420p_yuv.h"
#include "mandrill_420p_bt601_yuv.h"
#include "mandrill_420p_bt601f_yuv.h"
#include "mandrill_420p_bt709f_yuv.h"
#include "mandrill_420p16_yuv.h"
#include "mandrill_422p_yuv.h"
#include "mandrill_422p16_yuv.h"
#include "mandrill_yuyv422_yuv.h"
#include "vivio_420p_yuv.h"

namespace Y2R {

u32 GetBytesPerPixel(OutputFormat rgb_format) {
    switch (rgb_format) {
        case OUTPUT_RGB_32:
            return 4;
        case OUTPUT_RGB_24:
            return 3;
        case OUTPUT_RGB_16_555:
        case OUTPUT_RGB_16_565:
            return 2;
        default:
            return 0;
    }
}

#define Check(x) Print(Common::FormatString("[%08X]\n", (x)));

void ConvertY2R(const u8* yuv_input, const char* name, const ConversionParams& params) {
    Print(Common::FormatString("--- Converting %s\n", name));
    Print("Stopping old conversions...\n");
    Check(Y2RU_StopConversion(NULL));
    Print("Waiting until not busy...\n");
    bool is_busy = false;
    Result res;
    int tries = 0;
    do {
        svcSleepThread(1000000ull);
        res = Y2RU_IsBusyConversion(NULL, &is_busy);
        tries += 1;
    } while (is_busy && tries < 1000);
    Check(res);

    const static u32 img_w = params.input_line_width;
    const static u32 img_h = params.input_lines;
    const static u32 img_size = img_w * img_h;
    const u32 bpp = GetBytesPerPixel(params.output_format);
    const u32 rgb_size = img_size * bpp;

    Print("Setting Y2R variables...\n");
    Check(Y2RU_SetInputFormat(NULL, params.input_format));
    Check(Y2RU_SetOutputFormat(NULL, params.output_format));
    Check(Y2RU_SetRotation(NULL, params.rotation));
    Check(Y2RU_SetBlockAlignment(NULL, params.block_alignment));

    Check(Y2RU_SetTransferEndInterrupt(NULL, true));
    Handle end_event = 0;
    Check(Y2RU_GetTransferEndEvent(NULL, &end_event));

    Check(Y2RU_SetInputLineWidth(NULL, params.input_line_width));
    Check(Y2RU_SetInputLines(NULL, params.input_lines));
    Check(Y2RU_SetStandardCoefficient(NULL, params.standard_coefficient));
    Check(Y2RU_SetAlpha(NULL, params.alpha));

    std::unique_ptr<u8[]> rgb_buf(new u8[rgb_size]);
    memset(rgb_buf.get(), 0, rgb_size);

    size_t src_Y_size = 0;
    size_t src_UV_size = 0;
    switch (params.input_format) {
        case INPUT_YUV422_INDIV_8:
            src_Y_size = img_size;
            src_UV_size = img_size / 2;
            break;
        case INPUT_YUV420_INDIV_8:
            src_Y_size = img_size;
            src_UV_size = img_size / 4;
            break;
        case INPUT_YUV422_INDIV_16:
            src_Y_size = img_size * 2;
            src_UV_size = img_size / 2 * 2;
            break;
        case INPUT_YUV420_INDIV_16:
            src_Y_size = img_size * 2;
            src_UV_size = img_size / 4 * 2;
            break;
        case INPUT_YUV422_BATCH:
            src_Y_size = img_size * 2;
            src_UV_size = img_size * 2;
            break;
    }

    if (params.input_format == INPUT_YUV422_BATCH) {
        Check(Y2RU_SetSendingYUYV(NULL, yuv_input, src_Y_size, src_Y_size / img_h, 0, 0xFFFF8001));
    } else {
        const u8* src_Y = yuv_input;
        const u8* src_U = src_Y + src_Y_size;
        const u8* src_V = src_U + src_UV_size;
        Check(Y2RU_SetSendingY(NULL, src_Y, src_Y_size,  src_Y_size  / img_h, 0, 0xFFFF8001));
        Check(Y2RU_SetSendingU(NULL, src_U, src_UV_size, src_UV_size / img_h, 0, 0xFFFF8001));
        Check(Y2RU_SetSendingV(NULL, src_V, src_UV_size, src_UV_size / img_h, 0, 0xFFFF8001));
    }
    Check(Y2RU_SetReceiving(NULL, rgb_buf.get(), rgb_size, rgb_size / img_h, 0, 0xFFFF8001));

    Print("Starting conversion...\n");
    Check(Y2RU_StartConversion(NULL));
    Print("Waiting for end event...\n");
    svcWaitSynchronization(end_event, 5000000000ull);
    svcCloseHandle(end_event);
    Check(Y2RU_StopConversion(NULL));
    Print("Conversion stopped.\n");

    Print("Writing output\n");
    for (size_t i = 0; i < rgb_size; i += 3) {
        std::swap(rgb_buf[i + 0], rgb_buf[i + 2]);
    }
    char filename_buffer[128];
    snprintf(filename_buffer, sizeof(filename_buffer), "/y2r_output/%s.png", name);
    unsigned png_result = lodepng_encode24_file(filename_buffer, rgb_buf.get(), img_w, img_h);
    Print(Common::FormatString("[%u] %s\n", png_result, lodepng_error_text(png_result)));
}

void ConvertY2R(const u8* yuv_input, const char* name,
        InputFormat input_format, OutputFormat output_format, StandardCoefficient coefficient) {
    ConversionParams params = {};
    params.input_format = input_format;
    params.output_format = output_format;
    params.input_line_width = 128;
    params.input_lines = 128;
    params.standard_coefficient = coefficient;

    ConvertY2R(yuv_input, name, params);
}

void TestAll() {
    y2rInit();
    Print("Initializing driver...\n");

    u8 ping_process = 0;
    Y2RU_PingProcess(NULL, &ping_process);
    if (ping_process > 1) {
        Print("PingProcess shenanigans!\n");
    } else {
        Y2RU_DriverInitialize(NULL);
    }

#define Convert(input, in_fmt, in_coeff) ConvertY2R(input, #input, in_fmt, OUTPUT_RGB_24, in_coeff)
    Convert(mandrill_420p_yuv,        INPUT_YUV420_INDIV_8,  COEFFICIENT_ITU_R_BT_709_SCALING);
    Convert(mandrill_420p_bt601_yuv,  INPUT_YUV420_INDIV_8,  COEFFICIENT_ITU_R_BT_601_SCALING);
    Convert(mandrill_420p_bt601f_yuv, INPUT_YUV420_INDIV_8,  COEFFICIENT_ITU_R_BT_601);
    Convert(mandrill_420p_bt709f_yuv, INPUT_YUV420_INDIV_8,  COEFFICIENT_ITU_R_BT_709);
    Convert(mandrill_420p16_yuv,      INPUT_YUV420_INDIV_16, COEFFICIENT_ITU_R_BT_709_SCALING);
    Convert(mandrill_422p_yuv,        INPUT_YUV422_INDIV_8,  COEFFICIENT_ITU_R_BT_709_SCALING);
    Convert(mandrill_422p16_yuv,      INPUT_YUV422_INDIV_16, COEFFICIENT_ITU_R_BT_709_SCALING);
    Convert(mandrill_yuyv422_yuv,     INPUT_YUV422_BATCH,    COEFFICIENT_ITU_R_BT_709_SCALING);
    Convert(vivio_420p_yuv,           INPUT_YUV420_INDIV_8,  COEFFICIENT_ITU_R_BT_709_SCALING);
#undef Convert

    Y2RU_DriverFinalize(NULL);
    y2rExit();
}

// ConvertY2R(INPUT_YUV422_INDIV_8, OUTPUT_RGB_32, "/yuv422I8_rgb32_out.bin");
// ConvertY2R(INPUT_YUV422_INDIV_8, OUTPUT_RGB_24, "/yuv422I8_rgb24_out.bin");
// ConvertY2R(INPUT_YUV422_INDIV_8, OUTPUT_RGB_16_555, "/yuv422I8_rgb555_out.bin");
// ConvertY2R(INPUT_YUV422_INDIV_8, OUTPUT_RGB_16_565, "/yuv422I8_rgb565_out.bin");

// ConvertY2R(INPUT_YUV420_INDIV_8, OUTPUT_RGB_32, "/yuv420I8_rgb32_out.bin");
// ConvertY2R(INPUT_YUV420_INDIV_8, OUTPUT_RGB_24, "/yuv420I8_rgb24_out.bin");
// ConvertY2R(INPUT_YUV420_INDIV_8, OUTPUT_RGB_16_555, "/yuv420I8_rgb555_out.bin");
// ConvertY2R(INPUT_YUV420_INDIV_8, OUTPUT_RGB_16_565, "/yuv420I8_rgb565_out.bin");

// ConvertY2R(INPUT_YUV422_INDIV_16, OUTPUT_RGB_32, "/yuv422I16_rgb32_out.bin");
// ConvertY2R(INPUT_YUV422_INDIV_16, OUTPUT_RGB_24, "/yuv422I16_rgb24_out.bin");
// ConvertY2R(INPUT_YUV422_INDIV_16, OUTPUT_RGB_16_555, "/yuv422I16_rgb555_out.bin");
// ConvertY2R(INPUT_YUV422_INDIV_16, OUTPUT_RGB_16_565, "/yuv422I16_rgb565_out.bin");

// ConvertY2R(INPUT_YUV420_INDIV_16, OUTPUT_RGB_32, "/yuv420I16_rgb32_out.bin");
// ConvertY2R(INPUT_YUV420_INDIV_16, OUTPUT_RGB_24, "/yuv420I16_rgb24_out.bin");
// ConvertY2R(INPUT_YUV420_INDIV_16, OUTPUT_RGB_16_555, "/yuv420I16_rgb555_out.bin");
// ConvertY2R(INPUT_YUV420_INDIV_16, OUTPUT_RGB_16_565, "/yuv420I16_rgb565_out.bin");

// ConvertY2R(INPUT_YUV422_BATCH, OUTPUT_RGB_32, "/yuv422B_rgb32_out.bin");
// ConvertY2R(INPUT_YUV422_BATCH, OUTPUT_RGB_24, "/yuv422B_rgb24_out.bin");
// ConvertY2R(INPUT_YUV422_BATCH, OUTPUT_RGB_16_555, "/yuv422B_rgb555_out.bin");
// ConvertY2R(INPUT_YUV422_BATCH, OUTPUT_RGB_16_565, "/yuv422B_rgb565_out.bin");

}
