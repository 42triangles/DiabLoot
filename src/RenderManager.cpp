#include "RenderManager.h"
#include "SystemManager.h"
#include "Core.h"

RenderManager::RenderManager(System& system)
	: mGameWindow{ sf::VideoMode(1920u, 1080u), "DiabLoot", sf::Style::None }
	, mSystem{ system }
	, mWindowCenter{ mGameWindow.getSize().x / 2.f, mGameWindow.getSize().y / 2.f }
{
	mGameWindow.setFramerateLimit(60);
}

sf::RenderWindow& RenderManager::GetWindow()
{
	return mGameWindow;
}

sf::Cursor& RenderManager::GetDefaultCursor()
{
	return mCursorOpen;
}

sf::Cursor& RenderManager::GetClosedCursor()
{
	return mCursorClosed;
}

void RenderManager::CustomizeGameWindow()
{
	// Cursor
	auto& defaultCursor = mSystem.AssetMgr.GetImage(IMAGES::DEFAULT_CURSOR);
	auto& closedCursor = mSystem.AssetMgr.GetImage(IMAGES::CLOSED_CURSOR);
	mCursorOpen.loadFromPixels(defaultCursor.getPixelsPtr(), defaultCursor.getSize(), { 0, 0 });
	mCursorClosed.loadFromPixels(closedCursor.getPixelsPtr(), closedCursor.getSize(), { 0, 0 });
	mGameWindow.setMouseCursor(mCursorOpen);

	// Window icon
	auto& widnowIcon = mSystem.AssetMgr.GetImage(IMAGES::ICON);
	mGameWindow.setIcon(widnowIcon.getSize().x, widnowIcon.getSize().y, widnowIcon.getPixelsPtr());
}

void RenderManager::MainMenuRender()
{
	Draw(mSystem.AssetMgr.GetSprite(SPRITES::MAINMENU));

	Draw(mSystem.GUIMgr.GetButton(BUTTONS::START_ID).GetSprite());
	Draw(mSystem.GUIMgr.GetButton(BUTTONS::START_ID).GetText());

	Draw(mSystem.GUIMgr.GetButton(BUTTONS::LOAD_GAME_ID).GetSprite());
	Draw(mSystem.GUIMgr.GetButton(BUTTONS::LOAD_GAME_ID).GetText());

	Draw(mSystem.GUIMgr.GetButton(BUTTONS::EXIT_ID).GetSprite());
	Draw(mSystem.GUIMgr.GetButton(BUTTONS::EXIT_ID).GetText());

	Draw(mSystem.GUIMgr.GetButton(BUTTONS::MUTE_BUTTON_ID).GetSprite());
}

void RenderManager::PlayStateRender()
{
	Draw(mSystem.GUIMgr.GetButton(BUTTONS::NEXT_LEVEL_ID).GetSprite());
	Draw(mSystem.GUIMgr.GetButton(BUTTONS::OPEN_INVENTORY_ID).GetSprite());
	Draw(mSystem.GUIMgr.GetButton(BUTTONS::EXIT_PLAY_ID).GetSprite());

	Draw(mSystem.GUIMgr.GetButton(BUTTONS::NEXT_LEVEL_ID).GetText());
	Draw(mSystem.GUIMgr.GetButton(BUTTONS::OPEN_INVENTORY_ID).GetText());
	Draw(mSystem.GUIMgr.GetButton(BUTTONS::EXIT_PLAY_ID).GetText());

	Draw(mSystem.GUIMgr.GetButton(BUTTONS::MUTE_BUTTON_ID).GetSprite());
	Draw(mSystem.GUIMgr.GetButton(BUTTONS::UPGRADE_BUTTON_ID).GetSprite());

	InventoryRender();
	RenderItemCollectionProgress();
}

void RenderManager::InventoryRender()
{

	if (!mSystem.InventoryMgr.isOpen())
	{
		return;
	}

	Draw(mSystem.AssetMgr.GetSprite(SPRITES::INVENTORY));

	// Inventory buttons
	Draw(mSystem.GUIMgr.GetButton(BUTTONS::INVENTORY_ID).GetSprite());
	Draw(mSystem.GUIMgr.GetButton(BUTTONS::SORT_BUTTON_ID).GetSprite());

	// Render the amount of gold
	sf::Text goldText = mSystem.AssetMgr.GetTextForItemID(ITEMID::GOLD);
	goldText.setString(std::to_string(mSystem.InventoryMgr.getGold()));
	goldText.setOrigin(-8.f, goldText.getLocalBounds().getSize().y + 2.f);
	goldText.setPosition({ 1365, 922 });
	mGameWindow.draw(goldText);

	auto& slots = mSystem.InventoryMgr.getItemSlots();
	auto& rects = mSystem.InventoryMgr.getSlotRects();

	int clickedDownIndex = mSystem.InventoryMgr.getClickedDownIndex();

	for (int i{ 0 }; i < slots.size(); i++)
	{
		if (i == clickedDownIndex)
		{
			continue;
		}
		if (slots[i].isEmpty())
		{
			continue;
		}
		sf::Color blue({ 82 , 075, 143 , 100 });
		sf::Color green({ 44 , 190, 52, 100 });

		rects[i].setFillColor(blue);

		if (mSystem.InventoryMgr.isMouseOverSlot())
		{
			if (mSystem.InventoryMgr.getMouseOverSlotIndex() == i)
			{
				rects[i].setFillColor(green);
			}
			else
			{
				rects[i].setFillColor(blue);
			}
		}

		mGameWindow.draw(rects[i]);

		sf::Sprite itemSprite = mSystem.AssetMgr.GetSpriteForItem(slots[i].getItemId().first);
		itemSprite.setOrigin({ itemSprite.getLocalBounds().getSize().x / 2.f, itemSprite.getLocalBounds().getSize().y / 2.f });
		itemSprite.setPosition(rects[i].getPosition());

		mGameWindow.draw(itemSprite);
	}
	// If mouse is over a valid slot, render item name and text box
	if (mSystem.InventoryMgr.isMouseOverSlot())
	{
		int index = mSystem.InventoryMgr.getMouseOverSlotIndex();
		sf::Text hoverText = mSystem.AssetMgr.GetTextForItemID(slots[index].getItemId().first);
		if (slots[index].getQuantity() > 1)
		{
			hoverText.setString(hoverText.getString() + '\n' + "Quantity: " + std::to_string(slots[index].getQuantity()));
		}
		hoverText.setOrigin(hoverText.getLocalBounds().getSize().x / 2, hoverText.getLocalBounds().getSize().y / 2);
		hoverText.setPosition(mSystem.InventoryMgr.getLastMousePos().x, mSystem.InventoryMgr.getLastMousePos().y - 20);
		hoverText.setColor(mSystem.AssetMgr.GetColorForItemText(slots[index].getItemId()));
		// Text box to got under text
		sf::RectangleShape textBox{ sf::Vector2f{hoverText.getGlobalBounds().getSize().x + FONTS::PADDING, hoverText.getGlobalBounds().getSize().y + FONTS::PADDING} };
		textBox.setOrigin(textBox.getLocalBounds().getSize().x / 2 + FONTS::ORIGIN_YOFFSET, (textBox.getLocalBounds().getSize().y / 2) + FONTS::ORIGIN_YOFFSET);
		textBox.setFillColor(mSystem.AssetMgr.GetTextboxColor());
		textBox.setPosition(hoverText.getPosition());
		mGameWindow.draw(textBox);
		mGameWindow.draw(hoverText);
	}
	// Render the currently clicked item for swapping
	if (mSystem.InventoryMgr.isItemSlotClicked())
	{
		ITEMID::ITEM clickedItem = mSystem.InventoryMgr.getItemIdOfSlotClicked();
		sf::Sprite clickedSprite = mSystem.AssetMgr.GetSpriteForItem(clickedItem);
		clickedSprite.setOrigin(clickedSprite.getLocalBounds().getSize().x / 2.f, clickedSprite.getLocalBounds().getSize().y / 2.f);
		clickedSprite.setColor({ 255, 255, 255, 150 });
		clickedSprite.setPosition(mSystem.InventoryMgr.getLastMousePos().x, mSystem.InventoryMgr.getLastMousePos().y);
		Draw(clickedSprite);
	}
}

void RenderManager::RenderLevel(Level& level)
{
	Draw(mSystem.AssetMgr.GetLevelMap(level.GetLevelId()));
	RenderChests(level.GetChests());
	RenderParticles(level.GetParticles());
	RenderItems(level.GetItems());
}

void RenderManager::DrawToolTip(sf::Vector2f mousePos)
{
	if (mSystem.GUIMgr.GetButton(BUTTONS::UPGRADE_BUTTON_ID).GetSprite().getGlobalBounds().contains(mousePos))
	{
		sf::Text tooltipText;
		tooltipText.setFont(mSystem.AssetMgr.GetFont(FONTS::LIGHT));
		tooltipText.setString("    Upgrade Level:\nAdd Chest to Level");
		tooltipText.setCharacterSize(25);
		tooltipText.setFillColor(sf::Color::White);

		sf::FloatRect textBounds = tooltipText.getLocalBounds();
		float textWidth = textBounds.width;
		float textHeight = textBounds.height;

		sf::RectangleShape backgroundRect;
		backgroundRect.setSize(sf::Vector2f(textWidth + 20.f, textHeight + 20.f));
		backgroundRect.setFillColor(sf::Color(0, 0, 0, 160));

		float xPos = mousePos.x - (backgroundRect.getSize().x / 2.f);
		float yPos = mousePos.y - backgroundRect.getSize().y - 10.f;
		backgroundRect.setPosition(xPos, yPos);

		tooltipText.setPosition(xPos + 10.f, yPos + 5.f);

		mSystem.RenderMgr.Draw(backgroundRect);
		mSystem.RenderMgr.Draw(tooltipText);
	}
	if (mSystem.GUIMgr.GetButton(BUTTONS::SORT_BUTTON_ID).GetSprite().getGlobalBounds().contains(mousePos))
	{
		sf::Text tooltipText;
		tooltipText.setFont(mSystem.AssetMgr.GetFont(FONTS::LIGHT));
		tooltipText.setString("Auto Sort Inventory");
		tooltipText.setCharacterSize(25);
		tooltipText.setFillColor(sf::Color::White);

		sf::FloatRect textBounds = tooltipText.getLocalBounds();
		float textWidth = textBounds.width;
		float textHeight = textBounds.height;

		sf::RectangleShape backgroundRect;
		backgroundRect.setSize(sf::Vector2f(textWidth + 20.f, textHeight + 20.f));
		backgroundRect.setFillColor(sf::Color(0, 0, 0, 160));

		float xPos = mousePos.x - (backgroundRect.getSize().x / 2.f);
		float yPos = mousePos.y - backgroundRect.getSize().y - 10.f;
		backgroundRect.setPosition(xPos, yPos);

		tooltipText.setPosition(xPos + 10.f, yPos + 5.f);

		mSystem.RenderMgr.Draw(backgroundRect);
		mSystem.RenderMgr.Draw(tooltipText);
	}
}

void RenderManager::DrawToolTipWarning(sf::Vector2f mousePos)
{
	if (mSystem.GUIMgr.GetButton(BUTTONS::START_ID).GetSprite().getGlobalBounds().contains(mousePos))
	{
		sf::Text tooltipText;
		tooltipText.setFont(mSystem.AssetMgr.GetFont(FONTS::LIGHT));
		tooltipText.setString("       WARNING!\nA New Game Erases\n  Your Progress.");
		tooltipText.setCharacterSize(25);
		tooltipText.setFillColor(sf::Color::White);

		sf::FloatRect textBounds = tooltipText.getLocalBounds();
		float textWidth = textBounds.width;
		float textHeight = textBounds.height;

		sf::RectangleShape backgroundRect;
		backgroundRect.setSize(sf::Vector2f(textWidth + 20.f, textHeight + 20.f));
		backgroundRect.setFillColor(sf::Color(0, 0, 0, 160));

		float xPos = mousePos.x - (backgroundRect.getSize().x / 2.f);
		float yPos = mousePos.y - backgroundRect.getSize().y - 10.f;
		backgroundRect.setPosition(xPos, yPos);

		tooltipText.setPosition(xPos + 10.f, yPos + 5.f);

		mSystem.RenderMgr.Draw(backgroundRect);
		mSystem.RenderMgr.Draw(tooltipText);
	}
}

void RenderManager::RenderParticles(std::vector<std::unique_ptr<Particle>>& particles)
{
	for (auto& p : particles)
	{
		sf::Vector2f pos = p->getCurrentPos();
		// Change this to the correct sprite
		sf::Sprite particleSprite = mSystem.AssetMgr.GetSpriteForItem(p->getItemID().first);
		particleSprite.setOrigin(particleSprite.getTextureRect().getSize().x / 2.f, particleSprite.getTextureRect().getSize().y / 2.f);
		particleSprite.setPosition(pos);
		float particleScale = p->getProgress() * 0.5;
		particleSprite.setScale(particleScale, particleScale);
		Draw(particleSprite);
	}
}

void RenderManager::RenderChests(std::vector<std::unique_ptr<Chest>>& chests)
{
	for (auto& chest : chests)
	{
		if (!chest->isActive())
		{
			continue;
		}
		sf::Vector2f position = chest->GetPosition();
		sf::Sprite chestSprite;
		// Get the opened or closed sprite
		if (chest->IsOpened())
		{
			chestSprite = mSystem.AssetMgr.GetSprite(SPRITES::CHESTOPENED);
		}
		else
		{
			chestSprite = mSystem.AssetMgr.GetSprite(SPRITES::CHESTCLOSED);
		}
		chestSprite.setOrigin(chestSprite.getGlobalBounds().getSize().x / 2.f, chestSprite.getGlobalBounds().getSize().y / 2.f);
		chestSprite.setPosition(position);
		// Mirror it if needed
		if (chest->IsMirrored())
		{
			chestSprite.setScale({ -1,1 });
		}

		Draw(chestSprite);
	}
}

void RenderManager::RenderItems(std::vector<std::unique_ptr<Item>>& items)
{
	for (auto& item : items)
	{
		sf::Vector2f itemPos = item->getPosition();
		std::pair<ITEMID::ITEM, ITEMRARITY::RARITY> itemId = item->getItemID();
		sf::Sprite itemSprite;
		if (item->getItemID().first == ITEMID::GOLD)
		{
			itemSprite = mSystem.AssetMgr.GetSpriteForGoldQuantity(item->getQuantity());
		}
		else
		{
			itemSprite = mSystem.AssetMgr.GetSpriteForItem(itemId.first);
		}
		itemSprite.setScale(0.5, 0.5);
		itemSprite.setOrigin(itemSprite.getTextureRect().getSize().x / 2.f, itemSprite.getTextureRect().getSize().y / 2.f);
		itemSprite.setPosition(itemPos);


		mGameWindow.draw(itemSprite);
	}

	for (auto& item : items)
	{
		std::pair<ITEMID::ITEM, ITEMRARITY::RARITY> itemId = item->getItemID();
		sf::RectangleShape textRect = item->getTextRect();
		textRect.setFillColor(mSystem.AssetMgr.GetTextboxColor());
		sf::Text text = item->getItemText();
		text.setOrigin(text.getGlobalBounds().getSize().x / 2, text.getGlobalBounds().getSize().y / 2);
		text.setPosition(textRect.getPosition());
		text.setColor(mSystem.AssetMgr.GetColorForItemText(itemId));
		mGameWindow.draw(textRect);
		mGameWindow.draw(text);
	}
}

void RenderManager::RenderItemCollectionProgress()
{
	// Get the string with the appropriate numbers
	std::string string{ "Item Collection: " };
	string += std::to_string(mSystem.InventoryMgr.getNumberOfUniqueItems());
	string += " / ";
	string += std::to_string(ITEMGEN::TOTAL_UNIQUE_ITEMS);

	// Create the SF::Text and TextRect
	sf::Text text;
	text.setString(string);
	text.setFont(mSystem.AssetMgr.GetFont(FONTS::LIGHT));
	text.setCharacterSize(FONTS::CHARACTER_SIZE_NORMAL);
	text.setOrigin(text.getLocalBounds().getSize().x / 2.f, text.getLocalBounds().getSize().y / 2.f);
	sf::RectangleShape textRect;
	textRect.setSize({ text.getLocalBounds().getSize().x + FONTS::PADDING, text.getLocalBounds().getSize().y + FONTS::PADDING });
	textRect.setOrigin(textRect.getLocalBounds().getSize().x / 2.f + FONTS::ORIGIN_YOFFSET, textRect.getLocalBounds().getSize().y / 2.f + FONTS::ORIGIN_YOFFSET);
	textRect.setFillColor(mSystem.AssetMgr.GetTextboxColor());
	
	//Set positions
	sf::Vector2f position{mGameWindow.getSize().x /2.f, 50.f};
	textRect.setPosition(position);
	text.setPosition(position);
	
	//Draw
	Draw(textRect);
	Draw(text);

}
