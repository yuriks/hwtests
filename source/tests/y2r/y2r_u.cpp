#include "y2r_u.h"

#include <cstdlib>
#include <cstring>

#include <3ds.h>

static Handle y2rHandle;

Result y2rInit()
{
    return srvGetServiceHandle(&y2rHandle, "y2r:u");
}

Result y2rExit()
{
    return svcCloseHandle(y2rHandle);
}

Result Y2RU_SetInputFormat(Handle* servhandle, InputFormat format)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x00010040;
    cmdbuf[1] = (u32)format;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_SetOutputFormat(Handle* servhandle, OutputFormat format)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x00030040;
    cmdbuf[1] = (u32)format;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_SetRotation(Handle* servhandle, Rotation rotation)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x00050040;
    cmdbuf[1] = (u32)rotation;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_SetBlockAlignment(Handle* servhandle, BlockAlignment alignment)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x00070040;
    cmdbuf[1] = (u32)alignment;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_SetTransferEndInterrupt(Handle* servhandle, bool should_interrupt)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x000D0040;
    cmdbuf[1] = (u32)should_interrupt;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_GetTransferEndEvent(Handle* servhandle, Handle* end_event)
{
    if (!servhandle) servhandle = &y2rHandle;

    if (*end_event != 0) {
        svcCloseHandle(*end_event);
        *end_event = 0;
    }

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x000F0000;
    cmdbuf[0x80] = 0xF0000;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;

    *end_event = (Handle)cmdbuf[3];
    return (Result)cmdbuf[1];
}

Result Y2RU_SetSendingY(Handle* servhandle, const u8* src_buf, u32 image_size, u16 transfer_unit,
    u16 transfer_stride, Handle src_process)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x00100102;
    cmdbuf[1] = (u32)src_buf;
    cmdbuf[2] = image_size;
    cmdbuf[3] = (u32)transfer_unit;
    cmdbuf[4] = (u32)transfer_stride;
    cmdbuf[5] = 0;
    cmdbuf[6] = src_process;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_SetSendingU(Handle* servhandle, const u8* src_buf, u32 image_size, u16 transfer_unit,
    u16 transfer_stride, Handle src_process)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x00110102;
    cmdbuf[1] = (u32)src_buf;
    cmdbuf[2] = image_size;
    cmdbuf[3] = (u32)transfer_unit;
    cmdbuf[4] = (u32)transfer_stride;
    cmdbuf[5] = 0;
    cmdbuf[6] = src_process;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_SetSendingV(Handle* servhandle, const u8* src_buf, u32 image_size, u16 transfer_unit,
    u16 transfer_stride, Handle src_process)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x00120102;
    cmdbuf[1] = (u32)src_buf;
    cmdbuf[2] = image_size;
    cmdbuf[3] = (u32)transfer_unit;
    cmdbuf[4] = (u32)transfer_stride;
    cmdbuf[5] = 0;
    cmdbuf[6] = src_process;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_SetSendingYUYV(Handle* servhandle, const u8* src_buf, u32 image_size, u16 transfer_unit,
    u16 transfer_stride, Handle src_process)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x00130102;
    cmdbuf[1] = (u32)src_buf;
    cmdbuf[2] = image_size;
    cmdbuf[3] = (u32)transfer_unit;
    cmdbuf[4] = (u32)transfer_stride;
    cmdbuf[5] = 0;
    cmdbuf[6] = src_process;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_SetReceiving(Handle* servhandle, u8* dst_buf, u32 image_size, u16 transfer_unit,
    u16 transfer_stride, Handle dst_process)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x00180102;
    cmdbuf[1] = (u32)dst_buf;
    cmdbuf[2] = image_size;
    cmdbuf[3] = (u32)transfer_unit;
    cmdbuf[4] = (u32)transfer_stride;
    cmdbuf[5] = 0;
    cmdbuf[6] = dst_process;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_SetInputLineWidth(Handle* servhandle, u16 line_width)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x001A0040;
    cmdbuf[1] = (u32)line_width;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_SetInputLines(Handle* servhandle, u16 num_lines)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x001C0040;
    cmdbuf[1] = (u32)num_lines;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_SetCoefficient(Handle* servhandle, const u16* coefficient)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x001E0100;
	memcpy(&cmdbuf[1], coefficient, 16);

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_SetStandardCoefficient(Handle* servhandle, StandardCoefficient coefficient)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x00200040;
    cmdbuf[1] = (u32)coefficient;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_SetAlpha(Handle* servhandle, u16 alpha)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x00220040;
    cmdbuf[1] = (u32)alpha;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_StartConversion(Handle* servhandle)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x00260000;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_StopConversion(Handle* servhandle)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x00270000;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_IsBusyConversion(Handle* servhandle, bool* is_busy)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x00280000;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    *is_busy = (bool)cmdbuf[2];
    return (Result)cmdbuf[1];
}

Result Y2RU_PingProcess(Handle* servhandle, u8* ping)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x002A0000;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    *ping = (u8)cmdbuf[2];
    return (Result)cmdbuf[1];
}

Result Y2RU_DriverInitialize(Handle* servhandle)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x002B0000;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}

Result Y2RU_DriverFinalize(Handle* servhandle)
{
    if (!servhandle) servhandle = &y2rHandle;

    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x002C0000;

    if ((ret = svcSendSyncRequest(*servhandle)) != 0) return ret;
    return (Result)cmdbuf[1];
}
