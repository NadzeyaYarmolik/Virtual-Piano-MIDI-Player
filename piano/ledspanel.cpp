#include "ledspanel.h"
#include "channeldialog.h" // Теперь включаем здесь
#include "helperclass.h"
#include "playerwindow.h"
#include <QGridLayout>
#include <QFile>
#include <QContextMenuEvent>
#include <QMenu>

// ledspanel.cpp - исправить конструктор
LedsPanel::LedsPanel(PlayerWindow* parentPlayer, QWidget *parent)
    : QWidget(parent), m_parentPlayer(parentPlayer)
{

    // Проверяем, не создавались ли уже кнопки
    if (!m_leds.isEmpty()) {
        return;
    }

    // Создаем основной layout - 4x4 сетка
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(4, 4, 4, 4);

    // 16 светодиодов 4 на 4
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            int index = row * 4 + col;

            QPushButton *led = new QPushButton(this);
            led->setFixedSize(90, 90);
            led->setIconSize(QSize(80, 80));
            //led->setFlat(true);
            led->setStyleSheet("QPushButton { border: none; background: transparent; }");

            // контекстное меню по правому клику
            led->setContextMenuPolicy(Qt::CustomContextMenu);

            // установка контекстного меню для каждой кнопки
            connect(led, &QPushButton::customContextMenuRequested,
                    [this, index](const QPoint &localPos) {
                        // Преобразуем локальную позицию в глобальную
                        QPushButton *ledButton = m_leds.value(index, nullptr);
                        if (ledButton) {
                            QPoint globalPos = ledButton->mapToGlobal(localPos);
                            showChannelContextMenu(index, globalPos);
                        }
                    });

            m_leds.append(led); //к списку кнопок
            gridLayout->addWidget(led, row, col); //сетка
        }
    }

    setStyle("modern");//стиль по умолчанию
}

LedsPanel::~LedsPanel()
{
    // Очищаем LED кнопочки
    m_leds.clear();
    delete m_channelDialog;// сносим диалог
}

void LedsPanel::showChannelContextMenu(int channel, const QPoint &globalPos)
{
    //пункты
    QMenu menu(this);
    QAction *volumeAction = menu.addAction("Volume Control");
    QAction *soloAction = menu.addAction("Solo Channel");
    QAction *infoAction = menu.addAction("Channel Info");
    menu.addSeparator();
    QAction *resetAllAction = menu.addAction("Reset All Channels");

    // включен ли solo на этом канале
    bool isThisChannelSolo = false;
    if (m_parentPlayer) {
        isThisChannelSolo = m_parentPlayer->isSoloActive() &&
                            m_parentPlayer->soloChannel() == channel;
    }

    // soloAction с галочкой
    soloAction->setCheckable(true);
    soloAction->setChecked(isThisChannelSolo);

    // обработчики для каждого пункта в меню
    connect(volumeAction, &QAction::triggered, [this, channel]() {
        showChannelDialog(channel);
    });

    connect(soloAction, &QAction::triggered, [this, channel, isThisChannelSolo]() {
        if (m_parentPlayer) {
            if (isThisChannelSolo) {
                m_parentPlayer->unsoloAllChannels();  // Отключаем solo
            } else {
                m_parentPlayer->setSoloChannel(channel);  // Включаем solo
            }
        }
    });

    connect(infoAction, &QAction::triggered, [this, channel]() {
        if (m_parentPlayer) {
            // окошко с инфой о канале
            QString info = QString("Channel %1\nInstrument: %2\nCurrent Volume: %3%\nSaved Volume: %4%\nSolo: %5")
                               .arg(channel + 1)
                               .arg(m_parentPlayer->getChannelInstrumentName(channel))
                               .arg(m_parentPlayer->getChannelVolume(channel))
                               .arg(m_parentPlayer->getSavedChannelVolume(channel))
                               .arg(m_parentPlayer->isSoloActive() && m_parentPlayer->soloChannel() == channel ? "Yes" : "No");

            QMessageBox::information(this, "Channel Info", info);
        }
    });

    connect(resetAllAction, &QAction::triggered, [this]() {
        resetAllChannels();  // сброс всех каналов
    });

    menu.exec(globalPos);  // что меню было у соотв лампочки
}

void LedsPanel::updateChannels(const QVector<bool> &activeChannels)
{
    // если не соответствует размер массива
    if (activeChannels.size() != m_leds.size()) {
        return;
    }

    for (int i = 0; i < activeChannels.size(); ++i) {
        if (m_leds[i] && i < activeChannels.size()) {
            // иконка в зависимости от активности канала
            QIcon icon = activeChannels[i] ? m_onIcon : m_offIcon;
            if (!icon.isNull()) {
                m_leds[i]->setIcon(icon);
            }

            // tooltip с инфой о канале
            QString instrumentName = "Unknown";
            if (m_parentPlayer) {
                instrumentName = m_parentPlayer->getChannelInstrumentName(i);
            }

            m_leds[i]->setToolTip(QString("Channel %1 - %2\nRight-click for volume control")
            .arg(i).arg(instrumentName));
        }
    }
}

void LedsPanel::setStyle(const QString &styleName)
{
    // стиль уже установлен
    if (m_currentStyle == styleName) {
        return;
    }
    m_currentStyle = styleName;

    // пути к иконкам для темы
    QString basePath = ":/img/%1/led_%2.png";
    QString onPath = basePath.arg(styleName).arg("on");
    QString offPath = basePath.arg(styleName).arg("off");

    // если что произойдет не так с файлами то modern
    if (!QFile::exists(onPath)) {
        onPath = basePath.arg("modern").arg("on");
    }
    if (!QFile::exists(offPath)) {
        offPath = basePath.arg("modern").arg("off");
    }

    m_onIcon = QIcon(onPath);
    m_offIcon = QIcon(offPath);

    // обновляем отображение
    QVector<bool> defaultState(16, false);
    updateChannels(defaultState);
}

QString LedsPanel::currentStyle() const
{
    return m_currentStyle;
}

void LedsPanel::showChannelDialog(int channel)
{
    // создаем диалог если еще не создан
    if (!m_channelDialog) {
        if (m_parentPlayer) {
            m_channelDialog = new ChannelDialog(m_parentPlayer, this);
        } else {
            return;
        }
    }

    // настройка для работы с каналом
    m_channelDialog->setChannel(channel);
    m_channelDialog->show();
    m_channelDialog->raise();
    m_channelDialog->activateWindow();
}

void LedsPanel::updateFonts(const QFont& font)
{
    foreach (QPushButton* led, m_leds) {
        led->setFont(font);
        // обнова tooltip для применения нового шрифта
        QString tooltip = led->toolTip();
        led->setToolTip(""); // очищаем
        led->setToolTip(tooltip); // устанавливаем обратно
    }
}

void LedsPanel::soloChannel(int channel) {
    if (!m_parentPlayer) return;

    // если хотим выкл сча активный соло
    if (m_parentPlayer->isSoloActive() && m_parentPlayer->soloChannel() == channel) {
        m_parentPlayer->unsoloAllChannels();  // solo выкл
    } else {
        m_parentPlayer->setSoloChannel(channel);  // solo вкл
    }

    // обнова визуализации
    if (m_parentPlayer) {
        QVector<bool> activity(16, false);
        if (m_parentPlayer->isSoloActive()) {
            activity[m_parentPlayer->soloChannel()] = true;
        }
        updateChannels(activity);
    }
}

void LedsPanel::resetAllChannels()
{
    if (!m_parentPlayer) return;

    // воспросик???
    if (QMessageBox::question(this, "Reset All Channels",
                              "Are you sure you want to reset all channel volumes to 100%?\n"
                              "(Master volume will still affect overall loudness)",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        m_parentPlayer->resetAllChannelVolumes();  // сброс в плеере
    }
}
