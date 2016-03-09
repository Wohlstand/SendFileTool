#ifndef WOHLNET_SENDFILE_WINDOW_H
#define WOHLNET_SENDFILE_WINDOW_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QQueue>
#include <QString>

class QNetworkReply;

namespace Ui {
class Wohlnet_Sendfile_Window;
}

class Wohlnet_Sendfile_Window : public QDialog
{
    Q_OBJECT

public:
    explicit Wohlnet_Sendfile_Window(QWidget *parent = 0);
    ~Wohlnet_Sendfile_Window();
    void uploadFileS(QStringList files);

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
    QByteArray buildUploadString(QString sourceFile, bool &valid);

private slots:
    void printScriptReply(QNetworkReply* nr);
    void progressChanged(qint64 value, qint64 max);
    void disableLabel();
    void refreshLabel();

private:

    Ui::Wohlnet_Sendfile_Window *ui;
    void sendFile();
    int                     m_total;
    bool                    m_isBusy;
    QNetworkReply*          m_reply;
    QNetworkAccessManager   mNetworkManager;
    QQueue<QString>         filesToUpload;
    QString                 uploadedLinks;
};

#endif // WOHLNET_SENDFILE_WINDOW_H
