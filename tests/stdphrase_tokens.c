#include <dxatc-engine/stdphrase.h>
#include <stdio.h>
int main(int argc, char* argv[])
{
    const char* str = NULL;
    DxAtcStdPhraseToken** tokens;
    DxAtcStdPhraseToken* token, *subtoken;

    size_t tokens_count = 0;
    if(argc > 1)
        str = argv[1];
    if(str == NULL)
        str = "Test Airlines 1234. Cleared for the ILS Runway 12L. Contact Tower on 123.45. See ya!";

    dxAtcStdPhraseTokens(str, &tokens, &tokens_count);
    printf("str: %s\n", str);
    for(size_t i = 0; i < tokens_count; i++)
    {
        token = tokens[i];
        printf("tok: %s\n", token->token);
        for(size_t j = 0; j < token->subtokens_count; j++)
        {
            subtoken = (DxAtcStdPhraseToken*)token->subtokens[j];
            printf("\t%s\n", subtoken->token);
        }
    }
    dxAtcStdPhraseTokensFree(tokens);
}
