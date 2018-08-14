#pragma once

#include <QWidget>
#include <QListWidget>
#include <QMainWindow>
#include <QMenu>

#include "../data/array.hpp"
#include "../data/data_source.hpp"
#include "../json/json.hpp"

#include <list>

namespace datavis {

using std::list;
using nlohmann::json;

class DataSet;
class PlotView;
class Plot;
class LinePlot;
class DataLibrary;
class DataLibraryView;
class PlotDataSettingsView;

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget * parent = 0);

    void openData();
    void openDataFile(const QString & file_path);

    void saveProject();
    void openProject();
    void openProjectFile(const QString & file_path);

private:
    void makeMenu();
    void onOpenFailed(const QString & path);
    void onSelectedDataChanged();
    bool hasSelectedObject();
    void plotSelectedObject();
    PlotView * addPlotView();
    void removePlotView(PlotView*);
    void plot(DataSource *, int index);
    void restorePlot(PlotView *, const json & state);
    void removeSelectedPlot();
    bool eventFilter(QObject*, QEvent*) override;
    void showPlotContextMenu(Plot*, const QPoint & pos);


    DataLibrary * m_lib = nullptr;
    DataLibraryView * m_lib_view = nullptr;

    list<PlotView*> m_plot_views;

    PlotView * m_selected_plot_view = nullptr;
    Plot * m_selected_plot = nullptr;

    QMenu * m_plot_context_menu = nullptr;
};

}
