#include "../include/MenuManager.h"

bool MenuManager::mIsInitialized = false;
bool MenuManager::mIsVisible = false;
bool MenuManager::mNeedsRedraw = true;
bool MenuManager::mIsFirstTime = true;
MenuItemList *MenuManager::mMenuItemList = new MenuItemList();
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
                DisplayController::showMenuList(mMenuItemList, mCurrentIndex, mIsFirstTime);
            }
            else
            {
                DisplayController::showEditPanel(mMenuItemList->get(mCurrentIndex), mIsFirstTime);
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

void MenuManager::createMenuList()
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
    mMenuItemList->add(new MenuItem("Setting 11", MENU_ITEM_TYPE_INT, 0, 0, 10));
    mMenuItemList->add(new MenuItem("Setting 12", MENU_ITEM_TYPE_INT, 0, 0, 10));
}