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
/*
 * qt_window.cpp
 *
 *  Created on: Jan 14, 2011
 *      Author: ayaskanti
 */
#include <QtGui>

#include "ui/core/xultb_window.h"

C_CAPSULE_START

static QApplication*app;
int xultb_guicore_platform_init(int*argc, char *argv[]) {
    app = new QApplication(*argc, argv);
    return 0;
}

int xultb_guicore_platform_walk(int UNUSED_VAR(ms)) {
    app->processEvents(0,100);
	return 0;
}

void xultb_log_helper(void*UNUSED_VAR(fdptr), const char *fmt, ...) {
//	int fd = *(int *)fdptr;
	va_list ap;
	char somebuff[256];
	va_start(ap, fmt);
	vsnprintf(somebuff, sizeof(somebuff), fmt, ap);
	va_end(ap);
	printf("%s", somebuff);
}


C_CAPSULE_END
