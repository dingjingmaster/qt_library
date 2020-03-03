#ifndef ANIMAL_H
#define ANIMAL_H
#include "bil.h"

class BIL_SHARE IAnimal {

public:
    IAnimal ();
    virtual ~IAnimal ();

public:
    virtual void eat () = 0;
    virtual void run () = 0;
    virtual void sleep () = 0;
};

#endif // ANIMAL_H
