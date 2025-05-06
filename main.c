/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the USB 2.0 Mux Functionality Example
*              for ModusToolbox.
*
* Related Document: See README.md
*
*
*******************************************************************************
* Copyright 2024, Cypress Semiconductor Corporation (an Infineon company) or
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
#include "cybsp.h"
#include <stdio.h>
#include <inttypes.h>
#include "usb_mux.h"

/*******************************************************************************
* Macros
*******************************************************************************/
/* UART Debug Print */
#define DEBUG_PRINT             (0u)

/*******************************************************************************
* Global Variables
*******************************************************************************/

/* USB PD context */
cy_stc_usbpd_context_t           gl_UsbPdPort0Ctx;

/* PD device policy configuration and status structure. */
cy_stc_pd_dpm_config_t           gl_dpmConfig;

/*******************************************************************************
* Function Name: get_dpm_config
********************************************************************************
* Summary:
*  Gets the DPM configuration
*
* Parameters:
*  None
*
* Return:
*  cy_stc_pd_dpm_config_t
*
*******************************************************************************/
cy_stc_pd_dpm_config_t* get_dpm_config(void)
{
    return &(gl_dpmConfig);
}

/*******************************************************************************
* Function Prototypes
********************************************************************************/
#if DEBUG_PRINT
/* Structure for UART context */
cy_stc_scb_uart_context_t        CYBSP_UART_context;

/* Variable used for tracking the print status */
volatile bool ENTER_LOOP = true;
#endif


/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*  System entrance point. This function performs
*  - Initialize setup of device, USBPD
*  - Start Type-C
*  - Based on CC line status USBDP/DM_SYS pins routed
*    to correct DP/DM lines (USBDP/DM_TOP or USBDP/DM_BOT)
*
* Parameters:
*  None
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;
    cy_pd_cc_state_t ccStatus;
    bool cclineactive = false;
    cy_stc_usbpd_context_t* context = &gl_UsbPdPort0Ctx;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

#if DEBUG_PRINT
    cy_stc_scb_uart_context_t CYBSP_UART_context;

    /* Configure and enable the UART peripheral */
    Cy_SCB_UART_Init(CYBSP_UART_HW, &CYBSP_UART_config, &CYBSP_UART_context);
    Cy_SCB_UART_Enable(CYBSP_UART_HW);


    /* Sequence to clear screen */
    Cy_SCB_UART_PutString(CYBSP_UART_HW, "\x1b[2J\x1b[;H");

    /* Print "EZ-PDTM PMG1 MCU: USB2.0 Mux Functionality" */
    Cy_SCB_UART_PutString(CYBSP_UART_HW, "***************** ");
    Cy_SCB_UART_PutString(CYBSP_UART_HW, "EZ-PDTM PMG1 MCU: USB2.0 Mux Functionality ");
    Cy_SCB_UART_PutString(CYBSP_UART_HW, "***************** \r\n\n");
#endif

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize the USBPD driver */
    Cy_USBPD_Init(&gl_UsbPdPort0Ctx, 0, mtb_usbpd_port0_HW, mtb_usbpd_port0_HW_TRIM,
            (cy_stc_usbpd_config_t *)&mtb_usbpd_port0_config, get_dpm_config);

    /* Start Type-C */
    Cy_USBPD_TypeC_Start(&gl_UsbPdPort0Ctx);

    do
    {
        /* Get CC line status for Port0 */
        ccStatus = Cy_USBPD_TypeC_GetCCStatus(&gl_UsbPdPort0Ctx);
    } while ((ccStatus.cc[0] <= 1) && (ccStatus.cc[1] <= 1));

    /* Check for CC1 status, whether Rp and Rd applied */
    if((ccStatus.cc[0] >= (uint8_t)CY_PD_RD_1_5A) && (ccStatus.cc[0] <= (uint8_t)CY_PD_RD_3A))
    {
        /* CC1 is active */
        cclineactive = true;
    }
    /* Check for CC2 status, whether Rp and Rd applied */
    else if ((ccStatus.cc[1] >= (uint8_t)CY_PD_RD_1_5A) && (ccStatus.cc[1] <= (uint8_t)CY_PD_RD_3A))
    {
        /*CC2 is active */
        cclineactive = false;
    }

    /* Mux configuration function called to configure mux based on the active CC line */
    mux_ctrl_set_cfg(&gl_UsbPdPort0Ctx, MUX_CONFIG_INIT, cclineactive);

    if(context->curDpdmCfg == CY_USBPD_DPDM_MUX_CONN_USB_TOP)
    {
#if DEBUG_PRINT
        Cy_SCB_UART_PutString(CYBSP_UART_HW, "Orientation A \r\n");
        Cy_SCB_UART_PutString(CYBSP_UART_HW, "CC1 is Active \r\n");
#endif
    }
    else if(context->curDpdmCfg == CY_USBPD_DPDM_MUX_CONN_USB_BOT)
    {
#if DEBUG_PRINT
        Cy_SCB_UART_PutString(CYBSP_UART_HW, "Orientation B \r\n");
        Cy_SCB_UART_PutString(CYBSP_UART_HW, "CC2 is Active \r\n");
#endif
    }

    for (;;)
    {
#if DEBUG_PRINT
        if (ENTER_LOOP)
        {
            Cy_SCB_UART_PutString(CYBSP_UART_HW, "Entered for loop \r\n\n");
            ENTER_LOOP = false;
        }
#endif
    }
}

/* [] END OF FILE */
