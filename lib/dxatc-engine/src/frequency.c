#include "dxatc-engine/frequency.h"

#include <stdio.h>

int dxAtcFrequencyFormat(DxAtcFrequencyFormatted formatted, const DxAtcFrequency frequency)
{
    int mhz, khz;
    dxAtcFrequencySplit(&mhz, &khz, frequency);
    return snprintf(formatted, sizeof(DxAtcFrequencyFormatted), "%d.%s%s%d", mhz, khz < 100 ? "0" : "", khz < 10 ? "0" : "", khz);
}
int dxAtcFrequencySplit(int* mhz, int* khz, const DxAtcFrequency frequency)
{
    char mhz_s[4], khz_s[4];
    mhz_s[3] = 0;
    khz_s[3] = 0;

    sscanf(frequency, "%3c%3c", mhz_s, khz_s);

    sscanf(mhz_s, "%d", mhz);
    sscanf(khz_s, "%d", khz);

    return 1;
}
