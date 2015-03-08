#include <3ds.h>
#include "tests/gpu/gpu.h"
#include "tests/gpu/displaytransfer.h"
#include "tests/gpu/memoryfills.h"

namespace GPU {
    
void TestAll() {
    // Initialize GPU
    GPU_Init(nullptr);
    
    DisplayTransfer::TestAll();
    MemoryFills::TestAll();
}

}