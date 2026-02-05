#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QSplitter>
#include <QStandardPaths>
#include <QDirIterator>
#include <QDebug>
#include <QProcess>
#include <QApplication>
#include <QScreen>
#include <QColorDialog>
#include <QMessageBox>
#include <QGuiApplication>
#include <QDateTime>
#include <QImageReader>
#include <QPainter>

// X11 Includes
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    networkManager = new QNetworkAccessManager(this);
    
    // Setup Threading
    scanThread = new QThread(this);
    scanner = new WallpaperScanner();
    scanner->moveToThread(scanThread);
    
    connect(scanThread, &QThread::finished, scanner, &QObject::deleteLater);
    connect(this, &MainWindow::startScan, scanner, &WallpaperScanner::scan);
    connect(scanner, &WallpaperScanner::imageLoaded, this, &MainWindow::onImageLoaded);
    connect(scanner, &WallpaperScanner::finished, this, &MainWindow::onScanFinished);
    
    scanThread->start();
    
    setupUi();
    loadSettings();
    startScanning();
}

MainWindow::~MainWindow() {
    saveSettings();
    scanner->stop();
    scanThread->quit();
    scanThread->wait();
}

void MainWindow::setupUi() {
    setWindowTitle("Canvaz");
    resize(1000, 700);

    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    auto *mainLayout = new QVBoxLayout(centralWidget);
    // Increased margins for cleaner look
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Wallpaper View
    wallpaperView = new QListWidget(this);
    wallpaperView->setViewMode(QListWidget::IconMode);
    wallpaperView->setIconSize(QSize(160, 120));
    wallpaperView->setResizeMode(QListWidget::Adjust);
    wallpaperView->setGridSize(QSize(180, 150));
    wallpaperView->setMovement(QListWidget::Static);
    wallpaperView->setSelectionMode(QListWidget::SingleSelection);
    wallpaperView->setSpacing(10);
    mainLayout->addWidget(wallpaperView, 1);

    // Controls Layout
    auto *controlsLayout = new QHBoxLayout();
    controlsLayout->setSpacing(15);
    
    // Preferences
    prefsBtn = new QPushButton("Preferences", this);
    connect(prefsBtn, &QPushButton::clicked, this, &MainWindow::onPreferences);
    controlsLayout->addWidget(prefsBtn);

    // Download
    downloadBtn = new QPushButton("Download Random", this);
    connect(downloadBtn, &QPushButton::clicked, this, &MainWindow::onDownload);
    controlsLayout->addWidget(downloadBtn);

    controlsLayout->addStretch();

    // Monitor Selection
    monitorCombo = new QComboBox(this);
    monitorCombo->addItems({"Screen 1", "Screen 2", "Both Screens", "Full Screen"});
    controlsLayout->addWidget(monitorCombo);

    // Scaling Selection
    scalingCombo = new QComboBox(this);
    scalingCombo->addItems({"Automatic", "Scaled", "Centered", "Tiled", "Zoomed", "Zoomed Fill"});
    controlsLayout->addWidget(scalingCombo);

    // Color Picker
    colorBtn = new QPushButton("Color", this);
    connect(colorBtn, &QPushButton::clicked, this, &MainWindow::onColorPick);
    controlsLayout->addWidget(colorBtn);

    controlsLayout->addStretch();

    // Apply Button
    applyBtn = new QPushButton("Apply", this);
    connect(applyBtn, &QPushButton::clicked, this, &MainWindow::onApply);
    controlsLayout->addWidget(applyBtn);

    mainLayout->addLayout(controlsLayout);

    connect(wallpaperView, &QListWidget::itemClicked, this, &MainWindow::onWallpaperSelected);
}

void MainWindow::startScanning() {
    wallpaperView->clear();
    
    // Ensure cache dir is in search paths
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (!searchPaths.contains(cacheDir)) {
        searchPaths << cacheDir;
    }
    
    // Default Pictures dir if empty
    if (searchPaths.isEmpty()) {
        searchPaths << QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    }
    
    // Remove duplicates
    searchPaths.removeDuplicates();
    
    emit startScan(searchPaths, QSize(320, 240));
}

void MainWindow::onImageLoaded(const QString &path, const QImage &image, const QString &filename) {
    QIcon icon(QPixmap::fromImage(image));
    auto *item = new QListWidgetItem(icon, filename);
    item->setData(Qt::UserRole, path);
    item->setToolTip(path);
    wallpaperView->addItem(item);
}

void MainWindow::onScanFinished() {
    // Optional: Update status bar or something
}

void MainWindow::onPreferences() {
    PreferencesDialog dlg(this);
    dlg.setDirectories(searchPaths);
    if (dlg.exec() == QDialog::Accepted) {
        searchPaths = dlg.getDirectories();
        startScanning(); // Rescan needed if paths changed
        saveSettings();
    }
}

void MainWindow::onDownload() {
    downloadBtn->setEnabled(false);
    downloadBtn->setText("Downloading...");
    QNetworkRequest request(QUrl("https://picsum.photos/1920/1080"));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onDownloadFinished(reply);
    });
}

void MainWindow::onDownloadFinished(QNetworkReply *reply) {
    downloadBtn->setEnabled(true);
    downloadBtn->setText("Download Random");
    
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        QDir().mkpath(cacheDir);
        
        QString filename = QString("wallpaper_%1.jpg").arg(QDateTime::currentDateTime().toSecsSinceEpoch());
        QString fullPath = cacheDir + "/" + filename;
        
        QFile file(fullPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(data);
            file.close();
            
            // OPTIMIZATION: Just add the new item instead of full rescan
            QImageReader reader(fullPath);
            reader.setAllocationLimit(0);
            QSize thumbSize(320, 240);
            QSize originalSize = reader.size();
            if (originalSize.isValid()) {
                 QSize scaledSize = originalSize.scaled(thumbSize, Qt::KeepAspectRatio);
                 reader.setScaledSize(scaledSize);
            }
            QImage img = reader.read();
            if (!img.isNull()) {
                onImageLoaded(fullPath, img, filename);
                // Select it
                wallpaperView->scrollToBottom();
            }
        }
    } else {
        QMessageBox::warning(this, "Download Error", reply->errorString());
    }
    reply->deleteLater();
}

void MainWindow::onColorPick() {
    QColor color = QColorDialog::getColor(currentColor, this, "Select Background Color");
    if (color.isValid()) {
        currentColor = color;
        colorBtn->setStyleSheet(QString("background-color: %1; color: %2").arg(color.name()).arg(
            color.lightness() > 128 ? "black" : "white"
        ));
        
        // Clear selection to indicate Color Mode
        wallpaperView->clearSelection();
        wallpaperView->setCurrentItem(nullptr); 
    }
}

void MainWindow::onWallpaperSelected(QListWidgetItem *item) {
    (void)item;
}

// Native X11 Helper
void setX11Wallpaper(const QString &path1, const QString &path2, const QColor &bgColor, const QString &mode, bool isDual, bool isFullScreen) {
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        qDebug() << "Failed to open X display";
        return;
    }

    int screen_num = DefaultScreen(display);
    Window root = RootWindow(display, screen_num);
    
    int width = DisplayWidth(display, screen_num);
    int height = DisplayHeight(display, screen_num);
    int depth = DefaultDepth(display, screen_num);

    Pixmap pixmap = XCreatePixmap(display, root, width, height, depth);
    GC gc = XCreateGC(display, pixmap, 0, NULL);

    // Fill with background color first
    XColor xcolor;
    xcolor.red = bgColor.red() * 257;
    xcolor.green = bgColor.green() * 257;
    xcolor.blue = bgColor.blue() * 257;
    xcolor.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(display, DefaultColormap(display, screen_num), &xcolor);
    XSetForeground(display, gc, xcolor.pixel);
    XFillRectangle(display, pixmap, gc, 0, 0, width, height);

    QImage desktopImage(width, height, QImage::Format_ARGB32);
    desktopImage.fill(bgColor);
    
    QPainter painter(&desktopImage);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QList<QScreen*> screens = QGuiApplication::screens();
    
    if (isFullScreen) {
        QRect totalRect(0, 0, width, height);
        QImage img(path1); 
        if (!img.isNull()) {
             if (mode == "Zoomed Fill" || mode == "Zoomed") {
                 QImage s = img.scaled(totalRect.size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                 painter.drawImage(totalRect.x() + (totalRect.width() - s.width())/2, 
                                   totalRect.y() + (totalRect.height() - s.height())/2, s);
             } else if (mode == "Scaled") {
                 painter.drawImage(totalRect, img);
             } else if (mode == "Centered") {
                 painter.drawImage(totalRect.x() + (totalRect.width() - img.width())/2,
                                   totalRect.y() + (totalRect.height() - img.height())/2, img);
             } else {
                 QImage s = img.scaled(totalRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                 painter.drawImage(totalRect.x() + (totalRect.width() - s.width())/2,
                                   totalRect.y() + (totalRect.height() - s.height())/2, s);
             }
        }
    } else {
        for (int i = 0; i < screens.size(); ++i) {
            QRect geo = screens[i]->geometry();
            QString p = (i == 0) ? path1 : path2;
            if (p.isEmpty()) continue;
            
            QImage img(p);
            if (img.isNull()) continue;
            
            if (mode == "Zoomed Fill" || mode == "Zoomed") {
                 QImage s = img.scaled(geo.size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                 painter.drawImage(geo.x() + (geo.width() - s.width())/2, 
                                   geo.y() + (geo.height() - s.height())/2, s);
            } else if (mode == "Scaled") {
                 painter.drawImage(geo, img);
            } else if (mode == "Centered") {
                 painter.drawImage(geo.x() + (geo.width() - img.width())/2,
                                   geo.y() + (geo.height() - img.height())/2, img);
            } else {
                 QImage s = img.scaled(geo.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                 painter.drawImage(geo.x() + (geo.width() - s.width())/2,
                                   geo.y() + (geo.height() - s.height())/2, s);
            }
        }
    }
    painter.end();
    
    QImage x11Img = desktopImage.convertToFormat(QImage::Format_RGB32);
    
    XImage *ximage = XCreateImage(display, DefaultVisual(display, screen_num), depth, ZPixmap, 0,
                                  (char *)x11Img.bits(), width, height, 32, 0);
    
    XPutImage(display, pixmap, gc, ximage, 0, 0, 0, 0, width, height);
    
    XSetWindowBackgroundPixmap(display, root, pixmap);
    XClearWindow(display, root);
    
    Atom atomRootPmapId = XInternAtom(display, "_XROOTPMAP_ID", False);
    Atom atomEsetrootPmapId = XInternAtom(display, "ESETROOT_PMAP_ID", False);
    
    XChangeProperty(display, root, atomRootPmapId, XA_PIXMAP, 32, PropModeReplace, (unsigned char *)&pixmap, 1);
    XChangeProperty(display, root, atomEsetrootPmapId, XA_PIXMAP, 32, PropModeReplace, (unsigned char *)&pixmap, 1);

    ximage->data = NULL;
    XDestroyImage(ximage);
    XFreeGC(display, gc);
    XSetCloseDownMode(display, RetainPermanent);
    XCloseDisplay(display);
}

void MainWindow::onApply() {
    lastScalingMode = scalingCombo->currentText();
    lastMonitorConfig = monitorCombo->currentText();
    
    QString filePath;
    bool useImage = false;
    
    // Check if any item is selected
    if (!wallpaperView->selectedItems().isEmpty()) {
        auto *item = wallpaperView->selectedItems().first();
        if (item) {
            filePath = item->data(Qt::UserRole).toString();
            useImage = true;
        }
    }
    
    // Update State
    if (useImage) {
        if (lastMonitorConfig == "Screen 1") screen1Path = filePath;
        else if (lastMonitorConfig == "Screen 2") screen2Path = filePath;
        else if (lastMonitorConfig == "Both Screens") { screen1Path = filePath; screen2Path = filePath; }
        else if (lastMonitorConfig == "Full Screen") { screen1Path = filePath; screen2Path = filePath; }
    } else {
        if (lastMonitorConfig == "Screen 1") screen1Path = "";
        else if (lastMonitorConfig == "Screen 2") screen2Path = "";
        else if (lastMonitorConfig == "Both Screens") { screen1Path = ""; screen2Path = ""; }
        else if (lastMonitorConfig == "Full Screen") { screen1Path = ""; screen2Path = ""; }
    }

    applyWallpaper();
    saveSettings();
}

void MainWindow::applyWallpaper() {
    qDebug() << "Applying Wallpaper:" << screen1Path << "|" << screen2Path << "Mode:" << lastScalingMode;

    // Backend Execution
    QString currentDesktop = qgetenv("XDG_CURRENT_DESKTOP").toUpper();
    
    if (currentDesktop.contains("GNOME") || currentDesktop.contains("UNITY") || currentDesktop.contains("CINNAMON")) {
         // GNOME Implementation
         QString gsettingsMode = "zoom"; 
         if (lastScalingMode == "Centered") gsettingsMode = "centered";
         else if (lastScalingMode == "Scaled") gsettingsMode = "scaled";
         else if (lastScalingMode == "Tiled") gsettingsMode = "wallpaper";
         else if (lastScalingMode == "Automatic") gsettingsMode = "zoom";

         // Note: GNOME doesn't easily support different wallpapers per screen via gsettings natively 
         // without extra tools or complex script, so we use screen1Path as primary.
         QString filePath = screen1Path.isEmpty() ? screen2Path : screen1Path;

         if (filePath.isEmpty()) {
             QProcess::startDetached("gsettings", {"set", "org.gnome.desktop.background", "picture-uri", ""});
             QProcess::startDetached("gsettings", {"set", "org.gnome.desktop.background", "picture-uri-dark", ""});
             QProcess::startDetached("gsettings", {"set", "org.gnome.desktop.background", "primary-color", currentColor.name()});
         } else {
             QProcess::startDetached("gsettings", {"set", "org.gnome.desktop.background", "picture-uri", "file://" + filePath});
             QProcess::startDetached("gsettings", {"set", "org.gnome.desktop.background", "picture-uri-dark", "file://" + filePath});
             QProcess::startDetached("gsettings", {"set", "org.gnome.desktop.background", "picture-options", gsettingsMode});
         }
    } else {
        // Native X11
        bool fullScreen = (lastMonitorConfig == "Full Screen");
        setX11Wallpaper(screen1Path, screen2Path, currentColor, lastScalingMode, true, fullScreen);
    }
}

void MainWindow::restoreWallpaper() {
    applyWallpaper();
}

void MainWindow::loadSettings() {
    QSettings settings("Canvaz", "CanvazApp");
    searchPaths = settings.value("searchPaths").toStringList();
    
    if (settings.contains("colorR")) {
        int r = settings.value("colorR").toInt();
        int g = settings.value("colorG").toInt();
        int b = settings.value("colorB").toInt();
        currentColor = QColor(r, g, b);
        colorBtn->setStyleSheet(QString("background-color: %1; color: %2").arg(currentColor.name()).arg(
            currentColor.lightness() > 128 ? "black" : "white"
        ));
    } else {
        currentColor = Qt::black; // Default
    }
    
    screen1Path = settings.value("screen1Path").toString();
    screen2Path = settings.value("screen2Path").toString();
    lastScalingMode = settings.value("scalingMode", "Zoomed Fill").toString();
    lastMonitorConfig = settings.value("monitorConfig", "Both Screens").toString();

    // Update UI to match loaded settings
    int scaleIdx = scalingCombo->findText(lastScalingMode);
    if (scaleIdx != -1) scalingCombo->setCurrentIndex(scaleIdx);

    int monitorIdx = monitorCombo->findText(lastMonitorConfig);
    if (monitorIdx != -1) monitorCombo->setCurrentIndex(monitorIdx);
}

void MainWindow::saveSettings() {
    QSettings settings("Canvaz", "CanvazApp");
    settings.setValue("searchPaths", searchPaths);
    settings.setValue("colorR", currentColor.red());
    settings.setValue("colorG", currentColor.green());
    settings.setValue("colorB", currentColor.blue());
    settings.setValue("screen1Path", screen1Path);
    settings.setValue("screen2Path", screen2Path);
    settings.setValue("scalingMode", lastScalingMode);
    settings.setValue("monitorConfig", lastMonitorConfig);
}

void MainWindow::refreshWallpapers() {
    startScanning();
}