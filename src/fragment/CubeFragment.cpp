#include "../../include//fragment/CubeFragment.h"

bool CubeFragment::mIsVisible = false;
bool CubeFragment::mNeedsRedraw = false;
bool CubeFragment::mIsFirstTime = false;

TFT_eSprite *CubeFragment::mTftSpr[2];
bool CubeFragment::mTftSprSel = 0;
uint16_t *CubeFragment::mTftSprPtr[2];
bool CubeFragment::mSpin[3] = {true, true, true};
int CubeFragment::mMinX = 0;
int CubeFragment::mMinY = 0;
int CubeFragment::mMaxX = 0;
int CubeFragment::mMaxY = 0;

static constexpr const char *const TAG = "CUBE";

void CubeFragment::init()
{
    DisplayController::init();

    mTftSpr[0] = new TFT_eSprite(DisplayController::getTft());
    mTftSpr[1] = new TFT_eSprite(DisplayController::getTft());

    // Define cprite colour depth
    mTftSpr[0]->setColorDepth(COLOR_DEPTH);
    mTftSpr[1]->setColorDepth(COLOR_DEPTH);

    // Create the 2 sprites
    mTftSprPtr[0] = (uint16_t *)mTftSpr[0]->createSprite(IWIDTH, IHEIGHT);
    mTftSprPtr[1] = (uint16_t *)mTftSpr[1]->createSprite(IWIDTH, IHEIGHT);

    // Define text datum and text colour for Sprites
    mTftSpr[0]->setTextColor(TFT_BLACK);
    mTftSpr[0]->setTextDatum(MC_DATUM);
    mTftSpr[1]->setTextColor(TFT_BLACK);
    mTftSpr[1]->setTextDatum(MC_DATUM);

#ifdef USE_DMA_TO_TFT
    // DMA - should work with ESP32, STM32F2xx/F4xx/F7xx processors
    // NOTE: >>>>>> DMA IS FOR SPI DISPLAYS ONLY <<<<<<
    DisplayController::getTft()->initDMA(); // Initialise the DMA engine (tested with STM32F446 and STM32F767)
#endif

    // Grab exclusive use of the SPI bus
    // tft.startWrite();
    // Release exclusive use of SPI bus ( here as a reminder... forever loop prevents execution)
    // tft.endWrite();
}

void CubeFragment::loop()
{
    static uint32_t updateTime = 0; // time for next update
    static bool bounce = false;
    static int dx = 1, dy = 1;
    static uint16_t counter = 0;
    static long startMillis = millis();
    static uint16_t interval = 100;
    static int16_t xpos = 0, ypos = (TFT_HEIGHT - IHEIGHT) / 2;

    if (mIsVisible)
    {
        // Pull it back onto screen if it wanders off
        if (xpos < -mMinX)
        {
            dx = 1;
            bounce = true;
        }
        if (xpos >= DisplayController::getTft()->width() - mMaxX)
        {
            dx = -1;
            bounce = true;
        }
        if (ypos < -mMinY)
        {
            dy = 1;
            bounce = true;
        }
        if (ypos >= DisplayController::getTft()->height() - mMaxY)
        {
            dy = -1;
            bounce = true;
        }

        if (bounce)
        {
            // Randomise mSpin
            for (int i = 0; i < 3; i++)
            {
                mSpin[i] = random(2);
            }
            bounce = false;
        }

        if (millis() > updateTime)
        {
            // Use time delay so sprtie does not move fast when not all on screen
            updateTime = millis() + 5UL;
            mMinX = IWIDTH / 2;
            mMaxX = IWIDTH / 2;
            mMinY = IHEIGHT / 2;
            mMaxY = IHEIGHT / 2;
            drawCube();

#ifdef USE_DMA_TO_TFT
            DisplayController::getTft()->pushImageDMA(xpos, ypos, IWIDTH, IHEIGHT, mTftSprPtr[mTftSprSel]);
            mTftSprSel = !mTftSprSel;
#endif

            counter++;
            // only calculate the fps every <interval> iterations.
            if (counter % interval == 0)
            {
                int fps = interval * 1000.0 / (millis() - startMillis);
                LOG("%d fps", fps);
                startMillis = millis();
            }
            // Change coord for next loop
            xpos += dx;
            ypos += dy;
        }
    }
}

void CubeFragment::show()
{
    if (!mIsVisible)
    {
        LOG("Show");
        mIsVisible = true;
        mIsFirstTime = true;
        mNeedsRedraw = true;
    }
}

void CubeFragment::hide()
{
    if (mIsVisible)
    {
        LOG("Hide");
        mIsVisible = false;
        mIsFirstTime = true;
        mNeedsRedraw = true;
    }
}

bool CubeFragment::isVisible()
{
    return mIsVisible;
}

/**
  Detected visible triangles. If calculated area > 0 the triangle
  is rendered facing towards the viewer, since the vertices are CCW.
  If the area is negative the triangle is CW and thus facing away from us.
*/
int CubeFragment::shoelace(int x1, int y1, int x2, int y2, int x3, int y3)
{
    // (x1y2 - y1x2) + (x2y3 - y2x3)
    return x1 * y2 - y1 * x2 + x2 * y3 - y2 * x3 + x3 * y1 - y3 * x1;
}

/**
  Rotates and renders the cube.
**/
void CubeFragment::drawCube()
{
    // Define the cube face colors
    static uint16_t palette[] = {TFT_BLUE, TFT_GREEN, TFT_YELLOW, TFT_PINK, TFT_RED, TFT_CYAN};

    // size / 2 of cube edge
    static float px[] = {-15, 15, 15, -15, -15, 15, 15, -15};
    static float py[] = {-15, -15, 15, 15, -15, -15, 15, 15};
    static float pz[] = {-15, -15, -15, -15, 15, 15, 15, 15};

    // mapped coordinates on screen
    static float p2x[] = {0, 0, 0, 0, 0, 0, 0, 0};
    static float p2y[] = {0, 0, 0, 0, 0, 0, 0, 0};

    // rotation angle in radians
    static float r[] = {0, 0, 0};

    // Define the triangles
    // The order of the vertices MUST be CCW or the
    // shoelace method won't work to detect visible edges
    static int faces[12][3] = {{0, 1, 4},
                               {1, 5, 4},
                               {1, 2, 5},
                               {2, 6, 5},
                               {5, 7, 4},
                               {6, 7, 5},
                               {3, 4, 7},
                               {4, 3, 0},
                               {0, 3, 1},
                               {1, 3, 2},
                               {2, 3, 6},
                               {6, 3, 7}};

    double speed = 90;
    for (int i = 0; i < 3; i++)
    {
        if (mSpin[i])
        {
            r[i] = r[i] + PI / speed; // Add a degree
        }
        if (r[i] >= 360.0 * PI / 90.0)
        {
            r[i] = 0;
        }
    }

    float ax[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    float ay[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    float az[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    // Calculate all vertices of the cube
    for (int i = 0; i < 8; i++)
    {
        float px2 = px[i];
        float py2 = cos(r[0]) * py[i] - sin(r[0]) * pz[i];
        float pz2 = sin(r[0]) * py[i] + cos(r[0]) * pz[i];

        float px3 = cos(r[1]) * px2 + sin(r[1]) * pz2;
        float py3 = py2;
        float pz3 = -sin(r[1]) * px2 + cos(r[1]) * pz2;

        ax[i] = cos(r[2]) * px3 - sin(r[2]) * py3;
        ay[i] = sin(r[2]) * px3 + cos(r[2]) * py3;
        az[i] = pz3 - 150;

        p2x[i] = IWIDTH / 2 + ax[i] * CUBE_SIZE / az[i];
        p2y[i] = IHEIGHT / 2 + ay[i] * CUBE_SIZE / az[i];
    }

    // Fill the buffer with colour 0 (Black)
    mTftSpr[mTftSprSel]->fillSprite(TFT_BLACK);

    for (int i = 0; i < 12; i++)
    {
        if (shoelace(p2x[faces[i][0]], p2y[faces[i][0]], p2x[faces[i][1]], p2y[faces[i][1]], p2x[faces[i][2]], p2y[faces[i][2]]) > 0)
        {
            int x0 = p2x[faces[i][0]];
            int y0 = p2y[faces[i][0]];
            int x1 = p2x[faces[i][1]];
            int y1 = p2y[faces[i][1]];
            int x2 = p2x[faces[i][2]];
            int y2 = p2y[faces[i][2]];

            mMinX = min(mMinX, x0);
            mMinY = min(mMinY, y0);
            mMinX = min(mMinX, x1);
            mMinY = min(mMinY, y1);
            mMinX = min(mMinX, x2);
            mMinY = min(mMinY, y2);
            mMaxX = max(mMaxX, x0);
            mMaxY = max(mMaxY, y0);
            mMaxX = max(mMaxX, x1);
            mMaxY = max(mMaxY, y1);
            mMaxX = max(mMaxX, x2);
            mMaxY = max(mMaxY, y2);

            mTftSpr[mTftSprSel]->fillTriangle(x0, y0, x1, y1, x2, y2, palette[i / 2]);
            if (i % 2)
            {
                int avX = 0;
                int avY = 0;
                for (int v = 0; v < 3; v++)
                {
                    avX += p2x[faces[i][v]];
                    avY += p2y[faces[i][v]];
                }
                avX = avX / 3;
                avY = avY / 3;
            }
        }
    }

    // mTftSpr[mTftSprSel].drawString(fps, IWIDTH / 2, IHEIGHT / 2, 4);
    // delay(100);
}