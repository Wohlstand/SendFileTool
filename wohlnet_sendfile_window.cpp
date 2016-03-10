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

Wohlnet_Sendfile_Window::Wohlnet_Sendfile_Window(QWidget *parent) :
    QDialog(parent), m_closeOnFinish(false),
    ui(new Ui::Wohlnet_Sendfile_Window), m_total(0), m_isBusy(false),
    m_reply(NULL), mNetworkManager(this)
{
    ui->setupUi(this);
    connect(&mNetworkManager,
            SIGNAL(finished(QNetworkReply*)),
            this,
            SLOT(printScriptReply(QNetworkReply*)));

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

    m_total+=files.size();
    refreshLabel();
}

void Wohlnet_Sendfile_Window::dragEnterEvent(QDragEnterEvent *e)
{
    //if(isConverting) return;
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void Wohlnet_Sendfile_Window::dropEvent(QDropEvent *e)
{
    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);

    QStringList files;
    foreach (const QUrl &url, e->mimeData()->urls())
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
        m_reply=NULL;
        QApplication::clipboard()->setText(uploadedLinks);
        if(!m_closeOnFinish)
        {
            QMessageBox::information(this, "All files are sent!", "All files successfully sent and URLs are been copied into clipboard!");
        }
        disableLabel();
        m_isBusy=false;
        if(m_closeOnFinish) this->close();
        return;
    }

    sourceFile=filesToUpload.dequeue();
    QByteArray postData;

    //Look below for buildUploadString() function
    bool valid=false;
    postData = buildUploadString(sourceFile, valid);
    if(!valid)
        goto retryAgain;

    QUrl mResultsURL = QUrl("http://wohlnet.ru/sentfile/cmd_send_nolog.php");
    QString bound="margin"; //name of the boundary

    QNetworkRequest request(mResultsURL); //our server with php-script
    request.setRawHeader(QString("Content-Type").toLocal8Bit(),QString("multipart/form-data; boundary=" + bound).toLocal8Bit());
    request.setRawHeader(QString("Content-Length").toLocal8Bit(), QString::number(postData.length()).toLocal8Bit());
    m_reply = mNetworkManager.post(request, postData);
    connect(m_reply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(progressChanged(qint64,qint64)));
    m_isBusy=true;
    refreshLabel();
}

QByteArray Wohlnet_Sendfile_Window::buildUploadString(QString sourceFile, bool &valid)
{
    QString path = QFileInfo(sourceFile).absoluteDir().absolutePath();
    path.append("/");
    path.append(QFileInfo(sourceFile).fileName());

    qDebug()<< "Uploading file:"<<sourceFile;
    //action=oneday
    QString bound="margin";
    QByteArray data(QString("--" + bound + "\r\n").toLocal8Bit());
    data.append("Content-Disposition: form-data; name=\"action\"\r\n\r\n");
    data.append("oneday\r\n");
    data.append(QString("--" + bound + "\r\n").toLocal8Bit());
    data.append("Content-Disposition: form-data; name=\"File1\"; filename=\"");
    data.append(sourceFile);
    data.append("\"\r\n");
    data.append("Content-Type: application/bin\r\n\r\n"); //data type

    QFile file(path);
        if (!file.open(QIODevice::ReadOnly)){
            qDebug() << "QFile Error: File not found!";
            valid=false;
            return data;
        } else { qDebug() << "File found, proceed as planned"; }

    data.append(file.readAll());
    data.append("\r\n");
    data.append("--" + bound + "--\r\n");  //closing boundary according to rfc 1867

    file.close();
    valid=true;
    return data;
}

void Wohlnet_Sendfile_Window::printScriptReply(QNetworkReply *nr)
{
    QString reply=QString::fromLocal8Bit(nr->readAll());
    QUrl url=QUrl(reply, QUrl::StrictMode);
    if(url.isValid())
    {
        uploadedLinks+=reply;
        if(!filesToUpload.isEmpty()) uploadedLinks+="\n";
//        QApplication::clipboard()->setText(reply);
//        QMessageBox::information(this, "sent", reply);
    } else {
        QMessageBox::warning(this, "sent", reply);
    }
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
    ui->files_left->setText(tr("Files left %1/%2").arg(m_total-filesToUpload.size()).arg(m_total));
}

void Wohlnet_Sendfile_Window::closeOnFinish()
{
    m_closeOnFinish=true;
}
