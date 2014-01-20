#ifndef STORAGE_TANK_H
#define STORAGE_TANK_H

#include "chemicals.h"
#include "Updatable.h"

class StorageTank : public ChemicalContainer, public Updatable
{
public:
    StorageTank(float volume)
    : ChemicalContainer(volume)
    {
    }
    
    virtual void update(float delta) {}
};

class StorageTankLinks
{
public:
    PVector<StorageTank> tanks;
    
    void add(P<StorageTank> tank)
    {
        tanks.push_back(tank);
    }
    
    float total(EChemical chem)
    {
        float total = 0;
        foreach(StorageTank, t, tanks)
            total += t->contents[chem];
        return total;
    }
    
    float drain(float amount, EChemical chem)
    {
        foreach(StorageTank, t, tanks)
        {
            if (t->contents[chem] > amount)
            {
                t->contents[chem] -= amount;
                return 0.0;
            }else{
                amount -= t->contents[chem];
                t->contents[chem] = 0;
            }
        }
        return amount;
    }
};

#endif//STORAGE_TANK_H
