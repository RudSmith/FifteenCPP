#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "tile.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // Получен сигнал о перемещении плитки, обрабатываем его
    void moveTile(QPoint tile_pos);

private:
    Ui::MainWindow *ui;
    // Массив плиток
    QVector<Tile*> m_tiles;

    // Количество плиток
    const int m_tiles_count = 15;
    // Кол-во строк поля
    const int m_row_count = 4;
    // Кол-во столбцов поля
    const int m_column_count = 4;
};
#endif // MAINWINDOW_H
