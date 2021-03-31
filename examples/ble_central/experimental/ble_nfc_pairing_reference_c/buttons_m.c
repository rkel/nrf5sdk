/**
 * Copyright (c) 2016 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include "buttons_m.h"
#include "sdk_errors.h"
#include "bsp.h"
#include "nfc_pair_m.h"
#include "ble_m.h"
#include "peer_manager.h"

#define NRF_LOG_MODULE_NAME "BUTTONS_M"
#include "nrf_log.h"

#define APP_BTN_EVT_DISCONNECT  BSP_EVENT_KEY_2 /**< Event used to terminate connection with BLE peripheral device or erase bondings. */
#define APP_BTN_EVT_PAIR        BSP_EVENT_KEY_3 /**< Event used to initiate NFC pairing with BLE peripheral device. */
#define APP_BTN_RELEASE_BONDS   BSP_EVENT_KEY_0 /**< Deletes all bonds. */

/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
static void bsp_event_handler(bsp_event_t event)
{
    ret_code_t err_code;

    switch (event)
    {
        case APP_BTN_EVT_DISCONNECT:
            // Terminate connection or erase bondings depending on
            // current connection state.
            if(ble_is_connected())
            {
                NRF_LOG_INFO("Terminating BLE connection.\r\n");
                nfc_oob_pairing_tag_invalidate();
                ble_disconnect();
            }
            else
            {
                NRF_LOG_INFO("Releasing bondings.\r\n");
                err_code = pm_peers_delete();
                APP_ERROR_CHECK(err_code);
            }
            break;

        case APP_BTN_EVT_PAIR:
            NRF_LOG_INFO("Initiate NFC pairing.\r\n");
            nfc_pair_start();
            break;

        default:
            break;
    }
}

void buttons_init(void)
{
    ret_code_t err_code;

    // Initialize buttons and add bsp buttons task to the application timer.
    err_code = bsp_init(BSP_INIT_BUTTONS | BSP_INIT_LED, bsp_event_handler);
    APP_ERROR_CHECK(err_code);
}
