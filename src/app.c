#include "app.h"
#include "mt_sys.h"
#include "dbgPrint.h"
#include <stdlib.h>

uv_async_t state_flag;

void app_register_callbacks()
{
    mt_sys_register_callbacks();
}

void state_machine_cb(uv_async_t *state_data)
{
    if (!state_data)
    {
        log_err("Cannot read current app state : state variable is empty");
        return;
    }
    AppState current_state = *((AppState *)state_data->data);
    log_inf("State machine has been woken up");
    switch(current_state)
    {
        case APP_STATE_INIT:
            mt_sys_reset_dongle();
            break;
        case APP_STATE_DONGLE_UP:
            mt_sys_ping_dongle();
            break;
        case APP_STATE_DONGLE_PRESENT:
            log_inf("End of state machine");
            break;
        default:
            log_err("Received state is unknown (%02d)", current_state);
            break;
    }
}

void run_state_machine()
{
    AppState *state = calloc(1, sizeof(AppState));
    if(state)
    {
        *state = APP_STATE_INIT;
        state_flag.data = state;
    }
    uv_async_send(&state_flag);
}
