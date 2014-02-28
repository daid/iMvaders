#include "Collisionable.h"
#include "vectorUtils.h"

PVector<Collisionable> collisionableList;

Collisionable::Collisionable(float radius)
{
    collisionRadius = radius;
    collisionType = CT_Circle;
    collisionableList.push_back(this);
}

Collisionable::Collisionable(sf::Vector2f lineVector)
{
    collisionLineVector = lineVector;
    collisionType = CT_Line;
    collisionableList.push_back(this);
}

Collisionable::~Collisionable()
{
}

void Collisionable::setCollisionRadius(float radius)
{
    collisionRadius = radius;
    collisionType = CT_Circle;
}

void Collisionable::setCollisionLineVector(sf::Vector2f lineVector)
{
    collisionLineVector = lineVector;
    collisionType = CT_Line;
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
                if (c1->collisionType == CT_Circle && c2->collisionType == CT_Circle)
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
                }else if (c1->collisionType == CT_Line && c2->collisionType == CT_Circle)
                {
					float length = sf::length(c1->collisionLineVector);
					if (length == 0)
						continue;
					sf::Vector2f diff = c2->position - c1->position;
					float off = (c1->collisionLineVector.x * diff.x + c1->collisionLineVector.y * diff.y) / length;
					if (off < 0)
						off = 0;
					if (off > length)
						off = length;
					sf::Vector2f q = c1->position + c1->collisionLineVector/length * off;
					if (sf::length(q - c2->position) < c2->collisionRadius)
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
            }
            if (!c1)
                break;
        }
    }
}
