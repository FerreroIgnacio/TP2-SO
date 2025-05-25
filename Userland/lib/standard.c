#include <standard.h>


//uint16_t getWidth();
//uint16_t getHeight();


// Escribe el char str en la posición (x,y)
 void drawChar(char ascii, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size){
    if(ascii < 0 || ascii > 128) 
	    return;
	char * bmp = font8x8_basic[ascii];

    for(int j = 0; j < 64; j++){
        int fil = j / 8;    // fila (0-7)
        int col = j % 8;    // columna (0-7)

        // Acceder al byte de la fila y verificar el bit de la columna
        int isOn = bmp[fil] & (1 << col);  // Sin invertir el bit
        int color = isOn ? hexColor : backColor;

        for (int dx = 0; dx < size; dx++) {
            for (int dy = 0; dy < size; dy++) {
                putPixel(color,
                        x + col * size + dx,     // X: posición + columna
                        y + fil * size + dy);    // Y: posición + fila
            }
        }
    }
 }

// Escribe el string str en la posición (x,y)
void drawText(char* str, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size){
	//para cada letra
	int i = 0;
	while(str[i] != 0){
		drawChar(str[i], hexColor, backColor, x, y, size);
		x+= 8 * size;
		i++;
	}
}

// Dibuja un número entero en la pantalla en la posición (x,y)
void drawInt(int num, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size) {
    char buffer[12];
    int i = 0;
    
    // Manejo numeros negativos
    if (num < 0) {
        buffer[i++] = '-';
        num = -num;
    }
    
    // Caso especial: 0
    if (num == 0) {
        buffer[i++] = '0';
    } else {
        // Extraer dígitos en orden inverso
        int start = i;
        while (num > 0) {
            buffer[i++] = (num % 10) + '0';
            num /= 10;
        }
        // Invertir solo los dígitos
        for (int j = start; j < (start + i) / 2; j++) {
            char temp = buffer[j];
            buffer[j] = buffer[i - 1 - (j - start)];
            buffer[i - 1 - (j - start)] = temp;
        }
    }
    
    buffer[i] = '\0';
    drawText(buffer, hexColor, backColor, x, y, size);
}


// Dibuja un rectángulo de w pixeles por h pixeles en la posición (x,y)
// Siendo x,y la esquina inferior del rectángulo
//void drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t w, uint64_t h);

// Dibuja un círculo de r píxeles de radio en la posición (x,y)
//void drawCircle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t r);

// Cambia todos los píxeles a hexColor
//void fillScreen(uint32_t hexColor);

