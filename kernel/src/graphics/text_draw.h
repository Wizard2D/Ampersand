#ifndef TEXT_DRAW_H
#define TEXT_DRAW_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

extern uint32_t last_finish_x;

extern void putpixel(volatile uint32_t *fb, uint32_t pitch, uint32_t x, uint32_t y, uint32_t color);
extern void draw_char(volatile uint32_t *fb, uint32_t pitch, uint8_t font[][8], uint32_t x, uint32_t y, char c, uint32_t color, uint32_t scale);
extern void draw_str(volatile uint32_t *fb, uint32_t pitch, uint8_t font[][8], uint32_t x, uint32_t y, const char* str, uint32_t color, uint32_t scale);
extern uint32_t create_color(uint8_t red, uint8_t green, uint8_t blue);
void calc_text_size(const char* str, uint32_t scale, uint32_t *width, uint32_t *height);
#endif