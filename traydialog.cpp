#include "traydialog.h"
#include "ui_traydialog.h"

#include <QDateTime>
#include <QStyleFactory>
#include <QMenu>
#include <QActionGroup>
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
    updateButtonIcons();

    timerProgressBar = new QTimer();
    timerProgressBar->setInterval(33);
    timerProgressBar->setSingleShot(false);
    QObject::connect(timerProgressBar, &QTimer::timeout, [this]() {
        int value = ui->progressBar->value() + 1;
        if(value > ui->progressBar->maximum())
            value = 0;
        ui->progressBar->setValue(value);
    });

    menuSettings = new QMenu("Settings");
    menuTheme = new QMenu("Theme");

    QObject::connect(menuTheme, &QMenu::aboutToShow, [this]() {
        for(QAction *a : menuTheme->actions()) {
            a->setText(a->text().remove("> "));
        }
        QAction *active = menuTheme->actions().at((int)m_activeTheme);
        active->setText("> " + active->text());
    });
    menuSettings->addMenu(menuTheme);
    menuTheme->addAction(new QAction("Automatic"));
    QObject::connect(menuTheme->actions().last(), &QAction::triggered, [this]() { setTheme(Theme::Automatic); });
    menuTheme->addAction(new QAction("Light"));
    QObject::connect(menuTheme->actions().last(), &QAction::triggered, [this]() { setTheme(Theme::Light); });
    menuTheme->addAction(new QAction("Dark"));
    QObject::connect(menuTheme->actions().last(), &QAction::triggered, [this]() { setTheme(Theme::Dark); });

    QActionGroup groupTheme(this);
    groupTheme.setExclusive(true);
    for(QAction *a : menuTheme->actions()) {
        a->setActionGroup(&groupTheme);
    }

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
        updateButtonIcons();
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
    ui->labelResult->setText("Running ...");
    emit refresh();
}

void TrayDialog::on_bnLogs_clicked()
{
    emit showLogDialog();
}

void TrayDialog::updateButtonIcons()
{
    bool isDarkMode{true};
    if(m_activeTheme != Automatic) {
        isDarkMode = (m_activeTheme == Light) ? false : true;
    }
    if(isDarkMode) {
        ui->bnLogs->setIcon(QIcon(":/resources/svg/text-dark.svg"));
        ui->bnRefresh->setIcon(QIcon(":/resources/svg/refresh-dark.svg"));
        ui->bnSettings->setIcon(QIcon(":/resources/svg/settings-dark.svg"));
    } else {
        ui->bnLogs->setIcon(QIcon(":/resources/svg/text-light.svg"));
        ui->bnRefresh->setIcon(QIcon(":/resources/svg/refresh-light.svg"));
        ui->bnSettings->setIcon(QIcon(":/resources/svg/settings-light.svg"));
    }
}

void TrayDialog::setTheme(Theme theme)
{
    m_activeTheme = theme;
    switch(theme)
    {
        case Automatic: {
            qApp->setPalette(QPalette());
            break;
        }
        case Light: {
            QPalette lightPalette;
            lightPalette.setColor(QPalette::Window, QColor(240, 240, 240));
            lightPalette.setColor(QPalette::WindowText, Qt::black);
            lightPalette.setColor(QPalette::Base, Qt::white);
            lightPalette.setColor(QPalette::AlternateBase, QColor(225, 225, 225));
            lightPalette.setColor(QPalette::ToolTipBase, Qt::white);
            lightPalette.setColor(QPalette::ToolTipText, Qt::black);
            lightPalette.setColor(QPalette::Text, Qt::black);
            lightPalette.setColor(QPalette::Button, QColor(240, 240, 240));
            lightPalette.setColor(QPalette::ButtonText, Qt::black);
            lightPalette.setColor(QPalette::BrightText, Qt::red);
            lightPalette.setColor(QPalette::Highlight, QColor(76, 163, 224)); // light blue
            lightPalette.setColor(QPalette::HighlightedText, Qt::white);
            qApp->setPalette(lightPalette);
            break;
        }
        case Dark: {
            QPalette darkPalette;
            darkPalette.setColor(QPalette::Window, QColor(17, 17, 17));
            darkPalette.setColor(QPalette::WindowText, Qt::white);
            darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
            darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
            darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
            darkPalette.setColor(QPalette::ToolTipText, Qt::white);
            darkPalette.setColor(QPalette::Text, Qt::white);
            darkPalette.setColor(QPalette::Button, QColor(17, 17, 17));
            darkPalette.setColor(QPalette::ButtonText, Qt::white);
            darkPalette.setColor(QPalette::BrightText, Qt::red);
            darkPalette.setColor(QPalette::Highlight, QColor(142, 45, 197).lighter()); // purple-ish
            darkPalette.setColor(QPalette::HighlightedText, Qt::black);
            qApp->setPalette(darkPalette);
            break;
        }
    }
}

void TrayDialog::on_bnSettings_clicked()
{
    menuSettings->show();
    menuSettings->move(pos() + QPoint(width() - menuSettings->width(), (-1)*menuSettings->height()));
}

