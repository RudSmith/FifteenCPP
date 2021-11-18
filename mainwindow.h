#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

#include <QTimer>
#include <QTime>

#include <QRandomGenerator>

#include <QIcon>
#include <QImage>

#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>

#include <QFile>
#include <QIODevice>

#include <algorithm>

#include "tile.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Структура, обозначающая результат игры
struct LeaderBoardEntry {
public:
    LeaderBoardEntry(const QString & _name, const QTime & _time, int _turns_count)
        : name { _name },
          time { _time },
          turns_count { _turns_count }
    {}

    bool operator==(const LeaderBoardEntry &rhs){
        return (name == rhs.name && time == rhs.time && turns_count == rhs.turns_count);
    }

    QString toString() const
    {
        QString res = QString(name + " \t " +
                              time.toString() + " \t " + QString::number(turns_count) + " ход(-a/-ов).\n");
        return res;
    }

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
    // Показать таблицу лидеров
    void showLeaders();

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
    // Считать лидеров из файла
    void readLeaders();
    // Добавить лидера
    void addLeader(const LeaderBoardEntry &entry);
    // Записать лидеров в файл
    void writeLeaders();
    // Установить картинку на плитки
    void setImage();
    // Выделить из картинки кусочек, равный rect
    QImage createSubImage(QImage* image, const QRect & rect);

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

    // Вектор с результатами игр
    QVector<LeaderBoardEntry> m_entries;

    // Картинка поля
    QImage m_image;
};
#endif // MAINWINDOW_H
