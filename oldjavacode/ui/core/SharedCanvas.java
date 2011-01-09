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

import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.GameCanvas;

import net.ayaslive.miniim.ui.core.list.ListItemFactory;

/**
 * The SharedCanvas is singleton wrapper of GameCanvas. It is invisible to the external packages ..
 *
 * Creational: Singleton
 * Structural: Adaptor (to Window system)
 * 
 * @author kamanashisroy
 */
final class SharedCanvas extends GameCanvas implements Runnable {
	Window win = null;
	private int width = 0;
	private int xPos = 0;
	static SharedCanvas singleInstance = null;
	
	SharedCanvas(int w, int h) {
		super(false);
		width = w - 6*Menu.PADDING;
		xPos = 5*Menu.PADDING;
		initBalloons();
	}
	
	void show(Window win) {
		this.win = win;
		show();
	}
	
	void show() {
		setFullScreenMode(true);
		repaint();
		Window.dis.setCurrent(this);
	}
	
	protected void keyPressed(int keyCode) {
		if(keyPressedBalloon()) {
			return;
		}
		win.keyPressed(keyCode, this.getGameAction(keyCode));
	}
	
	public void paint(Graphics g) {
		win.paint(g);
		paintBaloons(g);
	}

	/** Balloon list */
	/*@{*/
	private final Balloon[] balloons = new Balloon[6];
	private boolean hasBalloon = false;
	
	private final void initBalloons() {
		// initiate balloons cache
		for(int i = 0; i < balloons.length; i++) {
			balloons[i] = new Balloon();
		}
		
		// start the thread ..
		new Thread(this).start();
	}
	
	public final void pushBalloon(String message, Image img, int hash, long timeout) throws OutOfMemoryError {
		synchronized(balloons) {
			// see if the hash already exists ..
			for(int i = 0; i < balloons.length; i++) {
				if(!balloons[i].inuse) {
					continue;
				}
				if(balloons[i].hash != -1 && balloons[i].hash == hash) {
					balloons[i].reset(message, img, hash, timeout);
					// indicate that we have balloons to draw ..
					hasBalloon = true;
					repaint();
					return;
				}
			}
			
			for(int i = 0; i < balloons.length; i++) {
				if(balloons[i].inuse) {
					continue;
				}
				balloons[i].reset(message, img, hash, timeout);
				// indicate that we have balloons to draw ..
				hasBalloon = true;
				repaint();
				return;
			}
		}
		throw new OutOfMemoryError("No space available for baloons");
	}
	
	private final void paintBaloons(Graphics g) {
		// a quick check up to see if we have balloons
		if(!hasBalloon) {
			return;
		}
		// do not draw balloon while showing menu ..
		if(Menu.isActive()) {
			return;
		}
		ListItemFactory label = null;
		int y = Window.menuY - ListItemFactory.FONT_HEIGHT - ListItemFactory.DPADDING - Menu.PADDING;
		synchronized(balloons) {
			for(int i = 0; i < balloons.length; i++) {
				if(!balloons[i].inuse) {
					continue;
				}
				label = ListItemFactory.createLabel(balloons[i].message, balloons[i].img, true, true);
				y -= label.paint(g, xPos, y, width, true) + Menu.PADDING;
			}
			if(label != null) {
				label.free();
			}
		}
	}
	
	private boolean keyPressedBalloon() {
		synchronized(balloons) {
			balloons.notifyAll();
		}
		return false;
	}
	
	public void run() {
		long currentTM = 0, lastIM = 0;
		boolean wasBalloon = false;
		while(!Window.isDestroying()) {
			synchronized(balloons) {
				try {
					balloons.wait(10000); // wait 10 seconds ..
					currentTM = System.currentTimeMillis();
					if(currentTM - lastIM <= 1500) {
						// we have to sleep 1.5 second .. 
						continue;
					}
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				
				wasBalloon = hasBalloon;
				hasBalloon = false;
				// try to reset the balloons
				for(int i = 0; i < balloons.length; i++) {
					if(!balloons[i].inuse) {
						continue;
					}
					if(!balloons[i].tryReset(currentTM)) {
						// we have at least one balloon in use
						hasBalloon = true;
					}
				}
				
				if(hasBalloon || wasBalloon) {
					repaint();
				}
				
				// dereferenced image and balloon contents can free some memory ..
				System.gc();
				
				lastIM = currentTM;
			}
		}
	}
	/*@}*/
	
	/** @name Balloon */
	/*@{*/
	private final static class Balloon {
		private String message = null;
		private long initTM, timeout = 0;
		private boolean inuse = false;
		private Image img = null;
		int hash = -1;
		
		private void reset(String message, Image img, int hash, long timeout) {
			this.message = message;
			this.inuse = true;
			this.initTM = System.currentTimeMillis();
			this.timeout = timeout;
			this.img = img;
			this.hash = hash;
		}
		
		private boolean tryReset(long currentTM) {
			if((currentTM - initTM) < timeout) {
				return false;
			}
			// cleanup
			message = null;
			inuse = false;
			img = null;
			return true;
		}
	}
	/*@}*/
}
