#include <stdio.h>
#include <string.h>
#include "text_fifo.h"

TEXT_FIFO(test_fifo, 128);

#define local_buf_len 1024
char local_buffer[local_buf_len];


size_t snprint_byte_array(char *buf, size_t bsize, uint8_t *arr, size_t len, uint16_t screen_width){
    size_t string_len = 6 * len + 1 + 1;
    size_t occupied = 0;
    const uint8_t width_of_byte = 3;
    uint16_t chars_in_line = 0;
    if (bsize > string_len) {
        buf[0] = '[';
        buf[1] = '\r';
        buf[2] = '\n';
        occupied += 3;
        for (int i = 0; i<len-1; i++) {
            size_t count = snprintf(&buf[occupied], bsize - occupied, " %02X", arr[i]);
            chars_in_line += count;
            occupied += count;
            if (chars_in_line + width_of_byte > screen_width) {
                buf[occupied] = '\r';
                buf[occupied + 1] = '\n';
                occupied += 2;
                chars_in_line = 0;
            }
        }
        occupied += snprintf(&buf[occupied], bsize-occupied, " %02X\n]\n", arr[len-1]);
        // snprintf only reports the number of char written. The null byte is not counted
        // so we add it here
        occupied += 1;
    }
    return occupied;
}

// Print the content of the memory block into a char buffer return the char count
// written including the terminating null byte
size_t snprint_mem_block(char *buf, size_t buf_len, uint32_t start_addr,
                         size_t mem_len, uint8_t *bytes, uint8_t bytes_per_line){
    // Calculate the string length and return if it is too long
    size_t lines = mem_len / bytes_per_line;
    if ( mem_len % bytes_per_line) {
        lines += 1;
    }
    size_t str_len = 3 * mem_len + 7 * lines;
    if (str_len > buf_len) {
        return 0;
    }
    size_t count = 0;
    for (int i = 0; i < mem_len; i += bytes_per_line){
        count += snprintf(&buf[count], buf_len - count, "%06x\t", i + start_addr);
        for (int j = 0; j < bytes_per_line-1 && i+j < mem_len-1; j++) {
            count += snprintf(&buf[count], buf_len - count, "%02X ", bytes[i+j]);
        }
        count += snprintf(&buf[count], buf_len - count, "%02X\n", bytes[mem_len-1]);
    }
    return count + 1;
}

int main() {
    size_t local_count = 0;
    char * strptr;
    int bytes_in_str = 0;
    
    printf("Bytes Remaining in fifo: %ld\n", text_fifo_space_remaining(&test_fifo));
    printf("Buffer readable: %d\n", text_fifo_readable(&test_fifo));
    
    local_count += snprintf(local_buffer, local_buf_len, "Hello World, current count: %ld\n", local_count);
    printf("String Loaded: %s", local_buffer);
    printf("Size of string loaded: %ld\n", local_count+ 1);
    text_fifo_push_string(&test_fifo, local_buffer);
    printf("Bytes Remaining in fifo: %ld\n", text_fifo_space_remaining(&test_fifo));
    printf("Buffer readable: %d\n", text_fifo_readable(&test_fifo));

    snprint_byte_array(local_buffer, local_buf_len, test_fifo.buffer, test_fifo.size, 80);
    printf("%s", local_buffer);
    
    strptr = strcpy(local_buffer, "This is another test string\n");
    printf("String to be loaded: %s", local_buffer);
    local_count = strlen(local_buffer);
    printf("Size of string loaded: %ld\n", local_count+ 1);
    text_fifo_push_string(&test_fifo, local_buffer);
    printf("Bytes Remaining in fifo: %ld\n", text_fifo_space_remaining(&test_fifo));
    printf("Buffer readable: %d\n", text_fifo_readable(&test_fifo));
    
    snprint_byte_array(local_buffer, local_buf_len, test_fifo.buffer, test_fifo.size, 80);
    printf("%s", local_buffer);
    
    text_fifo_pop_string(&test_fifo, local_buffer, local_buf_len);
    printf("String returned from fifo: %s", local_buffer);
    printf("Buffer readable: %d\n", text_fifo_readable(&test_fifo));
    printf("Bytes Remaining in fifo: %ld\n", text_fifo_space_remaining(&test_fifo));
    
    text_fifo_pop_string(&test_fifo, local_buffer, local_buf_len);
    printf("String returned from fifo: %s", local_buffer);
    printf("Buffer readable: %d\n", text_fifo_readable(&test_fifo));
    printf("Bytes Remaining in fifo: %ld\n", text_fifo_space_remaining(&test_fifo));


    strcpy(local_buffer, "The first test string\n");
    local_count = strlen(local_buffer);
    printf("String Loaded: %s", local_buffer);
    printf("Size of string loaded: %ld\n", local_count+ 1);
    text_fifo_push_string(&test_fifo, local_buffer);
    
    strcpy(local_buffer, "Second test string\n with a line break\n");
    local_count = strlen(local_buffer);
    printf("String Loaded: %s", local_buffer);
    printf("Size of string loaded: %ld\n", local_count+ 1);
    text_fifo_push_string(&test_fifo, local_buffer);

    strcpy(local_buffer, "A very long test string to try and get the buffer to role around\n");
    local_count = strlen(local_buffer);
    printf("String to be loaded: %s", local_buffer);
    printf("Size of string loaded: %ld\n", local_count+ 1);
    text_fifo_push_string(&test_fifo, local_buffer);


    printf("\nretrieving strings from buffer now:\n");
    while (text_fifo_readable(&test_fifo)) {
        text_fifo_pop_string(&test_fifo, local_buffer, local_buf_len);
        printf("String returned from fifo: %s", local_buffer);
        printf("Buffer readable: %d\n", text_fifo_readable(&test_fifo));
    }
    printf("fifo head: %ld, fifo tail: %ld\n", test_fifo.head, test_fifo.tail);

    printf("\n\n");
    printf("Testing partial readout");
    
    strcpy(local_buffer, "The first test string\n");
    local_count = strlen(local_buffer);
    printf("String Loaded: %s", local_buffer);
    printf("Size of string loaded: %ld\n", local_count+ 1);
    text_fifo_push_string(&test_fifo, local_buffer);
    printf("Bytes Remaining in fifo: %ld\n", text_fifo_space_remaining(&test_fifo));
    printf("fifo head: %ld, fifo tail: %ld\n", test_fifo.head, test_fifo.tail);
    
    strcpy(local_buffer, "Second test string\n with a line break\n");
    local_count = strlen(local_buffer);
    printf("String Loaded: %s", local_buffer);
    printf("Size of string loaded: %ld\n", local_count+ 1);
    text_fifo_push_string(&test_fifo, local_buffer);
    printf("Bytes Remaining in fifo: %ld\n", text_fifo_space_remaining(&test_fifo));
    printf("fifo head: %ld, fifo tail: %ld\n", test_fifo.head, test_fifo.tail);

    strcpy(local_buffer, "A very long test string to try and get the buffer to role\n");
    local_count = strlen(local_buffer);
    printf("String to be loaded: %s", local_buffer);
    printf("Size of string loaded: %ld\n", local_count+ 1);
    text_fifo_push_string(&test_fifo, local_buffer);
    printf("Bytes Remaining in fifo: %ld\n", text_fifo_space_remaining(&test_fifo));
    printf("fifo head: %ld, fifo tail: %ld\n", test_fifo.head, test_fifo.tail);


    printf("Partial Readout:\n");
    bytes_in_str = text_fifo_get_next_string_size(&test_fifo);
    printf("size of next string to read out: %d\n", bytes_in_str);
    text_fifo_pop_n(&test_fifo, local_buffer, bytes_in_str);
    printf("bytes read from fifo: %s\n", local_buffer);
}
