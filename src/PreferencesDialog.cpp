#include "PreferencesDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QStandardPaths>
#include <QLabel>

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Preferences");
    resize(400, 300);

    auto *layout = new QVBoxLayout(this);

    auto *label = new QLabel("Wallpaper Directories:", this);
    layout->addWidget(label);

    dirList = new QListWidget(this);
    layout->addWidget(dirList);

    auto *btnLayout = new QHBoxLayout();
    addBtn = new QPushButton("Add...", this);
    removeBtn = new QPushButton("Remove", this);
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(removeBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    auto *dialogBtnLayout = new QHBoxLayout();
    okBtn = new QPushButton("OK", this);
    cancelBtn = new QPushButton("Cancel", this);
    dialogBtnLayout->addStretch();
    dialogBtnLayout->addWidget(okBtn);
    dialogBtnLayout->addWidget(cancelBtn);
    layout->addLayout(dialogBtnLayout);

    connect(addBtn, &QPushButton::clicked, this, &PreferencesDialog::addDirectory);
    connect(removeBtn, &QPushButton::clicked, this, &PreferencesDialog::removeDirectory);
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

QStringList PreferencesDialog::getDirectories() const {
    QStringList dirs;
    for (int i = 0; i < dirList->count(); ++i) {
        dirs << dirList->item(i)->text();
    }
    return dirs;
}

void PreferencesDialog::setDirectories(const QStringList &dirs) {
    dirList->clear();
    dirList->addItems(dirs);
}

void PreferencesDialog::addDirectory() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Wallpaper Directory",
                                                  QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    if (!dir.isEmpty()) {
        // Check if already exists
        auto items = dirList->findItems(dir, Qt::MatchExactly);
        if (items.isEmpty()) {
            dirList->addItem(dir);
        }
    }
}

void PreferencesDialog::removeDirectory() {
    qDeleteAll(dirList->selectedItems());
}
