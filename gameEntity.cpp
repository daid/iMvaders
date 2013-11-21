#include "gameEntity.h"

PVector<GameEntity> entityList;
GameEntity::~GameEntity(){}
void GameEntity::update(float delta){}
void GameEntity::preRender(sf::RenderTarget& window) {}
void GameEntity::render(sf::RenderTarget& window) {}
void GameEntity::postRender(sf::RenderTarget& window) {}
bool GameEntity::takeDamage(sf::Vector2f position, int damageType, int damageAmount) { return false; }
