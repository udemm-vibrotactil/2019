// Reads raw PCM file format and prints the samples in signed integer format on the standard output.
// Initial version A, Deyan Levski, deyan.levski@gmail.com, 15.07.2014
// Pensando para leer PCM 16 bits LE
//

#include stdio.h
#include stdlib.h
#include math.h
#include string.h

#define SAMPLE_RATE 44100 // Hz
#define BITS_PER_SAMPLE 16 // bits
#define N_SAMPLES 5000 // n

int convertBitSize(unsigned int in, int bps)
{
        const unsigned int max = (1 << (bps-1)) - 1;
        return in > max ? in - (max<<1) : in;
}

int readPCM(int *data, unsigned int samp_rate, unsigned int bits_per_samp, unsigned int num_samp)

{
	FILE *fp;
	unsigned char buf;
	unsigned int i, j;

	fp = fopen("ttyburst.pcm", "r");

        for (i=0; i < num_samp; ++i) {

		unsigned int tmp = 0;

		for (j=0; j != bits_per_samp; j+=8) {
			fread(&buf, 1, 1, fp);
			tmp += buf << j;
		}

		data[i] = convertBitSize(tmp, bits_per_samp);

        }
}

int main(void)
{

	printf("SAMPLE RATE = [%d] BITS PER SAMPLE = [%d] N SAMPLES = [%d]\n", SAMPLE_RATE, BITS_PER_SAMPLE, N_SAMPLES);

	int *data = (int *) malloc(N_SAMPLES * sizeof(int));

        readPCM(data, SAMPLE_RATE, BITS_PER_SAMPLE, N_SAMPLES);

	unsigned int i;
	for (i = 0; i < N_SAMPLES; ++i) {
		printf("%d\n", data[i]);
	}

return 0;

}
