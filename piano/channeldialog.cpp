#include "channeldialog.h"
#include "ui_channeldialog.h"
#include "playerwindow.h"
#include <QSettings>

ChannelDialog::ChannelDialog(PlayerWindow* parentPlayer, QWidget *parent)
    : QDialog(parent), ui(new Ui::ChannelDialog), m_parentPlayer(parentPlayer), m_channel(-1)
{
    ui->setupUi(this);
    setWindowTitle("Channel Control");
    setFixedSize(450, 200); // Фиксируем размер окна

    // Настройка слайдера громкости (диапазон 0-100%)
    ui->volumeSlider->setRange(0, 100);

    // Кнопка mute
    ui->muteButton->setStyleSheet("border: none; background: transparent;");
    ui->muteButton->setFixedSize(100, 100);
    ui->muteButton->setIconSize(QSize(100, 100));

    // Сигналы + отработка кнопок и бегунка
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ChannelDialog::onAccepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(ui->volumeSlider, &QSlider::valueChanged, this, &ChannelDialog::on_volumeSlider_valueChanged);
    ui->volumeSlider->setToolTip("Channel volume: 100%");

    // Стиль + иконка
    applyThemeStyle();
    QString themeName = m_parentPlayer ? m_parentPlayer->getCurrentThemeName() : "modern";
    ui->muteButton->setIcon(QIcon(QString(":/img/%1/volume.png").arg(themeName)));
}

ChannelDialog::~ChannelDialog()
{
    delete ui;
}

void ChannelDialog::setChannel(int channel)
{
    m_channel = channel;

    int currentVolume = m_parentPlayer->getChannelVolume(channel);
    m_savedVolume = currentVolume;       // Основное значение на случай отмены
    m_tempSavedVolume = currentVolume;   // Временное значение для mute/unmute в пределах диалога
    m_isMute = (currentVolume == 0);     // Если канал muted

    ui->volumeSlider->setValue(currentVolume);

    // Стиль + тип иконки
    QString themeName = m_parentPlayer ? m_parentPlayer->getCurrentThemeName() : "modern";
    QString iconName = m_isMute ? "mute" : "volume";
    ui->muteButton->setIcon(QIcon(QString(":/img/%1/%2.png").arg(themeName, iconName)));
}

void ChannelDialog::on_muteButton_clicked()
{
    m_isMute = !m_isMute; // Переключаем состояние

    if (m_isMute) {
        // Сохранить temp
        m_tempSavedVolume = ui->volumeSlider->value();
        // Устанавливаем громкость в 0 + отправка к плееру
        ui->volumeSlider->setValue(0);
        if (m_parentPlayer) {
            m_parentPlayer->setChannelVolume(m_channel, 0);
        }
    } else {
        // Восстановить сохр значение, temp или из плеера
        int restoreVol = m_tempSavedVolume > 0 ? m_tempSavedVolume : m_savedVolume;
        ui->volumeSlider->setValue(restoreVol);
        if (m_parentPlayer) {
            m_parentPlayer->setChannelVolume(m_channel, restoreVol);
        }
    }

    // Иконка
    QString themeName = m_parentPlayer ? m_parentPlayer->getCurrentThemeName() : "modern";
    QString iconName = m_isMute ? "mute" : "volume";
    ui->muteButton->setIcon(QIcon(QString(":/img/%1/%2.png").arg(themeName, iconName)));
}

void ChannelDialog::on_volumeSlider_valueChanged(int value) {
    if (m_isMute && value > 0) // Mute + движение слайдером -> unmute
        m_isMute = false;

    if (m_parentPlayer) // Громкость в плеер
        m_parentPlayer->setChannelVolume(m_channel, value);

    if (!m_isMute && value > 0) // Не mute + ненулевая громкость -> сохранить temp
        m_tempSavedVolume = value;

    // Иконка
    QString themeName = m_parentPlayer ? m_parentPlayer->getCurrentThemeName() : "modern";
    QString iconName = (value == 0 || m_isMute) ? "mute" : "volume";
    ui->muteButton->setIcon(QIcon(QString(":/img/%1/%2.png").arg(themeName, iconName)));
}

void ChannelDialog::applyThemeStyle()
{
    if (m_parentPlayer) {
        QString sliderStyle = m_parentPlayer->setSliderStyleSheet();
        ui->volumeSlider->setStyleSheet(sliderStyle);

        // Обнова отображения слайдера
        ui->volumeSlider->style()->unpolish(ui->volumeSlider);
        ui->volumeSlider->style()->polish(ui->volumeSlider);
        ui->volumeSlider->update();

        HelperClass::setupButtonAnimation(ui->muteButton, ui->muteButton->iconSize(), 100);
    }
}

void ChannelDialog::onAccepted()
{
    QDialog::accept();
}

void ChannelDialog::reject() {
    // Восстановить громкость до диалога
    m_parentPlayer->setChannelVolume(m_channel, m_savedVolume);
    ui->volumeSlider->setValue(m_savedVolume);
    QDialog::reject();
}

void ChannelDialog::restoreInitialVolume() {
    if (!m_parentPlayer || m_channel == -1) return;

    ui->volumeSlider->setValue(m_initialVolume); // Слайдер в изначальное значение

    // Начальное значение громкости канала через плеер
    updateChannelVolume(m_initialVolume);
    m_isMute = (m_initialVolume == 0);

    //Обновляем сохр значение если не mute
    if (!m_isMute) {
        m_savedVolume = m_initialVolume;
    }
}

void ChannelDialog::updateChannelVolume(int volume) {
    if (!m_parentPlayer || m_channel == -1) return;
    m_parentPlayer->setChannelVolume(m_channel, volume);
}

