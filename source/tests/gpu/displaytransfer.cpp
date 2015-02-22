#include <limits>
#include <string.h>
#include "output.h"
#include "common/scope_exit.h"
#include "common/string_funcs.h"
#include "tests/test.h"
#include "tests/gpu/displaytransfer.h"
 
namespace GPU {
namespace DisplayTransfer {
    
enum PixelFormat {
        IN_RGBA8  = 0 << 8,
        IN_RGB8   = 1 << 8,
        IN_RGB565 = 2 << 8,
        IN_RGB5A1 = 3 << 8,
        IN_RGBA4  = 4 << 8,
        OUT_RGBA8  = 0 << 12,
        OUT_RGB8   = 1 << 12,
        OUT_RGB565 = 2 << 12,
        OUT_RGB5A1 = 3 << 12,
        OUT_RGBA4  = 4 << 12,
};	

enum Flags {
        HORIZONTAL_DOWNSCALE = 1 << 24,
};

union Dimensions {
    struct {
        u16 height;
        u16 width;
    } dims;
    u32 raw;
    
    Dimensions(u16 height, u16 width) {
        dims.height = height;
        dims.width = width;
    }
};

static void DisplayTransferAndWait(u32* input, u32* output, Dimensions input_dimensions, Dimensions output_dimensions, u32 flags) {
    GSPGPU_FlushDataCache(nullptr, (u8*)input, sizeof(u32));
    GSPGPU_InvalidateDataCache(nullptr, (u8*)output, sizeof(u32));
    Result res = GX_SetDisplayTransfer(nullptr, (u32*)input, input_dimensions.raw, (u32*)output, output_dimensions.raw, flags);
    if ((u32)res != 0) {
        Log(GFX_BOTTOM, Common::FormatString("Something went wrong: %u\r\n", (u32)res));
        return;
    }
    gfxFlushBuffers();
    gspWaitForPPF();
}

static bool RGBA4_To_RGB5A1(u32* input, u32* output) {
    memset(output, 0, 0x4000 * 4);
    memset(input, 0, 0x4000 * 4);
    
    // Test Red Input
    *input = 0xF000; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA4 | OUT_RGB5A1);
    TestEquals(*output, (u32)0xF800);
    
    // Test Green Input
    *input = 0x0F00; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA4 | OUT_RGB5A1);
    TestEquals(*output, (u32)0x07C0);
    
    // Test Blue Input
    *input = 0x00F0; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA4 | OUT_RGB5A1);
    TestEquals(*output, (u32)0x003E);
    
    // Test 15 Alpha Input
    *input = 0x000F; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA4 | OUT_RGB5A1);
    TestEquals(*output, (u32)0x0001);
    
    // Test 8 Alpha Input
    *input = 0x0008; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA4 | OUT_RGB5A1);
    TestEquals(*output, (u32)0x0001);
    
    // Test 7 Alpha Input
    *input = 0x0007; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA4 | OUT_RGB5A1);
    TestEquals(*output, (u32)0x0000);
    return true;
}

static bool RGB5A1_To_RGBA4(u32* input, u32* output) {
    memset(output, 0, 0x4000 * 4);
    memset(input, 0, 0x4000 * 4);
    
    // Test Red Input
    *input = 0xF800; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGB5A1 | OUT_RGBA4);
    TestEquals(*output, (u32)0xF000);
    
    // Test Green Input
    *input = 0x07C0; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGB5A1 | OUT_RGBA4);
    TestEquals(*output, (u32)0x0F00);
    
    // Test Blue Input
    *input = 0x003E; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGB5A1 | OUT_RGBA4);
    TestEquals(*output, (u32)0x00F0);
    
    // Test Alpha Input
    *input = 0x0001; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGB5A1 | OUT_RGBA4);
    TestEquals(*output, (u32)0x000F);
    return true;
}

static bool RGBA8_To_RGBA8(u32* input, u32* output) {
    memset(output, 0, 0x4000 * 4);
    memset(input, 0, 0x4000 * 4);
    
    //Test Red Input
    *input = 0xFF000000; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGBA8);
    TestEquals(*output, (u32)0xFF000000);

    //Test Green Input
    *input = 0x00FF0000; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGBA8);
    TestEquals(*output, (u32)0x00FF0000);

    //Test Blue Input
    *input = 0x0000FF00; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGBA8);
    TestEquals(*output, (u32)0x0000FF00);

    //Test Alpha Input
    *input = 0x000000FF; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGBA8);
    TestEquals(*output, (u32)0x000000FF);
    return true;
}

static bool RGBA8_To_RGB8(u32* input, u32* output) {
    memset(output, 0, 0x4000 * 4);
    memset(input, 0, 0x4000 * 4);
    
    //Test Red Input
    *input = 0xFF000000; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGB8);
    TestEquals(*output, (u32)0x00FF0000u);

    //Test Green Input
    *input = 0x00FF0000; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGB8);
    TestEquals(*output, (u32)0x0000FF00u);

    //Test Blue Input
    *input = 0x0000FF00; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGB8);
    TestEquals(*output, (u32)0x000000FFu);

    //Test Alpha Input
    *input = 0x000000FF; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGB8);
    TestEquals(*output, (u32)0x00000000u);
    return true;
}

static bool RGBA8_To_RGB565(u32* input, u32* output) {
    memset(output, 0, 0x4000 * 4);
    memset(input, 0, 0x4000 * 4);
    
    //Test Red Input
    *input = 0xFF000000; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGB565);
    TestEquals(*output, (u32)0xF800);

    //Test Green Input
    *input = 0x00FF0000; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGB565);
    TestEquals(*output, (u32)0x07E0);

    //Test Blue Input
    *input = 0x0000FF00; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGB565);
    TestEquals(*output, (u32)0x001F);

    //Test Alpha Input
    *input = 0x000000FF; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGB565);
    TestEquals(*output, (u32)0x0000);
    return true;
}

static bool RGBA8_To_RGB5A1(u32* input, u32* output) {
    memset(output, 0, 0x4000 * 4);
    memset(input, 0, 0x4000 * 4);
    
    // Test Red Input
    *input = 0xFF000000; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGB5A1);
    TestEquals(*output, (u32)0xF800);
    
    // Test Green Input
    *input = 0x00FF0000; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGB5A1);
    TestEquals(*output, (u32)0x07C0);
    
    // Test Blue Input
    *input = 0x0000FF00; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGB5A1);
    TestEquals(*output, (u32)0x003E);
    
    // Test 255 Alpha Input
    *input = 0x000000FF; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGB5A1);
    TestEquals(*output, (u32)0x0001);
    
    // Test 100 Alpha Input
    *input = 0x00000064; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGB5A1);
    TestEquals(*output, (u32)0x0000);
    
    // Test 127 Alpha Input
    *input = 0x0000007F; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGB5A1);
    TestEquals(*output, (u32)0x0000);
    
    // Test 128 Alpha Input
    *input = 0x00000080; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGB5A1);
    TestEquals(*output, (u32)0x0001);
    
    // Test 254 Alpha Input
    *input = 0x000000FE; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGB5A1);
    TestEquals(*output, (u32)0x0001);
    return true;
}
 
static bool RGBA8_To_RGBA4(u32* input, u32* output) {
    memset(output, 0, 0x4000 * 4);
    memset(input, 0, 0x4000 * 4);
    
    // Test Red Input
    *input = 0xFF000000; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGBA4);
    TestEquals(*output, (u32)0xF000);
    
    // Test Green Input
    *input = 0x00FF0000; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGBA4);
    TestEquals(*output, (u32)0x0F00);
    
    // Test Blue Input
    *input = 0x0000FF00; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGBA4);
    TestEquals(*output, (u32)0x00F0);
    
    // Test 255 Alpha Input
    *input = 0x000000FF; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGBA4);
    TestEquals(*output, (u32)0x000F);
    
    // Test 100 Alpha Input
    *input = 0x00000064; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGBA4);
    TestEquals(*output, (u32)0x0006);
    
    // Test 127 Alpha Input
    *input = 0x0000007F; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGBA4);
    TestEquals(*output, (u32)0x0007);
    
    // Test 128 Alpha Input
    *input = 0x00000080; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGBA4);
    TestEquals(*output, (u32)0x0008);
    
    // Test 254 Alpha Input
    *input = 0x000000FE; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x80, 0x80), IN_RGBA8 | OUT_RGBA4);
    TestEquals(*output, (u32)0x000F);
    return true;
}

static bool RGBA8_To_RGBA8_Scaled_Blending(u32* input, u32* output) {
    memset(output, 0, 0x4000 * 4);
    memset(input, 0, 0x4000 * 4);
    
    *input = 0xFF000000; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x40, 0x40), IN_RGBA8 | OUT_RGBA8 | HORIZONTAL_DOWNSCALE);
    TestEquals(*output, (u32)0x7F000000);
    
    // It doesn't average the pixels when the bit 24 isn't enabled
    *input = 0xFF000000; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x40, 0x40), IN_RGBA8 | OUT_RGBA8);
    TestEquals(*output, (u32)0xFF000000);
    
    *input = 0xFFFF0000; //Input
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x40, 0x40), IN_RGBA8 | OUT_RGBA8 | HORIZONTAL_DOWNSCALE);
    TestEquals(*output, (u32)0x7F7F0000);
    
    *input = 0xFFFF0000; //Input
    input[1] = 0xFF000000;
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x40, 0x40), IN_RGBA8 | OUT_RGBA8 | HORIZONTAL_DOWNSCALE);
    input[1] = 0;
    TestEquals(*output, (u32)0xFF7F0000);
    
    *input = 0xFFFF0000; //Input
    input[1] = 0xFF0000FF;
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x40, 0x40), IN_RGBA8 | OUT_RGBA8 | HORIZONTAL_DOWNSCALE);
    input[1] = 0;
    TestEquals(*output, (u32)0xFF7F007F);
    
    *input = 0xFFFF0000; //Input
    input[1] = 0x00FF0000; //Input
    input[2] = 0xFF000000;
    *output = 0; //Output
    DisplayTransferAndWait(input, output, Dimensions(0x80, 0x80), Dimensions(0x40, 0x40), IN_RGBA8 | OUT_RGBA8 | HORIZONTAL_DOWNSCALE);
    input[1] = 0;
    input[2] = 0;
    TestEquals(*output, (u32)0x7FFF0000);
    TestEquals(output[0x20], (u32)0x7F000000);  // Why did the output end up at offset 0x20?
    output[0x20] = 0;
    return true;
}

void TestAll() {
    const std::string tag = "DisplayTransfer";
 
    u32* input = (u32*)linearAlloc(0x4000 * 4);
    u32* output = (u32*)linearAlloc(0x4000 * 4);
    
    Test(tag, "RGBA8_To_RGBA8", RGBA8_To_RGBA8(input, output), true);
    Test(tag, "RGBA8_To_RGB8", RGBA8_To_RGB8(input, output), true);
    Test(tag, "RGBA8_To_RGB565", RGBA8_To_RGB565(input, output), true);
    Test(tag, "RGBA8_To_RGB5A1", RGBA8_To_RGB5A1(input, output), true);
    Test(tag, "RGBA8_To_RGBA4", RGBA8_To_RGBA4(input, output), true);
    Test(tag, "RGB5A1_To_RGBA4", RGB5A1_To_RGBA4(input, output), true);
    Test(tag, "RGBA4_To_RGB5A1", RGBA4_To_RGB5A1(input, output), true);
    Test(tag, "RGBA8_To_RGBA8_Scaled_Blending", RGBA8_To_RGBA8_Scaled_Blending(input, output), true);
    
    linearFree(input);
    linearFree(output);
}

}
}