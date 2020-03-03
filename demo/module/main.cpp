#include <QCoreApplication>
#include <QLibrary>
#include <QDebug>
#include "ianimal.h"
#include "bildog.h"

int main(void)
{
    QLibrary lib("/data/code/qt_library/module/plugin/build2/libanimal");
    if (lib.load()) {
        // 定义插件中两个导出函数的原型
        typedef IAnimal* (*CreateAnimalFunction)();
        typedef void (*ReleaseAnimalFunction)(IAnimal* animal);

        // 解析导出函数
        CreateAnimalFunction createAnimal = (CreateAnimalFunction) lib.resolve("CreateAnimal");
        ReleaseAnimalFunction releaseAnimal = (ReleaseAnimalFunction) lib.resolve("ReleaseAnimal");

        if (createAnimal && releaseAnimal) {
            // 创建 Animal 对象
            IAnimal* animal = createAnimal();
            if (animal) {
                // 使用插件功能
                animal->eat();
                animal->sleep();

                // 使用完之后删除对象
                releaseAnimal(animal);
            }
        } else {
            qDebug() << "parse error: " << lib.errorString();
        }
    } else {
        qDebug() << "error: " << lib.errorString();
    }

    // 卸载插件
    lib.unload();
    return 0;
}
