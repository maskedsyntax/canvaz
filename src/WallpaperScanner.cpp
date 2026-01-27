#include "WallpaperScanner.h"
#include <QDebug>
#include <QThread>

WallpaperScanner::WallpaperScanner(QObject *parent) : QObject(parent), m_stop(false) {}

void WallpaperScanner::scan(const QStringList &paths, const QSize &thumbSize) {
    m_stop = false;
    QStringList nameFilters;
    nameFilters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp" << "*.svg" << "*.webp";

    for (const auto &path : paths) {
        if (m_stop) break;
        
        QDirIterator it(path, nameFilters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            if (m_stop) break;
            
            QString filePath = it.next();
            
            QImageReader reader(filePath);
            reader.setAllocationLimit(0);
            
            QSize originalSize = reader.size();
            if (originalSize.isValid()) {
                 QSize scaledSize = originalSize.scaled(thumbSize, Qt::KeepAspectRatio);
                 reader.setScaledSize(scaledSize);
            }

            QImage img = reader.read();
            if (!img.isNull()) {
                emit imageLoaded(filePath, img, it.fileName());
            }
            
            // tiny sleep to allow event loop processing if needed, though we are in a thread
            // QThread::msleep(1); 
        }
    }
    emit finished();
}

void WallpaperScanner::stop() {
    m_stop = true;
}
