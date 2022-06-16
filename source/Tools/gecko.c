#include <gccore.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/iosupport.h>

#define UNUSED  __attribute__((unused))

static bool geckoinit = false;

static ssize_t __out_write(struct _reent *r UNUSED, int fd UNUSED, const char *ptr, size_t len)
{
	if(geckoinit && ptr)
	{
		u32 level;
		level = IRQ_Disable();
		usb_sendbuffer_safe(1, ptr, len);
		IRQ_Restore(level);
	}

	return len;
}

static const devoptab_t gecko_out = {
	"stdout",	// device name
	0,			// size of file structure
	NULL,		// device open
	NULL,		// device close
	__out_write,// device write
	NULL,		// device read
	NULL,		// device seek
	NULL,		// device fstat
	NULL,		// device stat
	NULL,		// device link
	NULL,		// device unlink
	NULL,		// device chdir
	NULL,		// device rename
	NULL,		// device mkdir
	0,			// dirStateSize
	NULL,		// device diropen_r
	NULL,		// device dirreset_r
	NULL,		// device dirnext_r
	NULL,		// device dirclose_r
	NULL,		// device statvfs_r
	NULL,		// device ftruncate_r
	NULL,		// device fsync_r
	NULL,		// device deviceData
	NULL,		// device chmod_r
	NULL,		// device fchmod_r
};

static void USBGeckoOutput()
{
	devoptab_list[STD_OUT] = &gecko_out;
	devoptab_list[STD_ERR] = &gecko_out;
}

bool InitGecko()
{
	if(geckoinit)
		return true;

	USBGeckoOutput();

	u32 geckoattached = usb_isgeckoalive(EXI_CHANNEL_1);
	if (geckoattached)
	{
		usb_flush(EXI_CHANNEL_1);
		geckoinit = true;
		return true;
	}
	else
		return false;
}

void gprintf(const char * format, ...)
{
	if (!geckoinit)
		return;

	char * tmp = NULL;
	va_list va;
	va_start(va, format);
	if((vasprintf(&tmp, format, va) >= 0) && tmp)
	{
		__out_write(NULL, 0, tmp, strlen(tmp));
	}
	va_end(va);

	if(tmp)
		free(tmp);
}

void gsenddata(const u8 *data, int length, const char *filename)
{
	if (!(geckoinit))return;

	// First, send a "\x1b[2B]" line (this will tell geckoreader that binary data is comming up next)
	const char *binary_data = "\x1b[2B]\n";

	__out_write(NULL, 0, binary_data, strlen(binary_data));

	u8 filenamelength = filename == NULL ? 0 : strlen(filename);

	// Send the length
	__out_write(NULL, 0, (char *) &length, 4);
	__out_write(NULL, 0, (char *) &filenamelength, 1);
	__out_write(NULL, 0, (char *) data, length);
	if (filename != NULL)
	{
		__out_write(NULL, 0, filename, strlen(filename));
	}
}

char ascii(char s)
{
	if(s < 0x20)
		return '.';
	if(s > 0x7E)
		return '.';
	return s;
}

void ghexdump(void *d, int len)
{
	u8 *data;
	int i, off;
	data = (u8*)d;

	gprintf("\n	   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  0123456789ABCDEF");
	gprintf("\n====  ===============================================  ================\n");

	for (off=0; off<len; off += 16)
	{
		gprintf("%04x  ",off);
		for(i=0; i<16; i++)
			if((i+off)>=len)
				gprintf("   ");
		else
			gprintf("%02x ",data[off+i]);

		gprintf(" ");
		for(i=0; i<16; i++)
			if((i+off)>=len)
				gprintf(" ");
			else
				gprintf("%c",ascii(data[off+i]));
		gprintf("\n");
	}
}
