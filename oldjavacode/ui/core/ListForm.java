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


package net.ayaslive.miniim.ui.core;

import java.util.Enumeration;

import net.ayaslive.miniim.ui.core.list.ListItem;

public abstract class ListForm extends List implements Enumeration, ActionListener {
	private final int size;
	
	protected ListForm(String title, String defaultCommand, int size) {
		super(title, defaultCommand);
		this.size = size;
		setActionListener(this);
	}

	protected int getCount() {
		return size;
	}

	protected String getHint() {
		return null;
	}

	protected Enumeration getItems() {
		counter = 0;
		return this;
	}

	protected ListItem getListItem(Object obj) {
		return null;
	}

	/** Enumeration implementation */
	/*@{*/
	private int counter = 0;
	public boolean hasMoreElements() {
		return counter < size;
	}
	public Object nextElement() {
		return getItemAt(counter++);
	}
	/*@}*/
	
	abstract protected Object getItemAt(int counter);
}
