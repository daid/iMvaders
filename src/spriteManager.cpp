#include "gameEntity.h"
#include "graphics/image.h"
#include "vectorUtils.h"


SpriteManager spriteManager;

struct SpriteInfo
{
    glm::ivec2 position{};
    glm::ivec2 size{};
};
struct TextureInfo
{
    glm::ivec2 size{-1, -1};
    std::vector<SpriteInfo> sprites;
};
std::unordered_map<std::string, TextureInfo> _textures;

TextureInfo& getTextureInfo(const std::string& key) {
    auto& e = _textures[key];
    if (e.size.x == -1) {
        e.size = {1, 1};
        sp::Image img;
        if (img.loadFromStream(getResourceStream(key + ".png"))) {
            e.size = img.getSize();
            auto ptr = img.getPtr();
            int w = img.getSize().x;

            for(int y=0; y<img.getSize().y; y++)
            {
                for(int x=0; x<img.getSize().x; x++)
                {
                    if (ptr[x + y * w] == glm::u8vec4{255,0,255, 255})
                    {
                        int x1=x+1;
                        for(; x1<img.getSize().x; x1++)
                            if (ptr[x1 + y * w] == glm::u8vec4{255,0,255, 255})
                                break;
                        int y1=y+1;
                        for(; y1<img.getSize().y; y1++)
                            if (ptr[x + y1 * w] == glm::u8vec4{255,0,255, 255})
                                break;
                        if (x1-x > 1 && y1-y > 1 && x1<img.getSize().x && y1<img.getSize().y)
                            e.sprites.push_back({{x+1, y+1}, {x1-x-1, y1-y-1}});
                        x = x1-1;
                    }
                }
            }
        }
    }
    return e;
}


void RectangleShape::draw(sp::RenderTarget& window)
{
    auto s0 = glm::vec2{-origin.x * scale.x, -origin.y * scale.y};
    auto s1 = glm::vec2{(size.x - origin.x) * scale.x, (size.y - origin.y) * scale.y};
    glm::vec2 p0 = position + rotateVec2({s0.x, s0.y}, rotation);
    glm::vec2 p1 = position + rotateVec2({s1.x, s0.y}, rotation);
    glm::vec2 p3 = position + rotateVec2({s0.x, s1.y}, rotation);
    glm::vec2 p2 = position + rotateVec2({s1.x, s1.y}, rotation);

    if (fill_color.a > 0)
        window.drawTriangleStrip({p0, p1, p2, p3}, fill_color);
    if (outline_thickness > 0) {
        auto t = outline_thickness;
        glm::vec2 o0 = position + rotateVec2({s0.x-t, s0.y-t}, rotation);
        glm::vec2 o1 = position + rotateVec2({s1.x+t, s0.y-t}, rotation);
        glm::vec2 o3 = position + rotateVec2({s0.x-t, s1.y+t}, rotation);
        glm::vec2 o2 = position + rotateVec2({s1.x+t, s1.y+t}, rotation);
        window.drawTriangleStrip({o0, o1, p0, p1}, outline_color);
        window.drawTriangleStrip({o1, o2, p1, p2}, outline_color);
        window.drawTriangleStrip({o2, o3, p2, p3}, outline_color);
        window.drawTriangleStrip({o3, o0, p3, p0}, outline_color);
    }
}

void CircleShape::draw(sp::RenderTarget& window)
{
    auto s0 = glm::vec2{0, size.y * scale.y};
    std::vector<glm::vec2> points;
    for(int n=0; n<point_count; n++) {
        points.push_back(position + rotateVec2(s0, float(n) / float(point_count) * 360 + rotation));
    }
    std::vector<uint16_t> indices;
    for(int n=0; n<point_count-2; n++) {
        indices.push_back(0);
        indices.push_back(n+1);
        indices.push_back(n+2);
    }
    window.drawTriangles(points, indices, fill_color);
}

void Sprite::draw(sp::RenderTarget& window)
{
    auto s0 = glm::vec2{-origin.x * scale.x, -origin.y * scale.y};
    auto s1 = glm::vec2{(size.x - origin.x) * scale.x, (size.y - origin.y) * scale.y};
    glm::vec2 p0 = position + rotateVec2({s0.x, s0.y}, rotation);
    glm::vec2 p1 = position + rotateVec2({s1.x, s0.y}, rotation);
    glm::vec2 p3 = position + rotateVec2({s0.x, s1.y}, rotation);
    glm::vec2 p2 = position + rotateVec2({s1.x, s1.y}, rotation);

    window.drawTexturedQuad(texture, p0, p1, p2, p3, uv0, {uv1.x, uv0.y}, uv1, {uv0.x, uv1.y}, color);
}

void SpriteManager::setTexture(Sprite& s, std::string key, int index)
{
    const auto& info = getTextureInfo(key);
    s.texture = key + ".png";
    s.size = info.size;
    if (index < int(info.sprites.size())) {
        s.size = info.sprites[index].size;
        s.uv0 = glm::vec2(info.sprites[index].position) / glm::vec2(info.size);
        s.uv1 = s.uv0 + (glm::vec2(info.sprites[index].size) / glm::vec2(info.size));
    }
    s.origin = s.size * 0.5f;
}

glm::vec2 SpriteManager::getSpriteSize(std::string key, int index) {
    const auto& info = getTextureInfo(key);
    if (index < int(info.sprites.size()))
        return info.sprites[index].size;
    return info.size;
}
