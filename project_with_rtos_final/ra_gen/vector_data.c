/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = sci_spi_rxi_isr, /* SCI0 RXI (Receive data full) */
            [1] = sci_spi_txi_isr, /* SCI0 TXI (Transmit data empty) */
            [2] = sci_spi_tei_isr, /* SCI0 TEI (Transmit end) */
            [3] = sci_spi_eri_isr, /* SCI0 ERI (Receive error) */
            [4] = sci_uart_rxi_isr, /* SCI4 RXI (Receive data full) */
            [5] = sci_uart_txi_isr, /* SCI4 TXI (Transmit data empty) */
            [6] = sci_uart_tei_isr, /* SCI4 TEI (Transmit end) */
            [7] = sci_uart_eri_isr, /* SCI4 ERI (Receive error) */
            [8] = sci_uart_rxi_isr, /* SCI9 RXI (Receive data full) */
            [9] = sci_uart_txi_isr, /* SCI9 TXI (Transmit data empty) */
            [10] = sci_uart_tei_isr, /* SCI9 TEI (Transmit end) */
            [11] = sci_uart_eri_isr, /* SCI9 ERI (Receive error) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_SCI0_RXI,GROUP0), /* SCI0 RXI (Receive data full) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_SCI0_TXI,GROUP1), /* SCI0 TXI (Transmit data empty) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_SCI0_TEI,GROUP2), /* SCI0 TEI (Transmit end) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_SCI0_ERI,GROUP3), /* SCI0 ERI (Receive error) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_SCI4_RXI,GROUP4), /* SCI4 RXI (Receive data full) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_SCI4_TXI,GROUP5), /* SCI4 TXI (Transmit data empty) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_SCI4_TEI,GROUP6), /* SCI4 TEI (Transmit end) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_SCI4_ERI,GROUP7), /* SCI4 ERI (Receive error) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_SCI9_RXI,GROUP0), /* SCI9 RXI (Receive data full) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_SCI9_TXI,GROUP1), /* SCI9 TXI (Transmit data empty) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_SCI9_TEI,GROUP2), /* SCI9 TEI (Transmit end) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_SCI9_ERI,GROUP3), /* SCI9 ERI (Receive error) */
        };
        #endif
        #endif
