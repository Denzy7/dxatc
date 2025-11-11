#include "dxatc-engine/stdphrase.h"

#include "dxatc-utils/vvtor.h"
#include "dxatc-utils/str.h"
#include "dxatc-engine/frequency.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum
{
    _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_NONE = 0,
    _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_RUNWAY = 1,
    _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_FREQUENCY = 2,
    _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_TIME = 1 << 2,
    _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_ALTITUDE = 1 << 3,
    _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_FLIGHTLEVEL = 1 << 4,
    _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_CLOUDS = 1 << 5,
}_DxAtcStdPhraseContextFlag;

static const char* alphabet[] = {
    "Alpha",
    "Bravo",
    "Charlie",
    "Delta",
    "Echo",
    "Foxtrot",
    "Golf",
    "Hotel",
    "India",
    "Juliet",
    "Kilo",
    "Lima",
    "Mike",
    "November",
    "Oscar",
    "Papa",
    "Quebec",
    "Romeo",
    "Sierra",
    "Tango",
    "Uniform",
    "Victor",
    "Whiskey",
    "Xray",
    "Yankee",
    "Zulu"
};

static const char* const numbermap[] =
{
    "Zero",
    "One",
    "Two",
    "Three",
    "Four",
    "Five",
    "Six",
    "Seven",
    "Eight",
    "Nine"
};

static const char*const numbermap_phonetic[] =
{
    "Zero",
    "Wan",
    "Two",
    "Tree",
    "Fower",
    "Five",
    "Six",
    "Seven",
    "Ayt",
    "Niner",
};

static const char*const asis[] = {
    "VOR",
    "ILS",
    "NDB",
    "QNH"
};

char* _dxAtcStdPhraseGetString(const char* str, char* buf, size_t bufsz, size_t* offset)
{
    size_t len = strlen(str + *offset);
    size_t n = 0;

    if(str[*offset] == 0)
        return NULL;

    for(size_t i = 0; i < len; i++)
    {
        n++;
        if(str[i + *offset] == '.' && str[i + *offset + 1] == ' '){
            break;
        }
    }

    if(n > bufsz)
        return NULL;

    memcpy(buf, str + *offset, n);
    *offset = *offset + n;

    buf[n] = 0;

    /* queue next sentence and offset the whitespace at start */
    if(buf[n - 1] == '.'){
        buf[n - 1] = 0;
        *offset = *offset + 1;
    }

    return buf;
}

const char* _dxAtcStdPhraseCharToPhoneticNumber(const char c, DxAtcStdPhraseFlag flags)
{
    const char*const* usenumbermap = numbermap;
    if(flags & DXATC_ENGINE_STDPHRASE_FLAG_PHONETIC_NUMBERS)
        usenumbermap = numbermap_phonetic;
    return usenumbermap[c - 48];
}

const char* _dxAtcStdPhraseCharToPhoneticAlphabet(const char c, DxAtcStdPhraseFlag flags)
{
    if(flags & DXATC_ENGINE_STDPHRASE_FLAG_PHONETIC_ALPHABET)
    {
    }
    return alphabet[c - 65];
}

int _dxAtcStdPhraseWriteToken(char* buffer, size_t bufsz, size_t* offset, const DxAtcStdPhraseToken* token, DxAtcStdPhraseFlag flags, _DxAtcStdPhraseContextFlag ctxflags, int lasttok)
{
    char stdphrase[2048];
    char singlechar[2];
    size_t stdphrase_off = 0;

    DxAtcFrequency freq;
    DxAtcFrequencyFormatted freq_fmted;
    char rwyloc[8];
    int leaveasis = 0;
    size_t toklen = 0;
    int isdigit_1st = 0, isdigit_2nd = 0, isupper_1st = 0, isupper_2nd = 0;
    int hadnum = 0, hadsinglechar = 0;
    int alt = 0, alt_tousand = 0, alt_tousand_tens = 0, alt_tousand_ones = 0,alt_hundreds = 0;

    toklen = strlen(token->token);
    memset(freq, 0, sizeof(freq));
    memset(freq_fmted, 0, sizeof(freq_fmted));
    memset(rwyloc, 0, sizeof(rwyloc));

    isdigit_1st = isdigit(token->token[0]);
    isdigit_2nd = isdigit(token->token[1]);
    isupper_1st = isupper(token->token[0]);
    isupper_2nd = isupper(token->token[1]);

    for(size_t i = 0; i < DXATC_UTILS_MACROS_ARYSZ(asis); i++)
    {
        if(!strcmp(token->token, asis[i]))
        {
            leaveasis = 1;
            break;
        }
    }

    snprintf(stdphrase, sizeof(stdphrase), "%s", token->token);

    if(isdigit_1st)
    {
        memset(stdphrase, 0, sizeof(stdphrase));
        stdphrase_off = 0;
        for(size_t i = 0; i < toklen; i++)
        {
            if(isdigit(token->token[i]))
                dxAtcStrApp(stdphrase, sizeof(stdphrase), &stdphrase_off, _dxAtcStdPhraseCharToPhoneticNumber(token->token[i], flags));
            else
                dxAtcStrApp(stdphrase, sizeof(stdphrase), &stdphrase_off, _dxAtcStdPhraseCharToPhoneticAlphabet(token->token[i], flags));

            if(i != toklen - 1)
                dxAtcStrApp(stdphrase, sizeof(stdphrase), &stdphrase_off, " ");
        }
        if(ctxflags & _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_RUNWAY)
        {
            if(token->token[2] == 'L')
                snprintf(rwyloc, sizeof(rwyloc), " Left");
            else if(token->token[2] == 'R')
                snprintf(rwyloc, sizeof(rwyloc), " Right");
            else if(token->token[2] == 'C')
                snprintf(rwyloc, sizeof(rwyloc), " Center");

            snprintf(stdphrase, sizeof(stdphrase),
                    "%s %s%s",
                    _dxAtcStdPhraseCharToPhoneticNumber(token->token[0], flags),
                    _dxAtcStdPhraseCharToPhoneticNumber(token->token[1], flags),
                    rwyloc);

        }
        if(ctxflags & _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_FREQUENCY)
        {
            dxAtcFrequencyParse(token->token, freq);
            dxAtcFrequencyFormat(freq_fmted, freq);
            snprintf(stdphrase, sizeof(stdphrase),
                    "%s %s %s %s %s %s %s",
                    _dxAtcStdPhraseCharToPhoneticNumber(freq_fmted[0], flags),
                    _dxAtcStdPhraseCharToPhoneticNumber(freq_fmted[1], flags),
                    _dxAtcStdPhraseCharToPhoneticNumber(freq_fmted[2], flags),
                    flags & DXATC_ENGINE_STDPHRASE_FLAG_DAYCIMAL ? "daycimal" : "decimal",
                    _dxAtcStdPhraseCharToPhoneticNumber(freq_fmted[4], flags),
                    freq_fmted[5] > 48 || freq_fmted[6] > 48  ? _dxAtcStdPhraseCharToPhoneticNumber(freq_fmted[5], flags) : "",
                    freq_fmted[6] > 48 ? _dxAtcStdPhraseCharToPhoneticNumber(freq_fmted[6], flags) : ""
                    );
        }
        if((ctxflags & _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_ALTITUDE) || (ctxflags & _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_CLOUDS))
        {
            memset(stdphrase, 0, sizeof(stdphrase));
            stdphrase_off = 0;
            sscanf(token->token, "%d", &alt);
            alt_tousand = alt / 1000;
            alt_tousand_tens = alt_tousand / 10;
            alt_tousand_ones = alt_tousand - (alt_tousand_tens * 10);
            alt_hundreds = (alt - (alt_tousand * 1000)) / 100;
            if(alt_tousand_tens || alt_tousand_ones)
            {
                if(alt_tousand_tens)
                    dxAtcStrApp(stdphrase, sizeof(stdphrase), &stdphrase_off, "%s", _dxAtcStdPhraseCharToPhoneticNumber(alt_tousand_tens + 48, flags));

                dxAtcStrApp(stdphrase, sizeof(stdphrase), &stdphrase_off, "%s%s ", alt_tousand_tens ? " " : "", _dxAtcStdPhraseCharToPhoneticNumber(alt_tousand_ones + 48, flags));
                dxAtcStrApp(stdphrase, sizeof(stdphrase), &stdphrase_off, "%s%s",
                        flags & DXATC_ENGINE_STDPHRASE_FLAG_TOUSAND ? "tousand" : "thousand",
                        alt_hundreds ? " " : "");
            }
            if(alt_hundreds)
            {
                dxAtcStrApp(stdphrase, sizeof(stdphrase), &stdphrase_off, "%s hundred",
                        _dxAtcStdPhraseCharToPhoneticNumber(alt_hundreds + 48, flags));
            }
        }

    }

    if(((isupper_1st && isupper_2nd) || (isupper_1st && isdigit_2nd) ) && !leaveasis)
    {
        memset(stdphrase, 0, sizeof(stdphrase));
        stdphrase_off = 0;
        for(size_t i = 0; i < toklen; i++)
        {
            snprintf(singlechar, sizeof(singlechar), "%c", token->token[i]);
            if(isalpha(token->token[i]))
            {
                if((flags & DXATC_ENGINE_STDPHRASE_FLAG_PHONETIC_ALPHABET) || hadnum || isdigit_2nd)
                {
                    dxAtcStrApp(stdphrase, sizeof(stdphrase), &stdphrase_off,
                            _dxAtcStdPhraseCharToPhoneticAlphabet(singlechar[0], flags));
                    if(i != toklen - 1)
                        dxAtcStrApp(stdphrase, sizeof(stdphrase), &stdphrase_off, " ");
                }else {
                    dxAtcStrApp(stdphrase, sizeof(stdphrase), &stdphrase_off, singlechar);
                    hadsinglechar = 1;
                }
            }else {
                if(hadsinglechar)
                {
                    dxAtcStrApp(stdphrase, sizeof(stdphrase), &stdphrase_off, " ");
                    hadsinglechar = 0;
                }
                dxAtcStrApp(stdphrase, sizeof(stdphrase), &stdphrase_off,
                        _dxAtcStdPhraseCharToPhoneticNumber(singlechar[0], flags)
                        );
                hadnum = 1;
                if(i != toklen - 1)
                    dxAtcStrApp(stdphrase, sizeof(stdphrase), &stdphrase_off, " ");
            }
        }
    }
    if((ctxflags & _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_FLIGHTLEVEL) && token->token[0] == 'F' && token->token[1] == 'L')
    {
        memset(stdphrase, 0, sizeof(stdphrase));
        stdphrase_off = 0;
        dxAtcStrApp(stdphrase, sizeof(stdphrase), &stdphrase_off,"flight level ");
        for(size_t i = 0; i < 3; i++)
        {
            dxAtcStrApp(stdphrase, sizeof(stdphrase), &stdphrase_off,"%s",
                    _dxAtcStdPhraseCharToPhoneticNumber(token->token[2 + i], flags));
            if(i < 2)
                dxAtcStrApp(stdphrase, sizeof(stdphrase), &stdphrase_off," ");
        }

    }

    dxAtcStrApp(buffer, bufsz, offset, stdphrase);
    if(!lasttok)
        dxAtcStrApp(buffer, bufsz, offset, " ");

    return 1;
}

int dxAtcStdPhrase(const char* string, char* stdphrase, const size_t stdphrase_sz, DxAtcStdPhraseFlag flags)
{
    DxAtcStdPhraseToken** tokens;
    size_t tokens_count;
    DxAtcStdPhraseToken* tok, *subtok;
    _DxAtcStdPhraseContextFlag ctxflags = 0;
    size_t offset = 0;
    const char* substr;

    dxAtcStdPhraseTokens(string, &tokens, &tokens_count);
    for(size_t i = 0; i < tokens_count; i++)
    {
        ctxflags = 0;
        tok = tokens[i];

        if(dxAtcStrStr(tok->token, "runway", 1))
            ctxflags |= _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_RUNWAY;
        if(dxAtcStrStr(tok->token, "zulu", 1) || dxAtcStrStr(tok->token, "at time", 1))
            ctxflags |= _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_TIME;
        if(dxAtcStrStr(tok->token, "and maintain", 1) || dxAtcStrStr(tok->token, "altitude", 1))
            ctxflags |= _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_ALTITUDE;
        if(dxAtcStrStr(tok->token, "contact ", 1) && (tok->token[0] == 'C' || tok->token[0] == 'c') )
            ctxflags |= _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_FREQUENCY;
        if(dxAtcStrStr(tok->token, "clouds", 1))
            ctxflags |= _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_CLOUDS;
        substr = dxAtcStrStr(tok->token, "FL", 0);
        if(substr && isdigit(substr[2]))
            ctxflags |= _DXATC_ENGINE_STDPHRASE_CONTEXT_FLAG_FLIGHTLEVEL;

        /*printf("tok %s\n", tok->token);*/
        for(size_t j = 0; j < tok->subtokens_count; j++)
        {
            subtok = (DxAtcStdPhraseToken*)tok->subtokens[j];
            _dxAtcStdPhraseWriteToken(stdphrase, stdphrase_sz, &offset, subtok, flags, ctxflags, j == tok->subtokens_count - 1);
            /*printf("subtok %s\n", subtok->token);*/
        }
        dxAtcStrApp(stdphrase, stdphrase_sz, &offset, ". ");
    }
    dxAtcStdPhraseTokensFree(tokens);
    return 1;
}
void dxAtcStdPhraseTokensFree(
        DxAtcStdPhraseToken** tokens)
{
    DxAtcStdPhraseToken* token, *subtok;
    size_t i = 0, j = 0;;
    while((token = tokens[i]))
    {
        j = 0;
        while((subtok = (DxAtcStdPhraseToken*)token->subtokens[j]))
        {
            free(subtok);
            j++;
        }
        free(token->subtokens);
        free(token);
        i++;
    }
    free(tokens);
}
int dxAtcStdPhraseTokens(
        const char* str,
        DxAtcStdPhraseToken*** tokens,
        size_t* tokens_count)
{
    char linebf[1024];
    size_t offset = 0;
    const char* tok;
    char* tokstr, *tokbuf;
    DxAtcStdPhraseToken stdtok, *stdtok_cpy, subtok, *subtok_cpy;

    *tokens_count = 0;
    memset(linebf, 0, sizeof(linebf));
    vvtor_init((void***)tokens);

    while(_dxAtcStdPhraseGetString(str, linebf, sizeof(linebf), &offset))
    {
        memset(&stdtok, 0, sizeof(stdtok));
        tokbuf = strdup(linebf);
        tokstr = tokbuf;
        snprintf(stdtok.token, sizeof(stdtok.token), "%s", tokbuf);

        vvtor_init((void***)&stdtok.subtokens);
        while((tok = strtok(tokstr, " ")))
        {
            tokstr = NULL;

            memset(&subtok, 0, sizeof(DxAtcStdPhraseToken));
            snprintf(subtok.token, sizeof(subtok.token), "%s", tok);
            subtok_cpy = calloc(1, sizeof(DxAtcStdPhraseToken));
            memcpy(subtok_cpy, &subtok, sizeof(DxAtcStdPhraseToken));

            vvtor_push((void***)&stdtok.subtokens, subtok_cpy);
            stdtok.subtokens_count++;
        }
        stdtok_cpy = calloc(1, sizeof(DxAtcStdPhraseToken));
        memcpy(stdtok_cpy, &stdtok, sizeof(DxAtcStdPhraseToken));
        vvtor_push((void***)tokens, stdtok_cpy);
        *tokens_count = *tokens_count + 1;
        free(tokbuf);
    }
    return 1;
}
