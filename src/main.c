#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#define ARG_FILE 1
#define TRAILING_WIDTH 6
#define TRAILING_WIDTH_MASK 0x3F
#define MSB_SET(byte) (((int8_t)byte) < 0)
#define IS_UTF_8(byte) MSB_SET(byte)

/**
 * @brief Determines the width of utf-8 encoding.
 * 
 * @param sbyte SIGNED start byte of the sequence.
 * @return int width in n bytes.
 */
static int encoding_width(int8_t sbyte) 
{
    int n;
    for (n = 0; MSB_SET(sbyte); n++) sbyte <<= 1;
    return n;
}

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
    int n = encoding_width(sbyte);
    // Start acculmulating result bit sequence.
    uint64_t utf8 = sbyte & (0xFF >> (n + 1)); // Include tail 0 from width encoding to byte mask.

    // Read bytes from n bytes from stream and push bits to result.
    // First byte is already read.
    for (int i = 0; i < n - 1; i++)
    {
        sbyte = fgetc(stream) & TRAILING_WIDTH_MASK;
        utf8 = (utf8 << TRAILING_WIDTH) | sbyte;
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
        if (IS_UTF_8(sbyte)) 
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