#include "../include/MenuManager.h"

bool MenuManager::mIsInitialized = false;
bool MenuManager::mIsVisible = false;
bool MenuManager::mNeedsRedraw = true;
bool MenuManager::mIsFirstTime = true;
MenuItem *MenuManager::mMenuItemList[MENU_ITEM_LIST_SIZE];
int MenuManager::mMenuItemCount = 0;
int MenuManager::mMenuItemNameMaxLength = 0;
int MenuManager::mUserSelection = 0;
bool MenuManager::mEditPanelVisible = false;

static constexpr const char *const TAG = "MENU";

void MenuManager::init()
{
    if (!mIsInitialized)
    {
        mIsInitialized = true;
        DisplayManager::init();
        createMenuList();
    }
}

void MenuManager::loop()
{
    static uint32_t timeTick = 0;
    static uint32_t measureTimeTick = 0;

    if (mNeedsRedraw && xTaskGetTickCount() > timeTick)
    {
        timeTick = xTaskGetTickCount() + 30 / portTICK_PERIOD_MS;
        if (mIsVisible)
        {
            measureTimeTick = xTaskGetTickCount();
            if (!getEditPanelVisible())
            {
                showMenuList(mIsFirstTime);
            }
            else
            {
                showEditPanel(mIsFirstTime);
            }
            measureTimeTick = xTaskGetTickCount() - measureTimeTick;
            LOG("Draw time: %dms", measureTimeTick / portTICK_PERIOD_MS);
        }
        else
        {
            DisplayManager::clear();
        }
        mNeedsRedraw = false;
        mIsFirstTime = false;
    }
}

void MenuManager::show()
{
    LOG("Show");
    if (!mIsVisible)
    {
        mIsVisible = true;
        mNeedsRedraw = true;
        mIsFirstTime = true;
        setEditPanelVisible(false);
    }
}

void MenuManager::hide()
{
    LOG("Hide");
    if (mIsVisible)
    {
        mIsVisible = false;
        mNeedsRedraw = true;
        mIsFirstTime = true;
        setEditPanelVisible(false);
    }
}

void MenuManager::up()
{
    LOG("Up");
    if (getEditPanelVisible())
    {
        if (mMenuItemList[mUserSelection]->inc())
        {
            mNeedsRedraw = true;
        }
    }
    else if (mUserSelection > 0)
    {
        mUserSelection--;
        mNeedsRedraw = true;
    }
}

void MenuManager::down()
{
    LOG("Down");
    if (getEditPanelVisible())
    {
        if (mMenuItemList[mUserSelection]->dec())
        {
            mNeedsRedraw = true;
        }
    }
    else if (mUserSelection < mMenuItemCount - 1)
    {
        mUserSelection++;
        mNeedsRedraw = true;
    }
}

void MenuManager::enter()
{
    setEditPanelVisible(!getEditPanelVisible());
}

void MenuManager::back()
{
    setEditPanelVisible(false);
}

void MenuManager::setEditPanelVisible(bool visible)
{
    if (mEditPanelVisible != visible)
    {
        mEditPanelVisible = visible;
        mNeedsRedraw = true;
        mIsFirstTime = true;
    }
}

bool MenuManager::getEditPanelVisible()
{
    return mEditPanelVisible;
}

void MenuManager::showHeader(const char *text)
{
    LOG("showHeader %s", text);
    DisplayManager::setFont(MENU_FONT);
    DisplayManager::setTextColor(TFT_WHITE);
    DisplayManager::setHeader(text, MENU_HEADER_COLOR);
}

void MenuManager::showMenuList(bool isFirstTime)
{
    static int prevUserSelection = -1;
    static int startIndex = 0;

    LOG("showMenuList %d", isFirstTime);

    if (isFirstTime)
    {
        DisplayManager::clear();
        showHeader("MENU");
    }

    uint16_t headerHeight = DisplayManager::getFontHeight();
    uint16_t itemHeight = DisplayManager::getFontHeight() * 1.3;
    uint16_t ypos = headerHeight;
    int maxDrawableItems = round((TFT_HEIGHT - headerHeight) * 1.0 / itemHeight);

    DisplayManager::setTextDatum(CL_DATUM);

    if (isFirstTime)
    {
        startIndex = 0;
        prevUserSelection = -1;
        mUserSelection = 0;
    }
    else
    {
        if (mUserSelection > prevUserSelection && startIndex + maxDrawableItems <= mMenuItemCount && mUserSelection > startIndex + maxDrawableItems - 2)
        {
            startIndex++;
        }
        else if (mUserSelection < prevUserSelection && startIndex > 0 && mUserSelection < startIndex + 1)
        {
            startIndex--;
        }
    }

    for (int i = startIndex; i < mMenuItemCount && i < startIndex + maxDrawableItems; i++)
    {
        if (mUserSelection != prevUserSelection)
        {
            if (prevUserSelection >= 0 && i == prevUserSelection)
            {
                DisplayManager::fillRect(0, ypos, TFT_WIDTH, itemHeight, MENU_BACKGROUND_COLOR);
            }
            else if (mUserSelection >= 0 && i == mUserSelection)
            {
                DisplayManager::fillRect(0, ypos, TFT_WIDTH, itemHeight, MENU_HIGHTLIGHT_COLOR);
            }
        }
        if (i == mUserSelection)
        {
            DisplayManager::setTextColor(TFT_WHITE);
        }
        else
        {
            DisplayManager::setTextColor(TFT_WHITE, MENU_BACKGROUND_COLOR);
        }
        String itemName = mMenuItemList[i]->getName();
        addSpaceToEnd(itemName, mMenuItemNameMaxLength + 4);
        DisplayManager::drawString(itemName.c_str(), MENU_ITEM_LEFT_MARGIN, ypos + itemHeight / 2);
        ypos += itemHeight;
    }
    if (startIndex + maxDrawableItems > mMenuItemCount)
    {
        DisplayManager::fillRect(0, ypos, TFT_WIDTH, TFT_HEIGHT - ypos, MENU_BACKGROUND_COLOR);
    }
    prevUserSelection = mUserSelection;
}

void MenuManager::showEditPanel(bool isFirstTime)
{
    LOG("showEditPanel %d", isFirstTime);
    if (isFirstTime)
    {
        DisplayManager::clear();
        showHeader(mMenuItemList[mUserSelection]->getName().c_str());
    }

    uint16_t headerHeight = DisplayManager::getFontHeight();
    DisplayManager::setTextColor(TFT_WHITE, MENU_BACKGROUND_COLOR);
    DisplayManager::setTextDatum(CC_DATUM);
    String str = "  <  " + String(mMenuItemList[mUserSelection]->getValue()) + "  >  ";
    DisplayManager::drawString(str.c_str(), TFT_WIDTH / 2, (TFT_HEIGHT + headerHeight) / 2);
}

void MenuManager::addMenuItem(MenuItem *menuItem)
{
    if (mMenuItemCount < MENU_ITEM_LIST_SIZE)
    {
        if (menuItem->getName().length() > mMenuItemNameMaxLength)
        {
            mMenuItemNameMaxLength = menuItem->getName().length();
        }
        mMenuItemList[mMenuItemCount] = menuItem;
        mMenuItemCount++;
    }
    else
    {
        LOG("Menu item list is full");
        if (menuItem)
        {
            delete menuItem;
        }
    }
}

void MenuManager::createMenuList()
{
    for (int i = 0; i < mMenuItemCount; i++)
    {
        mMenuItemList[i] = nullptr;
    }
    mMenuItemCount = 0;
    mMenuItemNameMaxLength = 0;
    addMenuItem(new MenuItem("Setting 1", 0, 0, 10));
    addMenuItem(new MenuItem("Setting 2", 0, 0, 10));
    addMenuItem(new MenuItem("Setting 3", 0, 0, 10));
    addMenuItem(new MenuItem("Setting 4", 0, 0, 10));
    addMenuItem(new MenuItem("Setting 5", 0, 0, 10));
    addMenuItem(new MenuItem("Setting 6", 0, 0, 10));
    addMenuItem(new MenuItem("Setting 7", 0, 0, 10));
    addMenuItem(new MenuItem("Setting 8", 0, 0, 10));
    addMenuItem(new MenuItem("Setting 9", 0, 0, 10));
    addMenuItem(new MenuItem("Setting 10", 0, 0, 10));
    addMenuItem(new MenuItem("Setting 11", 0, 0, 10));
    addMenuItem(new MenuItem("Setting 12", 0, 0, 10));
}

void MenuManager::addSpaceToEnd(String &string, int length)
{
    for (int i = string.length(); i < length; i++)
    {
        string += " ";
    }
}