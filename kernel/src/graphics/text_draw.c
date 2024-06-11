#include <graphics/text_draw.h>
#include <font8.h>

uint32_t line_counter = 0;
uint32_t last_finish_x = 0;

void putpixel(volatile uint32_t *fb, uint32_t pitch, uint32_t x, uint32_t y, uint32_t color) 
{
    fb[y * (pitch / 4) + x] = color;
}


void draw_char(volatile uint32_t *fb, uint32_t pitch, uint8_t font[][8], uint32_t x, uint32_t y, char c, uint32_t color, uint32_t scale) 
{
    const uint8_t *glyph = font[(uint8_t)c];
    for (uint32_t i = 0; i < 8; i++) {
        for (uint32_t j = 0; j < 8; j++) {
            // If the j-th bit of the i-th byte is set, draw a pixel
            if (glyph[i] & (1 << (7 - j))) {
                // Scale the pixel
                for (uint32_t dy = 0; dy < scale; dy++) {
                    for (uint32_t dx = 0; dx < scale; dx++) {
                        putpixel(fb, pitch, x + j * scale + dx, y + i * scale + dy, color);
                    }
                }
            }
        }
    }
}

void draw_str(volatile uint32_t *fb, uint32_t pitch, uint8_t font[][8], uint32_t x, uint32_t y, const char* str, uint32_t color, uint32_t scale)
{
    int stx = x;

    int ex = x;
    int ey = y;

    int len = strlen(str);

    for(int i = 0; i < len; i++)
    {
        if (str[i] == '\n') {
            // Move to the next line by incrementing y and resetting x
            ex = stx;
            line_counter++;
            last_finish_x = 0;
            continue;
        }

        draw_char(fb, pitch, font8, ex, ey + line_counter*(8 * scale + 4), str[i], color, scale);

        ex += scale*(3*scale);
    }

    last_finish_x = ex;
}

uint32_t create_color(uint8_t red, uint8_t green, uint8_t blue) {
    return (red << 16) | (green << 8) | blue | (0xFF << 24); // The last 0xFF is the unused padding
}

void calc_text_size(const char* str, uint32_t scale, uint32_t *width, uint32_t *height) {
    *width = 0;
    *height = 0;

    uint32_t current_width = 0;
    uint32_t max_width = 0;

    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == '\n') {
            // New line, so reset current width and increment height
            if (current_width > max_width) {
                max_width = current_width;
            }
            current_width = 0;
            *height += (8 * scale) + 4; // Character height + spacing
        } else {
            // Increase the width for each character
            current_width += scale*(3*scale); // Character width + spacing
        }
    }

    // Update max_width for the last line if it doesn't end with a newline
    if (current_width > max_width) {
        max_width = current_width;
    }

    // Set final width and height
    *width = max_width;
    *height += (8 * scale) + 4; // Add height for the last line
}