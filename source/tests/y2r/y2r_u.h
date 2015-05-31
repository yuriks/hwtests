#pragma once

#include <3ds.h>

enum InputFormat : u8
{
    INPUT_YUV422_INDIV_8 = 0x0,
    INPUT_YUV420_INDIV_8 = 0x1,
    INPUT_YUV422_INDIV_16 = 0x2,
    INPUT_YUV420_INDIV_16 = 0x3,
    INPUT_YUV422_BATCH = 0x4,
};

enum OutputFormat : u8
{
    OUTPUT_RGB_32 = 0x0,
    OUTPUT_RGB_24 = 0x1,
    OUTPUT_RGB_16_555 = 0x2,
    OUTPUT_RGB_16_565 = 0x3,
};

enum Rotation : u8
{
    ROTATION_NONE = 0x0,
    ROTATION_CLOCKWISE_90 = 0x1,
    ROTATION_CLOCKWISE_180 = 0x2,
    ROTATION_CLOCKWISE_270 = 0x3,
};

enum BlockAlignment : u8
{
    BLOCK_LINE = 0x0,
    BLOCK_8_BY_8 = 0x1,
};

enum StandardCoefficient : u8
{
    COEFFICIENT_ITU_R_BT_601 = 0x0,
    COEFFICIENT_ITU_R_BT_709 = 0x1,
    COEFFICIENT_ITU_R_BT_601_SCALING = 0x2,
    COEFFICIENT_ITU_R_BT_709_SCALING = 0x3,
    COEFFICIENT_MAX = 0x4,
};

struct ConversionParams {
    InputFormat input_format;
    OutputFormat output_format;
    Rotation rotation;
    BlockAlignment block_alignment;
    u16 input_line_width;
    u16 input_lines;
    StandardCoefficient standard_coefficient;
    u8 reserved;
    u16 alpha;
};

Result y2rInit();
Result y2rExit();

Result Y2RU_SetInputFormat(Handle* servhandle, InputFormat format);
Result Y2RU_SetOutputFormat(Handle* servhandle, OutputFormat format);
Result Y2RU_SetRotation(Handle* servhandle, Rotation rotation);
Result Y2RU_SetBlockAlignment(Handle* servhandle, BlockAlignment alignment);
Result Y2RU_SetTransferEndInterrupt(Handle* servhandle, bool should_interrupt);
Result Y2RU_GetTransferEndEvent(Handle* servhandle, Handle* end_event);

/* For these functions, `transfer_unit` seems to be `8 * bytes_per_pixel * img_width` */
Result Y2RU_SetSendingY(Handle* servhandle, const u8* src_buf, u32 image_size, u16 transfer_unit,
    u16 transfer_stride, Handle src_process);
Result Y2RU_SetSendingU(Handle* servhandle, const u8* src_buf, u32 image_size, u16 transfer_unit,
    u16 transfer_stride, Handle src_process);
Result Y2RU_SetSendingV(Handle* servhandle, const u8* src_buf, u32 image_size, u16 transfer_unit,
    u16 transfer_stride, Handle src_process);
Result Y2RU_SetSendingYUV(Handle* servhandle, const u8* src_buf, u32 image_size, u16 transfer_unit,
    u16 transfer_stride, Handle src_process);
Result Y2RU_SetReceiving(Handle* servhandle, u8* dst_buf, u32 image_size, u16 transfer_unit,
    u16 transfer_stride, Handle dst_process);

/* Sets the width of every "line" of the image (where "line" == "row")
 * In practice, sets the width of the image
 */
Result Y2RU_SetInputLineWidth(Handle* servhandle, u16 line_width);

/* Sets the number of "lines" in the image
 * In practice, sets the height of the image
 */
Result Y2RU_SetInputLines(Handle* servhandle, u16 num_lines);
Result Y2RU_SetStandardCoefficient(Handle* servhandle, StandardCoefficient coefficient);

/* Sets transparency of the resulting image */
Result Y2RU_SetAlpha(Handle* servhandle, u16 alpha);
Result Y2RU_StartConversion(Handle* servhandle);
Result Y2RU_StopConversion(Handle* servhandle);
Result Y2RU_IsBusyConversion(Handle* servhandle, bool* is_busy);

/* Seems to check whether y2r is ready to be used */
Result Y2RU_PingProcess(Handle* servhandle, u8* ping);

Result Y2RU_DriverInitialize(Handle* servhandle);
Result Y2RU_DriverFinalize(Handle* servhandle);

