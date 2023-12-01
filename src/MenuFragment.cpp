#include "../include/MenuFragment.h"

bool MenuFragment::mIsInitialized = false;
bool MenuFragment::mIsVisible = false;
bool MenuFragment::mNeedsRedraw = false;
bool MenuFragment::mIsFirstTime = false;
MenuItemList *MenuFragment::mMenuItemList = new MenuItemList();
int MenuFragment::mCurrentIndex = 0;
bool MenuFragment::mEditPanelVisible = false;

static constexpr const char *const TAG = "MENU";

void MenuFragment::init()
{
    if (!mIsInitialized)
    {
        mIsInitialized = true;
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
            if(mIsFirstTime)
            {
                DisplayController::setFont(MENU_FONT, MENU_FONT_SIZE);
            }
            if (!getEditPanelVisible())
            {
                DisplayController::showMenuList(mMenuItemList, mCurrentIndex, mIsFirstTime);
            }
            else
            {
                DisplayController::showEditPanel(mMenuItemList->get(mCurrentIndex), mIsFirstTime);
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
        setEditPanelVisible(false);
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
    setEditPanelVisible(!getEditPanelVisible());
}

void MenuFragment::back()
{
    if (!mIsVisible)
    {
        return;
    }
    setEditPanelVisible(false);
}

void MenuFragment::setEditPanelVisible(bool visible)
{
    if (mEditPanelVisible != visible)
    {
        mEditPanelVisible = visible;
        mNeedsRedraw = true;
        mIsFirstTime = true;
    }
}

bool MenuFragment::getEditPanelVisible()
{
    return mEditPanelVisible;
}

void MenuFragment::createMenuList()
{
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
}