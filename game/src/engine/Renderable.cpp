#include "Renderable.h"
PVector<Renderable> renderableList;
Renderable::Renderable()
{
    renderableList.push_back(this);
}

Renderable::~Renderable()
{
    //dtor
}
