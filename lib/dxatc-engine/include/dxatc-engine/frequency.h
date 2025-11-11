#ifndef DXATC_ENGINE_FREQUENCY_H
#define DXATC_ENGINE_FREQUENCY_H

#include "dxatc-utils/macros.h"

#include <stddef.h>

typedef char DxAtcFrequency[7];
typedef char DxAtcFrequencyFormatted[8];

DXATC_UTILS_MACROS_CDECLS_BEGIN

int dxAtcFrequencyFormat(DxAtcFrequencyFormatted formatted, const DxAtcFrequency frequency);

int dxAtcFrequencySplit(int* mhz, int* khz, const DxAtcFrequency frequency);

int dxAtcFrequencyParse(const char* str, DxAtcFrequency frequency);

DXATC_UTILS_MACROS_CDECLS_END


#endif
