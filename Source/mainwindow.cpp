#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QFileDialog>
#include <QProcess>

//------------------------------------------------------------------------------------
// Main Window
//------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Initialize();
    ui->chkExecute->setChecked(true);
    IsOpenSslSupported();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//------------------------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------------------------
void MainWindow::Initialize()
{
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);
}

//------------------------------------------------------------------------------------
// IsOpenSslSupported
//------------------------------------------------------------------------------------
bool MainWindow::IsOpenSslSupported()
{
    // if false then download open SSL from:
    // https://slproweb.com/products/Win32OpenSSL.html
    bool bSslSocketSupport = QSslSocket::supportsSsl();

    //qDebug() << "SSL Socket: " << QSslSocket::sslLibraryBuildVersionString();
    //qDebug() << "Library version: " << QSslSocket::sslLibraryVersionString();
    return bSslSocketSupport;
}

//------------------------------------------------------------------------------------
// StartDownload
//------------------------------------------------------------------------------------
void MainWindow::StartDownload(const QString & url, const QString & downloadFilePath)
{
    const QNetworkRequest& request = QNetworkRequest(m_url);

    QObject::connect(m_manager.get(request), SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(DownloadProgress(qint64,qint64)));
    QObject::connect(&m_manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(DownloadFinished(QNetworkReply*)));
}

//------------------------------------------------------------------------------------
// DownloadProgress
//------------------------------------------------------------------------------------
void MainWindow::DownloadProgress(qint64 recieved, qint64 total)
{
    qDebug() << "Downloaded " << recieved << "/" << total;
    int val = ((static_cast<float>(recieved)) / total) * 100;
    ui->progressBar->setValue(val);
    ui->statusBar->showMessage("Downloading [" + m_url + "] ...");
}

//------------------------------------------------------------------------------------
// DownloadFinished
//------------------------------------------------------------------------------------
void MainWindow::DownloadFinished(QNetworkReply* data)
{    
    QFile localFile(m_downloadPath);
    qDebug() << "Download filename  : " << m_downloadPath;
    if (!localFile.open(QIODevice::WriteOnly))
    {
        ui->statusBar->showMessage("Cannot create " + m_downloadPath);
        return;
    }

    QByteArray fileData = data->readAll();
    if (!fileData.isEmpty())
    {
        localFile.write(fileData);
        localFile.close();
        delete data;
        data = 0;
        ui->statusBar->showMessage("Download complete at " + m_downloadPath);

        if (ui->chkExecute->isChecked())
        {
            // Open downloaded file
            QDesktopServices::openUrl(QUrl(m_downloadPath));
        }
    }
    else
    {
        ui->statusBar->showMessage("Download failed!");
    }

}

bool MainWindow::ValidateUrl(const QString & url)
{
    QUrl qurl = QUrl::fromEncoded(m_url.toLocal8Bit());
    if (1)//qurl.isValid())
    {
        m_url = url;
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------
// ValidateDownloadDir
// Check if the specified directory exists
//------------------------------------------------------------------------------------
bool MainWindow::ValidateDownloadDir(const QString & downloadPath)
{
    if (!downloadPath.isEmpty())
    {
        m_downloadPath = downloadPath;
        return true;
    }

    return false;
}


//------------------------------------------------------------------------------------
// On button clicked: Start
//------------------------------------------------------------------------------------
void MainWindow::on_btnStart_clicked()
{
    Initialize();

    QString url = ui->inpUrl->text();    
    bool bIsUrlValid = ValidateUrl(url);
    if (!bIsUrlValid)
    {
        ui->statusBar->showMessage("Invalid URL specified");
        return;
    }

    QString downloadFilePath = ui->inpDir->text();
    bool bIsDownloadFilePathValid = ValidateDownloadDir(downloadFilePath);
    if (!bIsDownloadFilePathValid)
    {
        ui->statusBar->showMessage("Invalid download directory selected");
        return;
    }

    StartDownload(url, downloadFilePath);
}

//------------------------------------------------------------------------------------
// On button clicked: Browse
//------------------------------------------------------------------------------------
void MainWindow::on_btnStart_3_clicked()
{
    QString filters = "Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml);; All files (*.*)";
    QString selectedLocation = QFileDialog::getSaveFileName(this, "Download as", ".", filters);
    //QString selectedLocation = QFileDialog::getExistingDirectory(this, "Download location", "", QFileDialog::ShowDirsOnly);
    if (!selectedLocation.isEmpty())
    {
        ui->inpDir->setText(selectedLocation);
    }
}
