#include <QCoreApplication>
#include <QSharedMemory>
#include <iostream>
#include <QDebug>


// 注意，申请共享内存需要确定大小
struct mem_t
{
    int     a;
    char    b[1024];
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 确保非正常退出
    QSharedMemory* mem = new QSharedMemory("/tmp/test.shared");
    mem->attach();
    delete mem;

    mem = new QSharedMemory("/tmp/test.shared");
    std::cout << "create: " << mem->create(sizeof (struct mem_t)) << "\n";
    mem->lock();
    struct mem_t* m = static_cast<struct mem_t*>(mem->data());
    m->a = 100;
    memset(m->b, 0, 1024);
    memcpy(m->b, "aaaaaaa", 6);
    mem->unlock();
    mem->attach();

    mem->lock();
    auto c = (struct mem_t*)mem->constData();
    std::cout << c->a << std::endl;
    qDebug() << c->a;
    qDebug() << c->b;
    mem->unlock();
    delete mem;

    return 0; //a.exec();
}
