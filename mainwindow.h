#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QTimer>
#include <QTime>
#include <QRandomGenerator>
#include <QInputDialog>
#include <QFile>
#include <QIODevice>

#include <algorithm>

#include "tile.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct LeaderBoardEntry {
    QString name;
    QTime time;
    int turns_count;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Получен сигнал о перемещении плитки, обрабатываем его
    void moveTile(Tile * tile_to_move);
    // Обновить таймер
    void updateTimer();
    // Начать игру
    void startGame();
    // Начать игру
    void restartGame();

private:
    // Проверить, куда можно передвинуть плитку
    QPoint checkTilePossibleTurn(QPoint tile_pos);
    // Обновить счётчик ходов
    void updateTurnsCount();
    // Перемешать плитки
    void mixTiles();
    // Проверить, решается ли текущая комбинация
    bool isSolvable();
    // Проверить, решена ли игра
    bool isSolved();
    // Закончить игру и показать результат
    void finishGame();

    Ui::MainWindow *ui;
    // Массив плиток
    QVector<Tile*> m_tiles;
    // Время игры пользователя
    QTimer * m_timer;
    // Время с начала игры
    QTime m_time_passed;

    // Кол-во ходов
    int m_turns_count;

    // Количество плиток
    const int m_tiles_count = 15;
    // Кол-во строк поля
    const int m_row_count = 4;
    // Кол-во столбцов поля
    const int m_column_count = 4;

    QVector<LeaderBoardEntry> m_entries;

    // TODO убрать
    bool m_is_game_started;
};
#endif // MAINWINDOW_H
