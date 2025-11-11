#ifndef DXATC_ENGINE_STDPHRASE_H
#define DXATC_ENGINE_STDPHRASE_H

#include "dxatc-utils/macros.h"

#include <stddef.h>

typedef enum{
    DXATC_ENGINE_STDPHRASE_FLAG_NONE = 0,
    DXATC_ENGINE_STDPHRASE_FLAG_PHONETIC_NUMBERS = 1 << 0,
    DXATC_ENGINE_STDPHRASE_FLAG_PHONETIC_ALPHABET = 1 << 1,
    DXATC_ENGINE_STDPHRASE_FLAG_DAYCIMAL = 1 << 2,
    DXATC_ENGINE_STDPHRASE_FLAG_TOUSAND = 1 << 3,
}DxAtcStdPhraseFlag;

typedef struct
{
    char token[1024];
    struct DxAtcStdPhraseToken** subtokens;
    size_t subtokens_count;
}DxAtcStdPhraseToken;

DXATC_UTILS_MACROS_CDECLS_BEGIN

int dxAtcStdPhraseTokens(
        const char* str,
        DxAtcStdPhraseToken*** tokens,
        size_t* tokens_count);

void dxAtcStdPhraseTokensFree(
        DxAtcStdPhraseToken** tokens);

int dxAtcStdPhrase(
        const char* string,
        char* stdphrase, const size_t stdphrase_sz, DxAtcStdPhraseFlag flags);

DXATC_UTILS_MACROS_CDECLS_END
#endif
