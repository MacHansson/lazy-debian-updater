#include "logdialog.h"
#include "ui_logdialog.h"
#include <QFileInfoList>
#include <QDir>
#include <QScrollBar>
#include <QDateTime>
#include <algorithm>

#include <debugflag.h>

LogDialog::LogDialog(QString logDirectory, QWidget *parent)
    : FramelessDialog(parent)
    , ui(new Ui::LogDialog)
    , m_logDirectory(logDirectory)
{
    ui->setupUi(this);
}

LogDialog::~LogDialog()
{
    delete ui;
}

void LogDialog::updateLogs()
{
    QDir dir(m_logDirectory);
    QFileInfoList list = dir.entryInfoList();
    std::sort(list.begin(), list.end(), LogDialog::compareByDate);

    if(DEBUG) qDebug() << "Update log directory:" << m_logDirectory;

    ui->listLogs->clear();
    for (QFileInfo info : list) {
        if(info.baseName().size() > 3 && info.baseName().contains("20")) {
            ui->listLogs->addItem(info.baseName() + (!info.suffix().isEmpty() ? ("." + info.suffix()) : ""));
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

bool LogDialog::compareByDate(const QFileInfo &a, const QFileInfo &b) {
    return a.lastModified() > b.lastModified(); // ascending order
}

void LogDialog::on_listLogs_itemClicked(QListWidgetItem *item)
{
    QString logFile = QDir::toNativeSeparators(m_logDirectory + "/" + item->text());
    QStringList lines;
    QFile file(logFile);
    if(file.exists()) {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        while (!in.atEnd()) {
            lines << in.readLine();
        }
    }
    if(!lines.isEmpty()) {
        ui->textEditSelectedLog->clear();
        for(QString line : lines) {
            ui->textEditSelectedLog->append(line);
            ui->textEditSelectedLog->verticalScrollBar()->setValue(ui->textEditSelectedLog->verticalScrollBar()->minimum());        }
    }
}

void LogDialog::on_bnHide_clicked()
{
    hide();
}

