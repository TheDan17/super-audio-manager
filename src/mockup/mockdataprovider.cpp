#include "mockdataprovider.h"


QVector<ModuleDistillMetaInfo> MockDataProvider::createMockModuleList()
{
    QVector<ModuleDistillMetaInfo> modules;

    // --- Создаем несколько модулей для примера ---

    // 1. Плеер (поддерживает только панель)
    modules.append({
        "uuid-player-123", "org.example.player", "1.0.0",
        "Плеер", "", QIcon(),
        true, true,  // isEnabled, isPinned
        true, false  // supportsPanel, supportsWindow
    });

    // 2. Редактор метаданных (поддерживает и панель, и окно)
    modules.append({
        "uuid-metaedit-456", "org.example.metaeditor", "1.2.1",
        "Редактор метаданных", "Пакетное редактирование тегов и обложек для выбранных треков", QIcon(),
        true, false, // isEnabled, isPinned
        true, true   // supportsPanel, supportsWindow
    });

    // 3. Синхронизация (поддерживает только окно)
    modules.append({
        "uuid-sync-789", "org.example.sync", "0.9.0",
        "Синхронизация", "Синхронизация с внешними устройствами по настраиваемым профилям", QIcon(),
        true, false, // isEnabled, isPinned
        false, true  // supportsPanel, supportsWindow
    });

    // 4. Провайдер MusicBrainz (без UI, но включен)
    modules.append({
        "uuid-mbrainz-abc", "org.example.musicbrainz", "2.0.0",
        "Провайдер MusicBrainz", "Ищет метаданные на сервисе MusicBrainz.", QIcon(),
        true, false, // isEnabled, isPinned
        false, false // supportsPanel, supportsWindow
    });

    // 5. Конвертер (отключен)
    modules.append({
        "uuid-convert-def", "org.example.converter", "1.5.0",
        "Конвертер аудио", "Конвертирует аудиофайлы в различные форматы.", QIcon(),
        false, false, // isEnabled, isPinned
        true, true   // supportsPanel, supportsWindow
    });

    // Добавь еще 5-10 модулей, чтобы список выглядел полным
    for(int i = 0; i < 7; ++i) {
        modules.append({
            QString("uuid-dummy-%1").arg(i), QString("org.example.dummy%1").arg(i), "0.1.0",
            QString("Пример модуля %1").arg(i+1), QString("Это описание для примера модуля №%1, оно может быть достаточно длинным.1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19").arg(i+1), QIcon(),
            true, false, true, (i % 2 == 0) // Чередуем поддержку окна
        });
    }

    return modules;
}
