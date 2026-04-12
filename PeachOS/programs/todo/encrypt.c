#include "todo.h"
#include "string.h"

// Applies repeating-key XOR in-place; same routine encrypts and decrypts.
void xor_crypt_buffer(char* data, int len, const char* key)
{
    // Clamp key length lookup to avoid walking unbounded memory.
    int key_len = strnlen(key, 64);
    if (key_len <= 0)
    {
        return;
    }

    // Cycle through key bytes while processing the full data buffer.
    for (int i = 0; i < len; i++)
    {
        data[i] = data[i] ^ key[i % key_len];
    }
}