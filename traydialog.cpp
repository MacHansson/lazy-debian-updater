#include "traydialog.h"
#include "ui_traydialog.h"

#include <QDateTime>
#include <QStyleFactory>
#include <QMenu>
#include <QProcess>
#include <QStyle>

#include <debugflag.h>

TrayDialog::TrayDialog(QWidget *parent)
    : FramelessDialog(parent)
    , ui(new Ui::TrayDialog)
{
    ui->setupUi(this);
    ui->labelResult->setMouseTracking(false);
    ui->labelLastRun->setMouseTracking(false);
    ui->labelLastUpdate->setMouseTracking(false); // Set because mouse event needs to be passed on to dialog widget to enable click & drag
    updateButtonIcons(darkMode());

    timerProgressBar = new QTimer();
    timerProgressBar->setInterval(33);
    timerProgressBar->setSingleShot(false);
    QObject::connect(timerProgressBar, &QTimer::timeout, [this]() {
        int value = ui->progressBar->value() + 1;
        if(value > ui->progressBar->maximum())
            value = 0;
        ui->progressBar->setValue(value);
    });

    if(DEBUG) {
        setMessage("Line 1 ... Currently running in<br>Line 2 ... DEBUG MODE<br>Line 3 ... optional");
    }

    // Hide GUI elements
    ui->progressBar->hide();
    ui->labelLastRun->hide();
}

TrayDialog::~TrayDialog()
{
    delete ui;
}

void TrayDialog::setStatus(int status)
{
    switch(Status(status)) {
        case Finished: {
            ui->bnRefresh->show();
            ui->progressBar->hide();
            ui->labelLastRun->show();
            dateTimeLastRun = QDateTime::currentDateTime();
            ui->labelLastRun->setText("Now");
            timerProgressBar->stop();
            break;
        }
        case Active: {
            ui->bnRefresh->hide();
            ui->progressBar->setValue(0);
            ui->progressBar->show();
            ui->labelLastRun->hide();
            timerProgressBar->start();
            break;
        }
    }
}

void TrayDialog::setLastUpdateDateTime(QDateTime dateTime)
{
    float hoursElapsed = dateTime.secsTo(QDateTime::currentDateTime()) / 3600.0;
    float hoursToday = (float)QDateTime::currentDateTime().time().hour() + (float)QDateTime::currentDateTime().time().minute()/60.0 + (float)QDateTime::currentDateTime().time().second()/3600.0;
    float diff = hoursElapsed - hoursToday;

    if(diff < 0.0) {
        ui->labelLastUpdate->setText("Last updated today");
    } else if(diff < 24.0) {
        ui->labelLastUpdate->setText("Last updated yesterday");
    } else if(diff < 48.0) {
        ui->labelLastUpdate->setText("Last updated 2 days ago");
    } else if(diff < 72.0) {
        ui->labelLastUpdate->setText("Last updated 3 days ago");
    } else {
        ui->labelLastUpdate->setText("Last updated some days ago");
    }

    //ui->labelLastUpdate->setText("Last update: " + dateTime.toString("dd.MM.yyyy hh:mm"));
}

void TrayDialog::setMessage(QString message)
{
    ui->labelResult->setText(message);
}

bool TrayDialog::darkMode()
{
    return (palette().color(QPalette::Window).lightness() < 128);
}

void TrayDialog::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}

void TrayDialog::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
        case Qt::Key_Escape: {
            event->accept();
            hide();
        }
    }
}

void TrayDialog::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::PaletteChange) {
        if(DEBUG) qDebug() << "Theme changed to" << style()->objectName() << "with" << (darkMode() ? "dark" : "light") << "mode";
        updateButtonIcons(darkMode());
        emit themeChanged();
    }
    QWidget::changeEvent(event);
}

void TrayDialog::showEvent(QShowEvent *event)
{
    int minutesAgo = dateTimeLastRun.secsTo(QDateTime::currentDateTime()) / 60;
    QString text {""};

    if(minutesAgo == 0 ) {
        QWidget::showEvent(event);
        return;
    }

    if(minutesAgo == 1) {
        text = "1 minute ago";
    } else if(minutesAgo < 60) {
        text = QString::number(minutesAgo) + " minutes ago";
    } else if(minutesAgo < 90) {
        text =  "1 hour ago";
    } else {
        text = QString::number(qRound((float)minutesAgo / 60.0)) + " hours ago";
    }

    ui->labelLastRun->setText(text);
    QWidget::showEvent(event);
}

void TrayDialog::on_bnRefresh_clicked()
{
    emit refresh();
}

void TrayDialog::on_bnLogs_clicked()
{
    emit showLogDialog();
}

void TrayDialog::updateButtonIcons(bool darkMode)
{
    if(darkMode) {
        ui->bnLogs->setIcon(QIcon(":/resources/svg/text-dark.svg"));
        ui->bnRefresh->setIcon(QIcon(":/resources/svg/refresh-dark.svg"));
        ui->bnSettings->setIcon(QIcon(":/resources/svg/settings-dark.svg"));
    } else {
        ui->bnLogs->setIcon(QIcon(":/resources/svg/text-light.svg"));
        ui->bnRefresh->setIcon(QIcon(":/resources/svg/refresh-light.svg"));
        ui->bnSettings->setIcon(QIcon(":/resources/svg/settings-light.svg"));
    }
}

void TrayDialog::on_bnSettings_clicked()
{

}

