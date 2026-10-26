/*
 * TrueGlide - carte simulée : port USB (VCP), à la place de STM32/serial_usb_vcp.c et de la pile USB.
 * Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.
 *
 * Le port USB de la carte est relié à la page (ou à TCP sur l'ordinateur) par simulee/liaison.c. Il se comporte
 * comme une carte branchée et reconnue par l'ordinateur. Le mode clé USB (stockage de masse) et la manette USB
 * (HID) n'existent pas dans la carte simulée.
 */

#include <stdbool.h>
#include <stdint.h>

#include "platform.h"

#include "common/utils.h"

#include "drivers/io.h"
#include "drivers/serial.h"
#include "drivers/serial_usb_vcp.h"
#include "drivers/usb_msc.h"
#include "io/usb_cdc_hid.h"
#include "io/usb_msc.h"

#include "simulee/liaison.h"

static vcpPort_t vcpPort;
static uint32_t debitUsb = 115200;

static void usbVcpSetBaudRate(serialPort_t *instance, uint32_t baudRate)
{
    UNUSED(instance);
    UNUSED(baudRate);
}

static void usbVcpSetMode(serialPort_t *instance, portMode_e mode)
{
    UNUSED(instance);
    UNUSED(mode);
}

static void usbVcpSetCtrlLineStateCb(serialPort_t *instance, void (*cb)(void *context, uint16_t ctrlLineState), void *context)
{
    UNUSED(instance);
    UNUSED(cb);
    UNUSED(context);
}

static void usbVcpSetBaudRateCb(serialPort_t *instance, void (*cb)(serialPort_t *context, uint32_t baud), serialPort_t *context)
{
    UNUSED(instance);
    UNUSED(cb);
    UNUSED(context);
}

static bool isUsbVcpTransmitBufferEmpty(const serialPort_t *instance)
{
    UNUSED(instance);
    return true;
}

static uint32_t usbVcpAvailable(const serialPort_t *instance)
{
    UNUSED(instance);
    return tgEnAttente(TG_PORT_USB);
}

static uint8_t usbVcpRead(serialPort_t *instance)
{
    UNUSED(instance);
    return tgLire(TG_PORT_USB);
}

static void usbVcpWriteBuf(serialPort_t *instance, const void *data, int count)
{
    UNUSED(instance);
    tgEcrire(TG_PORT_USB, data, count);
}

static void usbVcpFlush(vcpPort_t *port)
{
    if (port->txAt) {
        tgEcrire(TG_PORT_USB, port->txBuf, port->txAt);
        port->txAt = 0;
    }
}

static void usbVcpWrite(serialPort_t *instance, uint8_t c)
{
    vcpPort_t *port = container_of(instance, vcpPort_t, port);
    port->txBuf[port->txAt++] = c;
    if (!port->buffering || port->txAt >= ARRAYLEN(port->txBuf)) {
        usbVcpFlush(port);
    }
}

static void usbVcpBeginWrite(serialPort_t *instance)
{
    vcpPort_t *port = container_of(instance, vcpPort_t, port);
    port->buffering = true;
}

static uint32_t usbTxBytesFree(const serialPort_t *instance)
{
    UNUSED(instance);
    return 4096; // la liaison n'est jamais pleine
}

static void usbVcpEndWrite(serialPort_t *instance)
{
    vcpPort_t *port = container_of(instance, vcpPort_t, port);
    port->buffering = false;
    usbVcpFlush(port);
}

static const struct serialPortVTable usbVTable[] = {
    {
        .serialWrite = usbVcpWrite,
        .serialTotalRxWaiting = usbVcpAvailable,
        .serialTotalTxFree = usbTxBytesFree,
        .serialRead = usbVcpRead,
        .serialSetBaudRate = usbVcpSetBaudRate,
        .isSerialTransmitBufferEmpty = isUsbVcpTransmitBufferEmpty,
        .setMode = usbVcpSetMode,
        .setCtrlLineStateCb = usbVcpSetCtrlLineStateCb,
        .setBaudRateCb = usbVcpSetBaudRateCb,
        .writeBuf = usbVcpWriteBuf,
        .beginWrite = usbVcpBeginWrite,
        .endWrite = usbVcpEndWrite
    }
};

void usbVcpInit(void)
{
    // broches USB de la STM32F405, comme la vraie carte
    IOInit(IOGetByTag(IO_TAG(PA11)), OWNER_USB, 0);
    IOInit(IOGetByTag(IO_TAG(PA12)), OWNER_USB, 0);
}

serialPort_t *usbVcpOpen(void)
{
    vcpPort.port.vTable = usbVTable;
    return &vcpPort.port;
}

uint32_t usbVcpGetBaudRate(serialPort_t *instance)
{
    UNUSED(instance);
    return debitUsb;
}

uint8_t usbVcpIsConnected(void)
{
    return 1;
}

uint8_t usbVcpIsActive(void)
{
    return 1;
}

bool cdcDeviceIsMayBeActive(void)
{
    return true;
}

// mode clé USB et manette USB : absents de la carte simulée
void mscButtonInit(void)
{
}

bool mscCheckBootAndReset(void)
{
    return false;
}

uint8_t mscStart(void)
{
    return 1; // échec : la carte simulée ne devient pas une clé USB
}

bool mscCheckButton(void)
{
    return false;
}

void mscWaitForButton(void)
{
}

bool mscCheckFilesystemReady(void)
{
    return false;
}

void sendRcDataToHid(void)
{
}

void mscSetActive(void)
{
}

void mscActivityLed(void)
{
}
