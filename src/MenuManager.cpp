#include "../include/MenuManager.h"

bool MenuManager::mIsInitialized = false;
bool MenuManager::mIsVisible = false;
bool MenuManager::mNeedsRedraw = false;
MenuItem *MenuManager::mMenuItemList[MENU_ITEM_LIST_SIZE];
int MenuManager::mMenuItemCount = 0;
int MenuManager::mMenuItemNameMaxLength = 0;
int MenuManager::mUserSelection = 0;

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
    static bool mPrevIsVisible = false;
    static uint32_t measureTimeTick = 0;

    if (mNeedsRedraw && xTaskGetTickCount() > timeTick)
    {
        timeTick = xTaskGetTickCount() + 30 / portTICK_PERIOD_MS;
        mNeedsRedraw = false;
        if (mPrevIsVisible != mIsVisible)
        {
            mPrevIsVisible = mIsVisible;
            if (mIsVisible)
            {
                measureTimeTick = xTaskGetTickCount();
                showHeader();
                showMenuList(true);
                measureTimeTick = xTaskGetTickCount() - measureTimeTick;
                LOG("Menu first draw time: %d ms", measureTimeTick / portTICK_PERIOD_MS);
            }
            else
            {
                DisplayManager::clear();
            }
        }
        else if (mIsVisible)
        {
            measureTimeTick = xTaskGetTickCount();
            showMenuList(false);
            measureTimeTick = xTaskGetTickCount() - measureTimeTick;
            LOG("Menu draw time: %d ms", measureTimeTick / portTICK_PERIOD_MS);
        }
    }
}

void MenuManager::show()
{
    LOG("Show");
    mIsVisible = true;
    mNeedsRedraw = true;
}

void MenuManager::hide()
{
    LOG("Hide");
    mIsVisible = false;
    mNeedsRedraw = true;
}

void MenuManager::up()
{
    if (mUserSelection > 0)
    {
        mUserSelection--;
        mNeedsRedraw = true;
    }
}

void MenuManager::down()
{
    if (mUserSelection < mMenuItemCount - 1)
    {
        mUserSelection++;
        mNeedsRedraw = true;
    }
}

void MenuManager::showHeader()
{
    DisplayManager::setFont(MENU_FONT);
    DisplayManager::setTextColor(TFT_WHITE);
    DisplayManager::setHeader("Menu", MENU_HEADER_COLOR);
}

void MenuManager::showMenuList(bool firstDraw)
{
    static int prevUserSelection = -1;
    static int startIndex = 0;

    uint16_t headerHeight = DisplayManager::getFontHeight();
    uint16_t itemHeight = DisplayManager::getFontHeight() * 1.3;
    uint16_t ypos = headerHeight;
    int maxDrawableItems = round((TFT_HEIGHT - headerHeight) * 1.0 / itemHeight);

    DisplayManager::setTextDatum(CL_DATUM);

    if (firstDraw)
    {
        startIndex = 0;
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
    addMenuItem(new MenuItem("Setting 1", 0));
    addMenuItem(new MenuItem("Setting 2", 0));
    addMenuItem(new MenuItem("Setting 3", 0));
    addMenuItem(new MenuItem("Setting 4", 0));
    addMenuItem(new MenuItem("Setting 5", 0));
    addMenuItem(new MenuItem("Setting 6", 0));
    addMenuItem(new MenuItem("Setting 7", 0));
    addMenuItem(new MenuItem("Setting 8", 0));
    addMenuItem(new MenuItem("Setting 9", 0));
    addMenuItem(new MenuItem("Setting 10", 0));
    addMenuItem(new MenuItem("Setting 11", 0));
    addMenuItem(new MenuItem("Setting 12", 0));
}

void MenuManager::addSpaceToEnd(String &string, int length)
{
    for (int i = string.length(); i < length; i++)
    {
        string += " ";
    }
}