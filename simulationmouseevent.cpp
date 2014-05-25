#include "simulationmouseevent.h"

SimulationMouseEvent::SimulationMouseEvent()
{
#ifdef __linux

#elif _WIN32

#else
#error Platform not supported
#endif
}

void SimulationMouseEvent::moveMouse(QPointF deltaPoint)
{
    QCursor::setPos (QCursor::pos().x() + deltaPoint.x(), QCursor::pos().y() + deltaPoint.y());
}

void SimulationMouseEvent::rightMouseClick()
{
    mouseButtonClick(Button3);
}

void SimulationMouseEvent::leftMouseClick()
{
    mouseButtonClick(Button1);
}

void SimulationMouseEvent::scrollMouseClick()
{
    mouseButtonClick(Button2);
}

void SimulationMouseEvent::rightMouseDown()
{
    mouseButtonEvent(Button3, true);
}

void SimulationMouseEvent::rightMouseUp()
{
    mouseButtonEvent(Button3, false);
}

void SimulationMouseEvent::leftMouseDown()
{
    mouseButtonEvent(Button1, true);
}

void SimulationMouseEvent::leftMouseUp()
{
    mouseButtonEvent(Button1, false);
}

void SimulationMouseEvent::upMouseScroll()
{
 //   mouseButtonClick(Button5);
    mouseButtonEvent(Button5,true);
}

void SimulationMouseEvent::downMouseScroll()
{
    //mouseButtonClick(Button4);
    mouseButtonEvent(Button4,true);

}

void SimulationMouseEvent::mouseButtonClick(int button)
{
#ifdef __linux
    Display *display = XOpenDisplay(NULL);
    XEvent event;
    if(display == NULL){
        return;
    }

    memset(&event, 0x00, sizeof(event));

    event.type = ButtonPress;
    event.xbutton.button = button;
    event.xbutton.same_screen = True;
    //event.xcrossing.focus = True;
    event.xmotion.send_event = true;
    event.xcrossing.focus = True;
    XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

    event.xbutton.subwindow = event.xbutton.window;

    while(event.xbutton.subwindow){
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
    }

    if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0){
        fprintf(stderr, "Errore nell'invio dell'evento !!!\n");
    }

    XFlush(display);
    usleep(100000);

    event.type = ButtonRelease;
    event.xbutton.state = 0x100;

    if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0){
        fprintf(stderr, "Errore nell'invio dell'evento !!!\n");
    }

    XFlush(display);
    XCloseDisplay(display);
#elif _WIN32

#else
#error Platform not supported
#endif
}

void SimulationMouseEvent::mouseButtonEvent(int button, bool isDown)
{
#ifdef __linux
    Display *display = XOpenDisplay(NULL);
    XEvent event;
    if(display == NULL){
        return;
    }

    memset(&event, 0x00, sizeof(event));

    event.type = isDown ? ButtonPress : ButtonRelease;
    event.xbutton.button = button;
    event.xbutton.same_screen = True;
   // event.xfocus.send_event = true;
    //event.xfocus.window = event.xbutton.subwindow;
    event.xmotion.send_event = true;
    event.xcrossing.focus = True;
    XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

    event.xbutton.subwindow = event.xbutton.window;

    while(event.xbutton.subwindow){
        event.xbutton.window = event.xbutton.subwindow;

        XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
    }

    if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0){
        fprintf(stderr, "Errore nell'invio dell'evento !!!\n");
    }

    XFlush(display);
    XCloseDisplay(display);
#elif _WIN32

#else
#error Platform not supported
#endif
}
