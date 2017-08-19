#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/videodev2.h>
#include <linux/vmalloc.h>
#include <linux/wait.h>
#include <asm/atomic.h>
#include <asm/unaligned.h>

#include <media/v4l2-common.h>

static const char *get_guid(const unsigned char *buf)
{
	static char guid[39];

	/* NOTE:  see RFC 4122 for more information about GUID/UUID
	 * structure.  The first fields fields are historically big
	 * endian numbers, dating from Apollo mc68000 workstations.
	 */
	sprintf(guid, "{%02x%02x%02x%02x"
			"-%02x%02x"
			"-%02x%02x"
			"-%02x%02x"
			"-%02x%02x%02x%02x%02x%02x}",
	       buf[0], buf[1], buf[2], buf[3],
	       buf[4], buf[5],
	       buf[6], buf[7],
	       buf[8], buf[9],
	       buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
	return guid;
}

static void parse_videocontrol_interface(struct usb_interface *intf, unsigned char *buf, int buflen)
{
    static const char * const ctrlnames[] = {
        "Brightness", "Contrast", "Hue", "Saturation", "Sharpness", "Gamma",
        "White Balance Temperature", "White Balance Component", "Backlight Compensation",
        "Gain", "Power Line Frequency", "Hue, Auto", "White Balance Temperature, Auto",
        "White Balance Component, Auto", "Digital Multiplier", "Digital Multiplier Limit",
        "Analog Video Standard", "Analog Video Lock Status"
    };
    static const char * const camctrlnames[] = {
        "Scanning Mode", "Auto-Exposure Mode", "Auto-Exposure Priority",
        "Exposure Time (Absolute)", "Exposure Time (Relative)", "Focus (Absolute)",
        "Focus (Relative)", "Iris (Absolute)", "Iris (Relative)", "Zoom (Absolute)",
        "Zoom (Relative)", "PanTilt (Absolute)", "PanTilt (Relative)",
        "Roll (Absolute)", "Roll (Relative)", "Reserved", "Reserved", "Focus, Auto",
        "Privacy"
    };
    static const char * const stdnames[] = {
        "None", "NTSC - 525/60", "PAL - 625/50", "SECAM - 625/50",
        "NTSC - 625/50", "PAL - 525/60" };
    unsigned int i, ctrls, stds, n, p, termt, freq;

    while (buflen > 0)
    {

        if (buf[1] != USB_DT_CS_INTERFACE)
            printk("      Warning: Invalid descriptor\n");
        else if (buf[0] < 3)
            printk("      Warning: Descriptor too short\n");
        printk("      VideoControl Interface Descriptor:\n"
               "        bLength             %5u\n"
               "        bDescriptorType     %5u\n"
               "        bDescriptorSubtype  %5u ",
               buf[0], buf[1], buf[2]);
        switch (buf[2]) {
        case 0x01:  /* HEADER */
            printk("(HEADER)\n");
            n = buf[11];
            if (buf[0] < 12+n)
                printk("      Warning: Descriptor too short\n");
            freq = buf[7] | (buf[8] << 8) | (buf[9] << 16) | (buf[10] << 24);
            printk("        bcdUVC              %2x.%02x\n"
                   "        wTotalLength        %5u\n"
                   "        dwClockFrequency    %5u.%06uMHz\n"
                   "        bInCollection       %5u\n",
                   buf[4], buf[3], buf[5] | (buf[6] << 8), freq / 1000000,
                   freq % 1000000, n);
            for (i = 0; i < n; i++)
                printk("        baInterfaceNr(%2u)   %5u\n", i, buf[12+i]);
            break;

        case 0x02:  /* INPUT_TERMINAL */
            printk("(INPUT_TERMINAL)\n");
            termt = buf[4] | (buf[5] << 8);
            n = termt == 0x0201 ? 7 : 0;
            if (buf[0] < 8 + n)
                printk("      Warning: Descriptor too short\n");
            printk("        bTerminalID         %5u\n"
                   "        wTerminalType      0x%04x\n"
                   "        bAssocTerminal      %5u\n",
                   buf[3], termt, buf[6]);
            printk("        iTerminal           %5u\n",
                   buf[7]);
            if (termt == 0x0201) {
                n += buf[14];
                printk("        wObjectiveFocalLengthMin  %5u\n"
                       "        wObjectiveFocalLengthMax  %5u\n"
                       "        wOcularFocalLength        %5u\n"
                       "        bControlSize              %5u\n",
                       buf[8] | (buf[9] << 8), buf[10] | (buf[11] << 8),
                       buf[12] | (buf[13] << 8), buf[14]);
                ctrls = 0;
                for (i = 0; i < 3 && i < buf[14]; i++)
                    ctrls = (ctrls << 8) | buf[8+n-i-1];
                printk("        bmControls           0x%08x\n", ctrls);
                for (i = 0; i < 19; i++)
                    if ((ctrls >> i) & 1)
                        printk("          %s\n", camctrlnames[i]);
            }
            break;

        case 0x03:  /* OUTPUT_TERMINAL */
            printk("(OUTPUT_TERMINAL)\n");
            termt = buf[4] | (buf[5] << 8);
            if (buf[0] < 9)
                printk("      Warning: Descriptor too short\n");
            printk("        bTerminalID         %5u\n"
                   "        wTerminalType      0x%04x\n"
                   "        bAssocTerminal      %5u\n"
                   "        bSourceID           %5u\n"
                   "        iTerminal           %5u\n",
                   buf[3], termt, buf[6], buf[7], buf[8]);
            break;

        case 0x04:  /* SELECTOR_UNIT */
            printk("(SELECTOR_UNIT)\n");
            p = buf[4];
            if (buf[0] < 6+p)
                printk("      Warning: Descriptor too short\n");

            printk("        bUnitID             %5u\n"
                   "        bNrInPins           %5u\n",
                   buf[3], p);
            for (i = 0; i < p; i++)
                printk("        baSource(%2u)        %5u\n", i, buf[5+i]);
            printk("        iSelector           %5u\n",
                   buf[5+p]);
            break;

        case 0x05:  /* PROCESSING_UNIT */
            printk("(PROCESSING_UNIT)\n");
            n = buf[7];
            if (buf[0] < 10+n)
                printk("      Warning: Descriptor too short\n");
            printk("        bUnitID             %5u\n"
                   "        bSourceID           %5u\n"
                   "        wMaxMultiplier      %5u\n"
                   "        bControlSize        %5u\n",
                   buf[3], buf[4], buf[5] | (buf[6] << 8), n);
            ctrls = 0;
            for (i = 0; i < 3 && i < n; i++)
                ctrls = (ctrls << 8) | buf[8+n-i-1];
            printk("        bmControls     0x%08x\n", ctrls);
            for (i = 0; i < 18; i++)
                if ((ctrls >> i) & 1)
                    printk("          %s\n", ctrlnames[i]);
            stds = buf[9+n];
            printk("        iProcessing         %5u\n"
                   "        bmVideoStandards     0x%2x\n", buf[8+n], stds);
            for (i = 0; i < 6; i++)
                if ((stds >> i) & 1)
                    printk("          %s\n", stdnames[i]);
            break;

        case 0x06:  /* EXTENSION_UNIT */
            printk("(EXTENSION_UNIT)\n");
            p = buf[21];
            n = buf[22+p];
            if (buf[0] < 24+p+n)
                printk("      Warning: Descriptor too short\n");
            printk("        bUnitID             %5u\n"
                   "        guidExtensionCode         %s\n"
                   "        bNumControl         %5u\n"
                   "        bNrPins             %5u\n",
                   buf[3], get_guid(&buf[4]), buf[20], buf[21]);
            for (i = 0; i < p; i++)
                printk("        baSourceID(%2u)      %5u\n", i, buf[22+i]);
            printk("        bControlSize        %5u\n", buf[22+p]);
            for (i = 0; i < n; i++)
                printk("        bmControls(%2u)       0x%02x\n", i, buf[23+p+i]);
            printk("        iExtension          %5u\n",
                   buf[23+p+n]);
            break;

        default:
            printk("(unknown)\n"
                   "        Invalid desc subtype:");
            break;
        }

        buflen -= buf[0];
        buf    += buf[0];
    }
}


static void parse_videostreaming_interface(struct usb_interface *intf, unsigned char *buf, int buflen)
{
}

static int myuvc_probe(struct usb_interface *intf,
		     const struct usb_device_id *id)
{
    static int cnt = 0;
	struct usb_device *dev = interface_to_usbdev(intf);
    struct usb_device_descriptor *descriptor = &dev->descriptor;
    struct usb_host_config *hostconfig;
    struct usb_config_descriptor *config;
	struct usb_interface_assoc_descriptor *assoc_desc;
    struct usb_interface_descriptor	*interface;
    int i, j, k, l;
	unsigned char *buffer;
	int buflen;
    int desc_len;
    int desc_cnt;

    printk("myuvc_probe : cnt = %d\n", cnt++);

    /* ¥Ú”°…Ë±∏√Ë ˆ∑˚ */
	printk("Device Descriptor:\n"
	       "  bLength             %5u\n"
	       "  bDescriptorType     %5u\n"
	       "  bcdUSB              %2x.%02x\n"
	       "  bDeviceClass        %5u \n"
	       "  bDeviceSubClass     %5u \n"
	       "  bDeviceProtocol     %5u \n"
	       "  bMaxPacketSize0     %5u\n"
	       "  idVendor           0x%04x \n"
	       "  idProduct          0x%04x \n"
	       "  bcdDevice           %2x.%02x\n"
	       "  iManufacturer       %5u\n"
	       "  iProduct            %5u\n"
	       "  iSerial             %5u\n"
	       "  bNumConfigurations  %5u\n",
	       descriptor->bLength, descriptor->bDescriptorType,
	       descriptor->bcdUSB >> 8, descriptor->bcdUSB & 0xff,
	       descriptor->bDeviceClass, 
	       descriptor->bDeviceSubClass,
	       descriptor->bDeviceProtocol, 
	       descriptor->bMaxPacketSize0,
	       descriptor->idVendor,  descriptor->idProduct,
	       descriptor->bcdDevice >> 8, descriptor->bcdDevice & 0xff,
	       descriptor->iManufacturer, 
	       descriptor->iProduct, 
	       descriptor->iSerialNumber, 
	       descriptor->bNumConfigurations);

    for (i = 0; i < descriptor->bNumConfigurations; i++)
    {
        hostconfig = &dev->config[i];
        config     = &hostconfig->desc;
        printk("  Configuration Descriptor %d:\n"
               "    bLength             %5u\n"
               "    bDescriptorType     %5u\n"
               "    wTotalLength        %5u\n"
               "    bNumInterfaces      %5u\n"
               "    bConfigurationValue %5u\n"
               "    iConfiguration      %5u\n"
               "    bmAttributes         0x%02x\n",
               i, 
               config->bLength, config->bDescriptorType,
               le16_to_cpu(config->wTotalLength),
               config->bNumInterfaces, config->bConfigurationValue,
               config->iConfiguration,
               config->bmAttributes);

        assoc_desc = hostconfig->intf_assoc[0];
        printk("    Interface Association:\n"
               "      bLength             %5u\n"
               "      bDescriptorType     %5u\n"
               "      bFirstInterface     %5u\n"
               "      bInterfaceCount     %5u\n"
               "      bFunctionClass      %5u\n"
               "      bFunctionSubClass   %5u\n"
               "      bFunctionProtocol   %5u\n"
               "      iFunction           %5u\n",
            assoc_desc->bLength,
            assoc_desc->bDescriptorType,
            assoc_desc->bFirstInterface,
            assoc_desc->bInterfaceCount,
            assoc_desc->bFunctionClass,
            assoc_desc->bFunctionSubClass,
            assoc_desc->bFunctionProtocol,
            assoc_desc->iFunction);    

            for (j = 0; j < intf->num_altsetting; j++)
            {
                interface = &intf->altsetting[j].desc;
                printk("    Interface Descriptor altsetting %d:\n"
                       "      bLength             %5u\n"
                       "      bDescriptorType     %5u\n"
                       "      bInterfaceNumber    %5u\n"
                       "      bAlternateSetting   %5u\n"
                       "      bNumEndpoints       %5u\n"
                       "      bInterfaceClass     %5u\n"
                       "      bInterfaceSubClass  %5u\n"
                       "      bInterfaceProtocol  %5u\n"
                       "      iInterface          %5u\n",
                       j, 
                       interface->bLength, interface->bDescriptorType, interface->bInterfaceNumber,
                       interface->bAlternateSetting, interface->bNumEndpoints, interface->bInterfaceClass,
                       interface->bInterfaceSubClass, interface->bInterfaceProtocol,
                       interface->iInterface);
            }

            buffer = intf->cur_altsetting->extra;
            buflen = intf->cur_altsetting->extralen;
            printk("extra buffer of interface %d:\n", cnt-1);
            k = 0;
            desc_cnt = 0;
            while (k < buflen)
            {
                desc_len = buffer[k];
                printk("extra desc %d: ", desc_cnt);
                for (l = 0; l < desc_len; l++, k++)
                {
                    printk("%02x ", buffer[k]);
                }
                desc_cnt++;
                printk("\n");
            }

            interface = &intf->cur_altsetting->desc;
            if ((buffer[1] == USB_DT_CS_INTERFACE) && (interface->bInterfaceSubClass == 1))
            {
                parse_videocontrol_interface(intf, buffer, buflen);
            }
            if ((buffer[1] == USB_DT_CS_INTERFACE) && (interface->bInterfaceSubClass == 2))
            {
                parse_videostreaming_interface(intf, buffer, buflen);
            }
        
    }
    
    
    return 0;
}

static void myuvc_disconnect(struct usb_interface *intf)
{
    static int cnt = 0;
    printk("myuvc_disconnect : cnt = %d\n", cnt++);
}

static struct usb_device_id myuvc_ids[] = {
	/* Generic USB Video Class */
	{ USB_INTERFACE_INFO(USB_CLASS_VIDEO, 1, 0) },  /* VideoControl Interface */
    { USB_INTERFACE_INFO(USB_CLASS_VIDEO, 2, 0) },  /* VideoStreaming Interface */
	{}
};

/* 1. ∑÷≈‰usb_driver */
/* 2. …Ë÷√ */
static struct usb_driver myuvc_driver = {
    .name       = "myuvc",
    .probe      = myuvc_probe,
    .disconnect = myuvc_disconnect,
    .id_table   = myuvc_ids,
};

static int myuvc_init(void)
{
    /* 3. ◊¢≤· */
    usb_register(&myuvc_driver);
    return 0;
}

static void myuvc_exit(void)
{
    usb_deregister(&myuvc_driver);
}

module_init(myuvc_init);
module_exit(myuvc_exit);

MODULE_LICENSE("GPL");



