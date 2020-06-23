#pragma once

namespace AlertBox
{
void drawAlert(const uint8_t x, const uint8_t y, const uint8_t w, const uint8_t h, const __FlashStringHelper* msg)
{
    ab.fastRect(x, y, w, h, DITHER);
    ab.fastRect(x, y, 64, 1, BLACK);

    gf.setCursor(x, y);
    gf.println(msg);
}

void lazyAlertBox(const uint8_t x, const uint8_t y, const uint8_t w, const uint8_t h, const __FlashStringHelper* msg)
{
    AlertBox::drawAlert(x, y, w, h, msg);

    for (;;)
    {
        if (!ab.nextFrame()) continue;
        ab.pollButtons();

        if (ab.justPressed(A_BUTTON)) break;

        ab.display();
    }
}
}
