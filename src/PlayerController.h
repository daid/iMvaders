#pragma once

#include "io/keybinding.h"
#include "P.h"

extern sp::io::Keybinding escape_key;

class PlayerController : public PObject
{
public:
    static const int buttonCount = 6;

    PlayerController(int idx);

    bool left() { return _left.get(); }
    bool right() { return _right.get(); }
    bool up() { return _up.get(); }
    bool down() { return _down.get(); }
    bool button(int n) { return _button[n].get(); }

    sp::io::Keybinding _left;
    sp::io::Keybinding _right;
    sp::io::Keybinding _up;
    sp::io::Keybinding _down;
    sp::io::Keybinding _button[buttonCount];
};
