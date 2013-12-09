#include "Collisionable.h"
PVector<Collisionable> collisionableList;
Collisionable::Collisionable()
{
    collisionableList.push_back(this);
}

Collisionable::~Collisionable()
{
    //dtor
}
void Collisionable::collision(Collisionable target){}
