#include "PlayerController.h"

sp::io::Keybinding escape_key{"ESCAPE", "ESCAPE"};

PlayerController::PlayerController(int idx)
: _left{"LEFT_" + std::to_string(idx)}
, _right{"RIGHT_" + std::to_string(idx)}
, _up{"UP_" + std::to_string(idx)}
, _down{"DOWN_" + std::to_string(idx)}
, _button{
    {"B0_" + std::to_string(idx)},
    {"B1_" + std::to_string(idx)},
    {"B2_" + std::to_string(idx)},
    {"B3_" + std::to_string(idx)},
    {"B4_" + std::to_string(idx)},
    {"B5_" + std::to_string(idx)},
}
{
    if (idx == 0) {
        _left.addKey("Left");
        _right.addKey("Right");
        _up.addKey("Up");
        _down.addKey("Down");
        _button[0].addKey("Space");
        _button[1].addKey("Z");
        _button[2].addKey("X");
        _button[3].addKey("C");
        _button[4].addKey("V");
        _button[5].addKey("B");
    }
    if (idx == 1) {
        _left.addKey("A");
        _right.addKey("D");
        _up.addKey("W");
        _down.addKey("S");
        _button[0].addKey("Q");
        _button[1].addKey("E");
        _button[2].addKey("R");
        _button[3].addKey("F");
        _button[4].addKey("T");
        _button[5].addKey("G");
    }
}
