#include <SDL/SDL.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <SDL/SDL_ttf.h>

#include <math.h>
int mandelbrot(int x, int y, double  p_r, double p_i);
void renduLigne(SDL_Surface *SurfMand,  SDL_Surface *image);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void dessinerRectangle(SDL_Surface *surface, SDL_Rect *rect, Uint32 couleur);

const int HEIGHT = 600 ,WIDTH = 800;
double ratioEcran = (double)HEIGHT/(double)WIDTH;
long double zoom= 0.4, start_x =-0.75, start_y= 0;
long double Pzoom= 0.4, Pstart_x =-0.75, Pstart_y= 0;

unsigned int MAX_ITERATION = 300;
int y =0;
int color = 4;

int pressed = 0;
int format;

int main(int argc, char* argv[])
{
    SDL_Surface *ecran;
    
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    ecran = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE);
    SDL_WM_SetCaption("Ma super fenêtre SDL !", NULL);

    SDL_Surface *image = SDL_LoadBMP("material.bmp");
    SDL_Surface *SurfMand =  SDL_CreateRGBSurface(0,WIDTH,HEIGHT,32,0,0,0,0);
    SDL_LockSurface(image);

    SDL_Rect position = {0,0,0,0};
    SDL_Rect position2 = {0,0,0,0};
    
    SDL_Rect *rect;
    TTF_Font *font=TTF_OpenFont("arial.ttf", 12);
    if(!font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
    }
      SDL_Color couleur;
      couleur.r = 255;

    SDL_Surface *texte;

    SDL_Event events;
    int isOpen = 1;

    while (isOpen)
    {
        while(SDL_PollEvent(&events) == 1)          
        {
            switch (events.type)
            {
                case SDL_QUIT:
                    isOpen = 0;
                    break;
                case SDL_KEYDOWN:
                    switch (events.key.keysym.sym)
                    {
                        case SDLK_DOWN:
                            MAX_ITERATION-=100;
                          
                            y=0;
                            break;
                        case SDLK_UP:
                            MAX_ITERATION+=100;
                            y=0;
                            break;
                        case SDLK_ESCAPE:
                            zoom =Pzoom;
                            start_x=Pstart_x;
                            start_y =Pstart_y;
                          
                            y=0;
                            break;
                        case SDLK_a:
                            color+=1;
                          
                            y=0;
                            break;
                        case SDLK_q:
                            color-=1;
                            y=0;
                            break;
                    }
                    break;
                    

                case SDL_MOUSEBUTTONDOWN:
                    
                    
                        rect->x = events.button.x;
                        rect->y = events.button.y;
                        rect->w = 0;
                        rect->h = 0;
                        pressed = 1;
                    
                    break;
                    

                    
                case SDL_MOUSEBUTTONUP:
                    
                    pressed = 0;
                    if (rect->h !=0 &&rect->w !=0)
                    {
                        Pzoom = zoom; Pstart_x =start_x; Pstart_y = start_y;
                        double cx = (rect->x+ rect->w/2) -WIDTH/2 ;
                        double cy = (rect->y+rect->h/2) -HEIGHT/2;
                        start_x = cx/zoom/(double) WIDTH+ start_x ;
                        start_y = cy/zoom/(double)HEIGHT+start_y;
                        zoom*= WIDTH/rect->w;
                        y = 0;
                    }
                    
                    
                    break;
                case SDL_MOUSEMOTION:
                    rect->w = events.motion.x - rect->x;
                    rect->h = (events.motion.x - rect->x)*(ratioEcran);

                    
                    
                    
            }
        }
        renduLigne(SurfMand, image);
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format,0,0,0));

        position.x=0;
        position.y=0;
        
        SDL_BlitSurface(SurfMand, NULL, ecran, &position); // Collage de la surface sur l'écran
        
        if(pressed){
            dessinerRectangle(ecran, rect, SDL_MapRGB(ecran->format,255,0,0));
        }
        char txt[100];
        sprintf(txt, "x = %g y = %g zoom = %g", start_x,start_y,zoom);
        
        texte  = TTF_RenderText_Solid(font, txt, couleur);
        position2.y = HEIGHT- texte->h;
        SDL_BlitSurface(texte, NULL, ecran, &position2);
        SDL_FreeSurface(texte);

        SDL_Flip(ecran);
        y++;
        fprintf(stderr, "%s", SDL_GetError());
        fprintf(stderr, "%s", TTF_GetError());


    }

    SDL_FreeSurface(image);
    SDL_FreeSurface(SurfMand);
    SDL_FreeSurface(ecran);
    TTF_CloseFont(font); 

    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}

void renduLigne(SDL_Surface *SurfMand, SDL_Surface *image){
    SDL_LockSurface(SurfMand);
    Uint8 r,g,b;
    Uint32 *p = image->pixels;
    int pitch;
    Uint32 *pixelsMand = SurfMand->pixels;
    
    if(y<HEIGHT){
         double p_i = ( y-HEIGHT/2 )/ (zoom* HEIGHT)+start_y;


        for (int x = 0; x < WIDTH; ++x) {
             double p_r =  (x-WIDTH/2 )/ (zoom* WIDTH)+start_x;

            int i  = mandelbrot(x, y, p_r, p_i);
            if (i == MAX_ITERATION)
            {
                pixelsMand[y *WIDTH + x] = 0;
            }
            else
            {
                i= i*color;
               // i= (int)((double) ((double) i *image->w/ MAX_ITERATION));
                i= i%image->w;
                pixelsMand[y *WIDTH + x] = p[i];

                
            }

            
        }
    }


    SDL_UnlockSurface(SurfMand);

}



int mandelbrot(int x, int y, double  p_r, double p_i){
    unsigned int j = 0;
    long double r = 0, i = 0, temp_r = 0;

    while ((r * r + i * i) < 4.0 && j < MAX_ITERATION) {
        temp_r = r;
        r = temp_r * temp_r - i * i + p_r;
        i = 2.0 * temp_r * i + p_i;
        ++j;
    }
    return j;

}


void dessinerRectangle(SDL_Surface *surface, SDL_Rect *rect, Uint32 couleur){
    SDL_Rect r = {rect->x,rect->y,rect->w,1};
    SDL_FillRect(surface, &r, couleur);
    r.y = rect->y+rect->h;
    SDL_FillRect(surface, &r, couleur);
    r.y = rect->y;
    r.w = 1;
    r.h = rect->h;
    SDL_FillRect(surface, &r, couleur);
    r.x = rect->x+rect->w;
    SDL_FillRect(surface, &r, couleur);
    
}