#ifndef TILE_H
#define TILE_H

#include <QPushButton>
#include <QWidget>
#include <QString>
#include <QPoint>
#include <QMouseEvent>

// Плитка, наследуется от обычной кнопки
class Tile : public QPushButton
{
    Q_OBJECT
public:
    Tile(QWidget *parent = nullptr);
    // Конструктор с установкой начального положения
    Tile(QPoint init_pos, QWidget *parent = nullptr);

    // Получить начальную позицию
    QPoint get_initial_pos() const;
    // Получить текущую позицию
    QPoint get_current_pos() const;

    // Установить начальную позицию
    void set_initial_pos(QPoint pos);
    // Установить текущую позицию
    void set_current_pos(QPoint pos);

    // Пользователь нажал на плитку
    void mousePressEvent(QMouseEvent *e);

signals:
    // Сигнал о том, что плитка нуждается в перемещении
    void needToMove(QPoint);

private:
    // Начальная позиция
    QPoint m_initial_pos;
    // Текущая позиция
    QPoint m_current_pos;
};

#endif // TILE_H
