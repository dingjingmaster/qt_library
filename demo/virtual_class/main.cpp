#include <iostream>
#include <QDebug>

using namespace std;

class BaseA {
public:
    virtual void a ();
};

class BaseB : virtual public BaseA {
//public:
//    virtual void a();
};

class BaseC : virtual public BaseA {
public:
    virtual void a();
};

class BaseD : public BaseC, BaseB {
public:
};

void BaseA::a() {
    qDebug() << "A  base a";
}

//void BaseB::a() {
//    qDebug() << "B  base b";
//}
void BaseC::a() {
    qDebug() << "C  base b";
}


int main()
{

    BaseA a;
    BaseB b;
    BaseC c;
    BaseD d;

    a.a();
    b.a();
    c.a();
    qDebug() << "----------";
    d.a();
    return 0;
}
