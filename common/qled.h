#ifndef QLED_H
#define QLED_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>

class QLed : public QWidget
{
    Q_OBJECT
public:
    explicit QLed(QWidget *parent = NULL);
    virtual ~QLed();
    bool value() const { return m_value; }
    enum ledColor {Black = 0,Blue, Green , Grey, Orange, Purple, Yellow, Red};

    ledColor onColor() const { return m_onColor; }
    ledColor offColor() const { return m_offColor; }

public slots:
    void setValue(bool);
    void setOnColor(ledColor newColor);
    void setOffColor(ledColor newColor);
    void toggleValue();

protected:
    bool m_value;
    ledColor m_onColor, m_offColor;
    QStringList colors;
    QString rcpath;
    QString suffix;
    void paintEvent(QPaintEvent *event);
};

#endif // QLED_H
