#pragma once

#include <QObject>
#include <QPainter>

namespace datavis {

class Selector;

class Plot : public QObject
{
    Q_OBJECT

public:
    struct Range
    {
        Range() {}
        Range(double min, double max): min(min), max(max) {}

        double min = 0;
        double max = 0;
    };

    Plot(QObject * parent = 0): QObject(parent) {}
    virtual void setSelector(Selector *) = 0;
    virtual bool isEmpty() const = 0;
    virtual Range xRange() = 0;
    virtual Range yRange() = 0;
    virtual Range selectorRange() = 0;
    virtual void plot(QPainter *, const QTransform &) = 0;

signals:
    void xRangeChanged();
    void yRangeChanged();
    void selectorRangeChanged();
    void contentChanged();
};

}
