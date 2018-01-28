#include <stdlib.h>
#include "mt.h"
#include "mt_af.h"
#include "mt_sys.h"
#include "mt_util.h"
#include "rpc.h"
#include "conf.h"

/********************************
 *          API                 *
 *******************************/
int zg_mt_init()
{
    int res = 0;
    res |= zg_rpc_init(zg_conf_get_znp_device_path());
    res |= zg_mt_af_init();
    res |= zg_mt_sys_init();
    res |= zg_mt_util_init();

    return res;
}

void zg_mt_shutdown()
{
    zg_mt_util_shutdown();
    zg_mt_sys_shutdown();
    zg_mt_af_shutdown();
    zg_rpc_shutdown();
}

void zg_mt_test_ping(void)
{
    zg_mt_sys_ping(NULL);
}
