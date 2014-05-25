#ifndef SIMULATIONMOUSEEVENT_H
#define SIMULATIONMOUSEEVENT_H

#include <QPointF>
#include <QCursor>
#include <QScreen>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <QX11Info>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

class SimulationMouseEvent
{
public:
    SimulationMouseEvent();
    static void moveMouse(QPointF deltaPoint);
    static void rightMouseClick();
    static void leftMouseClick();
    static void scrollMouseClick();
    static void rightMouseDown();
    static void rightMouseUp();
    static void leftMouseDown();
    static void leftMouseUp();
    static void upMouseScroll();
    static void downMouseScroll();
private:
    static void mouseButtonClick(int button);
    static void mouseButtonEvent(int button, bool isDown);
};

#endif // SIMULATIONMOUSEEVENT_H
