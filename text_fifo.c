#include "text_fifo.h"
#include <stdio.h>

uint8_t text_fifo_push(text_fifo *fifo, char elem) {
    if (fifo->head == fifo->tail && !fifo->empty) {
        return 0;
    }
    fifo->buffer[fifo->head] = elem;
    if (elem == 0) {
        fifo->contained_strings += 1;
    }
    fifo->head = (fifo->head + 1) % fifo->size;
    fifo->empty = 0;
    return 1;
}

uint8_t text_fifo_pop(text_fifo *fifo, char *output_elem) {
    if (fifo->empty) {
        return 0;
    }
    *output_elem = fifo->buffer[fifo->tail];
    if (*output_elem == 0) fifo->contained_strings -= 1;
    fifo->tail = (fifo->tail + 1) % fifo->size;
    if (fifo->tail == fifo->head) fifo->empty = 1;
    return 1;
}

size_t text_fifo_pop_n(text_fifo *fifo, char *output_buffer, size_t count) {
    char c;
    uint8_t success;
    uint32_t i;
    for (i = 0; i < count; i++) {
        success = text_fifo_pop(fifo, &c);
        if (!success) break;
        output_buffer[i] = c;
    }
    return i;
}

size_t text_fifo_get_next_string_size(text_fifo *fifo) {
    uint32_t i = 0;
    uint32_t pos = fifo->tail;
    while (pos != fifo->head) {
        if (fifo->buffer[pos] == 0) return i+1; 
        i += 1;
        pos = (pos + 1) % fifo->size;
    }
    return 0;
}

void text_fifo_clear(text_fifo *fifo) {
    fifo->head = 0;
    fifo->tail = 0;
    fifo->empty = 1;
    fifo->contained_strings = 0;
}

size_t text_fifo_pop_string(text_fifo *fifo, char *text_buffer, size_t buflen) {
    char output_char;
    size_t count = 0;
    uint8_t fifo_filled = 1;
    while(fifo_filled) {
        fifo_filled = text_fifo_pop(fifo, &output_char);
        count += fifo_filled;
        if (count <= buflen) text_buffer[count-1] = output_char;
        else break;
        if (output_char == 0) {
            break;
        }
    }
    return count;
}

size_t text_fifo_push_string(text_fifo *fifo, char *text_buffer) { 
    size_t count = 0;
    while (text_fifo_push(fifo, text_buffer[count])) {
        if (text_buffer[count] == 0) {
            break;
        };
        count += 1;
    }
    count += 1;
    return count;
}

size_t text_fifo_push_n_string(text_fifo *fifo, char *text_buffer, size_t buf_len) { 
    size_t count = 0;
    size_t push = 0;
    for (size_t i = 0; i < buf_len; i++){
        push = text_fifo_push(fifo, text_buffer[i]);
        if (push == 0) break;
        count += push;
        if (text_buffer[i] == 0) {
            break;
        };
    }
    return count;
}

uint8_t text_fifo_readable(text_fifo *fifo){
    if (fifo->contained_strings) return 1;
    return 0;
}

size_t text_fifo_strings_remaining(text_fifo *fifo){
    return fifo->contained_strings;
}

size_t text_fifo_space_remaining(text_fifo *fifo){
    if (fifo->head == fifo->tail) {
        if (fifo->empty) return fifo->size;
        else return 0;
    }
    if (fifo->head > fifo->tail) return fifo->size - fifo->head + fifo->tail;
    return fifo->tail - fifo->head;
}

size_t text_fifo_get_size(text_fifo *fifo) {
    return fifo->size;
}
