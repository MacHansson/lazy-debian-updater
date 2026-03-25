#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <framelessdialog.h>
#include <QFileInfoList>
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

private:

    void updateButtonIcons();

private slots:

    void on_listLogs_itemClicked(QListWidgetItem *item);
    void on_bnHide_clicked();
    void on_leSearch_editingFinished();
    void changeEvent(QEvent *event);

private:

    Ui::LogDialog *ui;
    QString m_logDirectory;
    QFileInfoList m_lFiles;
    QList<int> m_lSearchResults;

    static bool compareByDate(const QFileInfo &a, const QFileInfo &b);
};

#endif // LOGDIALOG_H
