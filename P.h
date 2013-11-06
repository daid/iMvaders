#ifndef PEE_POINTER_H
#define PEE_POINTER_H

#include <vector>
#include <assert.h>

/**
    P<T> is a reference counting pointer class. This class keeps track to the amount of P<T> pointers pointing to a Pobject.
    If no more P<T> pointers point to a Pobject, the object is deleted. The object also has a destroyed flag, if the destroyed
    flag is set, the pointers pointing towards it act as NULL pointers from that point on.
    NOTE: This does not free circular references.
    
    The Pobject is not copyable and should not be created on the stack, only on the heap (so with "new")
    
    The Pvector class is a specialized version of the std::vector template. This vector holds an array of P<> pointers
    The "foreach" macro can be used to walk trough all the Pobjects in a list without needing to dive into details
    and automaticly removes any pointer from the list that points to an Pobject which has been destroyed.
 */

class PObject: public sf::NonCopyable
{
private:
    int refCount;
    bool destroyed;
    
    //Make the P template a friend so it can access the private refCount and destroyed.
    template<typename> friend class P;
public:
    PObject()
    {
#ifdef DEBUG
        int stackTest = 0;
        ptrdiff_t diff = &stackTest - (int*)this;
        //Check if this object is created on the stack, PObjects should not be created on the stack, as they manage
        // their own destruction.
        assert(("Object on stack! Not allowed!", abs(diff) > 10000));
#endif
        refCount = 0;
        destroyed = false;
    }
    virtual ~PObject() {}
    
    void destroy()
    {
        destroyed = true;
    }
};
template<class T>
class Piterator;

template<class T>
class P
{
private:
    T* ptr;
    
public:
    P()
    {
        ptr = NULL;
    }
    P(const P& p)
    {
        ptr = NULL;
        set(p.ptr);
    }
    P(T* p)
    {
        ptr = NULL;
        set(p);
    }
    
    ~P()
    {
        release();
    }
    
    P& operator = (T* p)
    {
        set(p);
        return *this;
    }

    P& operator = (const P& p)
    {
        if (&p != this) set(p.ptr);
        return *this;
    }

    P& operator = (const Piterator<T>& i)
    {
        set(*i);
        return *this;
    }
    
    T* operator->()
    {
        check_release();
        return ptr;
    }
    T* operator*()
    {
        check_release();
        return ptr;
    }
    
    operator bool()
    {
        check_release();
        return ptr != NULL;
    }

private:
    void check_release()
    {
        if (ptr && ptr->destroyed)
            release();
    }
    
    void release()
    {
        if (ptr)
        {
            ptr->refCount--;
            if (ptr->refCount == 0)
                delete ptr;
            ptr = NULL;
        }
    }
    void set(T* p)
    {
        release();
        ptr = p;
        if (ptr != NULL)
            ptr->refCount ++;
    }
};

template<class T>
class PVector: public std::vector<P<T> > {};

template<class T>
class Piterator : public sf::NonCopyable
{
private:
    PVector<T>& list;
    unsigned int index;
    T* ptr;
public:
    Piterator(PVector<T>& list)
    : list(list), index(0), ptr(NULL)
    {
       next();
    }
    
    void next()
    {
        while(true)
        {
            if (index >= list.size())
            {
                ptr = NULL;
                return;
            }
            ptr = *(list[index]);
            if (ptr)
            {
                index++;
                return;
            }
            list.erase(list.begin() + index);
        }
    }
    
    operator bool() const
    {
        return ptr != NULL;
    }

    T* operator->() const
    {
        return ptr;
    }

    T* operator*() const
    {
        return ptr;
    }
};
#define foreach(type, var, list) for(Piterator<type> var(list); var; var.next())

#endif//PEE_POINTER_H
