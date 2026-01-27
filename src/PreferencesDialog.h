#pragma once

#include <QDialog>
#include <QListWidget>
#include <QPushButton>

class PreferencesDialog : public QDialog {
    Q_OBJECT
public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    QStringList getDirectories() const;
    void setDirectories(const QStringList &dirs);

private slots:
    void addDirectory();
    void removeDirectory();

private:
    QListWidget *dirList;
    QPushButton *addBtn;
    QPushButton *removeBtn;
    QPushButton *okBtn;
    QPushButton *cancelBtn;
};
