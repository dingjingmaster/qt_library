#include "dog.h"
#include <QDebug>

Dog::Dog()
{

}

Dog::~Dog()
{

}

void Dog::eat()
{
    qDebug() << "dog eating...";
}

void Dog::run()
{
    qDebug() << "dog run...";
}

void Dog::sleep()
{
    qDebug() << "dog sleep...";
}
