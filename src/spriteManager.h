#pragma once

#include "Updatable.h"
#include "Renderable.h"


struct BaseShape {
    glm::vec2 origin{};
    glm::vec2 position{};
    float rotation{0};
    glm::vec2 size{};
    glm::vec2 scale{1, 1};

    float getRotation() { return rotation; }
    void setRotation(float r) { rotation = r; }
    void setPosition(glm::vec2 v) { position = v; }
    void setPosition(float x, float y) { position.x = x; position.y = y; }
    glm::vec2 getPosition() { return position; }
    void setOrigin(float x, float y) { origin.x = x; origin.y = y; }
    void setScale(glm::vec2 s) { scale = s; }
    void setScale(float x, float y) { scale.x = x; scale.y = y; }
    glm::vec2 getScale() { return scale; }
};


struct RectangleShape : public BaseShape {
    glm::u8vec4 fill_color{};
    glm::u8vec4 outline_color{};
    float outline_thickness{0};

    RectangleShape(glm::vec2 s) { size = s; }
    void setFillColor(glm::u8vec4 c) { fill_color = c; }
    void setOutlineColor(glm::u8vec4 c) { outline_color = c; }
    void setOutlineThickness(float t) { outline_thickness = t; }

    void draw(sp::RenderTarget& window);
};

struct CircleShape : public BaseShape {
    glm::u8vec4 fill_color{};
    int point_count = 2;

    CircleShape(float radius, int point_count) : point_count(point_count) { size = {radius * 2.0f, radius * 2.0f}; }
    void setFillColor(glm::u8vec4 c) { fill_color = c; }

    void draw(sp::RenderTarget& window);
};

struct Sprite : public BaseShape {
    std::string texture;
    glm::u8vec4 color{255,255,255,255};
    glm::vec2 uv0{0, 0};
    glm::vec2 uv1{1, 1};

    void setColor(glm::u8vec4 c) { color = c; }

    void draw(sp::RenderTarget& window);
};

class SpriteManager
{
public:
    void setTexture(Sprite& s, std::string key, int index=0);
    glm::vec2 getSpriteSize(std::string key, int index);
};
extern SpriteManager spriteManager;

