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

#include "ui/markup/xultb_markup_item.h"

struct markup_item*xultb_markup_item_create(struct xultb_element*root, struct xultb_media_loader*loader, xultb_bool_t selectable, struct xultb_event_listener*el) {
	return NULL;
}

static void render_node(struct xultb_graphics*g, struct xultb_element*elem, struct xultb_font*font) {
	String tagName = elem.get_name(); /* Element name */
	struct xultb_font*newFont = font;
	int oldColor = g->get_color();

	if (tagName.equals("br")) {
		breakLine();
	} else if (tagName.equals("img")) {
		renderImage(elem);
	} else if (tagName.equals("b")) {
		newFont = Font.getFont(font.getFace(), font.getStyle()
				| Font.STYLE_BOLD, font.getSize());
	} else if (tagName.equals("i")) {
		newFont = Font.getFont(font.getFace(), font.getStyle()
				| Font.STYLE_ITALIC, font.getSize());
	} else if (tagName.equals("big")) {
		newFont
				= Font.getFont(font.getFace(), font.getStyle(), Font.SIZE_LARGE);
	} else if (tagName.equals("small")) {
		newFont
				= Font.getFont(font.getFace(), font.getStyle(), Font.SIZE_SMALL);
	} else if (tagName.equals("strong") || tagName.equals("em")) {
		/// \xxx what to do for strong text ??
		newFont = Font.getFont(font.getFace(), font.getStyle()
				| Font.STYLE_BOLD, Font.SIZE_MEDIUM);
	} else if (tagName.equals("u")) {
		newFont = Font.getFont(font.getFace(), font.getStyle()
				| Font.STYLE_UNDERLINED, font.getSize());
	} else if (tagName.equals("p")) {
		// line break
		breakLine();
		breakLine();
	} else if (tagName.equals("a")) {

		String link = elem.getAttributeValue(null, "href");

		// draw the anchor
		if (elem.getChildCount() == 0 || link == null) {
			// skip empty links
		} else if (isFocused(elem)) {
			// #expand g.setColor(%net.ayaslive.miniim.ui.core.markup.aFgHover%);
			g.setColor(0x0000FF);
			// #expand newFont = Font.getFont(font.getFace(), font.getStyle() | %net.ayaslive.miniim.ui.core.markup.aFontHover%, font.getSize());
			newFont = Font.getFont(font.getFace(), font.getStyle()
					| Font.STYLE_UNDERLINED | Font.STYLE_BOLD, font.getSize());
			// SimpleLogger.debug(this, "renderNode()\t\tFocused:" + elem.getChild(0));
		} else if (isActive(elem)) {
			// #expand g.setColor(%net.ayaslive.miniim.ui.core.markup.aFgActive%);
			g.setColor(0xCC99FF);

			// #expand newFont = Font.getFont(font.getFace(), font.getStyle() | %net.ayaslive.miniim.ui.core.markup.aFontActive%, font.getSize());
			newFont = Font.getFont(font.getFace(), font.getStyle()
					| Font.STYLE_UNDERLINED, font.getSize());
			// SimpleLogger.debug(this, "renderNode()\t\tActive:" + elem.getChild(0));
		} else {
			// #expand g.setColor(%net.ayaslive.miniim.ui.core.markup.aFg%);
			g.setColor(0x0000FF);

			// #expand newFont = Font.getFont(font.getFace(), font.getStyle() | %net.ayaslive.miniim.ui.core.markup.aFont%, font.getSize());
			newFont = Font.getFont(font.getFace(), font.getStyle()
					| Font.STYLE_UNDERLINED, font.getSize());
		}
	} else {
		// We do not know how to handle this element
		// SimpleLogger.debug(this, "renderNode()\t\tNothing to do for: " + tagName);
		// go on with inner elements
	}
	// render the inner nodes
	// System.out.println("<"+tagName+">");
	final int count = elem.getChildCount();
	for (int i = 0; i < count; i++) {
		switch (elem.getType(i)) {
		case Element.TEXT:
			renderText(newFont, (String) elem.getChild(i));
			break;
		case Element.ELEMENT:
			renderNode(newFont, (Element) elem.getChild(i));
			break;
		default:
			System.out.println("Nothing to do for " + elem.getChild(i));
			break;
		}
	}
	// System.out.println("</"+tagName+">");
	g.setColor(oldColor);
}

static void renderText(Font font, String text) {
	int off, ret, len;
	text = text.replace('\n', ' ').replace('\r', ' ').trim(); /*< skip the newlines */
	len = text.length();
	if (len == 0) { /*< empty string .. skip */
		return;
	}
	g.setFont(font);
	updateHeight(font);

	off = 0;
	while ((ret = Window.wrapNext(text, font, off, width - xPos)) != -1) {
		// draw the texts ..
		if (ret > off) {
			// draw the line
			g.drawString(text.substring(off, ret), xPos, yPos, Graphics.TOP
					| Graphics.LEFT);
			xPos += font.substringWidth(text, off, ret - off);
		}
		if (ret == off /* no place to write a word .. */
		|| ret < len /* there are more words so that we span into new line .. */
		|| width - xPos < 0 /* pushed too much */
		) {
			breakLine();
		}
		off = ret;
	}

	if (xPos != 0) {
		xPos += 4;/* finally add a space: 4px */
		if (width - xPos < 0) { /* pushed too much */
			breakLine();
		}
	}
}

static void renderImage(Element elem) {
	String src = elem.getAttributeValue(null, "src");
	if (src == null) {
		return;
	}
	Image img = loader.getImage(src);
	if (img == null) {
		return;
	}
	int imgWidth = img.getWidth();
	int imgHeight = img.getHeight();

	// so we can use it inline
	String position = elem.getAttributeValue(null, "p");
	if (position != null) {
		breakLine();
		if (position.equals("c")) {
			position = null;
		}
		g.drawImage(img, (position == null) ? width / 2 : xPos, yPos,
				Graphics.TOP
						| ((position == null) ? Graphics.HCENTER
								: position.equals("l") ? Graphics.LEFT
										: Graphics.RIGHT));
		updateHeight(imgHeight + PADDING);
		breakLine();
	} else {
		if (width - xPos < imgWidth) {
			breakLine();
		}
		g.drawImage(img, xPos, yPos, Graphics.TOP | Graphics.LEFT);

		// increase line height if the image height is larger ..
		imgHeight += PADDING;
		updateHeight(imgHeight > lineHeight ? imgHeight : lineHeight);

		xPos += imgWidth;
		xPos += 4;/* finally add a space: 4px */
		if (width - xPos < 0) { /* pushed too much */
			breakLine();
		}
	}
}

static int getActualCardHeight() {
	return yPos;
}

static void breakLine() {
	// put a line break
	yPos += lineHeight;
	xPos = 0;

	// reset line height to minimum
	lineHeight = minLineHeight;

	// clear the next line
	clearLine();
}

static void clearLine() {
	if (!selected)
		return;
	clearLine(yPos, lineHeight);
}

static void clearLine(int y, int height) {
	if (!selected)
		return;
	int oldColor = g.getColor();
	// #expand g.setColor(%net.ayaslive.miniim.ui.core.markup.bgHover%);
	g.setColor(0xCCCCCC);
	g.fillRect(0, y, width, height);
	g.setColor(oldColor);
}

static void updateHeight(Font font) {
	if (font == null) {
		return;
	}
	if (lineHeight < (font.getHeight() + PADDING)) {
		updateHeight(font.getHeight() + PADDING);
	}
}

static void updateHeight(int newHeight) {
	if (newHeight > lineHeight) {
		// fill with background color
		clearLine(yPos + lineHeight, newHeight - lineHeight);
	}
	lineHeight = newHeight;
}

/** List item implementation .. */
/*@{*/
static int paint(Graphics g, int x, int y, int width, boolean selected) {
	this.g = g;
	xPos = 0;
	yPos = 0;
	this.width = width;
	this.selected = selected;
	// #expand g.setColor(%net.ayaslive.miniim.ui.core.markup.fg%);
	g.setColor(0x006699);
	lineHeight = minLineHeight;
	g.translate(x, y);

	// draw the line background
	clearLine();

	// draw the node recursively
	renderNode(
			Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_SMALL),
			root);

	int ret = this.yPos;
	if (xPos != 0) {
		ret = this.yPos + lineHeight;
	}

	// fix the position and font
	g.translate(-x, -y);
	g.setFont(ITEM_FONT);
	return ret;
}
