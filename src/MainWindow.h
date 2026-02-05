#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QSettings>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QThread>
#include "PreferencesDialog.h"
#include "WallpaperScanner.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void restoreWallpaper();

private slots:
    void onPreferences();
    void onApply();
    void onColorPick();
    void onDownload();
    void onDownloadFinished(QNetworkReply *reply);
    void refreshWallpapers();
    void onWallpaperSelected(QListWidgetItem *item);
    
    // Async Scanner Slots
    void onImageLoaded(const QString &path, const QImage &image, const QString &filename);
    void onScanFinished();

signals:
    void startScan(const QStringList &paths, const QSize &thumbSize);

private:
    void setupUi();
    void loadSettings();
    void saveSettings();
    void startScanning();
    void applyWallpaper();

    QListWidget *wallpaperView;
    QComboBox *monitorCombo;
    QComboBox *scalingCombo;
    QPushButton *colorBtn;
    QPushButton *applyBtn;
    QPushButton *prefsBtn;
    QPushButton *downloadBtn;
    
    QNetworkAccessManager *networkManager;
    
    // Threading
    QThread *scanThread;
    WallpaperScanner *scanner;
    
    QStringList searchPaths;
    QColor currentColor;
    QString screen1Path;
    QString screen2Path;
    QString lastScalingMode;
    QString lastMonitorConfig;
};
