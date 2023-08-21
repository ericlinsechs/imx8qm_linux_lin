# NXP IMX8QM Linux LIN 

## Introduction
This repository contains a patch for the FSL LPUART (Low Power UART) driver in the Linux kernel. The patch introduces changes to the `fsl_lpuart.c` file to add LIN (Local Interconnect Network) support.
One can refer `lin_test.c` for sending and reading LIN data from userspace.

## Prerequisites
* [linux-lin](https://github.com/lin-bus/linux-lin) have to be integrated into your Linux OS or Android OS.

## Patch Details
- Introduces support for LIN (Local Interconnect Network) communication.
- Implements LIN break detection and handling.
- Configures timers for LIN Rx timeout.
- Generates an interrupt as soon as at least one byte is available in the UART Rx FIFO.

## Notes
- This patch is intended for the `drivers/tty/serial/fsl_lpuart.c` file in the `p9.0.0_2.0.0-ga` tag of the [NXP Linux kernel repository](https://github.com/nxp-imx/linux-imx/tree/p9.0.0_2.0.0-ga).

## Acknowledgements
* The patch is based on the infrastructure of [Example S32K116 LPUART LIN Slave TXRX ISR S32DS.ARM.2.2](https://community.nxp.com/t5/S32K-Knowledge-Base/Example-S32K116-LPUART-LIN-Slave-TXRX-ISR-S32DS-ARM-2-2/ta-p/1128538).
