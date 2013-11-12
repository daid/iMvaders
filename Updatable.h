#ifndef UPDATABLE_H
#define UPDATABLE_H

//Abstract class for entity that can be updated.
class Updatable
{
    public:
        Updatable();
        virtual ~Updatable();
        virtual void update() = 0;
    protected:
    private:
};

#endif // UPDATABLE_H
