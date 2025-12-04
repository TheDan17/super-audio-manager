#ifndef CORE_API_H
#define CORE_API_H

#include <QString>
#include <QObject>
#include <QIcon>
#include <QWidget> // Важно: нужен для QWidget*
#include "../items/modulemetainfo.h"

// Абстрактный интерфейс для всех модулей
class IModule {
public:
    virtual ~IModule() = default;

    // F-ARCH-1-7: Манифест модуля
    virtual QString uuid() const = 0;
    virtual QString stringId() const = 0;
    virtual QString version() const = 0;
    virtual QString localizedName() const = 0;
    virtual QString description() const = 0;
    virtual QIcon icon() const = 0;

    // Возможности модуля
    virtual bool supportsGuiPanel() const { return false; }
    virtual bool supportsGuiWindow() const { return false; }

    // Метод для создания UI (возвращает nullptr по умолчанию)
    // Добавляем его сюда, чтобы override в наследниках работал
    virtual QWidget* createPanelWidget(QWidget* parent) { return nullptr; }

    // Конвертация в структуру для UI
    ModuleDistillMetaInfo toMetaInfo() const {
        return ModuleDistillMetaInfo{
            uuid(), stringId(), version(),
            localizedName(), description(), icon(),
            true, false, // В реальном коде статус берется из конфига
            supportsGuiPanel(), supportsGuiWindow()
        };
    }
};

// (Опционально) Интерфейс для модулей, гарантированно имеющих UI
class IGuiModule : public IModule {
public:
    // Чисто виртуальный метод, обязательный для реализации
    virtual QWidget* createPanelWidget(QWidget* parent) override = 0;
};

#endif // CORE_API_H
