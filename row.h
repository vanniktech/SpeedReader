#ifndef ROW_H
#define ROW_H

#include <QWidget>
#include <QHBoxLayout>

class Row : public QWidget
{
    Q_OBJECT
public:
    explicit Row(QString title, bool clickable = false, QWidget *parent = 0);
    explicit Row(QString title, int unspeedread, QWidget *parent = 0);
    explicit Row(QString title, int unspeedread, QString subtitle, QWidget *parent = 0);

signals:
    void clicked();

public slots:

protected:
    void mousePressEvent(QMouseEvent* ev);
    void mouseReleaseEvent(QMouseEvent* ev);

private:
    bool mClickable;

    QHBoxLayout* setUpTitleUnSpeedRead(QString title, int unspeedread);
};

#endif // ROW_H
