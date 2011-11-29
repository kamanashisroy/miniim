/*
 * This file part of MiniIM.
 *
 * Copyright (C) 2007  Kamanashis Roy
 *
 * MiniIM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MiniIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MiniIM.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <QtGui>

//#ifdef __cplusplus
//extern "C" {
//#endif

#include "ui/core/xultb_window.h"

static QMainWindow*qwindow;
static QCoreApplication*app;
//static QGraphicsWidget*canvas;
int xultb_guicore_platform_init() {
    char argv[][10] = {"X"};
    int argc = 1;
    app = new QCoreApplication(argc, (char**)argv);
    //canvas = new QGraphicsWidget();
    //canvas->setSceneRect(0, 0, 800, 600);
    qwindow = new QMainWindow();
    qwindow->showMaximized();
}

int xultb_guicore_platform_show(struct xultb_window*win) {
	return 0;
}


//#ifdef __cplusplus
//}
//#endif
