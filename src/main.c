#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#define ARG_FILE 1
#define is_utf8(c) ((int8_t)c) < 0

/**
 * @brief Tries to read UTF-8 encoded byte sequence from
 *        provided stream.
 * 
 * @param sbyte SIGNED start byte of the sequence. 
 * @param stream source stream.
 * @return uint64_t Unicode character.
 */
static uint64_t read_unicode(int8_t sbyte, FILE *stream) 
{
    // Copy, first byte in sequence gets manipulated.
    uint8_t byte = sbyte;

    // Determine byte width of the encoding. If MSB is set, increment n.
    int n = 0;
    while (is_utf8(sbyte))
    {
        n++;
        sbyte <<= 1;
    }
    
    // Start acculmulating result bit sequence.
    uint64_t utf8 = byte & (0xFF >> (n + 1)); // Include tail 0 from width encoding to byte mask.

    // Read bytes from n bytes from stream and push bits to result.
    // First byte is already read.
    for (int i = 0; i < n - 1; i++)
    {
        byte = fgetc(stream) & 0x3F; // 6 LSB
        utf8 = (utf8 << 6) | byte;
    }

    return utf8;
}

/**
 * @brief Tries to open specified file. If not success,
 *        Prints error message to stderr before abort.
 * 
 * @param argc Program argument count.
 * @param argv Program arguments.
 * @return FILE* Opened file .
 */
static FILE *openfile(int argc, const char **argv)
{
    // Check if there exists a file path argument.
    if (argc < 2) 
    {
        fprintf(stderr, "Invalid arguments\n");
        exit(EXIT_FAILURE);
    }

    // Try opening specified file.
    FILE *file = fopen(argv[ARG_FILE], "r");
    if (!file) 
    {
        fprintf(stderr, "Could not open file: %s", argv[ARG_FILE]);
        exit(EXIT_FAILURE);
    }

    return file;
}


int main(int argc, char const **argv)
{
    FILE *file = openfile(argc, argv);

    int8_t sbyte;
    while ((sbyte = fgetc(file)) != EOF)
    {
        fprintf(stdout, "U+"); // Prefix
        if (is_utf8(sbyte)) 
        {
            // UTF-8 encoding encountered, delegate reading.
            const uint64_t utf8 = read_unicode(sbyte, file);
            fprintf(stdout, "%lX ", utf8);
        }
        else
        {
            // Same treatment as typical ASCII.
            fprintf(stdout, "%X ", sbyte);
        }
    }
    fputc('\n', stdout);

    fclose(file);
    return EXIT_SUCCESS;
}