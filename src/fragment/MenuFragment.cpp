#include "../../include/fragment/MenuFragment.h"
#include "../../include/controller/DisplayController.h"
#include "../../include/controller/WifiMaster.h"

bool MenuFragment::mIsVisible = false;
bool MenuFragment::mNeedsRedraw = false;
bool MenuFragment::mIsFirstTime = false;
int MenuFragment::mCurrentIndex = 0;
bool MenuFragment::mEditPanelVisible = false;
int MenuFragment::mPrevValue = 0;

static constexpr const char *const TAG = "MENU";

void MenuFragment::init()
{
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;
        DisplayController::init();
    }
}

void MenuFragment::loop()
{
    static uint32_t timeTick = 0;
#ifdef SHOW_RENDER_TIME
    static uint32_t measureTimeTick = 0;
#endif

    if (mNeedsRedraw && xTaskGetTickCount() > timeTick)
    {
        timeTick = xTaskGetTickCount() + 10 / portTICK_PERIOD_MS;
        if (mIsVisible)
        {
#ifdef SHOW_RENDER_TIME
            measureTimeTick = xTaskGetTickCount();
#endif
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
                showEditPanel(SettingsManager::getItem(mCurrentIndex));
            }
#ifdef SHOW_RENDER_TIME
            measureTimeTick = xTaskGetTickCount() - measureTimeTick;
            LOG("Draw time: %dms", measureTimeTick / portTICK_PERIOD_MS);
#endif
        }
        mNeedsRedraw = false;
        mIsFirstTime = false;
    }
}

void MenuFragment::show()
{
    if (!mIsVisible)
    {
        LOG("Show");
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
    if (mIsVisible)
    {
        LOG("Hide");
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
        if (SettingsManager::getItem(mCurrentIndex)->inc())
        {
            mNeedsRedraw = true;
            onDataChanged(SettingsManager::getItem(mCurrentIndex));
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
        if (SettingsManager::getItem(mCurrentIndex)->dec())
        {
            mNeedsRedraw = true;
            onDataChanged(SettingsManager::getItem(mCurrentIndex));
        }
    }
    else if (mCurrentIndex < SettingsManager::getLength() - 1)
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

    // Check if enter to MENU_ITEM_TYPE_RESET
    if (!getEditPanelVisible() && SettingsManager::getItem(mCurrentIndex)->getType() == MENU_ITEM_TYPE_RESET)
    {
        if (SettingsManager::getItem(mCurrentIndex)->getName().equals("Reset WiFi"))
        {
            PopupFragment::setCallback([](bool selection)
                                       {
                if (selection)
                {
                    WifiMaster::resetSettings();
                    Message message = {MESSAGE_TYPE_REBOOT, 0};
                    MessageEvent::send(message);
                }
                else
                {
                    Message message = {MESSAGE_TYPE_CHANGE_TO_PREVIOUS_FRAGMENT, 0};
                    MessageEvent::send(message);
                } });
        }
        else
        {
            PopupFragment::setCallback([](bool selection)
                                       {
                if (selection)
                {
                    SettingsManager::reset();
                    Message message = {MESSAGE_TYPE_REBOOT, 0};
                    MessageEvent::send(message);
                }
                else
                {
                    Message message = {MESSAGE_TYPE_CHANGE_TO_PREVIOUS_FRAGMENT, 0};
                    MessageEvent::send(message);
                } });
        }
        Message message = {MESSAGE_TYPE_SHOW_POPUP, POPUP_TYPE_CONFIRM_REBOOT};
        MessageEvent::send(message);
        return;
    }

    // Save data before exit edit panel
    bool reDraw = true;
    if (getEditPanelVisible() && mPrevValue != SettingsManager::getItem(mCurrentIndex)->getValue())
    {
        if (SettingsManager::getItem(mCurrentIndex)->needToReboot())
        {
            reDraw = false;
            static int lastIndex = mCurrentIndex;
            PopupFragment::setCallback([](bool selection)
                                       {
                if (selection)
                {
                    SettingsManager::getItem(lastIndex)->save();
                    Message message = {MESSAGE_TYPE_REBOOT, 0};
                    MessageEvent::send(message);
                }
                else
                {
                    Message message = {MESSAGE_TYPE_CHANGE_TO_PREVIOUS_FRAGMENT, 0};
                    MessageEvent::send(message);
                } });
            Message message = {MESSAGE_TYPE_SHOW_POPUP, POPUP_TYPE_CONFIRM_REBOOT};
            MessageEvent::send(message);
        }
        else
        {
            SettingsManager::getItem(mCurrentIndex)->save();
            // notify value changed on saved
            onDataSaved(SettingsManager::getItem(mCurrentIndex));
        }
    }
    else
    {
        // Restore old value
        SettingsManager::getItem(mCurrentIndex)->load();
    }

    // Toggle edit panel
    if (reDraw)
    {
        setEditPanelVisible(!getEditPanelVisible());
    }

    // After switch to edit panel
    if (getEditPanelVisible())
    {
        mPrevValue = SettingsManager::getItem(mCurrentIndex)->getValue();
    }
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
    if (SettingsManager::getLength() == 0)
    {
        LOG("Menu Item List is empty");
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
        if (mCurrentIndex > mPrevIndex && startIndex + maxDrawableItems <= SettingsManager::getLength() && mCurrentIndex > startIndex + maxDrawableItems - 2)
        {
            startIndex++;
        }
        else if (mCurrentIndex < mPrevIndex && startIndex > 0 && mCurrentIndex < startIndex + 1)
        {
            startIndex--;
        }
    }

    for (int i = startIndex; i < SettingsManager::getLength() && i < startIndex + maxDrawableItems; i++)
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
        String itemName = SettingsManager::getItem(i)->getName() + "    ";
        DisplayController::getTft()->drawString(itemName.c_str(), MENU_ITEM_LEFT_MARGIN, ypos + itemHeight / 2);
        ypos += itemHeight;
    }
    if (startIndex + maxDrawableItems > SettingsManager::getLength())
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

void MenuFragment::onDataChanged(MenuItem *item)
{
    MessageType type = item->getChangedNotifyType();
    if (type != MESSAGE_TYPE_NONE)
    {
        Message message = {type, 0};
        MessageEvent::send(message);
    }
}

void MenuFragment::onDataSaved(MenuItem *item)
{
    MessageType type = item->getSavedNotifyType();
    if (type != MESSAGE_TYPE_NONE)
    {
        Message message = {type, 0};
        MessageEvent::send(message);
    }
}