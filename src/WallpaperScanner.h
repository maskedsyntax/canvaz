#pragma once

#include <QObject>
#include <QStringList>
#include <QImage>
#include <QSize>
#include <QDirIterator>
#include <QImageReader>

class WallpaperScanner : public QObject {
    Q_OBJECT

public:
    explicit WallpaperScanner(QObject *parent = nullptr);

public slots:
    void scan(const QStringList &paths, const QSize &thumbSize);
    void stop();

signals:
    void imageLoaded(const QString &path, const QImage &image, const QString &filename);
    void finished();

private:
    bool m_stop;
};
