#include "ui/mainwindow.h"
#include <QApplication>
#include "core/core.h"
#include "modules/impl_modules.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1. Инициализация Ядра
    // В реальном приложении здесь был бы поиск .so/.dll файлов (F-ARCH-1-9)
    // Для лабы мы регистрируем классы вручную.
    auto& core = Core::instance();

    core.registerModule(std::make_shared<PlayerModule>());
    core.registerModule(std::make_shared<TagEditorModule>());
    core.registerModule(std::make_shared<SyncModule>());;

    MainWindow w;
    w.show();
    return a.exec();
}
