#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //setAttribute(Qt::WA_DeleteOnClose);

    setWindowTitle("Virtual piano");
    setWindowIcon(QIcon(":/img/icon.ico"));
    QPixmap background(":/img/wallpaper.png");
    resize(background.size());
    setStyleSheet(
        "QMainWindow {"
        "   background-image: url(:/img/wallpaper.png);"
        "   background-position: center;"
        "   background-repeat: no-repeat;"
        "}"
        );
    resize(background.width(), background.height());
    setFixedSize(background.width(), background.height());
    ui->gotoButton->setToolTip("Go to MIDI player");
    ui->infoButton->setToolTip("Read manual");

    ui->volumeBar->setValue(saved_volume);
    ui->mute->setIcon(QIcon(":/img/volume_.png"));

    ui->horizontalLayout_7->setStretch(0, 4);  // Левая панель (20%)
    ui->horizontalLayout_7->setStretch(4, 1);

    midiOut = new QMidiOut();
    QMap<QString, QString> devicesMap = QMidiOut::devices();
    if (!devicesMap.isEmpty()) {
        midiOut->connect(devicesMap.keys().first());}

    ui->instrumentBox->addItem("Acoustic Grand Piano", 0);
    ui->instrumentBox->addItem("Acoustic Guitar (nylon)", 24);
    ui->instrumentBox->addItem("Violin", 40);
    ui->instrumentBox->addItem("Trumpet", 56);
    ui->instrumentBox->addItem("Synth", 80);
    ui->octaveBox->setValue(currentOctave);

    connect(ui->instrumentBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onInstrumentChanged);
    connect(ui->octaveBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onOctaveChanged);

    setupPianoKeys();

    }

MainWindow::~MainWindow()
{
    delete ui;
    delete midiOut;
}

void MainWindow::on_mute_clicked() {
    is_mute = !is_mute;
    if (is_mute) {
    updateVolume(0);
    ui->mute->setIcon(QIcon(":/img/mute_.png"));}
    else {
    updateVolume(saved_volume);
    ui->mute->setIcon(QIcon(":/img/volume_.png"));
    ui->volumeBar->setValue(saved_volume);}
}



// void MainWindow::onVolumeChanged(int value)
// {float volume = value / 100.0f;
//     for (QSoundEffect *effect : Piano) {
//         effect->setVolume(volume);
//     }
// }

void MainWindow::onInstrumentChanged(int index) {
    currentInstrument = ui->instrumentBox->itemData(index).toInt();
    if (midiOut->isConnected()) {
        QMidiEvent event;
        event.setType(QMidiEvent::ProgramChange);
        event.setVoice(0);
        event.setNumber(currentInstrument);
        midiOut->sendEvent(event);
    }
}

void MainWindow::onOctaveChanged(int octave){
    currentOctave = octave;}

void MainWindow::setupPianoKeys() {
    QVector<QPushButton*> keys = {ui->c4, ui->c_4, ui->d4, ui->d_4, ui->e4,
    ui->f4, ui->f_4, ui->g4, ui->g_4, ui->a4, ui->a_4, ui->b4, ui->c5};
    for (QPushButton *key : keys) {
        key->setFocusPolicy(Qt::StrongFocus);
    }

    for (int i = 0; i < keys.size(); ++i) {
        keys[i]->setAutoRepeat(false); // Отключаем автоповтор

        connect(keys[i], &QPushButton::pressed, [this, i]() {
            int note = i + (currentOctave + 1) * 12;
            if (!activeNotes.value(note, false)) {
                activeNotes[note] = true;
                sendNoteOn(note);
            }
        });

        connect(keys[i], &QPushButton::released, [this, i]() {
            int note = i + (currentOctave + 1) * 12;
            if (activeNotes.value(note, false)) {
                activeNotes[note] = false;
                sendNoteOff(note);
            }
        });
    }
}

void MainWindow::sendNoteOn(int note) {
    if (!midiOut || !midiOut->isConnected()) return;

    QMidiEvent event;
    event.setType(QMidiEvent::NoteOn);
    event.setNote(note);
    event.setVoice(0);
    event.setVelocity(64);
    midiOut->sendEvent(event);
}

void MainWindow::sendNoteOff(int note) {
    if (!midiOut || !midiOut->isConnected()) return;

    QMidiEvent event;
    event.setType(QMidiEvent::NoteOff);
    event.setNote(note);
    event.setVoice(0);
    event.setVelocity(0);
    midiOut->sendEvent(event);
}


void MainWindow::on_volumeBar_valueChanged(int value) {
    if (!is_mute) {
        updateVolume(value);}
    saved_volume = value;
}



