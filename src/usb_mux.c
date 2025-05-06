/******************************************************************************
* File Name: usb_mux.c
*
* Description: This is source code for the USB2.0 Mux Functionality.
*              This file implements functions for handling USB 2.0 MUX DP/DM
*              configuration based on active CC line.
*
* Related Document: See README.md
*
********************************************************************************
* Copyright 2023-2024, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

/*******************************************************************************
 * Include header files
 ******************************************************************************/
#include "usb_mux.h"

/*******************************************************************************
* Function Name: mux_ctrl_set_cfg
********************************************************************************
* Summary:
*  Configure USB2.0 MUX based on active CC line
*
* Parameters:
*  context - USBPD context
*  cfg - settings for the Type-C Data MUX
*  cclineactive - Active CC line (CC1/CC2)
*
* Return:
*  None
*
*******************************************************************************/
void mux_ctrl_set_cfg(cy_stc_usbpd_context_t* context, mux_select_t cfg, bool cclineactive)
{
    /* Configure USB2.0 MUX based on CC polarity. */
    switch (cfg)
    {
        case MUX_CONFIG_ISOLATE:
            Cy_USBPD_Mux_ConfigDpDm(context, CY_USBPD_DPDM_MUX_CONN_NONE);
            break;
        case MUX_CONFIG_SAFE:
            break;
        case MUX_CONFIG_SS_ONLY:
            break;
        case MUX_CONFIG_INIT:
            if (cclineactive == true)
            {
                /* CC1 is active. */
                Cy_USBPD_Mux_ConfigDpDm(context, CY_USBPD_DPDM_MUX_CONN_USB_TOP);
            }
            else
            {
                /* CC2 is active. */
                Cy_USBPD_Mux_ConfigDpDm(context, CY_USBPD_DPDM_MUX_CONN_USB_BOT);
            }
            break;
        case MUX_CONFIG_DEINIT:
            break;    
        default:
            break;
    }
}

/* End of File */
