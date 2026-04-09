#include "todo.h"
#include "string.h"

void xor_crypt_buffer(char* data, int len, const char* key)
{
    int key_len = strnlen(key, 64);
    if (key_len <= 0)
    {
        return;
    }

    for (int i = 0; i < len; i++)
    {
        data[i] = data[i] ^ key[i % key_len];
    }
}