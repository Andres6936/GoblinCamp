// Joan Andrés (@Andres6936) Github.

#include <Goblin/Util/Flossy.hpp>
#include <Goblin/Util/Statistics.hpp>
#include <Goblin/Mechanism/Stats.hpp>
#include <Goblin/User/Interface/Frame.hpp>
#include <Goblin/User/Interface/Label.hpp>
#include <Goblin/User/Interface/UIList.hpp>
#include <Goblin/User/Interface/Button.hpp>
#include <Goblin/User/Interface/Dialog.hpp>
#include <Goblin/User/Interface/UIContainer.hpp>
#include <Goblin/User/Interface/ScrollPanel.hpp>

#include "Goblin/Screen/StatisticsDisplay.hpp"

#include <functional>
#include <unordered_map>

using namespace Goblin;

namespace
{
	void DrawText(std::pair<std::string, unsigned> text, int count, int x, int y, int width,
			bool selected, TCODConsole* console)
	{
		console->print(x, y, flossy::format("{} : {}", text.first, text.second).data());
	}

	void DrawDeathText(std::pair<std::string, unsigned> text, int count, int x, int y, int width,
			bool selected, TCODConsole* console)
	{
		console->print(x, y, flossy::format("{} : {}", text.second, text.first).data());
	}
}

void StatisticsDisplay::Show(const Statistics& statistics)
{
	UIContainer* contents = new UIContainer( 0, 0, 77, 39);
	Dialog* statDialog = new Dialog(contents, "Statistics", 77, 41);

	Label* points = new Label(flossy::format("Points: {}", statistics.GetScore()), 1, 2, TCOD_LEFT);
	contents->AddComponent(points);

	Frame* filthFrame = new Frame("Filth", 1, 4, 25, 4);
	filthFrame->AddComponent(
			new Label(flossy::format("created: {}", Stats::Inst()->GetFilthCreated()), 1, 1,
					TCOD_LEFT));
	filthFrame->AddComponent(
			new Label(flossy::format("off-map: {}", Stats::Inst()->GetFilthFlownOff()), 1, 2,
					TCOD_LEFT));
	contents->AddComponent(filthFrame);

	Label* burntItems = new Label(
			flossy::format("Burnt items: {}", statistics.GetAmountItemsBurned()), 1, 9, TCOD_LEFT);
	contents->AddComponent(burntItems);

	using namespace std::placeholders;  // for _1, _2, _3...

	Frame* productionFrame = new Frame("Production",  26, 1, 25, 34);
	productionFrame->AddComponent(
			new Label(flossy::format("items: {}", statistics.GetAmountItemsBuilt()), 1, 1,
					TCOD_LEFT));
	productionFrame->AddComponent(new ScrollPanel(1, 2, 23, 15,
			new UIList<std::unordered_map<std::string, unsigned> >(
					&Stats::Inst()->itemsBuilt, 0, 0, 24, Stats::Inst()->itemsBuilt.size(),
					std::bind(DrawText, _1, _2, _3, _4, _5, _6, _7), 0, false, 0)));

	productionFrame->AddComponent(
			new Label(flossy::format("constructions: {}", statistics.GetAmountConstructionsBuilt()),
					1, 17, TCOD_LEFT));
	productionFrame->AddComponent(new ScrollPanel(1, 18, 23, 15,
			new UIList<std::unordered_map<std::string, unsigned> >(
					&Stats::Inst()->constructionsBuilt, 0, 0, 24,
					Stats::Inst()->constructionsBuilt.size(),
					std::bind(DrawText, _1, _2, _3, _4, _5, _6, _7), 0, false, 0)));
	contents->AddComponent(productionFrame);

	Frame* deathFrame = new Frame("Deaths",  51, 1, 25, 34);
	deathFrame->AddComponent(new ScrollPanel(1, 1, 23, 32,
			new UIList<std::unordered_map<std::string, unsigned> >(
					&Stats::Inst()->deaths, 0, 0, 24, Stats::Inst()->deaths.size(),
					std::bind(DrawDeathText, _1, _2, _3, _4, _5, _6, _7), 0, false, 0)));
	contents->AddComponent(deathFrame);

	Button* okButton = new Button("OK", NULL, 33, 37, 10, 'o', true);
	contents->AddComponent(okButton);

	statDialog->ShowModal();
}
