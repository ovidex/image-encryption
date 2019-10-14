#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdint.h>

typedef struct
{
    unsigned char b;
    unsigned char g;
    unsigned char r;
} Pixel;
typedef struct
{
    unsigned char  fileMarker1; /* 'B' */
    unsigned char  fileMarker2; /* 'M' */
    unsigned int   bfSize; /* File's size */
    unsigned short unused1; /* Aplication specific */
    unsigned short unused2; /* Aplication specific */
    unsigned int   imageDataOffset; /* Offset to the start of image data */

    unsigned int   biSize; /* Size of the info header - 40 bytes */
    signed int     width; /* Width of the image */
    signed int     height; /* Height of the image */
    unsigned short planes;
    unsigned short bitPix; /* Number of bits per pixel = 3 * 8 (for each channel R, G, B we need 8 bits */
    unsigned int   biCompression; /* Type of compression */
    unsigned int   biSizeImage; /* Size of the image data */
    int            biXPelsPerMeter;
    int            biYPelsPerMeter;
    unsigned int   biClrUsed;
    unsigned int   biClrImportant;
} Fileheader;
typedef struct
{
    Fileheader seminte;
    Pixel* info;
} imagine;
uint32_t xorshift32(uint32_t state)
{

    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    uint32_t x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state = x;
    return x;
}
imagine citire(char* cale)
{
    imagine aux;
    int i,j;
    FILE* f = fopen(cale, "rb");
    fread(&aux.seminte.fileMarker1,sizeof(unsigned char),1,f);
    fread(&aux.seminte.fileMarker2,sizeof(unsigned char),1,f);
    fread(&aux.seminte.bfSize,4,1,f);
    fread(&aux.seminte.unused1,2,1,f);
    fread(&aux.seminte.unused2,2,1,f);
    fread(&aux.seminte.imageDataOffset,4,1,f);
    fread(&aux.seminte.biSize,4,1,f);
    fread(&aux.seminte.width,4,1,f);
    fread(&aux.seminte.height,4,1,f);
    fread(&aux.seminte.planes,2,1,f);
    fread(&aux.seminte.bitPix,2,1,f);
    fread(&aux.seminte.biCompression,4,1,f);
    fread(&aux.seminte.biSizeImage,4,1,f);
    fread(&aux.seminte.biXPelsPerMeter,4,1,f);
    fread(&aux.seminte.biYPelsPerMeter,4,1,f);
    fread(&aux.seminte.biClrUsed,4,1,f);
    fread(&aux.seminte.biClrImportant,4,1,f);

    int w=aux.seminte.width;
    int h=aux.seminte.height;

    aux.info=(Pixel*)malloc(sizeof(Pixel)*w*h);

    for(i = h - 1; i > 0; i--)
        for(j = 0; j < w; j++)
            fread(&aux.info[i*w + j], sizeof(Pixel), 1, f);
    return aux;
}
void afisare(char* cale,imagine aux)
{

    int i,j;
    FILE* f = fopen(cale, "wb+");
    //printf("%d",aux.seminte.bitPix);
    fwrite(&aux.seminte.fileMarker1,1,1,f);
    fwrite(&aux.seminte.fileMarker2,1,1,f);
    fwrite(&aux.seminte.bfSize,4,1,f);
    fwrite(&aux.seminte.unused1,2,1,f);
    fwrite(&aux.seminte.unused2,2,1,f);
    fwrite(&aux.seminte.imageDataOffset,4,1,f);
    fwrite(&aux.seminte.biSize,4,1,f);
    fwrite(&aux.seminte.width,4,1,f);
    fwrite(&aux.seminte.height,4,1,f);
    fwrite(&aux.seminte.planes,2,1,f);
    fwrite(&aux.seminte.bitPix,2,1,f);
    fwrite(&aux.seminte.biCompression,4,1,f);
    fwrite(&aux.seminte.biSizeImage,4,1,f);
    fwrite(&aux.seminte.biXPelsPerMeter,4,1,f);
    fwrite(&aux.seminte.biYPelsPerMeter,4,1,f);
    fwrite(&aux.seminte.biClrUsed,4,1,f);
    fwrite(&aux.seminte.biClrImportant,4,1,f);

    int w=aux.seminte.width;
    int h=aux.seminte.height;

    for (i = h - 1; i >= 0; i--)
        for (j = 0; j < w + ((4 - (w % 4)) % 4); j++)
        {
            if (j < w)
            {
                fwrite(&aux.info[i*w + j], sizeof(Pixel), 1, f);
            }
            else
            {
                fwrite(0, sizeof(int), 1, f);
            }
        }

}
uint32_t* inversa(uint32_t*v,imagine aux)
{
    int W = aux.seminte.width;
    int H = aux.seminte.height;
    int    n=W*H,i,j;
    uint32_t* v2;
    v2=(uint32_t*)malloc(sizeof(uint32_t)*n);
    for ( i = 0; i < n; i++)
        v2[v[i]] = i ;


    return v2;
}
uint32_t* cript_a(  imagine aux)
{
    uint32_t *v;
    int W = aux.seminte.width;
    int H = aux.seminte.height;
    int i,x;
    FILE* input;
    input = fopen("secret_key.txt", "rt");
    fscanf(input, "%d", &x);
    v=malloc((2*W*H+1)*sizeof(int));
    v[0]=W*H;
    v[0]=xorshift32(x);
    for(i=1; i<2*W*H-1; i++)
    {
        v[i]=xorshift32(v[i-1]);

    }
    return v;
    fclose(input);
}
unsigned char nth(uint32_t number,int n)
{
    unsigned char x = (number >> (8*n)) & 0xff;
    return x;
}
uint32_t *cript_b(uint32_t *a,imagine aux)
{
    int W = aux.seminte.width;
    uint32_t *b;
    int H = aux.seminte.height;
    int bux;
    int i,n;
    n=W*H;

    b=(uint32_t*)malloc(n*sizeof(uint32_t));
    for(i=n-1; i>=0; i--)
        b[i]=i;
    for (i=n-1; i>=1; i--)
    {
        bux=b[i];
//printf("%u ",a[n-i]);
        b[i]=b[a[n-i]%(i+1)];
        b[a[n-i]%(i+1)]=bux;
    }
    return b;
}
Pixel* cript_c(uint32_t *v,imagine aux)
{
    int W = aux.seminte.width;
    int H = aux.seminte.height;
    Pixel *p2;
    int k;
    int    n=W*H;
    p2=(Pixel*)malloc(n*sizeof(Pixel));
    for(k=1; k<n; k++)
    {
        p2[v[k]].r=aux.info[k].r;
        p2[v[k]].g=aux.info[k].g;
        p2[v[k]].b=aux.info[k].b;
    }
    return p2;
}
Pixel* cript_d(Pixel* p2,uint32_t *v,imagine aux)
{
    FILE* input;
    input = fopen("secret_key.txt", "rt");
    int W = aux.seminte.width;
    int H = aux.seminte.height;
    int i,x;
    int    n=H*W;
    Pixel* c=malloc(sizeof(Pixel)*n);
    fscanf(input, "%d", &x);
    fscanf(input, "%d", &x);

    c[0].r=nth(x,2)^p2[0].r^nth(v[n],2);
    c[0].g=nth(x,1)^p2[0].g^nth(v[n],1);
    c[0].b=nth(x,0)^p2[0].b^nth(v[n],0);

    for(i=1; i<n; i++)
    {
        c[i].r=c[i-1].r^p2[i].r^nth(v[n+i],2);
        c[i].g=c[i-1].g^p2[i].g^nth(v[n+i],1);
        c[i].b=c[i-1].b^p2[i].b^nth(v[n+i],0);
    }
    return c;
}
Pixel* decript_c(Pixel* c,uint32_t *v,imagine aux)
{
    FILE* input;
    input = fopen("secret_key.txt", "rt");
    int W = aux.seminte.width;
    int H = aux.seminte.height;
    int i,x;
    int    n=H*W;
    Pixel* c2=malloc(sizeof(Pixel)*n);
    fscanf(input, "%d", &x);
    fscanf(input, "%d", &x);

    c2[0].r=nth(x,2)^c[0].r^nth(v[n],2);
    c2[0].g=nth(x,1)^c[0].g^nth(v[n],1);
    c2[0].b=nth(x,0)^c[0].b^nth(v[n],0);

    for(i=1; i<n; i++)
    {
        c2[i].r=c[i-1].r^c[i].r^nth(v[n+i],2);
        c2[i].g=c[i-1].g^c[i].g^nth(v[n+i],1);
        c2[i].b=c[i-1].b^c[i].b^nth(v[n+i],0);
    }
    return c2;
}
Pixel* decript_d(Pixel *p,uint32_t *v,imagine aux)
{
    int W = aux.seminte.width;
    int H = aux.seminte.height;
    int k;
    int    n=W*H;
    Pixel*p2=malloc(sizeof(Pixel)*n);
    for(k=1; k<n; k++)
    {
        p2[v[k]].r=p[k].r;
        p2[v[k]].g=p[k].g;
        p2[v[k]].b=p[k].b;
    }
    return p2;
}
void testchi(imagine aux){
double *r,*g,*b,R=0,G=0,B=0,fb;
int i;
 int W = aux.seminte.width;
    int H = aux.seminte.height;
    int k;
    int    n=W*H;
    fb=n/256;
r=malloc(sizeof(double)*256);
g=malloc(sizeof(double)*256);
b=malloc(sizeof(double)*256);
for(i=0;i<=255;i++)
{
    r[i]=0;
    g[i]=0;
    b[i]=0;
}
for(i=0;i<n;i++)
{
    r[aux.info[i].r]++;
    g[aux.info[i].g]++;
    b[aux.info[i].b]++;

}
for(i=0;i<=255;i++){
R+=((r[i]-fb)*(r[i]-fb))/fb;
G+=((g[i]-fb)*(g[i]-fb))/fb;
B+=((b[i]-fb)*(b[i]-fb))/fb;
}
printf("%lf %lf %lf",R,G,B);

}
int main()
{
    imagine koala=citire("peppers.bmp");testchi(koala);
    koala.info=decript_d(decript_c(cript_d(cript_c(cript_b(cript_a(koala),koala),koala),cript_a(koala),koala),cript_a(koala),koala),inversa(cript_b(cript_a(koala),koala),koala),
                         koala);
    afisare("peppers3.bmp",koala);

    return 0;
}
