#include "../include/MenuManager.h"

bool MenuManager::mIsInitialized = false;
bool MenuManager::mIsVisible = false;
bool MenuManager::mNeedsRedraw = true;
bool MenuManager::mIsFirstTime = true;
MenuItemList *MenuManager::mMenuItemList = new MenuItemList();
int MenuManager::mMenuItemNameMaxLength = 0;
int MenuManager::mCurrentIndex = 0;
bool MenuManager::mEditPanelVisible = false;

static constexpr const char *const TAG = "MENU";

void MenuManager::init()
{
    if (!mIsInitialized)
    {
        mIsInitialized = true;
        DisplayController::init();
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
            DisplayController::clear();
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
    if (!mIsVisible)
    {
        return;
    }
    LOG("Up");
    if (getEditPanelVisible())
    {
        if (mMenuItemList->get(mCurrentIndex)->inc())
        {
            mNeedsRedraw = true;
        }
    }
    else if (mCurrentIndex > 0)
    {
        mCurrentIndex--;
        mNeedsRedraw = true;
    }
}

void MenuManager::down()
{
    if (!mIsVisible)
    {
        return;
    }
    LOG("Down");
    if (getEditPanelVisible())
    {
        if (mMenuItemList->get(mCurrentIndex)->dec())
        {
            mNeedsRedraw = true;
        }
    }
    else if (mCurrentIndex < mMenuItemList->length() - 1)
    {
        mCurrentIndex++;
        mNeedsRedraw = true;
    }
}

void MenuManager::enter()
{
    if (!mIsVisible)
    {
        return;
    }
    setEditPanelVisible(!getEditPanelVisible());
}

void MenuManager::back()
{
    if (!mIsVisible)
    {
        return;
    }
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
    DisplayController::setFont(MENU_FONT);
    DisplayController::setTextColor(TFT_WHITE);
    DisplayController::setHeader(text, MENU_HEADER_COLOR);
}

void MenuManager::showMenuList(bool isFirstTime)
{
    static int mPrevIndex = -1;
    static int startIndex = 0;

    LOG("showMenuList %d", isFirstTime);

    if (isFirstTime)
    {
        DisplayController::clear();
        showHeader("MENU");
    }

    uint16_t headerHeight = DisplayController::getFontHeight();
    uint16_t itemHeight = DisplayController::getFontHeight() * 1.3;
    uint16_t ypos = headerHeight;
    int maxDrawableItems = round((TFT_HEIGHT - headerHeight) * 1.0 / itemHeight);

    DisplayController::setTextDatum(CL_DATUM);

    if (isFirstTime)
    {
        startIndex = 0;
        mPrevIndex = -1;
        mCurrentIndex = 0;
    }
    else
    {
        if (mCurrentIndex > mPrevIndex && startIndex + maxDrawableItems <= mMenuItemList->length() && mCurrentIndex > startIndex + maxDrawableItems - 2)
        {
            startIndex++;
        }
        else if (mCurrentIndex < mPrevIndex && startIndex > 0 && mCurrentIndex < startIndex + 1)
        {
            startIndex--;
        }
    }

    for (int i = startIndex; i < mMenuItemList->length() && i < startIndex + maxDrawableItems; i++)
    {
        if (mCurrentIndex != mPrevIndex)
        {
            if (mPrevIndex >= 0 && i == mPrevIndex)
            {
                DisplayController::fillRect(0, ypos, TFT_WIDTH, itemHeight, MENU_BACKGROUND_COLOR);
            }
            else if (mCurrentIndex >= 0 && i == mCurrentIndex)
            {
                DisplayController::fillRect(0, ypos, TFT_WIDTH, itemHeight, MENU_HIGHTLIGHT_COLOR);
            }
        }
        if (i == mCurrentIndex)
        {
            DisplayController::setTextColor(TFT_WHITE);
        }
        else
        {
            DisplayController::setTextColor(TFT_WHITE, MENU_BACKGROUND_COLOR);
        }
        String itemName = mMenuItemList->get(i)->getName();
        addSpaceToEnd(itemName, mMenuItemNameMaxLength + 4);
        DisplayController::drawString(itemName.c_str(), MENU_ITEM_LEFT_MARGIN, ypos + itemHeight / 2);
        ypos += itemHeight;
    }
    if (startIndex + maxDrawableItems > mMenuItemList->length())
    {
        DisplayController::fillRect(0, ypos, TFT_WIDTH, TFT_HEIGHT - ypos, MENU_BACKGROUND_COLOR);
    }
    mPrevIndex = mCurrentIndex;
}

void MenuManager::showEditPanel(bool isFirstTime)
{
    LOG("showEditPanel %d", isFirstTime);
    if (isFirstTime)
    {
        DisplayController::clear();
        showHeader(mMenuItemList->get(mCurrentIndex)->getName().c_str());
    }

    uint16_t headerHeight = DisplayController::getFontHeight();
    DisplayController::setTextColor(TFT_WHITE, MENU_BACKGROUND_COLOR);
    DisplayController::setTextDatum(CC_DATUM);
    String before = mMenuItemList->get(mCurrentIndex)->isMinimum() ? "     " : "  <  ";
    String after = mMenuItemList->get(mCurrentIndex)->isMaximum() ? "     " : "  >  ";
    String str = before + mMenuItemList->get(mCurrentIndex)->getValueAsString() + after;
    DisplayController::drawString(str.c_str(), TFT_WIDTH / 2, (TFT_HEIGHT + headerHeight) / 2);
}

void MenuManager::createMenuList()
{
    //hard code
    mMenuItemNameMaxLength = 10;
    mMenuItemList->add(new MenuItem("Setting 1", MENU_ITEM_TYPE_BOOL, false));
    mMenuItemList->add(new MenuItem("Setting 2", MENU_ITEM_TYPE_BOOL, false));
    mMenuItemList->add(new MenuItem("Setting 3", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 4", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 5", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 6", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 7", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 8", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 9", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 10", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 11", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 12", MENU_ITEM_TYPE_INT, 0, 0, 10));
}

void MenuManager::addSpaceToEnd(String &string, int length)
{
    for (int i = string.length(); i < length; i++)
    {
        string += " ";
    }
}