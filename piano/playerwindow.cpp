#include "playerwindow.h"
#include "ui_playerwindow.h"

PlayerWindow::PlayerWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerWindow)
{
    ui->setupUi(this);
    connect(ui->homeButton, &QPushButton::clicked, this, &QWidget::close);
    setWindowTitle("MIDI player");
    setWindowIcon(QIcon(":/img/iconplayer.ico"));
    QPixmap background(":/img/wallpaperplayer.png");
    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(background));
    this->setPalette(palette);
    this->setAutoFillBackground(true);
    resize(background.width(), background.height());
    setFixedSize(background.width(), background.height());

    ui->Table->verticalHeader()->setVisible(false);
    ui->Table->setColumnCount(1);
    ui->Table->setHorizontalHeaderLabels(QStringList()<< "File name");
    ui->Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->Table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->Table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->Table, &QTableWidget::cellDoubleClicked,
            this, &PlayerWindow::playFile);

    ui->homeButton->setToolTip("Return to Virtual Piano");
    ui->infoButton->setToolTip("Read manual");
    ui->pauseButton->setToolTip("Play/Pause");
    ui->uploadButton->setToolTip("Upload files");
    ui->prevButton->setToolTip("Previous file");
    ui->nextButton->setToolTip("Next file");
    ui->stopButton->setToolTip("Reset to start");

    midiOut = new QMidiOut();
    QMap<QString, QString> devicesMap = QMidiOut::devices();
    if (!devicesMap.isEmpty()) {
        midiOut->connect(devicesMap.keys().first());}

    playTimer.setInterval(50);
    connect(&playTimer, &QTimer::timeout, this, &PlayerWindow::updatePlayProgress);

    // Настройка слайдера
    ui->progressSlider->setRange(0, 1000);
    ui->progressSlider->setEnabled(false);
    ui->progressSlider->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    connect(ui->progressSlider, &QSlider::sliderReleased, this, &PlayerWindow::seekToPos);


}

PlayerWindow::~PlayerWindow()
{
    delete ui;
    Files.clear();
    Paths.clear();
    Durs.clear();
}

void PlayerWindow::on_uploadButton_clicked() {
    QString filePath = QFileDialog::getOpenFileName( this, "Выберите MIDI файл",
        QDir::homePath(), "MIDI Files (*.mid *.midi)");
    QFile file(filePath);
    if (!file.exists()) {
        QMessageBox::warning(this, "Error", "File doesn't exist");
        return;}
    if (file.size() > 20 * 1024 * 1024) {
        QMessageBox::warning(this, "Error", "File is too big (max 20 МБ)");
        return;}
    if (Paths.contains(filePath)) {
        QMessageBox::information(this, "Info", "This file is already loaded");
        return;}
    QMidiFile midiFile;
    if (!midiFile.load(filePath)) {
        QMessageBox::warning(this, "Error", "Couldn't load MIDI file");
        return;}
    Files.emplace_back(std::make_shared<QMidiFile>());
    if (!Files.back()->load(filePath)) {
        Files.pop_back(); } //если чтото пойдет не так
    Durs.push_back(durInSec(Files.back().get()));
    Paths.push_back(filePath);
    QTimer::singleShot(0, this, &PlayerWindow::refreshTable);
}

void PlayerWindow::refreshTable() {
    ui->Table->setRowCount(0);
    ui->Table->setColumnCount(1);
    ui->Table->setHorizontalHeaderLabels(QStringList()<< "File name");
    ui->Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for (int i = 0; i < Paths.size(); ++i) {
        int row = ui->Table->rowCount();
        ui->Table->insertRow(row);
        QFileInfo fileInfo(Paths[i]);
        QTableWidgetItem *nameItem = new QTableWidgetItem(fileInfo.fileName());
        nameItem->setData(Qt::UserRole, Paths[i]);
        ui->Table->setItem(row, 0, nameItem);
        nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);
    }
    updateButtonStates();
}

void PlayerWindow::playFile(int row) {
    if (row < 0 || row >= Files.size() || Files.empty())
        return;
    stopPlay(0);
    currentFileIndex = row;
    startPlay();
}

void PlayerWindow::startPlay() {
    if (currentFileIndex < 0 || currentFileIndex >= Files.size())
        return;
    auto& midiFile = Files[currentFileIndex];
    totalDur = Durs[currentFileIndex] * 1000; //в мс
    currentPlayPos=0;
    if (midiOut && midiOut->isConnected()) {
        midiOut->stopAll();
        playTimer.start();
        for (QMidiEvent* event : midiFile->events()) {
            midiOut->sendEvent(*event);
        }
    }
    is_playing = true;
    updatePlay();}

void PlayerWindow::updatePlayProgress() {
    if (!is_playing) return;
    currentPlayPos += playTimer.interval();
    ui->progressSlider->setValue((currentPlayPos * 1000) / totalDur);
    if (currentPlayPos >= totalDur) {
        stopPlay(1);
    }
}

void PlayerWindow::updateButtonStates()
{
    bool hasFiles = !Files.empty(); //кнопки активны ток когда есть файлы
    ui->pauseButton->setEnabled(hasFiles);
    ui->stopButton->setEnabled(hasFiles);
    ui->prevButton->setEnabled(hasFiles);
    ui->nextButton->setEnabled(hasFiles);
    ui->progressSlider->setEnabled(hasFiles);
    if (!hasFiles) {
        currentFileIndex = -1;
        is_playing = 0;
        ui->progressSlider->setValue(0);}
    updatePlay();
}

void PlayerWindow::pausePlay() {
    playTimer.stop();
    if (midiOut) {
        midiOut->stopAll();}
    is_playing = false;
    updatePlay();
}

void PlayerWindow::stopPlay(bool resetPos) {
    playTimer.stop();
    if (midiOut) {
        midiOut->stopAll();}
    is_playing = false;
    if (resetPos) {
        currentPlayPos = 0;
        ui->progressSlider->setValue(0);
    }
    updatePlay();
}

void PlayerWindow::updatePlay() {
    if (is_playing) {
        ui->pauseButton->setIcon(QIcon(":/img/pause.png"));}
    else {
        ui->pauseButton->setIcon(QIcon(":/img/play.png"));}
}

void PlayerWindow::seekToPos() {
    int sliderVal = ui->progressSlider->value();
    currentPlayPos = (sliderVal * totalDur)/1000;

    // Здесь должна быть логика перемотки MIDI
    // (это сложная часть, требующая обработки событий)
    midiOut->stopAll();
    auto& midiFile = Files[currentFileIndex];
    qint32 targetTick = midiFile->tickFromTime(currentPlayPos / 1000.0f);

    for (QMidiEvent* event : midiFile->events()) {
        if (event->tick() >= targetTick) {
            midiOut->sendEvent(*event);}}
    if (is_playing) {
        midiOut->stopAll();}
}

void PlayerWindow::on_homeButton_clicked(){
emit homeButtonClicked();
this->close();}

void PlayerWindow::on_pauseButton_clicked() {
    if (Files.empty()) return;
    if (is_playing) {
        pausePlay();
        ui->pauseButton->setIcon(QIcon(":/img/play.png"));}
    else {
        if (currentFileIndex == -1 && !Files.empty()) {
            currentFileIndex = 0;} // пусть само играет сначала
        startPlay();
        ui->pauseButton->setIcon(QIcon(":/img/pause.png"));
    }
}

void PlayerWindow::on_stopButton_clicked() {
    if (!Files.empty())
        stopPlay(1);}

float PlayerWindow::durInSec(QMidiFile* midiFile) const {
    if (!midiFile || midiFile->events().isEmpty())
        return 0.0f;

    // Находим последний тик среди всех событий
    qint32 maxTick = 0;
    for (QMidiEvent* event : midiFile->events()) {
        if (event->tick() > maxTick) {
            maxTick = event->tick();
        }
    }
    return midiFile->timeFromTick(maxTick);
}
