#ifndef IMPL_MODULES_H
#define IMPL_MODULES_H

#include "../core/api.h"
#include "../ui/module_panels/playerpanelwidget.h" // Подключаем реальный виджет плеера
#include <QLabel>
#include <QVBoxLayout>

// --- ПЛЕЕР ---
class PlayerModule : public IModule {
public:
    QString uuid() const override { return "uuid-player-real-1"; }
    QString stringId() const override { return "org.audio.player"; }
    QString version() const override { return "1.0.0"; }
    QString localizedName() const override { return "Музыкальный Плеер"; }
    QString description() const override { return "Базовый модуль воспроизведения."; }
    QIcon icon() const override { return QIcon::fromTheme("media-playback-start"); }

    bool supportsGuiPanel() const override { return true; }

    // Плеер возвращает настоящий виджет
    QWidget* createPanelWidget(QWidget* parent) override {
        return new PlayerPanelWidget(parent);
    }
    // в PlayerModule в файле impl_modules.h
    bool supportsGuiWindow() const override { return false; }
};

// --- РЕДАКТОР ТЕГОВ (Заглушка) ---
class TagEditorModule : public IModule {
public:
    QString uuid() const override { return "uuid-tagedit-real-2"; }
    QString stringId() const override { return "org.audio.tageditor"; }
    QString version() const override { return "0.5.beta"; }
    QString localizedName() const override { return "Редактор тегов"; }
    QString description() const override { return "Редактирование ID3 тегов."; }
    QIcon icon() const override { return QIcon::fromTheme("mail-attachment"); }

    bool supportsGuiPanel() const override { return false; } // Разрешаем открыть панель
    bool supportsGuiWindow() const override { return false; }

    // Возвращаем заглушку, чтобы показать, что модуль открылся
    QWidget* createPanelWidget(QWidget* parent) override {
        QLabel* label = new QLabel("Интерфейс редактора тегов в разработке", parent);
        label->setAlignment(Qt::AlignCenter);
        return label;
    }
};

// --- СИНХРОНИЗАЦИЯ (Заглушка) ---
class SyncModule : public IModule {
public:
    QString uuid() const override { return "uuid-sync-real-3"; }
    QString stringId() const override { return "org.audio.sync"; }
    QString version() const override { return "0.9.0"; }
    QString localizedName() const override { return "Синхронизация"; }
    QString description() const override { return "Синхронизация с устройствами."; }
    QIcon icon() const override { return QIcon::fromTheme("drive-removable-media"); }

    bool supportsGuiWindow() const override { return false; } // Только окно
    bool supportsGuiPanel() const override { return false; } // В панель нельзя

    QWidget* createPanelWidget(QWidget* parent) override { return nullptr; }
};

#endif // IMPL_MODULES_H
