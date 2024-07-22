#ifndef _text_fifo_h_
#define _text_fifo_h_
#include <stddef.h>
#include <stdint.h>

typedef struct text_fifo {
  size_t head;
  size_t tail;
  uint8_t empty;
  char *buffer;
  size_t contained_strings;
  size_t size;
} text_fifo;

#define TEXT_FIFO(fifo_name_, max_elem_count_) \
  char fifo_name_ ## _fifo_buffer[max_elem_count_];\
  text_fifo fifo_name_ = {\
    .size = max_elem_count_,\
    .head = 0,\
    .tail = 0,\
    .empty = 1,\
    .contained_strings = 0,\
    .buffer = fifo_name_ ## _fifo_buffer\
  }

// push a char to the fifo. If the char is a \0 byte increase
// the number of stored strings
uint8_t text_fifo_push(text_fifo *fifo, char elem);

// push but_len chars onto the text fifo, as long as there is space 
size_t text_fifo_push_n(text_fifo *fifo, char *text_buffer, size_t buf_len);

// Push at most n chars onto the fifo, stop pushing, when the end of a string is
// encountered
size_t text_fifo_push_n_string(text_fifo *fifo, char *text_buffer, size_t buf_len);
// Push a string to the fifo
//
// This function pushes a string to the buffer, assuming that there is enough
// space in the fifo and that text_buffer actually points to a string
// the space left in the fifo can be queried with `text_fifo_space_remaining`
size_t text_fifo_push_string(text_fifo *fifo, char *text_buffer);

// pop a char from the fifo
// if the byte is \0 then decrease the number of stored strings
uint8_t text_fifo_pop(text_fifo *fifo, char *output_elem);

// get count characters from the fifo
size_t text_fifo_pop_n(text_fifo *fifo, char *output_buffer, size_t count);

// get the size of the next string in bytes
size_t text_fifo_get_next_string_size(text_fifo *fifo);

// empty the fifo
void text_fifo_clear(text_fifo *fifo);



// read a string from the fifo
//
// This function is only valid if text_fifo_readable returns 1
// Otherwise there is no '\0' char in the buffer and it
// will read until either the buffer is empty or output size is reached
// it returns the number of bytes read (including the terminating char)
// reading the string pops all it's elements (including the null byte) from the fifo
size_t text_fifo_pop_string(text_fifo *fifo, char *output_buf, size_t output_size);

// returns true if the fifo has a string in it, false otherwise
uint8_t text_fifo_readable(text_fifo *fifo);
size_t text_fifo_get_size(text_fifo *fifo);
size_t text_fifo_space_remaining(text_fifo *fifo);
#endif
