#ifndef BILDOG_H
#define BILDOG_H
#include "dog.h"

// 还有一个非常重要的工作，我们得提供一个能够创建（释放）Animal具体对象（这里是Dog）的接口，
// 并且把这些函数导出，让主程序（Test.exe）能够解析这个接口函数，动态创建Animal对象，并访问其功能。

// extern "C" 生成的导出符号没有任何修饰，方便主程序找到它
extern "C" {
Q_DECL_EXPORT IAnimal* CreateAnimal();
Q_DECL_EXPORT void ReleaseAnimal(IAnimal* animal);
}

#endif // BILDOG_H
