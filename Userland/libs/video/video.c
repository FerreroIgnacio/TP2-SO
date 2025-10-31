#include "./video.h"
extern void sys_setFB(uint8_t index);
extern void sys_freeFB(uint8_t index);

typedef struct frameBuffer_struct
{
    uint8_t fbId;
    uint16_t width;
    uint16_t height;
    uint16_t bpp;
    uint32_t pitch;
    uint8_t *frameBuffer;
} frameBuffer_struct_t;

void setFB(frameBuffer fb)
{
    sys_setFB(fb->fbId);
}

void freeFB(frameBuffer fb)
{
    sys_freeFB(fb->fbId);
}

uint8_t frameGetWidth(frameBuffer fb)
{
    return fb->width;
}

uint8_t frameGetHeight(frameBuffer fb)
{
    return fb->height;
}

uint8_t frameGetBpp(frameBuffer fb)
{
    return fb->bpp;
}

uint32_t frameGetPitch(frameBuffer fb)
{
    return fb->pitch;
}

// Cambia el pixel (x,y) del frame por hexColor
void framePutPixel(frameBuffer fb, uint32_t hexColor, uint64_t x, uint64_t y)
{
    uint64_t offset = (x * (fb->bpp / 8)) + (y * fb->pitch);
    fb->frameBuffer[offset] = (hexColor) & 0xFF;
    fb->frameBuffer[offset + 1] = (hexColor >> 8) & 0xFF;
    fb->frameBuffer[offset + 2] = (hexColor >> 16) & 0xFF;
}

// Dibujar caracter en la posición (y,x) del frame
void frameDrawChar(frameBuffer fb, unsigned char ascii, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y)
{
    font_info_t currentFont = fontmanager_get_current_font();
    int width = currentFont.width;
    int height = currentFont.height;
    int bytes_per_row = (width + 7) / 8;

    uint8_t *bmp = (uint8_t *)currentFont.data + (ascii * currentFont.bytes_per_char);

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            int byte_offset = row * bytes_per_row + (col / 8);
            int bit_index;

            // Handle mirrored vs non-mirrored fonts
            if (!currentFont.mirrored)
            {
                bit_index = col % 8;
            }
            else
            {
                bit_index = 7 - (col % 8); // Reverse bit order for non-mirrored fonts
            }

            int pixel_on = bmp[byte_offset] & (1 << bit_index);
            uint32_t color = pixel_on ? hexColor : backColor;
            framePutPixel(fb, color, x + col, y + row);
        }
    }
}

// Dibujar string en la posición (y,x) del frame
void frameDrawText(frameBuffer fb, char *str, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y)
{
    int i = 0;
    while (str[i] != 0)
    {
        frameDrawChar(fb, (unsigned char)str[i], hexColor, backColor, x, y);
        x += fontmanager_get_current_font().width; // Avanzar a la siguiente posición
        i++;
    }
}

// Dibujar número en la posición (y,x) del frame
void frameDrawInt(frameBuffer fb, int num, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y)
{
    char buffer[12];
    int i = 0;

    // Manejo numeros negativos
    if (num < 0)
    {
        buffer[i++] = '-';
        num = -num;
    }

    // Caso especial: 0
    if (num == 0)
    {
        buffer[i++] = '0';
    }
    else
    {
        // Extraer dígitos en orden inverso
        int start = i;
        while (num > 0)
        {
            buffer[i++] = (num % 10) + '0';
            num /= 10;
        }
        // Invertir solo los dígitos
        for (int j = start; j < (start + i) / 2; j++)
        {
            char temp = buffer[j];
            buffer[j] = buffer[i - 1 - (j - start)];
            buffer[i - 1 - (j - start)] = temp;
        }
    }

    buffer[i] = '\0';
    frameDrawText(fb, buffer, hexColor, backColor, x, y);
}

// Dibuja un rectángulo de w pixeles por h pixeles en la posición (x,y) del frame
void frameDrawRectangle(frameBuffer fb, uint32_t hexColor, uint64_t x, uint64_t y, uint64_t w, uint64_t h)
{
    for (uint64_t i = 0; i < h; i++)
    {
        for (uint64_t j = 0; j < w; j++)
        {
            uint64_t pixelX = x + j;
            uint64_t pixelY = y + i;

            if (pixelX < fb->width && pixelY < fb->height)
            {
                framePutPixel(fb, hexColor, pixelX, pixelY);
            }
        }
    }
}

// Dibuja un círculo de r píxeles de radio en la posición (x,y) del frame
void frameDrawCircle(frameBuffer fb, uint32_t hexColor, uint64_t x, uint64_t y, int64_t r)
{
    for (signed int dx = -r; dx <= r; dx++)
    {
        for (int dy = -r; dy <= r; dy++)
        {
            if (dx * dx + dy * dy <= r * r)
            {
                uint64_t pixelX = x + dx;
                uint64_t pixelY = y + dy;
                if (pixelX >= 0 && pixelY >= 0 && pixelX < fb->width && pixelY < fb->height)
                {
                    framePutPixel(fb, hexColor, pixelX, pixelY);
                }
            }
        }
    }
}

// Llenar el frame con hexColor
void frameFill(frameBuffer fb, uint32_t hexColor)
{
    uint8_t r = (hexColor >> 16) & 0xFF;
    uint8_t g = (hexColor >> 8) & 0xFF;
    uint8_t b = hexColor & 0xFF;
    uint8_t bytesPerPixel = fb->bpp / 8;

    for (uint16_t y = 0; y < fb->height; y++)
    {
        for (uint16_t x = 0; x < fb->width; x++)
        {
            uint64_t offset = y * fb->pitch + x * bytesPerPixel;
            fb->frameBuffer[offset] = b;
            fb->frameBuffer[offset + 1] = g;
            fb->frameBuffer[offset + 2] = r;
        }
    }
}

void frameCopyRegion(uint64_t startX, uint64_t startY, uint64_t width, uint64_t height, frameBuffer targetFrame, frameBuffer sourceFrame)
{
    if (!targetFrame || !sourceFrame || width == 0 || height == 0)
    {
        return;
    }

    // Calculate actual region bounds, clipping to both source and target frames
    uint64_t endX = startX + width;
    uint64_t endY = startY + height;

    // Clip to source frame bounds
    if (endX > sourceFrame->width)
    {
        endX = sourceFrame->width;
    }
    if (endY > sourceFrame->height)
    {
        endY = sourceFrame->height;
    }

    // Clip to target frame bounds
    if (endX > targetFrame->width)
    {
        endX = targetFrame->width;
    }
    if (endY > targetFrame->height)
    {
        endY = targetFrame->height;
    }

    // If start coordinates are out of bounds, nothing to copy
    if (startX >= endX || startY >= endY)
    {
        return;
    }

    // Copy pixels byte by byte using bpp
    uint16_t bytesPerPixel = sourceFrame->bpp / 8;

    for (uint64_t y = startY; y < endY; y++)
    {
        for (uint64_t x = startX; x < endX; x++)
        {
            uint64_t sourceOffset = (y * sourceFrame->width + x) * bytesPerPixel;
            uint64_t targetOffset = (y * targetFrame->width + x) * bytesPerPixel;

            // Copy pixel byte by byte
            for (uint16_t b = 0; b < bytesPerPixel; b++)
            {
                targetFrame->frameBuffer[targetOffset + b] = sourceFrame->frameBuffer[sourceOffset + b];
            }
        }
    }
}

void frameCopyCircle(uint64_t centerX, uint64_t centerY, uint64_t radius, frameBuffer targetFrame, frameBuffer sourceFrame)
{
    if (!targetFrame || !sourceFrame || radius == 0)
    {
        return;
    }

    // Calculate bounding box (handle potential underflow with signed arithmetic)
    int64_t minX_signed = (int64_t)centerX - (int64_t)radius;
    int64_t minY_signed = (int64_t)centerY - (int64_t)radius;

    uint64_t minX = (minX_signed < 0) ? 0 : (uint64_t)minX_signed;
    uint64_t minY = (minY_signed < 0) ? 0 : (uint64_t)minY_signed;
    uint64_t maxX = centerX + radius;
    uint64_t maxY = centerY + radius;

    // Clamp to both source and target frame bounds
    if (maxX > sourceFrame->width)
        maxX = sourceFrame->width;
    if (maxY > sourceFrame->height)
        maxY = sourceFrame->height;
    if (maxX > targetFrame->width)
        maxX = targetFrame->width;
    if (maxY > targetFrame->height)
        maxY = targetFrame->height;

    // If the bounding box doesn't intersect with the frames, nothing to copy
    if (minX >= maxX || minY >= maxY)
    {
        return;
    }

    // Copy pixels byte by byte using bpp
    uint16_t bytesPerPixel = sourceFrame->bpp / 8;

    // Copy pixels within circle to same coordinates in target frame
    for (uint64_t y = minY; y < maxY; y++)
    {
        for (uint64_t x = minX; x < maxX; x++)
        {
            int64_t dx = (int64_t)x - (int64_t)centerX;
            int64_t dy = (int64_t)y - (int64_t)centerY;

            if (dx * dx + dy * dy <= (int64_t)radius * (int64_t)radius)
            {
                uint64_t sourceOffset = (y * sourceFrame->width + x) * bytesPerPixel;
                uint64_t targetOffset = (y * targetFrame->width + x) * bytesPerPixel;

                // Copy pixel byte by byte
                for (uint16_t b = 0; b < bytesPerPixel; b++)
                {
                    targetFrame->frameBuffer[targetOffset + b] = sourceFrame->frameBuffer[sourceOffset + b];
                }
            }
        }
    }
}