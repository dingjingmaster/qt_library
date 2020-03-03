#ifndef DOG_H
#define DOG_H
#include "ianimal.h"

// 插件dog
class Dog : public IAnimal
{
public:
    Dog();
    virtual ~Dog ();

    virtual void eat() override;
    virtual void run() override;
    virtual void sleep() override;
};

#endif // DOG_H
