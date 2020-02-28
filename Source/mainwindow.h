#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void StartDownloadWorkerThread(const QString & url, const QString & downloadDir);
    bool ValidateUrl(const QString & url);
    bool ValidateDownloadDir(const QString & downloadDir);
    void StartDownload(const QString & url, const QString & downloadDir);
    bool IsOpenSslSupported();
    void Initialize();

private slots:
    void on_btnStart_clicked();

    void on_btnStart_3_clicked();


public slots:
    void DownloadFinished(QNetworkReply* data);
    void DownloadProgress(qint64 recieved, qint64 total);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager m_manager;
    QFile *m_file;
    QString m_url;
    QString m_downloadPath;
};


#endif // MAINWINDOW_H
