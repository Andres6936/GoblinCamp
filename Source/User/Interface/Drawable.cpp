/* Copyright 2010-2011 Ilkka Halila
This file is part of Goblin Camp.

Goblin Camp is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Goblin Camp is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with Goblin Camp. If not, see <http://www.gnu.org/licenses/>.*/

#include <string>
#include <cmath>

#include <libtcod.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "Goblin/User/Interface/Drawable.hpp"
#include "UI.hpp"
#include <Goblin/Config/WindowConfig.hpp>

using namespace Goblin;

void Panel::selected(int newSel) {}
void Panel::Open() {}
void Panel::Close() {
	UI::Inst()->SetTextMode(false);
}

void Panel::ShowModal() {
	TCODConsole* background = new TCODConsole(WindowConfig::getWidth(),
			WindowConfig::getHeight());
	TCODConsole::blit(TCODConsole::root, 0, 0, WindowConfig::getWidth(),
			WindowConfig::getHeight(),
			background, 0, 0);

	int _x = (WindowConfig::getWidth() - width) / 2;
	int _y = (WindowConfig::getHeight() - height) / 2;
	TCOD_key_t key;
	TCOD_mouse_t mouseStatus;
	TCODMouse::showCursor(true);
	while (true)
	{
		TCODConsole::root->clear();
		TCODConsole::root->setDefaultForeground(TCODColor::white);
		TCODConsole::root->setDefaultBackground(TCODColor::black);
		TCODConsole::blit(background, 0, 0, WindowConfig::getWidth(), WindowConfig::getHeight(),
				TCODConsole::root, 0, 0, 0.7F, 1.0F);

		Draw(_x, _y, TCODConsole::root);
		TCODConsole::root->flush();

		key = TCODConsole::checkForKeypress();
		mouseStatus = TCODMouse::getStatus();

		MenuResult result = Update(mouseStatus.cx, mouseStatus.cy, mouseStatus.lbutton_pressed != 0,
				key);
		if ((result & DISMISS) || key.vk == TCODK_ESCAPE)
		{
			delete this;
			return;
		}
	}    
}

void UIContainer::AddComponent(Drawable *component) {
	components.push_back(component);
}

void UIContainer::Draw(int x, int y, TCODConsole *console) {
	for(std::vector<Drawable *>::iterator it = components.begin(); it != components.end(); it++) {
		Drawable *component = *it;
		if(component->Visible()) {
			component->Draw(x + _x, y + _y, console);
		}
	}
}

MenuResult UIContainer::Update(int x, int y, bool clicked, TCOD_key_t key) {
	for(std::vector<Drawable *>::iterator it = components.begin(); it != components.end(); it++) {
		Drawable *component = *it;
		if(component->Visible()) {
			MenuResult result = component->Update(x - _x, y - _y, clicked, key);
			if(!(result & NOMENUHIT)) {
				return result;
			}
		}
	}

	if (x >= _x && x < _x + width && y >= _y && y < _y + height) {
		return MENUHIT;
	}

	return NOMENUHIT;
}

void UIContainer::GetTooltip(int x, int y, Tooltip *tooltip) {
	Drawable::GetTooltip(x, y, tooltip);
	for(std::vector<Drawable *>::iterator it = components.begin(); it != components.end(); it++) {
		Drawable *component = *it;
		if(component->Visible()) {
			component->GetTooltip(x - _x, y - _y, tooltip);
		}
	}
}

UIContainer::~UIContainer() {
	for(std::vector<Drawable *>::iterator it = components.begin(); it != components.end(); it++) {
		delete *it;
	}
}
