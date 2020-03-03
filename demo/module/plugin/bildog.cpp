#ifndef BILDOG_H
#define BILDOG_H
#include "bildog.h"
#include "ianimal.h"
#include "dog.h"

extern "C" {
IAnimal* CreateAnimal()
{
    return new Dog();
}
void ReleaseAnimal(IAnimal* animal)
{
    delete animal;
}

}
#endif // BILDOG_H
