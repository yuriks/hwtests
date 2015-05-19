#include "y2r.h"

#include <memory>
#include <cstdio>

#include <3ds.h>
#include "y2r_u.h"

#include "output.h"
#include "yuv_random_bin.h"

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

	Print("Stopping old conversions...\n");
	Y2RU_StopConversion(NULL);
	Print("Waiting until not busy...\n");
	bool is_busy = false;
	do {
		Y2RU_IsBusyConversion(NULL, &is_busy);
	} while (is_busy);

	const static u32 img_w = 32;
	const static u32 img_h = 32;
	const static u32 img_size = img_w * img_h;
	const static u32 uv_size = img_w * img_h / 4;
	const u32 bpp = GetBytesPerPixel(OUTPUT_RGB_24);
	const u32 rgb_size = img_size * bpp;

	Print("Setting Y2R variables...\n");
	Y2RU_SetInputFormat(NULL, INPUT_YUV420_INDIV_8);
	Y2RU_SetOutputFormat(NULL, OUTPUT_RGB_24);
	Y2RU_SetRotation(NULL, ROTATION_NONE);
	Y2RU_SetBlockAlignment(NULL, BLOCK_LINE);

	Handle end_event;
	Y2RU_SetTransferEndInterrupt(NULL, true);
	Y2RU_GetTransferEndEvent(NULL, &end_event);

	Y2RU_SetInputLineWidth(NULL, 32);
	Y2RU_SetInputLines(NULL, 32);
	Y2RU_SetStandardCoefficient(NULL, COEFFICIENT_ITU_R_BT_601_SCALING);
	Y2RU_SetAlpha(NULL, 255);

	std::unique_ptr<u8[]> rgb_buf(new u8[rgb_size]);
	Y2RU_SetSendingY(NULL, yuv_random_bin, img_size, img_w, 0, 0xFFFF8001);
	Y2RU_SetSendingU(NULL, yuv_random_bin + img_size, uv_size, img_w / 2, 0, 0xFFFF8001);
	Y2RU_SetSendingV(NULL, yuv_random_bin + img_size + uv_size, uv_size, img_w / 2, 0, 0xFFFF8001);
	Y2RU_SetReceiving(NULL, rgb_buf.get(), rgb_size, 8 * img_w * bpp, 0, 0xFFFF8001);

	Print("Starting conversion...\n");
	Y2RU_StartConversion(NULL);
	Print("Waiting for end event...\n");
	svcWaitSynchronization(end_event, -1);
	Y2RU_StopConversion(NULL);
	Print("Conversion stopped.\n");

	FILE* rgb_out = fopen("/rgb_out.bin", "wb");
	fwrite(rgb_buf.get(), 1, rgb_size, rgb_out);
	fclose(rgb_out);

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
