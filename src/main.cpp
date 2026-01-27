#include <QApplication>
#include <QStyleFactory>
#include <QFont>
#include <QPalette>
#include <QFontDatabase>
#include <QDebug>
#include "MainWindow.h"

void loadStyle(QApplication& app) {
    app.setStyle(QStyleFactory::create("Fusion"));

    // Modern Minimalist Grayscale Theme
    const QString qss = R"(
        QMainWindow {
            background-color: #0d0d0d;
        }
        QWidget {
            color: #e0e0e0;
            font-family: "JetBrains Mono", "Monospace";
            font-size: 14px;
        }
        
        /* Buttons */
        QPushButton {
            background-color: #1a1a1a;
            border: 1px solid #333333;
            border-radius: 3px;
            padding: 8px 16px;
            color: #ffffff;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #333333;
            border-color: #555555;
            color: #ffffff;
        }
        QPushButton:pressed {
            background-color: #000000;
            border-color: #1a1a1a;
        }
        QPushButton:disabled {
            background-color: #121212;
            color: #444444;
            border-color: #222222;
        }

        /* Combo Box */
        QComboBox {
            background-color: #1a1a1a;
            border: 1px solid #333333;
            border-radius: 3px;
            padding: 6px 12px;
            color: #ffffff;
        }
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 20px;
            border-left: 1px solid #333333;
        }
        QComboBox::down-arrow {
            /* Optional: Custom arrow or just leave default/empty */
            /* image: url(:/icons/down_arrow.png); */
            width: 0; 
            height: 0;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid #ffffff;
            margin: 2px;
        }
        
        /* Combo Box Dropdown List */
        QComboBox QAbstractItemView {
            background-color: #1a1a1a;
            border: 1px solid #333333;
            color: #ffffff;
            selection-background-color: #ffffff;
            selection-color: #000000;
            outline: none;
        }
        
        /* Item Styles - Focus on :selected which captures hover in dropdowns */
        QComboBox QAbstractItemView::item {
            min-height: 24px;
            padding: 4px 8px;
        }
        QComboBox QAbstractItemView::item:selected {
            background-color: #ffffff;
            color: #000000;
        }

        /* List Widget */
        QListWidget {
            background-color: #050505;
            border: 1px solid #222222;
            border-radius: 4px;
            outline: none;
            padding: 5px;
        }
        QListWidget::item {
            border-radius: 2px;
            padding: 5px;
            margin: 2px;
            color: #aaaaaa;
        }
        QListWidget::item:selected {
            background-color: #ffffff;
            color: #000000;
            font-weight: bold;
        }
        QListWidget::item:hover {
            background-color: #222222;
            color: #ffffff;
        }

        /* Scrollbars */
        QScrollBar:vertical {
            border: none;
            background: #050505;
            width: 8px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background: #333333;
            min-height: 20px;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical:hover {
            background: #444444;
        }

        /* Dialogs */
        QDialog {
            background-color: #0d0d0d;
        }
        QLabel {
            color: #ffffff;
        }
        
        /* Tooltips */
        QToolTip {
            color: #000000;
            background-color: #ffffff;
            border: 1px solid #ffffff;
            border-radius: 2px;
        }
    )";

    app.setStyleSheet(qss);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Load Custom Font
    int fontId = QFontDatabase::addApplicationFont(":/fonts/JetBrainsMono-Regular.ttf");
    QString fontFamily = "Monospace"; // Fallback
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            fontFamily = fontFamilies.first();
        }
    } else {
        qWarning() << "Failed to load custom font";
    }

    // Set Application Font
    QFont font(fontFamily);
    font.setStyleHint(QFont::Monospace);
    font.setPointSize(10);
    app.setFont(font);

    loadStyle(app);

    MainWindow window;
    window.setWindowTitle("Canvaz");
    window.resize(1100, 750);
    window.show();

    return app.exec();
}