#ifndef VVTOR_H
#define VVTOR_H
/* 
 * vtor simplified and adapted for the experienced C developer
 *
 * original vtor: https://github.com/Denzy7/dengine/tree/main/lib/dengine-utils/include/dengine-utils/vtor.h
 */

#ifdef _cplusplus
extern "C" {
#endif

/* pass your vvtor** reference here. free(vvtor**) when done */
int vvtor_init(void*** vvtor);
/* pass your vvtor** reference here and the value to pushback. when freeing vtor, remember to free values inside if they were heap allocated*/
int vvtor_push(void*** vvtor, const void* value);

#ifdef _cplusplus
}
#endif

#endif

/*#define VVTOR_IMPLEMENTATION*/
#ifdef VVTOR_IMPLEMENTATION
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
int vvtor_init(void*** vvtor)
{
    char** cptr = NULL;
    *vvtor = malloc(4 * sizeof(void*));
    cptr = (char**)*vvtor;
    cptr[0] = NULL;
    cptr[1] = NULL;
    cptr[2] = NULL;
    cptr[3] = (void*)SIZE_MAX;;
    return 1;
}
int vvtor_push(void*** vvtor, const void* value)
{
    const char** cptr = (const char**)*vvtor;
    size_t i = 0;

    while(cptr[i] && i < SIZE_MAX)
    {
        i++;
    }
    cptr[i] = value;
    if(cptr[i + 1] == (void*)SIZE_MAX)
    {
        size_t in = 2 * (i + 1);
        *vvtor = realloc(*vvtor, in * sizeof(void*));
        cptr = (const char**)*vvtor;
        i++;
        while(i < in)
        {
            cptr[i] = NULL;
            i++;
        }
        cptr[in - 1] = (void*)SIZE_MAX;
    }
    return 1;
}
#endif
