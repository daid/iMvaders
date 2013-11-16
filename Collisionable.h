#ifndef COLLISIONABLE_H
#define COLLISIONABLE_H
#include "P.h"
class Collisionable;
extern PVector<Collisionable> collisionableList;
class Collisionable: public virtual PObject
{
    public:
        Collisionable();
        virtual ~Collisionable();
        virtual void collision(Collisionable target);
    protected:
    private:
};

#endif // COLLISIONABLE_H
