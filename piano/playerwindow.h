#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QMediaPlayer>
#include <QFileInfo>
#include <QTimer>
#include <QActionGroup>
#include <QPoint>
#include <QMenu>
#include <QContextMenuEvent>
#include <QThread>
#include <QStyle>
#include <QSettings>
#include <QComboBox>
#include <QSoundEffect>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMutex>
#include <QTextEdit>
#include "thirdparty/QMidi-master/src/QMidiFile.h"
#include "thirdparty/QMidi-master/src/QMidiOut.h"
#include <algorithm>
#include <random>
#include <format>
#include "helperclass.h"

class LedsPanel;

namespace Ui {
class PlayerWindow;
}

class PlayerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerWindow(QMidiOut* mainMidiOut, QWidget *parent = nullptr);
    ~PlayerWindow();

    std::vector<std::shared_ptr<QMidiFile> > Files;  // Загруженные MIDI файлы
    QStringList Paths;                               // Пути к файлам

    enum PlaybackMode {
        RepeatAll,    // Повтор всего плейлиста
        NoRepeat,     // Без повторения
        RepeatOne     // Повтор текущего файла
    };

    enum class Theme {
        Modern,       // Современная тема
        Paper,        // Бумажная тема
        Water         // Водная тема
    };

    enum SortMode {
        SortByName,      // Сортировка по имени
        SortByDuration,  // Сортировка по длительности
        SortByLoadOrder  // Сортировка по порядку загрузки
    };

    enum PlaybackSpeed {
        SpeedHalf = 0,    // 0.5x скорость
        SpeedNormal = 1,  // 1.0x скорость
        SpeedDouble = 2   // 2.0x скорость
    };

    // Состояние MIDI канала
    struct ChannelState {
        int currentVolume = 100;   // Текущая громкость
        int savedVolume = 100;     // Сохраненная громкость (до mute)
        bool isMuted = false;      // Флаг mute состояния
    };

    // Настройки MIDI канала
    struct ChannelSettings {
        int volume = 100;                 // Громкость канала
        bool isMuted = false;             // Mute состояние
        int program = -1;                 // Текущий инструмент
        QMap<int, int> controllerValues;  // Значения контроллеров
    };

    //ДОСТУП
    QString setSliderStyleSheet() const;                         // Возвращает стиль для слайдеров
    QMidiOut* getMidiOut() const { return midiOut; }             // Возвращает MIDI output
    QString getCurrentThemeName() const;                         // Возвращает имя текущей темы
    int getChannelVolume(int channel) const;                     // Возвращает громкость канала
    int getSavedChannelVolume(int channel) const;                // Возвращает сохраненную громкость
    bool isChannelMuted(int channel) const;                      // Проверяет mute состояние канала
    void setChannelVolume(int channel, int volume);              // Устанавливает громкость канала
    void saveChannelSettings(int channel);                       // Сохраняет настройки канала
    void loadChannelSettings();                                  // Загружает настройки каналов
    void resetAllChannelVolumes();                               // Сбрасывает громкости всех каналов
    QString getChannelInstrumentName(int channel) const;         // Возвращает название инструмента канала

    //УПРАВЛЕНИЕ SOLO
    void setSoloChannel(int channel);       // Включает/выключает solo для канала
    void unsoloAllChannels();               // Отключает solo для всех каналов
    bool isSoloActive() const { return m_isSoloActive; }  // Проверяет активность solo
    int soloChannel() const { return m_soloChannel; }     // Возвращает solo канал

    //УПРАВЛЕНИЕ СКОРОСТЬЮ
    PlaybackSpeed playbackSpeed() const { return m_playbackSpeed; }  // Возвращает текущую скорость
    void setPlaybackSpeed(PlaybackSpeed speed);                      // Устанавливает скорость
    QString playbackSpeedName(PlaybackSpeed speed) const;            // Возвращает название скорости
    float playbackSpeedMultiplier() const;                           // Возвращает множитель скорости

protected:
    void updateVolume(int volume);                      // Обновляет общую громкость
    void closeEvent(QCloseEvent *event) override;       // Обработчик закрытия окна
    void contextMenuEvent(QContextMenuEvent *event) override;  // Контекстное меню
    void keyPressEvent(QKeyEvent *event) override;      // Обработка клавиатуры

private slots:
    //ОБРАБОТКА UI
    void on_uploadButton_clicked();        // Загрузка файла
    void on_homeButton_clicked();          // Возврат на главный экран
    void on_pauseButton_clicked();         // Play/Pause
    void on_stopButton_clicked();          // Stop
    void on_volumeSlider_valueChanged(int value);  // Изменение громкости
    void on_muteButton_clicked();          // Mute/Unmute
    void on_settingsButton_clicked();      // Настройки
    void on_nextButton_clicked();          // Следующий трек
    void on_prevButton_clicked();          // Предыдущий трек
    void onShowVisualizationTriggered();   // Показать/скрыть визуализацию
    void refreshTable();                   // Обновление таблицы плейлиста

    void on_infoButton_clicked();

private:
    Ui::PlayerWindow *ui;                  // UI компоненты
    QMidiOut* midiOut;                     // MIDI output устройство

    //СОСТОЯНИЕ ВОСПРОИЗВЕДЕНИЯ
    bool is_playing = 0;                   // Флаг воспроизведения
    bool is_paused = false;                // Флаг паузы
    bool is_mute = 0;                      // Флаг mute
    int saved_volume = 100;                // Сохраненная громкость
    qint64 m_pausePosition = 0;            // Позиция при паузе
    int currentFileIndex = -1;             // Индекс текущего файла
    int currentPlayPos = 0;                // Текущая позиция воспроизведения
    QTimer playTimer;                      // Таймер воспроизведения
    int totalDur = 0;                      // Общая длительность
    float lastProcessedTime = 0.0f;        // Время последней обработки

    //MIDI ДАННЫЕ
    QMap<int, int> channelInstruments;     // Инструменты по каналам
    QSet<QPair<int, int>> activeNotes;     // Активные ноты
    QVector<float> Durs;                   // Длительности файлов

    //НАСТРОЙКИ ВОСПРОИЗВЕДЕНИЯ
    PlaybackMode m_playbackMode = NoRepeat;// Режим воспроизведения
    PlaybackSpeed m_playbackSpeed;         // Скорость воспроизведения
    SortMode m_sortMode;                   // Режим сортировки
    Theme currentTheme = Theme::Modern;    // Текущая тема

    //ВИЗУАЛИЗАЦИЯ
    LedsPanel *m_visualizationPanel;       // Панель визуализации каналов
    QVector<bool> m_channelActivity;       // Активность каналов

    //УПРАВЛЕНИЕ ГРОМКОСТЬЮ
    QVector<int> m_channelVolumes;         // Громкости каналов (0-127)
    int m_masterVolume;                    // Мастер-громкость (0-100)
    QVector<int> m_savedVolumes;           // Сохраненные громкости
    QVector<bool> m_channelMuted;          // Mute состояния каналов
    QVector<ChannelState> channelStates;   // Состояния каналов

    //SOLO УПРАВЛЕНИЕ
    bool m_isSoloActive = false;           // Флаг активности solo
    int m_soloChannel = -1;                // Текущий solo канал
    QVector<int> m_preSoloVolumes;         // Громкости до solo
    QSet<int> m_manualEditedChannels;      // Вручную измененные каналы

    //СИСТЕМНЫЕ ФЛАГИ
    bool m_isDestructing = false;          // Флаг разрушения объекта
    bool wasPlayingBeforeSeek = false;     // Флаг воспроизведения до seek
    bool m_isSeeking = false;              // Флаг перемотки

    //MIDI ТАЙМИНГ
    qint32 m_previousTick;                 // Последний обработанный тик
    qint32 m_totalTicks;                   // Общее количество тиков

    //ВСПОМОГАТЕЛЬНОЕ
    void playFile(int row);                // Воспроизведение файла
    void startPlay();                      // Запуск воспроизведения
    void pausePlay();                      // Пауза/продолжение
    void stopPlay(bool resetPos);          // Остановка воспроизведения
    void updatePlay();                     // Обновление состояния play/pause
    void updatePlayProgress();             // Обновление прогресса
    void updateButtonStates();             // Обновление состояний кнопок
    float durInSec(QMidiFile* midiFile) const;  // Расчет длительности
    void processMidiEvent(QMidiEvent* event);   // Обработка MIDI события
    void handlePlaybackFinished();         // Обработка завершения воспроизведения
    void savePlaybackMode();               // Сохранение режима воспроизведения
    void loadPlaybackMode();               // Загрузка режима воспроизведения
    void shufflePlaylist();                // Перемешивание плейлиста
    void seekToPosition(int position);     // Перемотка на позицию
    void updateThemeIcons();               // Обновление иконок темы
    void saveThemeSettings();              // Сохранение настроек темы
    void loadThemeSettings();              // Загрузка настроек темы
    void updateNavButtonsState();          // Обновление кнопок навигации
    QString getElidedText(const QString &text, const QFont &font, int width);  // Обрезка текста
    void updateChannelActivity(int channel, bool active);  // Обновление активности канала
    bool hasActiveNotesOnChannel(int channel) const;  // Проверка активных нот
    void resetAllNotes();                  // Сброс всех нот
    void resetMidiChannels();              // Сброс MIDI каналов
    void sortPlaylist();                   // Сортировка плейлиста
    void saveSortSettings();               // Сохранение настроек сортировки
    void loadSortSettings();               // Загрузка настроек сортировки
    void saveSoloState();                  // Сохранение состояния solo
    void restoreSoloState();               // Восстановление состояния solo
    void saveCurrentState();               // Сохранение текущего состояния
    void updateProgressUI();               // Обновление UI прогресса
    void restoreChannelVolumes();          // Восстановление громкостей
    void restoreInstrumentsAndControllers();  // Восстановление инструментов
    void savePlaybackSpeedSettings();      // Сохранение настроек скорости
    void loadPlaybackSpeedSettings();      // Загрузка настроек скорости
    void resetMidiState();                 // Сброс MIDI состояния

    QVector<int> m_currentIndividualVolumes;// Текущие индивидуальные громкости
    bool m_wasSoloActive;                  // Предыдущее состояние solo
    int m_savedSoloChannel;                // Сохраненный solo канал
    QVector<int> m_savedPreSoloVolumes;    // Сохраненные громкости до solo

signals:
    void homeButtonClicked();              // Нажатие кнопки домой
    void windowClosed();                   // Закрытие окна
};

#endif // PLAYERWINDOW_H
