#ifndef TEXT_DRAW_H
#define TEXT_DRAW_H

#include <stdio.h>
#include "graphics/renderTarget.h"
#include "stringImproved.h"

enum textAlign
{
    align_center = 0,
    align_left = 1,
    align_right = 2
};

void drawText(sp::RenderTarget& window, float x, float y, string str, textAlign align = align_center, float scale = 1.0);

#endif//TEXT_DRAW_H
