#include "../gnuboy/defs.h"
#include "../gnuboy/pcm.h"
#include "../gnuboy/rc.h"
#include <string.h>
#include <libdragon.h>

struct pcm pcm;

static byte buf[512];
static int16_t upsampled[sizeof(buf)];


rcvar_t pcm_exports[] =
{
	RCV_END
};
//#define SAMPLE_RATE (44100)
#define SAMPLE_RATE (11025)

void convert_u8_to_s16(const uint8_t *input, int16_t *output, size_t length) {
    for (size_t i = 0; i < length; i++) {
        output[i] = ((int16_t)input[i] - 128) << 8;
    }
}

void pcm_init()
{
	pcm.hz = SAMPLE_RATE;
	pcm.buf = buf;
	pcm.len = sizeof buf;
	pcm.pos = 0;
	pcm.stereo = 1;
	audio_init(pcm.hz, 50);
}

void pcm_close()
{
	memset(&pcm, 0, sizeof pcm);
}

int pcm_submit()
{
	//return 0;
	if (!pcm.buf) return 0;
	if (pcm.pos < pcm.len) return 1;
	size_t numFrames = sizeof(buf) / (pcm.stereo + 1);
	convert_u8_to_s16(pcm.buf, upsampled, sizeof(buf));
	audio_push(upsampled, numFrames, true);
	pcm.pos = 0;
	// returning 0 will cause the emu to call sys_sleep() for the remainder of the frame
	// returning 1 will not cause that (assumes it syncs via audio)
	//return 1;
	return 0;
}





