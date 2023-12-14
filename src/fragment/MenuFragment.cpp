#include "../../include/fragment/MenuFragment.h"
#include "../../include/controller/DisplayController.h"

bool MenuFragment::mIsVisible = false;
bool MenuFragment::mNeedsRedraw = false;
bool MenuFragment::mIsFirstTime = false;
MenuItemList *MenuFragment::mMenuItemList = new MenuItemList();
int MenuFragment::mCurrentIndex = 0;
bool MenuFragment::mEditPanelVisible = false;
bool MenuFragment::mDataChanged = false;

static constexpr const char *const TAG = "MENU";

void MenuFragment::init()
{
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;
        DisplayController::init();
        createMenuList();
    }
}

void MenuFragment::loop()
{
    static uint32_t timeTick = 0;
    static uint32_t measureTimeTick = 0;

    if (mNeedsRedraw && xTaskGetTickCount() > timeTick)
    {
        timeTick = xTaskGetTickCount() + 10 / portTICK_PERIOD_MS;
        if (mIsVisible)
        {
            measureTimeTick = xTaskGetTickCount();
            if (mIsFirstTime)
            {
                DisplayController::setFont(MENU_FONT, MENU_FONT_SIZE);
            }
            if (!getEditPanelVisible())
            {
                showMenuList();
            }
            else
            {
                showEditPanel(mMenuItemList->get(mCurrentIndex));
            }
            measureTimeTick = xTaskGetTickCount() - measureTimeTick;
            LOG("Draw time: %dms", measureTimeTick / portTICK_PERIOD_MS);
        }
        mNeedsRedraw = false;
        mIsFirstTime = false;
    }
}

void MenuFragment::show()
{
    LOG("Show");
    if (!mIsVisible)
    {
        mIsVisible = true;
        mNeedsRedraw = true;
        mIsFirstTime = true;
        mCurrentIndex = 0;
        setEditPanelVisible(false);
        DisplayController::selectDisplay(0);
    }
}

void MenuFragment::hide()
{
    LOG("Hide");
    if (mIsVisible)
    {
        mIsVisible = false;
        mNeedsRedraw = true;
        mIsFirstTime = true;
        mCurrentIndex = 0;
        setEditPanelVisible(false);
    }
}

bool MenuFragment::isVisible()
{
    return mIsVisible;
}

void MenuFragment::up()
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
            mDataChanged = true;
            mNeedsRedraw = true;
        }
    }
    else if (mCurrentIndex > 0)
    {
        mCurrentIndex--;
        mNeedsRedraw = true;
    }
}

void MenuFragment::down()
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
            mDataChanged = true;
            mNeedsRedraw = true;
        }
    }
    else if (mCurrentIndex < mMenuItemList->length() - 1)
    {
        mCurrentIndex++;
        mNeedsRedraw = true;
    }
}

void MenuFragment::enter()
{
    if (!mIsVisible)
    {
        return;
    }
    bool reDraw = true;
    if (mDataChanged && getEditPanelVisible())
    {
        mMenuItemList->get(mCurrentIndex)->save();
        if (mMenuItemList->get(mCurrentIndex)->needToReboot())
        {
            LOG("Need to reboot");
            reDraw = false;
            MessageEvent::send(MESSAGE_TYPE_REBOOT);
        }
    }
    if (reDraw)
    {
        setEditPanelVisible(!getEditPanelVisible());
    }
    mDataChanged = false;
}

void MenuFragment::back()
{
    if (!mIsVisible)
    {
        return;
    }
    setEditPanelVisible(false);
}

/* ATTENTION: The index of the setting MUST BE same as the list */
bool MenuFragment::isWebServerEnabled()
{
    return mMenuItemList->get(0)->getBoolValue();
}

bool MenuFragment::isRTCDebugEnabled()
{
    return mMenuItemList->get(1)->getBoolValue();
}

void MenuFragment::createMenuList()
{
    mMenuItemList->add(new MenuItem("Web server", MENU_ITEM_TYPE_BOOL, false, 0, 1, true));
    mMenuItemList->add(new MenuItem("RTC debug", MENU_ITEM_TYPE_BOOL, false));
    mMenuItemList->add(new MenuItem("Setting 3", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 4", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 5", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 6", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 7", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 8", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 9", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 10", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->loadData();

    LOG("Settings list loaded:");
    for (int i = 0; i < mMenuItemList->length(); i++)
    {
        MenuItem *item = mMenuItemList->get(i);
        LOG("%d. %s: %s", i + 1, item->getName().c_str(), item->getStringValue().c_str());
    }
}

void MenuFragment::setEditPanelVisible(bool visible)
{
    if (mEditPanelVisible != visible)
    {
        mEditPanelVisible = visible;
        mNeedsRedraw = true;
        mIsFirstTime = true;
        if (!mEditPanelVisible)
        {
            mCurrentIndex = 0;
        }
    }
}

bool MenuFragment::getEditPanelVisible()
{
    return mEditPanelVisible;
}

void MenuFragment::showHeader(const char *text)
{
    DisplayController::getTft()->setTextColor(TFT_WHITE);

    uint32_t height = DisplayController::getTft()->fontHeight();
    uint32_t x = TFT_WIDTH / 2;
    uint32_t y = height / 2;

    DisplayController::getTft()->fillRect(0, 0, TFT_WIDTH, height, MENU_HEADER_COLOR);
    DisplayController::getTft()->setTextDatum(MC_DATUM);
    DisplayController::getTft()->drawString(text, x, y);
}

void MenuFragment::showMenuList()
{
    static int mPrevIndex = -1;
    static int startIndex = 0;

    LOG("showMenuList %d", mIsFirstTime);
    if (!mMenuItemList)
    {
        LOG("mMenuItemList is null");
        return;
    }

    if (mIsFirstTime)
    {
        DisplayController::clear();
        showHeader("MENU");
    }

    uint16_t headerHeight = DisplayController::getTft()->fontHeight();
    uint16_t itemHeight = DisplayController::getTft()->fontHeight() * 1.3;
    uint16_t ypos = headerHeight;
    int maxDrawableItems = round((TFT_HEIGHT - headerHeight) * 1.0 / itemHeight);

    DisplayController::getTft()->setTextDatum(CL_DATUM);

    if (mIsFirstTime)
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
                DisplayController::getTft()->fillRect(0, ypos, TFT_WIDTH, itemHeight, MENU_BACKGROUND_COLOR);
            }
            else if (mCurrentIndex >= 0 && i == mCurrentIndex)
            {
                DisplayController::getTft()->fillRect(0, ypos, TFT_WIDTH, itemHeight, MENU_HIGHTLIGHT_COLOR);
            }
        }
        if (i == mCurrentIndex)
        {
            DisplayController::getTft()->setTextColor(TFT_WHITE);
        }
        else
        {
            DisplayController::getTft()->setTextColor(TFT_WHITE, MENU_BACKGROUND_COLOR);
        }
        String itemName = mMenuItemList->get(i)->getName() + "    ";
        DisplayController::getTft()->drawString(itemName.c_str(), MENU_ITEM_LEFT_MARGIN, ypos + itemHeight / 2);
        ypos += itemHeight;
    }
    if (startIndex + maxDrawableItems > mMenuItemList->length())
    {
        DisplayController::getTft()->fillRect(0, ypos, TFT_WIDTH, TFT_HEIGHT - ypos, MENU_BACKGROUND_COLOR);
    }
    mPrevIndex = mCurrentIndex;
}

void MenuFragment::showEditPanel(MenuItem *item)
{
    LOG("showEditPanel %d", mIsFirstTime);
    if (!item)
    {
        LOG("Item is null");
        return;
    }

    if (mIsFirstTime)
    {
        DisplayController::clear();
        showHeader(item->getName().c_str());
    }

    uint16_t headerHeight = DisplayController::getTft()->fontHeight();
    DisplayController::getTft()->setTextColor(TFT_WHITE, MENU_BACKGROUND_COLOR);
    DisplayController::getTft()->setTextDatum(CC_DATUM);
    String before = item->isMinimum() ? "     " : "  <  ";
    String after = item->isMaximum() ? "     " : "  >  ";
    String str = before + item->getStringValue() + after;
    DisplayController::getTft()->drawString(str.c_str(), TFT_WIDTH / 2, (TFT_HEIGHT + headerHeight) / 2);
}