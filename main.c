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
    int x, y;            // Button position
    int width, height;    // Button size
    char label[20];       // Button label
    int isHovered;        // To indicate if the button is hovered
} Button;

void mouseClick(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
RGBPixel* copyImage(const RGBPixel* source, int width, int height);



// Window and texture variables
int width, height;
GLuint tex, tex2;
Img pic;
RGBPixel * imgCopy;

Button reloadButton, grayButton;

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

    width = 2 * pic.width + 10;
    height = pic.height + 12;

    reloadButton = (Button){width+10, 10, 100, 40, "Reload", 0};
    grayButton = (Button){width+10, 60, 100, 40, "Gray", 0};

    glutInitWindowSize(width + 120, height);
    glutCreateWindow("Fundamentos de Processamento de Imagens");

    glutDisplayFunc(draw);
    glutMouseFunc(mouseClick);
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

    // Draw the reload and gray buttons (if needed)
    drawButton(reloadButton);
    drawButton(grayButton);

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
    tex2 = SOIL_create_OGL_texture((unsigned char*)imgCopy, pic.width, pic.height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'r') {
    reload();
    glutPostRedisplay();
}
    if (key == 'g') {
        gray();
        glutPostRedisplay();
    }
    if(key==27) {
        free(pic.img);
        free(imgCopy);
        exit(1);
    }
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

/*
 * Function to handle mouse clicks
 */
void mouseClick(int button, int state, int x, int y) {
    // Print the raw mouse coordinates to verify they're being captured
    //printf("Mouse click detected at: %d, %d\n", x, y);
    //printf("x: %d -> %d ; y:%d -> %d\n", reloadButton.x, reloadButton.x+reloadButton.width,reloadButton.y, reloadButton.y+reloadButton.height);
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

        if (x >= reloadButton.x && x <= reloadButton.x + reloadButton.width &&
            y >= reloadButton.y && y <= reloadButton.y + reloadButton.height) {
            reload();
        }

        if (x >= grayButton.x && x <= grayButton.x + grayButton.width &&
            y >= grayButton.y && y <= grayButton.y + grayButton.height) {
            gray();
        }
    }
    glutPostRedisplay();
}

RGBPixel* copyImage(const RGBPixel* source, int width, int height) {
    RGBPixel* newImage = (RGBPixel*)malloc(width * height * sizeof(RGBPixel));
    if (!newImage) {
        printf("Failed to allocate memory for image copy.\n");
        return NULL;  // Indicate failure
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            newImage[i * width + j] = source[i * width + j];
        }
    }
    return newImage;
}