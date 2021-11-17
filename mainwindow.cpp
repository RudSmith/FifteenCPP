#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_time_passed(0, 0, 0),
      m_turns_count{}
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

            // TODO Убрать позже!!
            m_tiles[i * m_row_count + j]->setText(QString::number(i * m_row_count + j));

            ui->gridLayout->addWidget(m_tiles[i * m_row_count + j], i, j);
            connect(m_tiles[i * m_row_count + j], &Tile::needToMove, this, &MainWindow::moveTile);
        }
    }

    // Настраиваем таймер
    m_timer = new QTimer();
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::updateTimer);

    // Настраиваем кнопку "Начать заново"
    connect(ui->replay_pushButton, &QPushButton::clicked, this, &MainWindow::restartGame);

    // Настраиваем кнопку "Перемешать"
    connect(ui->mix_pushButton, &QPushButton::clicked, this, &MainWindow::mixTiles);
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

    // Обновляем счётчик ходов, если плитка сдвинулась
    if(new_pos != tile_to_move->get_current_pos()) {
        // Если ходов ещё не было, запускаем таймер
        if(m_turns_count == 0)
            m_timer->start();

        updateTurnsCount();
    }

    // Помещаем её на новую позицию, перед этим обновив свойство current_pos у Tile
    tile_to_move->set_current_pos(new_pos);
    ui->gridLayout->addItem(tile_at_grid, new_pos.x(), new_pos.y());
}

QPoint MainWindow::checkTilePossibleTurn(QPoint tile_pos)
{
    // Определяем, куда её нужно поместить
    if(tile_pos.y() > 0 && ui->gridLayout->itemAtPosition(tile_pos.x(), tile_pos.y() - 1) == nullptr){
        return QPoint(tile_pos.x(), tile_pos.y() - 1);
    }
    else if (tile_pos.y() < m_column_count - 1 && ui->gridLayout->itemAtPosition(tile_pos.x(), tile_pos.y() + 1) == nullptr){
        return QPoint(tile_pos.x(), tile_pos.y() + 1);
    }
    else if (tile_pos.x() > 0 && ui->gridLayout->itemAtPosition(tile_pos.x() - 1, tile_pos.y()) == nullptr){
        return QPoint(tile_pos.x() - 1, tile_pos.y());
    }
    else if (tile_pos.x() < m_row_count - 1 && ui->gridLayout->itemAtPosition(tile_pos.x() + 1, tile_pos.y()) == nullptr){
        return QPoint(tile_pos.x() + 1, tile_pos.y());
    }
    else {
        return QPoint(tile_pos.x(), tile_pos.y());
    }

    return QPoint();
}

void MainWindow::updateTurnsCount()
{
    m_turns_count++;
    ui->turns_count_label->setText(QString::number(m_turns_count));
}

void MainWindow::restartGame()
{
    ui->time_label->setText("00:00");
    ui->turns_count_label->setText("0");

    // Удаляем таймер, создаём новый
    delete m_timer;
    m_timer = new QTimer();
    // Настраиваем таймер
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::updateTimer);

    // Обнуляем счётчики ходов
    m_time_passed.setHMS(0, 0, 0);
    m_turns_count = 0;

    // Устанавливаем начальные позиции всем плиткам
    for (auto & tile : m_tiles) {
        auto tile_at_grid = ui->gridLayout->itemAtPosition(tile->get_current_pos().x(), tile->get_current_pos().y());
        ui->gridLayout->removeItem(tile_at_grid);
        ui->gridLayout->addItem(tile_at_grid, tile->get_initial_pos().x(), tile->get_initial_pos().y());
        tile->set_current_pos(tile->get_initial_pos());
    }
}

void MainWindow::mixTiles()
{
    // В цикле проходим по всему гриду и свапаем элементы
    for(int i = 0; i < m_row_count; ++i){
        for (int j = 0; j < m_column_count; ++j) {
            // У нас 15 плиток, так что на 16 элементе выходим из цикла
            if(i * m_row_count + j == m_row_count * m_column_count - 1)
                break;

            // Генерируем два числа - строку и столбец, с которой будем свапать текущий элемент
            int new_row = QRandomGenerator::global()->bounded(0, m_row_count - 1);
            int new_column = QRandomGenerator::global()->bounded(0, m_row_count - 1);

            // Получаем указатели на текущий и подлежащий обмену элемент грида
            auto tile_1 = ui->gridLayout->itemAtPosition(i, j);
            auto tile_2 = ui->gridLayout->itemAtPosition(new_row, new_column);

            // Получаем итератор на текущий элемент в векторе
            QVector<Tile*>::iterator it_1 = std::find_if(m_tiles.begin(), m_tiles.end(), [i, j](Tile *p){
                    return p->get_current_pos() == QPoint(i, j);
                }
            );

            // Получаем итератор на подлежащий обмену элемент в векторе
            QVector<Tile*>::iterator it_2 = std::find_if(m_tiles.begin(), m_tiles.end(), [new_row, new_column](Tile *p){
                    return p->get_current_pos() == QPoint(new_row, new_column);
                }
            );

            // Обмениваем позициями плитки
            ui->gridLayout->removeItem(tile_1);
            ui->gridLayout->removeItem(tile_2);

            (*it_1)->set_current_pos(QPoint(new_row, new_column));
            (*it_2)->set_current_pos(QPoint(i, j));

            ui->gridLayout->addItem(tile_1, new_row, new_column);
            ui->gridLayout->addItem(tile_2, i, j);
        }
    }
}

void MainWindow::updateTimer()
{
    m_time_passed = m_time_passed.addSecs(1);
    ui->time_label->setText(m_time_passed.toString("mm:ss"));
}

