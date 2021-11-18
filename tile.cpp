#include "tile.h"


Tile::Tile(QWidget *parent)
    : QPushButton(parent)
{
    setMinimumSize(101, 81);
    setMaximumSize(101, 81);
}

Tile::Tile(QPoint init_pos, int value, QWidget *parent)
    : QPushButton(parent),
      m_initial_pos { init_pos },
      m_current_pos { m_initial_pos },
      m_value { value }
{
    setMinimumSize(148, 120);
    setMaximumSize(148, 120);
}

QPoint Tile::get_initial_pos() const
{
    return m_initial_pos;
}

QPoint Tile::get_current_pos() const
{
    return m_current_pos;
}

int Tile::get_value() const
{
    return m_value;
}

void Tile::set_initial_pos(QPoint pos)
{
    m_initial_pos = pos;
}

void Tile::set_current_pos(QPoint pos)
{
    m_current_pos = pos;
}

void Tile::mousePressEvent(QMouseEvent *e)
{
    emit needToMove(this);
}
