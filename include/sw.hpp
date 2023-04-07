#pragma once

#ifdef _WIN32
#include <Windows.h>
#include <windowsx.h>
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#include <stdint.h>
#include "mutex.hpp"
#include "ctx.hpp"
#include <functional>
#include <thread>

struct mousePos{
    int32_t x;
    int32_t y;
};

#ifdef _WIN32

#endif

class simpleWindow{
        //zmienne okna
        #ifdef _WIN32
        HWND hwnd;
        WNDCLASSEX wc;
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        void DrawPixels();
        #else
        Display *display;
        int screen;
        Window window;
        XImage *image;
        GC gc;
        #endif
        //mouse
        bool isMouseDown = false;
        mousePos lastMousePosition = {0, 0};
        mousePos currentMousePosition = {0, 0};
        //thread Safe
        Lock mutex;
        //running
        bool running = true;
        //kontekst
        dsl::ctx8888 ctx;
        //eventy
        std::function<void(dsl::ctx8888&)> frame = [](dsl::ctx8888& ctx){};
        std::function<void(dsl::ctx8888&,char)> keyDown = [](dsl::ctx8888& ctx,char key){};
        std::function<void(dsl::ctx8888&,mousePos)> mouseDown = [](dsl::ctx8888& ctx,mousePos mouse){};
        std::function<void(dsl::ctx8888&,mousePos)> mouseUp = [](dsl::ctx8888& ctx,mousePos mouse){};
        std::function<void(dsl::ctx8888&,mousePos)> mouseMove = [](dsl::ctx8888& ctx,mousePos mouse){};
        void reffreshImg();
    public:
        uint32_t width;
        uint32_t height;
        simpleWindow(uint32_t width,uint32_t height,const char* name = " ");
        ~simpleWindow();
        dsl::ctx8888& getCTX(){return ctx;};
        void setFrame(std::function<void(dsl::ctx8888&)> frame);
        void setKeyDown(std::function<void(dsl::ctx8888&,char)> keyDown);
        void setMouseDown(std::function<void(dsl::ctx8888&,mousePos)> mouseDown);
        void setMouseUp(std::function<void(dsl::ctx8888&,mousePos)> mouseUp);
        void setMouseMove(std::function<void(dsl::ctx8888&,mousePos)> mouseMove);
        bool isRunning();
        void wait();
        void tframe(){frame(ctx);}
        void tkeyDown(char c){keyDown(ctx,c);};
        void tmouseDown(mousePos p){mouseDown(ctx,p);};
        void tmouseUp(mousePos p){mouseUp(ctx,p);};
        void tmouseMove(mousePos p){mouseMove(ctx,p);};
};

//definicje

#ifdef _WIN32

simpleWindow::simpleWindow(uint32_t width,uint32_t height,const char* name):ctx(width,height),width(width),height(height){
    

    std::thread([&](){
        HINSTANCE hInstance = GetModuleHandle(NULL);

        wc = {0};
        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = "simpleWindowClass";
        RegisterClassEx(&wc);

        RECT windowRect = { 0, 0, (int32_t)this->width, (int32_t)this->height };
        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
        int adjustedWidth = windowRect.right - windowRect.left;
        int adjustedHeight = windowRect.bottom - windowRect.top;

        HWND hwnd = CreateWindowEx(
            0,
            "simpleWindowClass",
            name,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, adjustedWidth, adjustedHeight,
            NULL, NULL, hInstance, NULL
        );
        
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
        
        ShowWindow(hwnd, SW_SHOWDEFAULT);
        UpdateWindow(hwnd);
        
        std::thread([&](){
            while(true){
                std::thread time([](){
                    std::this_thread::sleep_for(std::chrono::milliseconds(16));
                });
                tframe();
                InvalidateRect(hwnd,nullptr,0);
                time.join();
            }
            
        }).detach();
        
        MSG msg = { 0 };
        while (GetMessage(&msg, NULL, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        
    }).detach();

    
    
    
}

simpleWindow::~simpleWindow(){
    DestroyWindow(hwnd);
}

void simpleWindow::reffreshImg(){

}

void simpleWindow::DrawPixels()
{

}

LRESULT CALLBACK simpleWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    simpleWindow* pThis = (simpleWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    switch (uMsg) {
        case WM_DESTROY:
            abort();
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            uint32_t* framebuffer = (uint32_t*)pThis->getCTX().img;
            
            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = pThis->width;
            bmi.bmiHeader.biHeight = -(int32_t)pThis->height;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            bmi.bmiHeader.biSizeImage = pThis->width * pThis->height * sizeof(uint32_t);
            StretchDIBits(hdc,0,0,pThis->width,pThis->height,
                0,0,pThis->width,pThis->height,
                framebuffer,&bmi,DIB_RGB_COLORS,SRCCOPY
            );

            ReleaseDC(hwnd,hdc);

            
            
            EndPaint(hwnd, &ps);

            }
            break;
        case WM_KEYDOWN:
            // Call tkeyDown() with the pressed key code
            pThis->tkeyDown((char)wParam);
            break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            // Call tmouseDown() with the mouse position
            pThis->tmouseDown({ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
            break;

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            // Call tmouseUp() with the mouse position
            pThis->tmouseUp({ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
            break;

        case WM_MOUSEMOVE:
            // Call tmouseMove() with the mouse position
            pThis->tmouseMove({ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
            break;
        default:
             return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
   
}

#else

simpleWindow::simpleWindow(uint32_t width,uint32_t height,const char* name):ctx(width,height),width(width),height(height){
    display = XOpenDisplay(nullptr);
    if (display == nullptr) {
        throw std::runtime_error("Could not open display");
    }

    screen = DefaultScreen(display);
    window = XCreateWindow(display, RootWindow(display, screen), 0, 0, width, height, 0, CopyFromParent, InputOutput, CopyFromParent, 0, nullptr);

    if (window == None) {
        throw std::runtime_error("Could not create window");
        XCloseDisplay(display);
    }

    XStoreName(display, window, name);

    XSelectInput(display, window, StructureNotifyMask | ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    XMapWindow(display, window);

    XEvent eve;
    do {
        XNextEvent(display, &eve);
    } while (eve.type != MapNotify || eve.xmap.event != window);

    image = XCreateImage(display, DefaultVisual(display, screen), 24, ZPixmap, 0, (char *)ctx.img, width, height, 8, 0);

    gc = XCreateGC(display, window, 0, nullptr);

    std::thread([&](){
        while (isRunning()){
            WriteLock(mutex);
            std::thread time([](){
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            });
            frame(ctx);
            reffreshImg();
            time.join();
        }
    }).detach();

    std::thread([&](){
        XEvent event;
        while (isRunning()) {
            XNextEvent(display, &event);
            WriteLock(mutex);
            switch (event.type) {
                case KeyPress: {
                    char buffer[1];
                    KeySym keysym;
                    XLookupString(&event.xkey, buffer, sizeof(buffer), &keysym, nullptr);
                    if (buffer[0] != '\0') {
                        keyDown(ctx, buffer[0]);
                    }
                    break;
                }
                case ButtonPress: {
                    isMouseDown = true;
                    lastMousePosition.x = event.xbutton.x;
                    lastMousePosition.y = event.xbutton.y;
                    mouseDown(ctx, lastMousePosition);
                    break;
                }
                case ButtonRelease: {
                    isMouseDown = false;
                    lastMousePosition.x = event.xbutton.x;
                    lastMousePosition.y = event.xbutton.y;
                    mouseUp(ctx, lastMousePosition);
                    break;
                }
                case MotionNotify: {
                    currentMousePosition.x = event.xmotion.x;
                    currentMousePosition.y = event.xmotion.y;
                    mouseMove(ctx, currentMousePosition);
                    lastMousePosition.x = currentMousePosition.x;
                    lastMousePosition.y = currentMousePosition.y;
                    break;
                }
            }
        }
    }).detach();
    
};
simpleWindow::~simpleWindow(){
    WriteLock(mutex);
    running = false;
}

void simpleWindow::reffreshImg(){
    XResizeWindow(display, window, width, height);
    XFlush(display);
    XPutImage(display, window, gc, image, 0, 0, 0, 0, width, height);
}

#endif

bool simpleWindow::isRunning(){
    WriteLock(mutex);
    return running;
}

void simpleWindow::setFrame(std::function<void(dsl::ctx8888&)> frame){
    WriteLock(mutex);
    this->frame = frame;
};
void simpleWindow::setKeyDown(std::function<void(dsl::ctx8888&,char)> keyDown){
    WriteLock(mutex);
    this->keyDown = keyDown;
};
void simpleWindow::setMouseDown(std::function<void(dsl::ctx8888&,mousePos)> mouseDown){
    WriteLock(mutex);
    this->mouseDown = mouseDown;
};
void simpleWindow::setMouseUp(std::function<void(dsl::ctx8888&,mousePos)> mouseUp){
    WriteLock(mutex);
    this->mouseUp = mouseUp;
};
void simpleWindow::setMouseMove(std::function<void(dsl::ctx8888&,mousePos)> mouseMove){
    WriteLock(mutex);
    this->mouseMove = mouseMove;
};

void simpleWindow::wait(){
    while (isRunning()){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}