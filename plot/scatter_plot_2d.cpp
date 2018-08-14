#include "scatter_plot_2d.hpp"

namespace datavis {

ScatterPlot2d::ScatterPlot2d(QObject * parent):
    Plot(parent)
{}

void ScatterPlot2d::setData(DataSetPtr data, int xDim, int yDim)
{
    m_dataset = data;

    m_x_dim = xDim;
    m_y_dim = yDim;

    m_x_range = range(m_x_dim);
    m_y_range = range(m_y_dim);

    //cerr << "X range: " << m_x_range.min << ", " << m_x_range.max << endl;
    //cerr << "Y range: " << m_y_range.min << ", " << m_y_range.max << endl;

    make_points();
}

void ScatterPlot2d::make_points()
{
    auto data_region = get_region(*m_dataset->data(),
                                  vector<int>(m_dataset->dimensionCount(), 0),
                                  m_dataset->data()->size());

    for(auto item : data_region)
    {
        Point2d p;
        p.x = value(m_x_dim, item);
        p.y = value(m_y_dim, item);

        m_points.push_back(p);
    }
}


Plot::Range ScatterPlot2d::xRange()
{
    return m_x_range;
}

Plot::Range ScatterPlot2d::yRange()
{
    return m_y_range;
}

tuple<vector<double>, vector<double>> ScatterPlot2d::dataLocation(const QPointF & point)
{
    if (m_dataset)
    {
        vector<double> loc(m_dataset->dimensionCount(), 0);
        vector<double> att(m_dataset->attributeCount(), 0);

        if (m_x_dim < m_dataset->dimensionCount())
            loc[m_x_dim] = point.x();
        else
            att[m_x_dim - m_dataset->dimensionCount()] = point.x();

        if (m_y_dim < m_dataset->dimensionCount())
            loc[m_y_dim] = point.y();
        else
            att[m_y_dim - m_dataset->dimensionCount()] = point.y();

        return { loc, att };
    }
    else
    {
        return {};
    }
}

Plot::Range ScatterPlot2d::range(int dim_index)
{
    if (dim_index < m_dataset->dimensionCount())
    {
        const auto & dim = m_dataset->dimension(dim_index);
        return Range(dim.minimum(), dim.maximum());
    }
    else
    {
        int att_idx = dim_index - m_dataset->dimensionCount();
        int ndim = m_dataset->dimensionCount();
        auto data_region = get_region(m_dataset->data(att_idx),
                                      vector<int>(ndim, 0),
                                      m_dataset->data()->size());
        auto min = std::min_element(data_region.begin(), data_region.end());
        auto max = std::max_element(data_region.begin(), data_region.end());
        return Range(min.value(), max.value());
    }
}

inline double ScatterPlot2d::value(int dim_index,  const array_region<double>::iterator & iter)
{
    if (dim_index < m_dataset->dimensionCount())
    {
        const auto & dim = m_dataset->dimension(dim_index);
        return dim.map * iter.location()[dim_index];
    }
    else
    {
        int att_idx = dim_index - m_dataset->dimensionCount();
        return m_dataset->data(att_idx).data()[iter.index()];
    }
}

void ScatterPlot2d::plot(QPainter * painter,  const Mapping2d & view_map, const QRectF & region)
{
#if 0
    int ndim = m_dataset->dimensionCount();

    auto data_region = get_region(*m_dataset->data(), vector<int>(ndim, 0),  m_dataset->data()->size());

    QColor c;

    int count = 0;

    for(auto item : data_region)
    {
        item.index();
        Point2d p;
        p.x = value(m_x_dim, item);
        p.y = value(m_y_dim, item);

        p = view_map * p;

        //if (++count % 1000 == 0)
        {
            QRectF r(0, 0, 1, 1);
            r.moveCenter(QPointF(p.x, p.y));
            painter->fillRect(r, c);
        }
    }
#endif

#if 1
    QColor c;

    for (auto & point : m_points)
    {
        auto p = view_map * point;

        QRectF r(p.x - 2, p.y - 2, 4, 4);
        painter->fillRect(r, c);
    }
#endif
}

}