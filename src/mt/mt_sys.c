#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include "mt_sys.h"
#include "keys.h"
#include "logs.h"
#include "rpc.h"

/********************************
 *       Constant data          *
 *******************************/

#define MT_SYS_RESET_TIMEOUT_MS     5000
#define MT_SYS_PING_TIMEOUT_MS      50

#define MT_CAP_SYS      0x0001
#define MT_CAP_MAC      0x0002
#define MT_CAP_NWK      0x0004
#define MT_CAP_AF       0x0008
#define MT_CAP_ZDO      0x0010
#define MT_CAP_SAPI     0x0020
#define MT_CAP_UTIL     0x0040
#define MT_CAP_DEBUG    0x0080
#define MT_CAP_APP      0x0100
#define MT_CAP_ZOAD     0x0200

/********************************
 *       Local variables        *
 *******************************/

static int _log_domain = -1;
/*
static uint8_t nv_coord_data[] = {0};
static uint8_t nv_disable_sec_data[] = {0};
static uint8_t nv_enable_sec_data[] = {1};
static uint8_t nv_set_pan_id[] = {0xCD, 0xAB};
*/

/* Callback set for any synchronous operation (see SREQ in MT specification) */
/*
static SyncActionCb sync_action_cb = NULL;
static uint64_t _ext_addr = 0x0000000000000000;
*/

/********************************
 *     MT SYS callbacks         *
 *******************************/
/*
static uint8_t _ping_srsp_cb(PingSrspFormat_t *msg)
{
    INF("System ping SRSP received. Capabilities : %04X",
            msg->Capabilities);
    if(sync_action_cb)
        sync_action_cb();

    return 0;
}

static uint8_t _get_ext_addr_srsp_cb(GetExtAddrSrspFormat_t *msg)
{
    INF("Received extended address (0x%016X)", msg->ExtAddr);
    _ext_addr = msg->ExtAddr;

    if(sync_action_cb)
        sync_action_cb();
    return 0;
}

static uint8_t _reset_ind_cb(ResetIndFormat_t *msg)
{
    INF("System reset ind. received. Reason : %s ",
            msg->Reason == 0 ? "power up":(msg->Reason == 1 ?
                "External":"Watchdog"));
    INF("Transport version : %d", msg->TransportRev);
    INF("Product ID : %d", msg->ProductId);
    INF("ZNP version : %d.%d.%d", msg->MajorRel, msg->MinorRel, msg->HwRev);
    if(sync_action_cb)
        sync_action_cb();

    return 0;
}

static uint8_t _osal_nv_write_srsp_cb(OsalNvWriteSrspFormat_t *msg)
{
    if(msg->Status != ZSuccess)
        ERR("Error writing NV element : %s", znp_strerror(msg->Status));
    else
        INF("NV element written");

    if(sync_action_cb)
        sync_action_cb();

    return 0;
}


static void mt_sys_osal_nv_write(uint16_t id, uint8_t offset, uint8_t length, uint8_t *data)
{
    if(!data)
        return;

    OsalNvWriteFormat_t req;
    req.Id = id;
    req.Offset = offset;
    req.Len = length;
    memcpy(req.Value, data, length);
    sysOsalNvWrite(&req);
}
*/

static void _process_mt_sys_cb(ZgMtMsg *msg)
{
    if(msg->type == ZG_MT_CMD_POLL)
        DBG("Received SYS POLL command");
    else if(msg->type == ZG_MT_CMD_SREQ)
        DBG("Received SYS SREQ request");
    else if(msg->type == ZG_MT_CMD_AREQ)
        DBG("Received SYS AREQ request");
    else if(msg->type == ZG_MT_CMD_SRSP)
        DBG("Received SYS SRSP response");

    switch(msg->cmd)
    {
        default:
            WRN("Unsupported SYS command 0x%02X", msg->type);
            break;
    }
}


/********************************
 *          API                 *
 *******************************/

int zg_mt_sys_init(void)
{
    _log_domain = zg_logs_domain_register("zg_mt_sys", ZG_COLOR_LIGHTRED);
    zg_rpc_subsys_cb_set(ZG_MT_SUBSYS_SYS, _process_mt_sys_cb);
    INF("MT SYS module initialized");
    return 0;
}

void zg_mt_sys_shutdown(void)
{
    INF("MT SYS module shut down");
}

/*
void mt_sys_reset_dongle (SyncActionCb cb)
{
    INF("Resetting ZNP");
    sync_action_cb = cb;
    ResetReqFormat_t resReq;
    resReq.Type = 1;
    sysResetReq(&resReq);
}

void mt_sys_ping_dongle(SyncActionCb cb)
{
    INF("Ping dongle");
    sync_action_cb = cb;
    sysPing();
}

void mt_sys_nv_write_startup_options(MtSysStartupOptions options,SyncActionCb cb)
{
    uint8_t nv_options = 0x00;
    INF("Writing startup options");
    sync_action_cb = cb;

    if(options & STARTUP_CLEAR_NWK_FRAME_COUNTER)
        nv_options |= 0x1 << 7;
    if(options & STARTUP_CLEAR_STATE)
        nv_options |= 0x1 << 1;
    if(options & STARTUP_CLEAR_CONFIG)
        nv_options |= 0x1;

    mt_sys_osal_nv_write(3, 0, 1, &nv_options);
}

void mt_sys_nv_write_coord_flag(SyncActionCb cb)
{
    INF("Setting device as coordinator");
    sync_action_cb = cb;
    mt_sys_osal_nv_write(0x87, 0, 1, nv_coord_data);
}

void mt_sys_nv_write_disable_security(SyncActionCb cb)
{
    INF("Disabling NWK security");
    sync_action_cb = cb;
    mt_sys_osal_nv_write(0x64, 0, 1, nv_disable_sec_data);
}

void mt_sys_nv_write_enable_security(SyncActionCb cb)
{
    INF("Enabling NWK security");
    sync_action_cb = cb;
    mt_sys_osal_nv_write(0x64, 0, 1, nv_enable_sec_data);
}

void mt_sys_nv_set_pan_id(SyncActionCb cb)
{
    INF("Setting PAN ID");
    sync_action_cb = cb;
    mt_sys_osal_nv_write(0x83, 0, 2, nv_set_pan_id);
}

void mt_sys_nv_write_nwk_key(SyncActionCb cb)
{
    INF("Setting network key");

    sync_action_cb = cb;
    mt_sys_osal_nv_write(0x62, 0, zg_keys_network_key_size_get(), zg_keys_network_key_get());
}

void mt_sys_check_ext_addr(SyncActionCb cb)
{
    INF("Retrieving extended address");

    sync_action_cb = cb;
    sysGetExtAddr();
}

uint64_t mt_sys_get_ext_addr(void)
{
    return _ext_addr;
}

void mt_sys_nv_write_channel(uint8_t channel, SyncActionCb cb)
{
    uint32_t channel_mask = 0x0000;

    switch(channel)
    {
        case 11:
            channel_mask = 0x00000800;
            break;
        case 12:
            channel_mask = 0x00001000;
            break;
        case 13:
            channel_mask = 0x00002000;
            break;
        case 14:
            channel_mask = 0x00004000;
            break;
        case 15:
            channel_mask = 0x00008000;
            break;
        case 16:
            channel_mask = 0x00010000;
            break;
        case 17:
            channel_mask = 0x00020000;
            break;
        case 18:
            channel_mask = 0x00040000;
            break;
        case 19:
            channel_mask = 0x00080000;
            break;
        case 20:
            channel_mask = 0x00100000;
            break;
        case 21:
            channel_mask = 0x00200000;
            break;
        case 22:
            channel_mask = 0x00400000;
            break;
        case 23:
            channel_mask = 0x00800000;
            break;
        case 24:
            channel_mask = 0x01000000;
            break;
        case 25:
            channel_mask = 0x02000000;
            break;
        case 26:
            channel_mask = 0x04000000;
            break;
        default:
            ERR("Cannot set channel %d (not a proper channel value)", channel);
            if(cb)
                cb();
            return;
            break;
    }
    INF("Setting radio to operate on channel %d", channel);
    sync_action_cb = cb;
    mt_sys_osal_nv_write(0x84, 0, sizeof(channel_mask), (uint8_t *)&channel_mask);
}
*/