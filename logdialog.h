#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <framelessdialog.h>
#include <QFileInfo>
#include <QListWidgetItem>

namespace Ui {
class LogDialog;
}

class LogDialog : public FramelessDialog
{
    Q_OBJECT

public:

    explicit LogDialog(QString logDirectory, QWidget *parent = nullptr);
    ~LogDialog();

    void updateLogs();
    QDateTime getLastLogDateTime();

private slots:

    void on_listLogs_itemClicked(QListWidgetItem *item);
    void on_bnHide_clicked();

private:

    Ui::LogDialog *ui;
    QString m_logDirectory;

    static bool compareByDate(const QFileInfo &a, const QFileInfo &b);
};

#endif // LOGDIALOG_H
