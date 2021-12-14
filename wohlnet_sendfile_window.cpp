#include "wohlnet_sendfile_window.h"
#include "ui_wohlnet_sendfile_window.h"
#include <QDir>
#include <QHttpMultiPart>
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QNetworkReply>
#include <QMessageBox>
#include <QClipboard>

#include <QtDebug>

#include "defines.h"

Wohlnet_Sendfile_Window::Wohlnet_Sendfile_Window(QWidget *parent) :
    QDialog(parent), m_closeOnFinish(false),
    ui(new Ui::Wohlnet_Sendfile_Window), m_total(0), m_isBusy(false),
    m_reply(NULL),
    mNetworkManager(this),
    m_curPostData(nullptr)
{
    ui->setupUi(this);
    QObject::connect(&mNetworkManager, &QNetworkAccessManager::finished,
                     this, &Wohlnet_Sendfile_Window::printScriptReply);

    //This slot is used to debug the output of the server script
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->progressBar->hide();
    ui->files_left->hide();
}

Wohlnet_Sendfile_Window::~Wohlnet_Sendfile_Window()
{
    delete ui;
}

void Wohlnet_Sendfile_Window::uploadFileS(QStringList files)
{
    if(files.isEmpty()) return;

    foreach(QString file, files)
        filesToUpload.enqueue(file);

    qApp->setActiveWindow(this);
    if(!m_isBusy && !filesToUpload.isEmpty())
    {
        ui->progressBar->setEnabled(true);
        uploadedLinks.clear();
        sendFile();
    }

    m_total += files.size();
    refreshLabel();
}

void Wohlnet_Sendfile_Window::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void Wohlnet_Sendfile_Window::dropEvent(QDropEvent *e)
{
    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);

    QStringList files;
    foreach(const QUrl &url, e->mimeData()->urls())
    {
        const QString fileName = url.toLocalFile();
        files.push_back(fileName);
    }
    uploadFileS(files);
}


void Wohlnet_Sendfile_Window::sendFile()
{
    QString sourceFile;

retryAgain:
    if(filesToUpload.isEmpty())
    {
        ui->progressBar->setEnabled(false);
        ui->progressBar->setValue(0);

        if(m_reply)
            delete m_reply;
        m_reply = nullptr;

        disableLabel();

        QApplication::clipboard()->setText(uploadedLinks);
        if(!m_closeOnFinish)
            QMessageBox::information(this, tr("All files are sent!"), tr("All files successfully sent and URLs are been copied into clipboard!"));

        m_isBusy = false;

        if(m_closeOnFinish)
            this->close();

        m_total = 0;
        return;
    }

    sourceFile = filesToUpload.dequeue();

    bool valid = false;
    {
        m_postFile.close();

        QString path = QFileInfo(sourceFile).absoluteDir().absolutePath();
        path.append("/");
        path.append(QFileInfo(sourceFile).fileName());
        m_postFile.setFileName(path);

        if(!m_postFile.open(QIODevice::ReadOnly))
        {
            qDebug() << "QFile Error: File not found!";
            valid = false;
        }
        else
        {
            qDebug() << "File found, proceed as planned";
            valid = true;
        }
    }

    if(!valid)
        goto retryAgain;

    QHttpMultiPart *data = new QHttpMultiPart();
    data->setContentType(QHttpMultiPart::FormDataType);

    //    request.setRawHeader(QString("Content-Type").toUtf8(), QString("multipart/form-data; boundary=" + bound).toUtf8());
    //    request.setRawHeader(QString("Content-Length").toUtf8(), QString::number(postFile.size()).toUtf8());

    QHttpPart action;
    action.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    action.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"action\""));
    action.setBody("oneday");
    data->append(action);

    QHttpPart fileDesc;
    fileDesc.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/bin"));
    fileDesc.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant(QString("multipart/form-data; name=\"File1\"; filename=\"%1\"").arg(sourceFile)));
    fileDesc.setRawHeader("Content-Transfer-Encoding", "binary");
    fileDesc.setBodyDevice(&m_postFile);
    data->append(fileDesc);

    m_curPostData = data;

    QUrl    mResultsURL = QUrl(UPLOAD_URL);
    QNetworkRequest request(mResultsURL);

    m_reply = mNetworkManager.post(request, data);
    QObject::connect(m_reply, &QNetworkReply::uploadProgress,
                     this, &Wohlnet_Sendfile_Window::progressChanged);
    m_isBusy = true;
    refreshLabel();
}

void Wohlnet_Sendfile_Window::printScriptReply(QNetworkReply *nr)
{
    if(m_curPostData)
        delete m_curPostData;

    m_curPostData = nullptr;

    m_postFile.close();

    QString reply = QString::fromLocal8Bit(nr->readAll());

    QUrl url = QUrl(reply, QUrl::StrictMode);
    if(url.isValid())
    {
        uploadedLinks += reply;
        if(!filesToUpload.isEmpty())
            uploadedLinks += "\n";
    }
    else
        QMessageBox::warning(this, tr("Error of file transfering"), reply);

    sendFile();//continue next item
}

void Wohlnet_Sendfile_Window::progressChanged(qint64 value, qint64 max)
{
    ui->progressBar->setMaximum(max);
    ui->progressBar->setValue(value);
}

void Wohlnet_Sendfile_Window::disableLabel()
{
    ui->progressBar->hide();
    ui->files_left->hide();
    ui->files_left->setText("...");
}

void Wohlnet_Sendfile_Window::refreshLabel()
{
    ui->progressBar->show();
    ui->files_left->show();
    ui->files_left->setText(tr("Sending files %1/%2...").arg(m_total - filesToUpload.size()).arg(m_total));
}

void Wohlnet_Sendfile_Window::closeOnFinish()
{
    m_closeOnFinish = true;
}
