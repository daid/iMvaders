#include "gameEntity.h"

PVector<GameEntity> entityList;

GameEntity::~GameEntity() {}

void GameEntity::update(float delta) {}
void GameEntity::render(sp::RenderTarget& window) {}
bool GameEntity::takeDamage(glm::vec2 position, int damageType, int damage_amount) { return false; }
