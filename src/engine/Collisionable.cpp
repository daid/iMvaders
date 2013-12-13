#include "Collisionable.h"
#include "vectorUtils.h"

PVector<Collisionable> collisionableList;

Collisionable::Collisionable(float radius)
{
    collisionRadius = radius;
    collisionableList.push_back(this);
}

Collisionable::~Collisionable()
{
}

void Collisionable::setCollisionRadius(float radius)
{
    collisionRadius = radius;
}

void Collisionable::collision(Collisionable* target)
{
}

void Collisionable::setPosition(sf::Vector2f v)
{
    position = v;
}
sf::Vector2f Collisionable::getPosition()
{
    return position;
}

void Collisionable::handleCollisions()
{
    foreach(Collisionable, c1, collisionableList)
    {
        for(unsigned int n=0; n<collisionableList.size(); n++)
        {
            P<Collisionable> c2 = collisionableList[n];
            if (c2 && c1 != c2)
            {
                if ((c1->position - c2->position) <= c1->collisionRadius + c2->collisionRadius)
                {
                    //Get normal pointers from the two collision objects, this to make sure both collision calls have valid object pointers,
                    //  as the P<> pointers can get cleared if the collision of c1 with c2 destroys c1
                    Collisionable* pc1 = *c1;
                    Collisionable* pc2 = *c2;
                    //Call the virtual collision functions to handle collision.
                    pc1->collision(pc2);
                    pc2->collision(pc1);
                }
            }
            if (!c1)
                break;
        }
    }
}
