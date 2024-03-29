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

#include "Goblin/Task/Task.hpp"
#include "Goblin/Util/Game.hpp"
#include "Goblin/User/Interface/UIList.hpp"
#include "Goblin/User/Interface/NPCDialog.hpp"
#include "Goblin/User/Interface/ScrollPanel.hpp"

#include <Goblin/Config/WindowConfig.hpp>

using namespace Goblin;

Dialog* NPCDialog::npcListDialog = 0;

Dialog* NPCDialog::NPCListDialog()
{
	if (!npcListDialog)
	{
		npcListDialog = new Dialog(new NPCDialog(), "NPCs", WindowConfig::getWidth() - 20,
				WindowConfig::getHeight() - 20);
	}
	return npcListDialog;
}

NPCDialog::NPCDialog() : UIContainer(0, 0, WindowConfig::getWidth() - 20,
		WindowConfig::getHeight() - 20)
{
	AddComponent(new ScrollPanel(0, 0, width, height,
			new UIList<std::map<int, std::shared_ptr<NPC> > >(
					&(Game::Inst()->npcList), 0, 0, width - 2, height, NPCDialog::DrawNPC), false));
}

void NPCDialog::DrawNPC(std::pair<int, std::shared_ptr<NPC> > npci, int i, int x, int y, int width, bool selected,
		TCODConsole* console)
{
	console->print(x, y, "NPC: %d", npci.second->Uid());
	console->print(x + 11, y, "%s: %s",
			npci.second->currentJob().lock() ? npci.second->currentJob().lock()->name.c_str() : "No job",
			npci.second->currentTask() ? Job::ActionToString(npci.second->currentTask()->action).c_str() : "No task");
}
