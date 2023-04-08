#include "sw.hpp"
#include <iostream>
#include "random"

using namespace dsl;

const uint16_t height = 50;
const uint16_t width = 50;

bool gridNow = false;

bool start = false;

bool grid[2][height][width];

uint8_t tick = 0;
uint8_t speed = 5;

void randomize(){
    for(uint16_t i = 0; i < height; i++){
        for(uint16_t j = 0; j < width; j++){
            grid[gridNow][i][j] = (rand()%7)==1;
        }
    }
}

void newGen(){
    gridNow = !gridNow;
    for(uint16_t i = 0; i < height; i++){
        for(uint16_t j = 0; j < width; j++){
            grid[gridNow][i][j] = false;
        }
    }
    for(uint16_t i = 0; i < height; i++){
        for(uint16_t j = 0; j < width; j++){
            uint8_t hStart = i-1;
            uint8_t hLenght = 3;
            uint8_t wStart = j-1;
            uint8_t wLenght = 3;
            if(i==0){
                hStart = 0;
                hLenght--;
            }
            if(i==height-1){
                hLenght--;
            }
            if(j==0){
                wStart = 0;
                wLenght--;
            }
            if(j==width-1){
                wLenght--;
            }
            uint8_t count = 0;
            bool alive = grid[!gridNow][i][j];
            for(uint8_t h = hStart; h < hStart+hLenght; h++){
                for(uint8_t w = wStart; w < wStart+wLenght; w++){
                    if(grid[!gridNow][h][w])count++;
                }
            }
            if(alive){
                if(count==3||count==4){
                    grid[gridNow][i][j] = true;
                }else{
                    grid[gridNow][i][j] = false;
                }
            }else{
                if(count==3){
                    grid[gridNow][i][j] = true;
                }
            }
        }
    }
}

void frame(dsl::ctx8888& ctx){
    tick=(tick+1)%speed;
    if(tick==0&&start){
        newGen();
    }
    ctx.fill(dsl::argb8888(0,0,0));
    for(uint16_t i = 0; i < height; i++){
        for(uint16_t j = 0; j < width; j++){
            
            if(grid[gridNow][i][j])
                ctx.fillRect(j*10+1,i*10+1,8,8,dsl::argb8888(255,255,255));
        }
    }
    ctx.fillRect(0,height*10,(width*10)-5,10,dsl::argb8888(255*!start,255*start,0));
    ctx.fillRect((width*10)-5,height*10,5,10,dsl::argb8888(255,255,0));
}

void clear(){
    for(uint16_t i = 0; i < height; i++){
        for(uint16_t j = 0; j < width; j++){
            grid[gridNow][i][j] = false;
        }
    }
}

bool isInside(mousePos point,int32_t x,int32_t y,int32_t w,int32_t h){
    if(point.x>=x&&point.x<x+w&&point.y>=y&&point.y<y+h){
        return true;
    }
    return false;
}

mousePos last;

bool down = false;

void mouseDown(ctx8888& ctx,mousePos pos){
    down = true;
    if(pos.y<height*10){
        
        for(uint16_t i = 0; i < height; i++){
            for(uint16_t j = 0; j < width; j++){
                
                if(isInside(pos,j*10,i*10,10,10)){
                    grid[gridNow][i][j] = !grid[gridNow][i][j];
                    last.x = j;
                    last.y = i;
                }
            }
        }
    }else{
        if(pos.x<(width*10)-5){
            start = !start;
        }else{
            clear();
        }
    }
    
}

void mouseUp(dsl::ctx8888& ctx,mousePos pos){
    down = false;
    last.x = -1;
    last.y = -1;
}

void mouseMove(dsl::ctx8888& ctx,mousePos pos){
    if(down){
        if(pos.y<height*10){
            for(uint16_t i = 0; i < height; i++){
                for(uint16_t j = 0; j < width; j++){
                    
                    if(isInside(pos,j*10,i*10,10,10)){
                        if(last.x==j&&last.y==i)continue;
                        grid[gridNow][i][j] = !grid[gridNow][i][j];
                        last.x = j;
                        last.y = i;
                    }
                }
            }
        }
    }
}

void keyUp(dsl::ctx8888& ctx,char key){
    std::cout << "UP key : " << key << std::endl;
}
void keyDown(dsl::ctx8888& ctx,char key){
    std::cout << "DOWN key : " << key << std::endl;
    randomize();
}

int main(){
    uint8_t l = 0;
    
    srand((uint32_t)time(NULL));

    simpleWindow window(width*10,height*10+10,"graWzycie");
    window.setFrame(frame);
    window.setMouseDown(mouseDown);
    window.setMouseUp(mouseUp);
    window.setMouseMove(mouseMove);
    window.setKeyDown(keyDown);
    window.setKeyUp(keyUp);
    window.wait();
    std::cout << "papa" << std::endl;
    
    return 0;
}


#ifdef _WIN32
//brak terminala w windows
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    return main();
}

#endif