#include <stdio.h>
#include <stdlib.h>
#include <string.h>		// for using strings

#ifdef _WIN32
#include <windows.h>    /* includes only in MSWindows not in UNIX */
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>     /* OpenGL functions */
#include <GL/glu.h>    /* OpenGL utilitarian functions */
#include <GL/glut.h>
#endif

#include "include/SOIL.h"

void init();
void draw();
void drawButton();

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGBPixel;

typedef struct {
    int width, height;
    RGBPixel *img;
} Img;

typedef struct {
    int x, y;
    int width, height;
    char label[20];
    int isHovered;
} Button;

typedef struct {
    int x, y;
    int width, height;
    char text[4];        // display
    int isActive;         // selected
} InputBox;

InputBox intInputBox;

void handleClick(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
RGBPixel* copyImage(const RGBPixel* source, int width, int height);
void drawInputBox(InputBox *box);
void quantify(int n);

int width, height, shades;
GLuint tex, tex2;
Img pic;
RGBPixel * imgCopy;

Button reloadButton, grayButton, flipButton, mirrorButton;

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    int chan;
    pic.img = (RGBPixel*)SOIL_load_image(argv[1], &pic.width, &pic.height, &chan, SOIL_LOAD_AUTO);
    if (!pic.img) {
        printf("SOIL loading error: '%s'\n", SOIL_last_result());
        exit(1);
    }

    imgCopy = copyImage(pic.img, pic.width, pic.height);

    if (!imgCopy) {
        printf("Image copy failed!\n");
        exit(1);
    }

    width = 2 * pic.width + 20;
    height = pic.height + 12;

    reloadButton = (Button){width+10, 10, 100, 40, "Reload", 0};
    grayButton = (Button){width+10, 60, 100, 40, "Gray", 0};
    flipButton = (Button) {width+10, 110, 100, 40, "Flip", 0};
    mirrorButton = (Button) {width+10, 160, 100, 40, "Mirror", 0};
    intInputBox = (InputBox) {width+10, 210, 100, 40, "", 0};

    glutInitWindowSize(width + 120, height);
    glutCreateWindow("Fundamentos de Processamento de Imagens");

    glutDisplayFunc(draw);
    glutMouseFunc(handleClick);
    glutKeyboardFunc(keyboard);

    // Create the texture for the original image
    tex = SOIL_create_OGL_texture((unsigned char*)imgCopy, pic.width, pic.height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

    // Create the texture for the second image (original image)
    tex2 = SOIL_create_OGL_texture((unsigned char*)pic.img, pic.width, pic.height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

    printf("Image: %d x %d (%d)\n", pic.width, pic.height, chan);

    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, width + 120, height, 0.0);
    glMatrixMode(GL_MODELVIEW);

    glutMainLoop();
    return 0;
}

void draw() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the first image texture
    glColor3ub(255, 255, 255);
    glBindTexture(GL_TEXTURE_2D, tex);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(6, 6);
    glTexCoord2f(1, 0); glVertex2f(pic.width, 6);
    glTexCoord2f(1, 1); glVertex2f(pic.width, pic.height + 6);
    glTexCoord2f(0, 1); glVertex2f(6, pic.height + 6);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Draw the second image texture
    glBindTexture(GL_TEXTURE_2D, tex2);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(pic.width + 16, 6);
    glTexCoord2f(1, 0); glVertex2f(pic.width + 16 + pic.width, 6);
    glTexCoord2f(1, 1); glVertex2f(pic.width + 16 + pic.width, pic.height + 6);
    glTexCoord2f(0, 1); glVertex2f(pic.width + 16, pic.height + 6);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Draw the components
    drawButton(reloadButton);
    drawButton(grayButton);
    drawButton(flipButton);
    drawButton(mirrorButton);
    drawInputBox(&intInputBox);

    glutSwapBuffers();
}

void gray(){
    int i, j;
        int gray;

        for (i = 0; i < pic.height; i++) {
            for (j = 0; j < pic.width; j++) {

                RGBPixel *pixel = &pic.img[i * pic.width + j];

                gray = (int)(pixel->r * 0.299 + pixel->g * 0.587 + pixel->b * 0.114);

                pixel->r = gray;
                pixel->g = gray;
                pixel->b = gray;
            }
        }

        tex2 = SOIL_create_OGL_texture((unsigned char*)pic.img, pic.width, pic.height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
}

void reload(){
    free(pic.img);
    pic.img = copyImage(imgCopy, pic.width, pic.height);
    tex2 = SOIL_create_OGL_texture((unsigned char*)pic.img, pic.width, pic.height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
}

void keyboard(unsigned char key, int x, int y) {
    //Inputting numbers
    if (intInputBox.isActive) {
        int len = strlen(intInputBox.text);

        if (key >= '0' && key <= '9' && len < 3) {
            intInputBox.text[len] = key;
            intInputBox.text[len + 1] = '\0';
        }

        if (key == 8 && len > 0) {
            intInputBox.text[len - 1] = '\0';
        }
        if(key==13) {
            shades = atoi(intInputBox.text);
            quantify(shades);
        }
    }
    
    

    //Esc to close
    if(key==27) {
        free(pic.img);
        free(imgCopy);
        exit(1);
    }

    glutPostRedisplay();
}


void drawButton(Button btn) {
    // Check if the button is hovered
    if (btn.isHovered) {
        glColor3f(0.7f, 0.7f, 0.7f);  // A lighter color when hovered
    } else {
        glColor3f(0.5f, 0.5f, 0.5f);  // Default button color
    }

    // Draw the button rectangle
    glBegin(GL_QUADS);
    glVertex2i(btn.x, btn.y);
    glVertex2i(btn.x + btn.width, btn.y);
    glVertex2i(btn.x + btn.width, btn.y + btn.height);
    glVertex2i(btn.x, btn.y + btn.height);
    glEnd();

    // Draw the border
    glColor3f(0.0f, 0.0f, 0.0f);  // Border color (black)
    glBegin(GL_LINE_LOOP);  // Use GL_LINE_LOOP to create a border
    glVertex2i(btn.x, btn.y);
    glVertex2i(btn.x + btn.width, btn.y);
    glVertex2i(btn.x + btn.width, btn.y + btn.height);
    glVertex2i(btn.x, btn.y + btn.height);
    glEnd();

    // Draw the button label (text)
    glColor3f(1.0f, 1.0f, 1.0f);  // Text color (white)
    glRasterPos2i(btn.x + 10, btn.y + 25);  // Position the text inside the button
    for (int i = 0; i < strlen(btn.label); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, btn.label[i]);  // Draw each character
    }
}

void flip() {
    RGBPixel* newImage = (RGBPixel*)malloc(pic.width * pic.height * sizeof(RGBPixel));

    int w = pic.width;
    int h = pic.height;

    for (int row = 0; row < h; row++) {
        RGBPixel* rowOriginal = pic.img + row * w;
        RGBPixel* rowFlipped = newImage + (h - row - 1) * w;
        memcpy(rowFlipped, rowOriginal, w * sizeof(RGBPixel));
    }
    free(pic.img);

    pic.img = newImage;

    tex2 = SOIL_create_OGL_texture((unsigned char*)pic.img, pic.width, pic.height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
}

void mirror(){
    RGBPixel* newImage = (RGBPixel*)malloc(width * height * sizeof(RGBPixel));
    int k = 0;
    for (int i=0; i<pic.height; i++){
        for (int j=pic.width-1; j>=0; j--){
            newImage[k] = pic.img[i*pic.width+j];
            k++;
        }
    }
    free(pic.img);
    pic.img = newImage;
    tex2 = SOIL_create_OGL_texture((unsigned char*)pic.img, pic.width, pic.height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
}

void quantify(int n){
    reload(); //shades of the original image
    gray(); //confirm it's gray
    int t1 = 255, t2 = 0, t_orig;
    RGBPixel* pixel;

    for (int i=0; i<pic.height; i++){
        for(int j=0; j<pic.width; j++){
            pixel = &pic.img[i*pic.width+j];
            t_orig = pixel->r; //values of r, g and b refer to the luminance
            if(t_orig<t1) t1 = t_orig;
            if(t_orig>t2) t2 = t_orig;
        }
    }

    float tam_int = t2-t1+1.0;

    if(n<tam_int){
        float tb = tam_int/n;
        int bucket;
        int shade;
        for (int i=0; i<pic.height; i++){
            for(int j=0; j<pic.width; j++){
                pixel = &pic.img[i*pic.width+j];
                t_orig = pixel->r;
                bucket = (int)((t_orig+0.5)/tb); //knowing which bucket it falls into
                shade = (int)(t1-0.5+((2*bucket+1)*tb)/2); //integer value at the center of the bucket

                pixel->r = shade;
                pixel->g = shade;
                pixel->b = shade;
            }
        }
    }
    tex2 = SOIL_create_OGL_texture((unsigned char*)pic.img, pic.width, pic.height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
}

//Handling mouse clicks
void handleClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

        if (x >= reloadButton.x && x <= reloadButton.x + reloadButton.width &&
            y >= reloadButton.y && y <= reloadButton.y + reloadButton.height) {
            reload();
        }

        if (x >= grayButton.x && x <= grayButton.x + grayButton.width &&
            y >= grayButton.y && y <= grayButton.y + grayButton.height) {
            gray();
        }
        if (x >= flipButton.x && x <= flipButton.x + flipButton.width &&
            y >= flipButton.y && y <= flipButton.y + flipButton.height) {
            flip();
        }
        if (x >= mirrorButton.x && x <= mirrorButton.x + mirrorButton.width &&
            y >= mirrorButton.y && y <= mirrorButton.y + mirrorButton.height) {
            mirror();
        }
        if (x >= intInputBox.x && x <= intInputBox.x + intInputBox.width &&
            y >= intInputBox.y && y <= intInputBox.y + intInputBox.height) {
            intInputBox.isActive = 1;
        } else {
            intInputBox.isActive = 0;
        }
    }
    glutPostRedisplay();
}

RGBPixel* copyImage(const RGBPixel* source, int width, int height) {
    RGBPixel* newImage = (RGBPixel*)malloc(width * height * sizeof(RGBPixel));
    if (!newImage) {
        printf("Failed to allocate memory for image copy.\n");
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            newImage[i * width + j] = source[i * width + j];
        }
    }
    return newImage;
}

void drawInputBox(InputBox *box) {
    if (box->isActive) {
        glColor3f(0.8f, 0.8f, 0.8f);
    } else {
        glColor3f(0.7f, 0.7f, 0.7f);
    }

    glBegin(GL_QUADS);
    glVertex2i(box->x, box->y);
    glVertex2i(box->x + box->width, box->y);
    glVertex2i(box->x + box->width, box->y + box->height);
    glVertex2i(box->x, box->y + box->height);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(box->x + 5, box->y + 25);
    for (int i = 0; i < strlen(box->text); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, box->text[i]);
    }

    int textWidth = strlen(box->text) * 10;
        glBegin(GL_LINES);
        glVertex2i(box->x + 5 + textWidth, box->y + 5);
        glVertex2i(box->x + 5 + textWidth, box->y + box->height - 5);
        glEnd();
}
