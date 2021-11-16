#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Резервируем место для плиток в векторе
    m_tiles.resize(m_tiles_count);

    // Создаём плитки, пихаем их в gridLayout, соединяем сигнал needToMove и слот moveTile
    for(int i = 0; i < m_row_count; ++i){
        for (int j = 0; j < m_column_count; ++j) {
            if(i * m_row_count + j == m_row_count * m_column_count - 1)
                break;
            m_tiles[i * m_row_count + j] = new Tile(QPoint(i, j));
            ui->gridLayout->addWidget(m_tiles[i * m_row_count + j], i, j);
            connect(m_tiles[i * m_row_count + j], &Tile::needToMove, this, &MainWindow::moveTile);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::moveTile(QPoint tile_pos)
{
    // Сохраняем нажатую плитку в виде layoutItem* и Tile*
    auto pressed_tile_layout_item = ui->gridLayout->itemAtPosition(tile_pos.x(), tile_pos.y());
    Tile *pressed_tile = nullptr;

    for (auto & tile : m_tiles) {
        if(tile->get_current_pos() == tile_pos){
            pressed_tile = tile;
            break;
        }
    }

    // Убираем с лэйаута нажатую плитку
    ui->gridLayout->removeItem(pressed_tile_layout_item);

    int new_x{};
    int new_y{};

    // Определяем, куда её нужно поместить
    if(tile_pos.y() > 0 && ui->gridLayout->itemAtPosition(tile_pos.x(), tile_pos.y() - 1) == nullptr){
        new_x = tile_pos.x();
        new_y = tile_pos.y() - 1;
    }
    else if (tile_pos.y() < m_column_count - 1 && ui->gridLayout->itemAtPosition(tile_pos.x(), tile_pos.y() + 1) == nullptr){
        new_x = tile_pos.x();
        new_y = tile_pos.y() + 1;
    }
    else if (tile_pos.x() > 0 && ui->gridLayout->itemAtPosition(tile_pos.x() - 1, tile_pos.y()) == nullptr){
        new_x = tile_pos.x() - 1;
        new_y = tile_pos.y();
    }
    else if (tile_pos.x() < m_row_count - 1 && ui->gridLayout->itemAtPosition(tile_pos.x() + 1, tile_pos.y()) == nullptr){
        new_x = tile_pos.x() + 1;
        new_y = tile_pos.y();
    }
    else {
        new_x = tile_pos.x();
        new_y = tile_pos.y();
    }

    // Помещаем её на новую позицию, перед этим обновив свойство current_pos у Tile
    pressed_tile->set_current_pos(QPoint(new_x, new_y));
    ui->gridLayout->addItem(pressed_tile_layout_item, new_x, new_y);
}

