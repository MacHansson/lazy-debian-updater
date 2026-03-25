#include "logdialog.h"
#include "ui_logdialog.h"
#include <QTimer>
#include <QDir>
#include <QScrollBar>
#include <QDateTime>
#include <algorithm>
#include <QElapsedTimer>
#include <debugflag.h>

LogDialog::LogDialog(QString logDirectory, QWidget *parent)
    : FramelessDialog(parent)
    , ui(new Ui::LogDialog)
    , m_logDirectory(logDirectory)
{
    ui->setupUi(this);
    updateButtonIcons();

    ui->laInfo->setVisible(false);
    ui->laWarning->setVisible(false);
    QObject::connect(ui->bnClearFilter, &QPushButton::clicked, this, [this] () { ui->leSearch->clear(); on_leSearch_editingFinished(); });
    QObject::connect(ui->bnPin, &QPushButton::clicked, this, [this] () { setPinned(ui->bnPin->isChecked()); ui->bnHide->setVisible(!ui->bnPin->isChecked()); });
}

LogDialog::~LogDialog()
{
    delete ui;
}

void LogDialog::updateLogs()
{
    QDir dir(m_logDirectory);
    m_lFiles = dir.entryInfoList();
    std::sort(m_lFiles.begin(), m_lFiles.end(), LogDialog::compareByDate);

    if(DEBUG) qDebug().noquote() << "Reading file list from log directory" << m_logDirectory << "and found" << m_lFiles.size() << "files";

    ui->listLogs->clear();
    if(m_lSearchResults.isEmpty()) { // No search active
        for (QFileInfo info : m_lFiles) {
            if(info.baseName().size() > 3 && !info.baseName().contains("last")) {
                ui->listLogs->addItem(info.baseName() + (!info.suffix().isEmpty() ? ("." + info.suffix()) : ""));
            }
        }
    } else { // Search active ...
        if(m_lSearchResults.at(0) == -1) { // ... nothing found, no results available
            ui->listLogs->clear();
            ui->textEditSelectedLog->setText("Search string not found.");
        } else { // ... results available
            for(int i : m_lSearchResults) {
                QFileInfo info = m_lFiles.at(i);
                ui->listLogs->addItem(info.baseName() + (!info.suffix().isEmpty() ? ("." + info.suffix()) : ""));
            }
        }
    }

    if(ui->listLogs->count() > 0) {
        QListWidgetItem *item = ui->listLogs->item(0);
        ui->listLogs->setCurrentItem(item);
        on_listLogs_itemClicked(item);
    }
}

QDateTime LogDialog::getLastLogDateTime()
{
    if(ui->listLogs->count() < 1) {
        return QDateTime();
    }

    QListWidgetItem *item = ui->listLogs->item(0);
    QFileInfo info(QDir::toNativeSeparators(m_logDirectory + "/" + item->text()));
    return info.lastModified();
}

void LogDialog::updateButtonIcons()
{
    bool isDarkMode{true};
    QColor windowColor = palette().color(QPalette::Window);
    double brightness = 0.299 * windowColor.red() +
                        0.587 * windowColor.green() +
                        0.114 * windowColor.blue();
    isDarkMode = (brightness < 128);

    QIcon pin;
    if(isDarkMode) {
        ui->bnHide->setIcon(QIcon(":/resources/svg/Close-Dark.svg"));
        ui->bnClearFilter->setIcon(QIcon(":/resources/svg/Delete-Dark.svg"));
        pin.addFile(":/resources/svg/PinOff-Dark.svg", QSize(), QIcon::Normal, QIcon::Off);
        pin.addFile(":/resources/svg/PinOn.svg", QSize(), QIcon::Normal, QIcon::On);
    } else {
        ui->bnHide->setIcon(QIcon(":/resources/svg/Close-Light.svg"));
        ui->bnClearFilter->setIcon(QIcon(":/resources/svg/Delete-Light.svg"));
        pin.addFile(":/resources/svg/PinOff-Light.svg", QSize(), QIcon::Normal, QIcon::Off);
        pin.addFile(":/resources/svg/PinOn.svg", QSize(), QIcon::Normal, QIcon::On);
    }
    ui->bnPin->setIcon(pin);

    if(DEBUG) qDebug().noquote() << "Changing theme of log dialog to:" << (isDarkMode ? "dark" : "light");
}

void LogDialog::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::PaletteChange) {
        updateButtonIcons();
    }
    QWidget::changeEvent(event);
}

bool LogDialog::compareByDate(const QFileInfo &a, const QFileInfo &b) {
    return a.lastModified() > b.lastModified(); // ascending order
}

void LogDialog::on_listLogs_itemClicked(QListWidgetItem *item)
{
    QString logFile = QDir::toNativeSeparators(m_logDirectory + "/" + item->text());
    QStringList lines;
    QFile file(logFile);
    if(file.exists()) {
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                lines << in.readLine();
            }
        } else {
            ui->textEditSelectedLog->setText("Failed to open file: " + file.fileName());
            return;
        }
    }

    ui->textEditSelectedLog->clear();
    QTimer::singleShot(50, this, [this, lines] () {
        if(!lines.isEmpty()) {
            ui->textEditSelectedLog->clear();
            for(QString line : lines) {
                ui->textEditSelectedLog->append(line);
                ui->textEditSelectedLog->verticalScrollBar()->setValue(ui->textEditSelectedLog->verticalScrollBar()->minimum());        }
        }
    });
}

void LogDialog::on_bnHide_clicked()
{
    //ui->leSearch->clear();
    hideBeautiful();
}

void LogDialog::on_leSearch_editingFinished()
{
    if(DEBUG) qDebug() << "Starting search in" << m_lFiles.size() << "files";

    QElapsedTimer timer;
    timer.start();
    m_lSearchResults.clear();

    QStringList search = ui->leSearch->text().simplified().split(" ");
    for(QString str : search) {
        if(str.size() < 3) {
            if(!ui->leSearch->text().isEmpty()) {
                if(DEBUG) qWarning().noquote() << "WARNING: Found invalid search string (every search string needs to be at least 3 characters long)";
                ui->laWarning->setText(" Invalid search string found, aborting search. Every search string needs to be at least 3 characters long.");
                ui->laWarning->setVisible(true);
                ui->laInfo->setVisible(false);
                //QTimer::singleShot(10000, this, [this] () { ui->laWarning->setVisible(false); });
                ui->leSearch->clear();
            } else {
                ui->laInfo->setVisible(false);
                ui->laWarning->setVisible(false);
                if(DEBUG) qWarning().noquote() << "Clearing search filter and loading complete log list";
            }
            m_lSearchResults.clear();
            updateLogs();
            return;
        }
    }

    if(ui->laWarning->isVisible()) {
        ui->laWarning->setVisible(false);
    }

    ui->laInfo->setText(" Current search: " + search.join(", "));
    ui->laInfo->setVisible(true);

    int filesSearched {0};
    bool found {false};
    QStringList lines;
    for(int i=0; i<m_lFiles.size(); i++) {
        filesSearched++;
        found = false;
        QFileInfo info = m_lFiles.at(i);
        QFile file(info.absoluteFilePath());
        if(file.exists()) {
            if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                lines.clear();
                while (!in.atEnd()) {
                    lines << in.readLine();
                }
            } else {
                if(DEBUG) qDebug() << "Failed to open file" << file.fileName();
            }
        }
        for(QString line : lines) {
            for(QString str : search) {
                if(line.contains(str, Qt::CaseInsensitive)) {
                    if(DEBUG) qDebug() << "Found" << str << "in file" << info.fileName();
                    m_lSearchResults.append(i);
                    found = true;
                    break;
                }
            }
            if(found) {
                break;
            }
        }
    }

    if(m_lSearchResults.isEmpty()) {
        m_lSearchResults.append(-1); // nothing found indicator
    }

    if(DEBUG) qDebug().noquote() << "Finished search in" << QString::number(timer.elapsed() / 1000.0, 'f', 2) << "seconds with" << m_lSearchResults.size() << "result(s)" << "in" << filesSearched << "file(s)";
    timer.start();
    updateLogs();
    if(DEBUG) qDebug().noquote() << "Finished updating logs in" << QString::number(timer.elapsed() / 1000.0, 'f', 2) << "seconds";
}

