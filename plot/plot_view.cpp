#include "plot_view.hpp"
#include "plot.hpp"
#include "selector.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QDebug>
#include <algorithm>
#include <iostream>

using namespace std;

namespace datavis {

PlotView::PlotView(QWidget * parent):
    QWidget(parent)
{
    m_canvas = new PlotCanvas;
    m_selector = new Selector;
    m_selector_slider = new QSlider;
    m_selector_slider->setOrientation(Qt::Horizontal);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(m_canvas);
    layout->addWidget(m_selector_slider);

    connect(m_selector, &Selector::valueChanged,
            m_selector_slider, &QSlider::setValue);
    connect(m_selector_slider, &QSlider::valueChanged,
            m_selector, &Selector::setValue);
}

void PlotView::addPlot(Plot * plot)
{
    if (!plot)
        return;

    plot->setSelector(m_selector);

    m_canvas->m_plots.push_back(plot);

    connect(plot, &Plot::xRangeChanged,
            this, &PlotView::onPlotRangeChanged);
    connect(plot, &Plot::yRangeChanged,
            this, &PlotView::onPlotRangeChanged);
    connect(plot, &Plot::selectorRangeChanged,
            this, &PlotView::onPlotSelectorRangeChanged);
    connect(plot, &Plot::contentChanged,
            this, &PlotView::onPlotContentChanged);

    updateSelectorRange();

    m_canvas->updatePlotMap();
    m_canvas->updateViewMap();

    m_canvas->update();
}

void PlotView::removePlot(Plot * plot)
{
    if (!plot)
        return;

    auto & plots = m_canvas->m_plots;

    auto handle = std::find(plots.begin(), plots.end(), plot);
    if (handle != plots.end())
    {
        plot->setSelector(nullptr);
        disconnect(plot, 0, this, 0);
        m_canvas->m_plots.erase(handle);
    }

    m_canvas->updatePlotMap();
    m_canvas->updateViewMap();

    m_canvas->update();
}

void PlotView::onPlotRangeChanged()
{
    m_canvas->updatePlotMap();
    m_canvas->updateViewMap();
    m_canvas->update();
}

void PlotView::onPlotSelectorRangeChanged()
{
    updateSelectorRange();
}

void PlotView::onPlotContentChanged()
{
    m_canvas->update();
}

void PlotView::updateSelectorRange()
{
    double min;
    double max;
    bool first = true;
    for (auto plot : m_canvas->m_plots)
    {
        auto range = plot->selectorRange();
        if (first)
        {
            min = range.min;
            max = range.max;
        }
        else
        {
            min = std::min(min, range.min);
            max = std::max(max, range.max);
        }
        first = false;
    }

    qDebug() << "Plot selector range:" << min << "," << max;

    m_selector_slider->setRange(min, max);
}

void PlotCanvas::updateViewMap()
{
    auto size = this->size();

    int margin = 10;

    QTransform map;
    map.translate(margin, size.height() - margin);
    map.scale(size.width() - 2 * margin, - (size.height() - 2 * margin));

    m_view_map = m_plot_map * map;
}

void PlotCanvas::updatePlotMap()
{
    Plot::Range total_x_range;
    Plot::Range total_y_range;

    {
        bool first = true;
        for (auto plot : m_plots)
        {
            if (plot->isEmpty())
                continue;

            auto x_range = plot->xRange();
            auto y_range = plot->yRange();

            if (first)
            {
                total_x_range = x_range;
                total_y_range = y_range;
            }
            else
            {
                total_x_range.min = min(total_x_range.min, x_range.min);
                total_x_range.max = max(total_x_range.max, x_range.max);

                total_y_range.min = min(total_y_range.min, y_range.min);
                total_y_range.max = max(total_y_range.max, y_range.max);
            }
            first = false;
        }
    }
#if 0
    cout << "View range: "
         << "(" << total_range.min.x() << "," << total_range.max.x() << ")"
         << " -> "
         << "(" << total_range.min.y() << "," << total_range.max.y() << ")"
         << endl;
#endif
    QPointF max(total_x_range.max, total_y_range.max);
    QPointF min(total_x_range.min, total_y_range.min);

    QPointF offset = min;
    QPointF extent = max - min;

#if 0
    qDebug() << "offset:" << offset;
    qDebug() << "extent:" << extent;
#endif
    double x_scale = extent.x() == 0 ? 1 : 1.0 / extent.x();
    double y_scale = extent.y() == 0 ? 1 : 1.0 / extent.y();
#if 0
    qDebug() << "x scale:" << x_scale;
    qDebug() << "y scale:" << y_scale;
#endif
    QTransform transform;
    transform.scale(x_scale, y_scale);
    transform.translate(-offset.x(), -offset.y());

    m_plot_map = transform;
}

void PlotCanvas::resizeEvent(QResizeEvent*)
{
    updateViewMap();
}

void PlotCanvas::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    painter.fillRect(rect(), Qt::white);

    for (auto plot : m_plots)
    {
        if (plot->isEmpty())
            continue;

        plot->plot(&painter, m_view_map);
    }
}

}
