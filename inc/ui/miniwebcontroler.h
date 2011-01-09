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

#if 0
package net.ayaslive.miniim.ui;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Hashtable;

import javax.microedition.io.ContentConnection;
import javax.microedition.lcdui.Image;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreFullException;
import javax.microedition.rms.RecordStoreNotFoundException;
import javax.microedition.rms.RecordStoreNotOpenException;

import org.kxml2.kdom.Document;
import org.kxml2.kdom.Element;

import net.ayaslive.miniim.common.Account;
import net.ayaslive.miniim.common.Contact;
import net.ayaslive.miniim.ui.core.Window;
import net.ayaslive.miniim.ui.markup.extended.DefaultComplexListener;
import net.ayaslive.miniim.ui.markup.extended.MarkupList;
import net.ayaslive.miniim.utils.io.resource.ResourceConnector;
import net.ayaslive.miniim.utils.io.resource.ResourceFetcher;
import net.ayaslive.miniim.utils.io.resource.ResourceIdentifier;
import net.ayaslive.miniim.utils.simplelogger.SimpleLogListener;
import net.ayaslive.miniim.utils.simplelogger.SimpleLogger;
#endif

struct miniwebcontroler // extends DefaultComplexListener implements ResourceConnector
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
}

