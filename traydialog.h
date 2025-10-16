#ifndef TRAYDIALOG_H
#define TRAYDIALOG_H

#include <framelessdialog.h>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QDateTime>
#include <QTimer>

namespace Ui {
class TrayDialog;
}

class TrayDialog : public FramelessDialog
{
    Q_OBJECT

    enum Status {
        Finished = 0,
        Active = 1
    };

public:

    explicit TrayDialog(QWidget *parent = nullptr);
    ~TrayDialog();

    void setStatus(int status);
    void setLastUpdateDateTime(QDateTime dateTime);
    void setMessage(QString message);
    bool darkMode();

private slots:

    void on_bnRefresh_clicked();
    void on_bnLogs_clicked();
    void on_bnSettings_clicked();

private:

    Ui::TrayDialog *ui;
    QDateTime dateTimeLastRun;
    QTimer *timerProgressBar;

    void updateButtonIcons(bool darkMode);

    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void changeEvent(QEvent *event);
    void showEvent(QShowEvent *event);

signals:

    void themeChanged();
    void refresh();
    void showLogDialog();

};

#endif // TRAYDIALOG_H
