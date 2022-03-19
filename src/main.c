#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#define ARG_FILE 1
#define MASK_FIRST_6 0x3F
#define ALL_BITS_SET 0xFF
#define IS_WIDE(c) ((int8_t)c) < 0

// TODO: Make it work with generic byte length
static uint64_t read_wide(int8_t sbyte, FILE *stream) 
{
    // Copy, first byte in sequence gets manipulated.
    uint8_t byte = sbyte;

    // Determine byte width.
    int width = 0;
    while (IS_WIDE(sbyte))
    {
        width++;
        sbyte <<= 1;
    }
    
    uint64_t utf8 = byte & (ALL_BITS_SET >> (width + 1)); 

    // First byte is already read.
    for (int i = 0; i < width - 1; i++)
    {
        byte = fgetc(stream) & MASK_FIRST_6;
        utf8 = (utf8 << 6) | byte;
    }

    return utf8;
}

static FILE *openfile(int argc, const char **argv)
{
    // Check if there exists a file path argument.
    if (argc < 2) {
        fprintf(stderr, "Invalid arguments\n");
        exit(1);
    }

    // Try opening specified file.
    FILE *file = fopen(argv[ARG_FILE], "r");
    if (!file) {
        fprintf(stderr, "Could not open file: %s", argv[ARG_FILE]);
        exit(1);
    }

    return file;
}


int main(int argc, char const *argv[])
{
    FILE *file = openfile(argc, argv);

    int8_t sbyte;
    while ((sbyte = fgetc(file)) != EOF)
    {
        fprintf(stdout, "U+");
        if (IS_WIDE(sbyte)) 
        {
            // UTF-8 encoding encountered, delegate reading.
            const uint64_t utf8_byte_seq = read_wide(sbyte, file);
            fprintf(stdout, "%lX ", utf8_byte_seq);
            
        }
        else
        {
            // Same treatment as typical ASCII.
            fprintf(stdout, "%X ", sbyte);
        }
    }
    fputc('\n', stdout);

    fclose(file);
    return 0;
}