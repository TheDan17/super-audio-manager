#ifndef CORE_H
#define CORE_H

#include <QVector>
#include <QStringList>
#include <QDirIterator>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaMetaData>
#include <QSettings>
#include <QObject>
#include <QFileInfo>
#include <QDateTime> // Для даты
#include <QDir>
#include <memory>
#include "api.h"

struct AudioTrack {
    QString filePath;
    QString fileName;
    QString extension;
    qint64 size;

    // Метаданные
    QString title;
    QString artist;
    QString album;
    QString year; // <--- Новое поле

    bool operator==(const AudioTrack& other) const { return filePath == other.filePath; }
};

struct UserFilter {
    QString name;
    QString searchPattern;
};

class Core : public QObject {
    Q_OBJECT
public:
    static Core& instance() {
        static Core s_instance;
        return s_instance;
    }

    // --- Модули ---
    void registerModule(std::shared_ptr<IModule> module) { m_modules.push_back(module); }
    QVector<std::shared_ptr<IModule>> modules() const { return m_modules; }

    QVector<ModuleDistillMetaInfo> getModulesMetaInfo() const {
        QVector<ModuleDistillMetaInfo> result;
        for(const auto& mod : m_modules) result.append(mod->toMetaInfo());
        return result;
    }

    // --- Источники ---
    void addSource(const QString& path) {
        if (!m_sources.contains(path)) {
            m_sources.append(path);
            saveSettings();
            rescanLibrary();
        }
    }
    void removeSource(const QString& path) {
        m_sources.removeAll(path);
        saveSettings();
        rescanLibrary();
    }
    QStringList getSources() const { return m_sources; }

    // --- Фильтры ---
    void addFilter(const UserFilter& filter) { m_filters.append(filter); }
    QVector<UserFilter> getFilters() const { return m_filters; }

    // --- Библиотека ---
    // --- Библиотека ---
    void rescanLibrary() {
        m_library.clear();
        for (const auto& path : m_sources) {
            QDirIterator it(path, QStringList() << "*.mp3" << "*.wav" << "*.ogg" << "*.flac",
                            QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                it.next();
                QFileInfo info = it.fileInfo();

                QString baseName = info.baseName();
                QString artist = "Неизвестный исполнитель"; // <--- Меняем дефолт
                QString title = baseName;

                // Пробуем разделить "Artist - Title"
                if (baseName.contains(" - ")) {
                    QStringList parts = baseName.split(" - ");
                    if (parts.size() >= 2) {
                        artist = parts[0].trimmed();
                        title = parts[1].trimmed();
                        // Если есть еще дефисы, приклеиваем обратно к названию
                        for(int i=2; i<parts.size(); i++) title += " - " + parts[i];
                    }
                }

                // Год
                QString year = info.lastModified().toString("yyyy");

                // Альбом (Имя папки)
                QString album = info.dir().dirName();

                // ПРАВКА 2: Если папка называется так же, как исполнитель, это скорее всего папка артиста, а не альбома
                // Но для простоты оставим имя папки.
                if (album == ".") album = "Неизвестный альбом";

                m_library.append({
                    info.absoluteFilePath(),
                    baseName,
                    info.suffix(),
                    info.size(),
                    title, artist, album, year
                });
            }
        }
    }
    const QVector<AudioTrack>& getLibrary() const { return m_library; }

    // --- ПЛЕЕР И ОЧЕРЕДЬ ---

    void playQueue(const QVector<AudioTrack>& newQueue, int startIndex = 0) {
        m_queue = newQueue;
        m_currentIndex = startIndex;
        emit queueChanged(m_queue.size());

        if (m_currentIndex >= 0 && m_currentIndex < m_queue.size()) {
            playTrackInternal(m_queue[m_currentIndex]);
        } else {
            m_player->stop(); // Если очередь пуста
        }
    }

    QVector<AudioTrack> getQueue() const { return m_queue; }

    void togglePlayback() {
        if (m_player->playbackState() == QMediaPlayer::PlayingState) m_player->pause();
        else if (!m_queue.isEmpty()) m_player->play();
    }

    void nextTrack() {
        if (m_queue.isEmpty()) return;
        m_currentIndex++;
        if (m_currentIndex >= m_queue.size()) m_currentIndex = 0;
        playTrackInternal(m_queue[m_currentIndex]);
    }

    void prevTrack() {
        if (m_queue.isEmpty()) return;
        m_currentIndex--;
        if (m_currentIndex < 0) m_currentIndex = m_queue.size() - 1;
        playTrackInternal(m_queue[m_currentIndex]);
    }

    void seek(qint64 position) { m_player->setPosition(position); }

    QMediaPlayer* rawPlayer() { return m_player; }
    AudioTrack currentTrack() const {
        if (m_currentIndex >= 0 && m_currentIndex < m_queue.size()) return m_queue[m_currentIndex];
        return {};
    }

signals:
    void stateChanged(bool isPlaying);
    void trackChanged(AudioTrack track);
    void positionChanged(qint64 pos);
    void durationChanged(qint64 duration);
    void metaDataChanged();
    void queueChanged(int count);

private:
    Core() {
        m_player = new QMediaPlayer(this);
        m_audioOutput = new QAudioOutput(this);
        m_player->setAudioOutput(m_audioOutput);
        m_audioOutput->setVolume(0.5);

        QSettings settings("AudioLab", "MusicManager");
        m_sources = settings.value("library/sources").toStringList();
        if (m_sources.isEmpty()) m_sources.append(QDir::homePath() + "/Music");

        connect(m_player, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state){
            emit stateChanged(state == QMediaPlayer::PlayingState);
        });
        connect(m_player, &QMediaPlayer::positionChanged, this, &Core::positionChanged);
        connect(m_player, &QMediaPlayer::durationChanged, this, &Core::durationChanged);
        connect(m_player, &QMediaPlayer::metaDataChanged, this, &Core::metaDataChanged);
    }

    void playTrackInternal(const AudioTrack& track) {
        m_player->setSource(QUrl::fromLocalFile(track.filePath));
        m_player->play();
        emit trackChanged(track);
    }

    void saveSettings() {
        QSettings settings("AudioLab", "MusicManager");
        settings.setValue("library/sources", m_sources);
    }

    QVector<std::shared_ptr<IModule>> m_modules;
    QStringList m_sources;
    QVector<UserFilter> m_filters;
    QVector<AudioTrack> m_library;
    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;

    QVector<AudioTrack> m_queue;
    int m_currentIndex = -1;
};

#endif // CORE_H
