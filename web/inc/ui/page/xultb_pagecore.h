/*
 * guicore.h
 *
 *  Created on: Jan 14, 2011
 *      Author: ayaskanti
 */

#ifndef XULTB_PAGECORE_H
#define XULTB_PAGECORE_H

#include "ui/xultb_guicore.h"

C_CAPSULE_START

/*
 * Some library initialization code especially
 * qt requires the exact argc address passsed into
 * the main() function.
 * */
int xultb_pagecore_system_init(int*argc, char *argv[]);
int xultb_pagecore_system_deinit();

C_CAPSULE_END

#endif /* XULTB_PAGECORE_H */
