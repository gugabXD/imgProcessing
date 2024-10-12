#include "quadtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>     /* OpenGL functions */
#endif

unsigned int first = 1;
char desenhaBorda = 1;

QuadNode* newNode(int x, int y, int width, int height)
{
    QuadNode* n = malloc(sizeof(QuadNode));
    n->x = x;
    n->y = y;
    n->width = width;
    n->height = height;
    n->NW = n->NE = n->SW = n->SE = NULL;
    n->color[0] = n->color[1] = n->color[2] = 0;
    n->id = first++;
    return n;
}

QuadNode* geraQuadtree(int x, int y, Img* pic, float minError)
{
  // Converte o vetor RGBPixel para uma MATRIZ que pode acessada por pixels[linha][coluna]
    //printf("width: %d, height: %d// ", pic->width, pic->height);
    RGBPixel (*pixels)[pic->width] = (RGBPixel(*)[pic->height]) pic->img;
    int i,j, intensidade;
    float ravg=0.0, gavg=0.0, bavg=0.0;
    int *histograma = malloc(sizeof(int)*256);
    for(i=0; i<256; i++){
        histograma[i] = 0;
    }
    for(i=0; i<pic->height; i++){
        for(j=0; j<pic->width; j++){
            //intensidade cinza
            intensidade = (int)(pixels[i][j].r*0.3 + pixels[i][j].g*0.59 + pixels[i][j].b*0.11);
            ravg += pixels[i][j].r;
            gavg += pixels[i][j].g;
            bavg += pixels[i][j].b;
            //histograma
            histograma[intensidade]++;
        }
    }
    int width = pic->width;
    int height = pic->height;
    //cor media
    ravg = ravg/(width*height);
    gavg = gavg/(width*height);
    bavg = bavg/(width*height);
    //printf("r: %d, g: %d, b: %d\n", ravg, gavg, bavg);
    int imedia=0;
    for(int i=0; i<256; i++){
        imedia += histograma[i]*i;
    }
    //intensidade media
    imedia = imedia/(width*height);
    //printf("imedia: %d\n", imedia);

    //calculando o erro;
    float dif=0.0;
    for(i=0; i<height; i++){
        for(j=0; j<width; j++){
            intensidade = (int)(pixels[i][j].r*0.3 + pixels[i][j].g*0.59 + pixels[i][j].b*0.11);
            dif += pow((intensidade-imedia*1.0),2.0);
        }
    }
    //printf("dif: %f\n", dif);
   // printf("teste: %f\n",(1/(width*height*1.0)) * dif);
    double erro = sqrt((1/(width*height*1.0)) * dif);
//Criacao do nodo
QuadNode* raiz = newNode(x,y,width, height);
    raiz->color[0] = ravg;
    raiz->color[1] = gavg;
    raiz->color[2] = bavg;
   // printf("erro encontrado: %f\n",erro);
    if(erro<minError){
        raiz->status = CHEIO;
        return raiz;
    }
        raiz->status = PARCIAL;
    //recursao
    int halfw = width/2;
    int halfh = height/2;
    
    int restow = width%2;
    int restoh = height%2;

    Img picNW = *pic;
    picNW.width = halfw+restow;
    picNW.height = halfh+restoh;

    Img picNE = *pic;
    picNE.img = &pic->img[picNW.width];
    picNE.width = halfw;
    picNE.height = halfh+restoh;

    Img picSW = *pic;
    picSW.img = &pic->img[width*picNW.height];
    picSW.width = halfw+restow;
    picSW.height = halfh;


    Img picSE = *pic;
    picSE.img = &pic->img[width*picNW.height+picNW.width];
    picSE.width = halfw;
    picSE.height = halfh;

    //printf("NW: %p\n NE: %p\n SW: %p\n SE: %p\n", picNE.img, picNW.img, picSW.img, picSE.img);

    raiz->NW = geraQuadtree(x, y, &picNW, minError);
    raiz->NE = geraQuadtree(x+picNW.width, y, &picNE, minError);
    raiz->SW = geraQuadtree(x, y+picNW.height, &picSW, minError);
    raiz->SE = geraQuadtree(x+picNW.width, y+picNW.height, &picSE, minError);
    free(histograma);

// COMENTE a linha abaixo quando seu algoritmo ja estiver funcionando
// Caso contrario, ele ira gerar uma arvore de teste com 3 nodos

#define DEMO
#ifdef DEMO


#endif
    // Finalmente, retorna a raiz da árvore
    return raiz;
}







// Limpa a memória ocupada pela árvore
void clearTree(QuadNode* n)
{
    if(n == NULL) return;
    if(n->status == PARCIAL)
    {
        clearTree(n->NE);
        clearTree(n->NW);
        clearTree(n->SE);
        clearTree(n->SW);
    }
    //printf("Liberando... %d - %.2f %.2f %.2f %.2f\n", n->status, n->x, n->y, n->width, n->height);
    free(n);
}

// Ativa/desativa o desenho das bordas de cada região
void toggleBorder() {
    desenhaBorda = !desenhaBorda;
    printf("Desenhando borda: %s\n", desenhaBorda ? "SIM" : "NÃO");
}

// Desenha toda a quadtree
void drawTree(QuadNode* raiz) {
    if(raiz != NULL)
        drawNode(raiz);
}

// Grava a árvore no formato do Graphviz
void writeTree(QuadNode* raiz) {
    FILE* fp = fopen("quad.dot", "w");
    fprintf(fp, "digraph quadtree {\n");
    if (raiz != NULL)
        writeNode(fp, raiz);
    fprintf(fp, "}\n");
    fclose(fp);
    printf("\nFim!\n");
}

void writeNode(FILE* fp, QuadNode* n)
{
    if(n == NULL) return;

    if(n->NE != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->NE->id);
    if(n->NW != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->NW->id);
    if(n->SE != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->SE->id);
    if(n->SW != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->SW->id);
    writeNode(fp, n->NE);
    writeNode(fp, n->NW);
    writeNode(fp, n->SE);
    writeNode(fp, n->SW);
}

// Desenha todos os nodos da quadtree, recursivamente
void drawNode(QuadNode* n)
{
    if(n == NULL) return;

    glLineWidth(0.1);

    if(n->status == CHEIO) {
        glBegin(GL_QUADS);
        glColor3ubv(n->color);
        glVertex2f(n->x, n->y);
        glVertex2f(n->x+n->width-1, n->y);
        glVertex2f(n->x+n->width-1, n->y+n->height-1);
        glVertex2f(n->x, n->y+n->height-1);
        glEnd();
    }

    else if(n->status == PARCIAL)
    {
        if(desenhaBorda) {
            glBegin(GL_LINE_LOOP);
            glColor3ubv(n->color);
            glVertex2f(n->x, n->y);
            glVertex2f(n->x+n->width-1, n->y);
            glVertex2f(n->x+n->width-1, n->y+n->height-1);
            glVertex2f(n->x, n->y+n->height-1);
            glEnd();
        }
        drawNode(n->NE);
        drawNode(n->NW);
        drawNode(n->SE);
        drawNode(n->SW);
    }
    // Nodos vazios não precisam ser desenhados... nem armazenados!
}

