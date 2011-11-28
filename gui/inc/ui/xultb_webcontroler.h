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

#include "core/config.h"

C_CAPSULE_START

struct xultb_webcontroler
{
	int initialize(void*data);
	int finalize(void*data);
	public boolean pushWrapper(ResourceIdentifier id, boolean back);
	public void handleContent(String url, Object obj);
	public Image getImage(String src);
	public ContentConnection open(String url);
	private void readSettings();
	private void saveSettings();
	static void show();
	public ContentConnection open(String url, Hashtable postData);
	public void trace(int type, String classDesc, int hashCode, int threadHashCode, String str, String exceptionClassDesc, String trace);
	public void changed();
	public void run();
};

C_CAPSULE_END
