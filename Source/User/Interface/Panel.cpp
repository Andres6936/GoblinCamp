// Joan Andrés (@Andres6936) Github.

#include "Goblin/User/Interface/Panel.hpp"
#include <Goblin/Config/WindowConfig.hpp>
#include "Goblin/Graphics/UI.hpp"

using namespace Goblin;

void Panel::selected(int newSel)
{
}

void Panel::Open()
{
}

void Panel::Close()
{
	UI::Inst()->SetTextMode(false);
}

void Panel::ShowModal()
{
	auto* background = new TCODConsole(WindowConfig::getWidth(),
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
		TCODConsole::flush();

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


