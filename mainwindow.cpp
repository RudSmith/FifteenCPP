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
    for (int i = 0; i < m_tiles.size(); ++i) {
        delete m_tiles[i];
    }

    delete ui;
}

void MainWindow::moveTile(Tile * tile_to_move)
{
    auto tile_at_grid = ui->gridLayout->itemAtPosition(tile_to_move->get_current_pos().x(), tile_to_move->get_current_pos().y());
    // Убираем с лэйаута нажатую плитку
    ui->gridLayout->removeItem(tile_at_grid);

    // Проверяем, куда плитку можно переместить
    QPoint new_pos = checkTilePossibleTurn(tile_to_move->get_current_pos());

    // Помещаем её на новую позицию, перед этим обновив свойство current_pos у Tile
    tile_to_move->set_current_pos(new_pos);
    ui->gridLayout->addItem(tile_at_grid, new_pos.x(), new_pos.y());
}

QPoint MainWindow::checkTilePossibleTurn(QPoint tile_pos)
{
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

    return QPoint(new_x, new_y);
}

