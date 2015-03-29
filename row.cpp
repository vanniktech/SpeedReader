#include "row.h"

#include <QVBoxLayout>
#include <QLabel>

Row::Row(QString title, bool clickable, QWidget *parent) : QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout();
    QLabel* titleLabel = new QLabel(title);
    layout->addWidget(titleLabel);
    this->setLayout(layout);

    mClickable = clickable;

    if (!mClickable) {
        this->setStyleSheet("QWidget {"
            "border: 2px solid #000;"
            "border-top: none;"
            "border-right: none;"
            "border-left: none;"
        "}");
    }
}

Row::Row(QString title, int unspeedread, QWidget *parent) : QWidget(parent) {
    this->setLayout(this->setUpTitleUnSpeedRead(title, unspeedread));
}

Row::Row(QString title, int unspeedread, QString subtitle, QWidget *parent) : QWidget(parent) {
    QVBoxLayout* parentLayout = new QVBoxLayout();
    parentLayout->addLayout(this->setUpTitleUnSpeedRead(title, unspeedread));

    QLabel* subtitleLabel = new QLabel(subtitle);
    parentLayout->addWidget(subtitleLabel);

    this->setLayout(parentLayout);
}

QHBoxLayout* Row::setUpTitleUnSpeedRead(QString title, int unspeedread) {
    this->setContentsMargins(10, 0, 10, 0);
    mClickable = true;

    QHBoxLayout* layout = new QHBoxLayout();
    QLabel* titleLabel = new QLabel(title);
    QLabel* unSpeedReadLabel = new QLabel(QString::number(unspeedread));
    layout->addWidget(titleLabel);
    layout->addWidget(unSpeedReadLabel, 1, Qt::AlignRight);

    return layout;
}

void Row::mousePressEvent(QMouseEvent* mouseEvent) {
    Q_UNUSED(mouseEvent);

    if (mClickable) {
        this->setAutoFillBackground(true);
        QPalette p = this->palette();
        p.setColor(this->backgroundRole(), QColor(222, 222, 222));
        this->setPalette(p);
    }
}

void Row::mouseReleaseEvent(QMouseEvent* mouseEvent) {
    Q_UNUSED(mouseEvent);

    if (mClickable) {
        this->setStyleSheet("QWidget { background-color: transparent; }");
        emit clicked();
    }
}
