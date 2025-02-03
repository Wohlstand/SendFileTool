#ifndef WOHLNET_SENDFILE_WINDOW_H
#define WOHLNET_SENDFILE_WINDOW_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QQueue>
#include <QFile>
#include <QString>
#include <QAtomicInteger>

class QNetworkReply;

namespace Ui {
class Wohlnet_Sendfile_Window;
}

class Wohlnet_Sendfile_Window : public QDialog
{
    Q_OBJECT

public:
    explicit Wohlnet_Sendfile_Window(QWidget *parent = 0);
    virtual ~Wohlnet_Sendfile_Window();
    void uploadFile(const QString &file);
    void uploadFileS(const QStringList &files);
    void closeOnFinish();

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private slots:
    void printScriptReply(QNetworkReply* nr);
    void progressChanged(qint64 value, qint64 max);
    void disableLabel();
    void refreshLabel();

private:
    bool m_closeOnFinish;

    Ui::Wohlnet_Sendfile_Window *ui = nullptr;
    void sendFile();

    QAtomicInteger<int>     m_total = QAtomicInteger<int>(0);
    QAtomicInteger<bool>    m_isBusy = QAtomicInteger<bool>(false);
    QNetworkReply*          m_reply;
    QNetworkAccessManager   mNetworkManager;
    QFile                   m_postFile;
    QHttpMultiPart          *m_curPostData = nullptr;
    QQueue<QString>         filesToUpload;
    QString                 uploadedLinks;
};

#endif // WOHLNET_SENDFILE_WINDOW_H
