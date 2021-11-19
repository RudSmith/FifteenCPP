#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_timer { nullptr },
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
            m_tiles[i * m_row_count + j] = new Tile(QPoint(i, j), i * m_row_count + j + 1);
            m_tiles[i * m_row_count + j]->setEnabled(false);

            m_tiles[i * m_row_count + j]->setText(QString::number(m_tiles[i * m_row_count + j]->get_value()));

            ui->gridLayout->addWidget(m_tiles[i * m_row_count + j], i, j);
            connect(m_tiles[i * m_row_count + j], &Tile::needToMove, this, &MainWindow::moveTile);
        }
    }

    ui->replay_pushButton->setEnabled(false);

    // Настраиваем таймер
    m_timer = new QTimer();
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::updateTimer);

    //Коннектим кнопку "Начать заново"
    connect(ui->replay_pushButton, &QPushButton::clicked, this, &MainWindow::restartGame);

    // Коннектим кнопку "Перемешать"
    connect(ui->start_pushButton, &QPushButton::clicked, this, &MainWindow::startGame);

    // Коннектим кнопку "Таблица лидеров"
    connect(ui->leaderboard_pushButton, &QPushButton::clicked, this, &MainWindow::showLeaders);

    // Коннектим кнопку "Таблица лидеров"
    connect(ui->upload_image_pushButton, &QPushButton::clicked, this, &MainWindow::setImage);

    // Считываем лидеров из файла
    readLeaders();
}

MainWindow::~MainWindow()
{
    // Записываем лидеров в файл
    writeLeaders();

    // очищаем память от плиток
    for (int i = 0; i < m_tiles.size(); ++i) {
        delete m_tiles[i];
    }

    delete m_timer;
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
        updateTurnsCount();
    }

    // Помещаем её на новую позицию, перед этим обновив свойство current_pos у Tile
    tile_to_move->set_current_pos(new_pos);
    ui->gridLayout->addItem(tile_at_grid, new_pos.x(), new_pos.y());

    // Если игра решена, обрабатываем это
    if(isSolved()) {
        finishGame();
    }
}

QPoint MainWindow::checkTilePossibleTurn(QPoint tile_pos)
{
    // Определяем, куда плитку нужно поместить
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

void MainWindow::startGame()
{
    mixTiles();

    for (auto & tile : m_tiles) {
        tile->setEnabled(true);
    }

    m_timer->start();
    ui->upload_image_pushButton->setEnabled(false);
    ui->replay_pushButton->setEnabled(true);
    ui->start_pushButton->setEnabled(false);
}

void MainWindow::restartGame()
{

    for (auto & tile : m_tiles) {
        tile->setEnabled(false);
    }

    ui->upload_image_pushButton->setEnabled(true);
    ui->replay_pushButton->setEnabled(false);
    ui->start_pushButton->setEnabled(true);

    ui->time_label->setText("00:00");
    ui->turns_count_label->setText("0");

    // Останавливаем таймер
    m_timer->stop();

    // Обнуляем счётчики ходов
    m_time_passed.setHMS(0, 0, 0);
    m_turns_count = 0;

    // Удаляем с поля все плитки
    for (auto &tile : m_tiles) {
        ui->gridLayout->removeWidget(tile);
        tile->set_current_pos(tile->get_initial_pos());
    }

    // Сортируем массив плиток
    std::sort(m_tiles.begin(), m_tiles.end(), [](Tile* t1, Tile* t2){ return t1->get_value() < t2->get_value(); });

    // Помещаем плитки назад
    for (auto & tile : m_tiles) {
        ui->gridLayout->addWidget(tile, tile->get_current_pos().x(), tile->get_current_pos().y());
    }
}

void MainWindow::mixTiles()
{
    // Удаляем плитки с поля
    for (auto & tile : m_tiles) {
        ui->gridLayout->removeWidget(tile);
    }

    // Перемешиваем плитки
    for (int i = 0; i < m_tiles.size(); ++i) {
        int new_index = QRandomGenerator::global()->bounded(0, m_tiles_count - 1);
        m_tiles.swapItemsAt(i, new_index);

        QPoint temp = m_tiles[i]->get_current_pos();
        m_tiles[i]->set_current_pos(m_tiles[new_index]->get_current_pos());
        m_tiles[new_index]->set_current_pos(temp);
    }

    // Проверяем, решается ли комбинация
    while (!isSolvable()) {
        for (int i = 0; i < m_tiles.size(); ++i) {
            int new_index = QRandomGenerator::global()->bounded(0, m_tiles_count - 1);
            m_tiles.swapItemsAt(i, new_index);

            QPoint temp = m_tiles[i]->get_current_pos();
            m_tiles[i]->set_current_pos(m_tiles[new_index]->get_current_pos());
            m_tiles[new_index]->set_current_pos(temp);
        }
    }

    // Помещаем плитки на поле
    for (auto & tile : m_tiles) {
        ui->gridLayout->addWidget(tile, tile->get_current_pos().x(), tile->get_current_pos().y());
    }
}

bool MainWindow::isSolvable()
{
    // Алгоритм взят из учебника по алгоритмам
    int sum = 0;

    for (int i = 0; i < m_tiles.size(); ++i) {
        for (int j = 0; j < i; ++j) {
            if (m_tiles[j]->get_value() > m_tiles[i]->get_value())
                ++sum;
        }
    }

    sum += 1 + ((m_tiles.size() - 1) / m_row_count);
    return (sum % 2 == 0);
}

bool MainWindow::isSolved()
{
    // Проверяем, находятся ли все плитки на своих местах
    bool isSolved = true;

    for (const auto &tile : m_tiles) {
        if(tile->get_current_pos() != tile->get_initial_pos())
            isSolved = false;
    }

    return isSolved;
}

void MainWindow::finishGame()
{
    m_timer->stop();

    bool ok;

    // Выводим диалог с предложением ввести ник
    QString nickname = QInputDialog::getText(this, "Победа!", "Вы решили пятнашки за " + m_time_passed.toString("mm:ss") +
                                             ". Это заняло у вас " + QString::number(m_turns_count)  + " ходов." +
                                             "Введите своё имя для таблицы лидеров: ", QLineEdit::EchoMode::Normal, QString(), &ok);

    // Если нажата кнопка ОК, записываем результат в таблицу лидеров
    if(ok && !nickname.isEmpty()) {
        addLeader(LeaderBoardEntry(nickname, m_time_passed, m_turns_count));
    }

    // Очищаем поле
    restartGame();
}

void MainWindow::showLeaders()
{
    // Простой вывод в messageBox
    QMessageBox mb;
    mb.setWindowTitle("Таблица лидеров");

    for(auto & entry : m_entries){
        mb.setText(mb.text() + entry.toString());
    }

    mb.exec();
}

void MainWindow::readLeaders()
{
    // Открываем файл, читаем оттуда всё
    QFile leaders_file("leaders.txt");
    leaders_file.open(QIODevice::ReadOnly | QIODevice::Text);

    while(!leaders_file.atEnd()){
        QString cur_entry = leaders_file.readLine();
        QStringList splitted_entry = cur_entry.split(" ");

        QString nick = splitted_entry[0];
        QTime time = QTime::fromString(splitted_entry[1], "mm:ss");
        int turns = splitted_entry[2].toInt();

        m_entries.push_back(LeaderBoardEntry(nick, time, turns));
    }

    // Сортируем вектор с лидерами по следующему критерию: если кол-во ходов одинаково, сравниваем результаты по времени, иначе по кол-ву ходов
    std::sort(m_entries.begin(), m_entries.end(),
              [](const LeaderBoardEntry & e1, const LeaderBoardEntry & e2)
              { return (e1.turns_count == e2.turns_count ? e1.time < e2.time : e1.turns_count < e2.turns_count); });
}

void MainWindow::addLeader(const LeaderBoardEntry &entry)
{
    // Здесь проверяем, есть ли уже лидер с указанным ником
   LeaderBoardEntry * existing_leader = nullptr;

   for (auto & leader : m_entries) {
       if(leader.name == entry.name)
           existing_leader = &leader;
   }

    // Если есть, обновляем его результат
    if(existing_leader != nullptr){
        existing_leader->time = entry.time;
        existing_leader->turns_count = entry.turns_count;
    }
    else
        m_entries.push_back(entry);

    // Сортируем вектор лидеров
    std::sort(m_entries.begin(), m_entries.end(),
              [](const LeaderBoardEntry & e1, const LeaderBoardEntry & e2)
              { return (e1.turns_count == e2.turns_count ? e1.time < e2.time : e1.turns_count < e2.turns_count); });
}

void MainWindow::writeLeaders()
{
    // В деструкторе окна выписываем всех отсортированных лидеров назад в файл
    QFile leaders_file("leaders.txt");
    leaders_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);

    for (auto & entry : m_entries) {
        leaders_file.write(entry.name.toUtf8());
        leaders_file.write(" ");
        leaders_file.write(entry.time.toString("mm:ss").toUtf8());
        leaders_file.write(" ");
        leaders_file.write(QByteArray::number(entry.turns_count));
        leaders_file.write(" ");
        leaders_file.write("\n");
    }
}

void MainWindow::setImage()
{
    // Открываем файл с картинкой
    auto fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));

    // Сжимаем картинку
    const int image_width = 660;
    const int image_height = 480;

    m_image.load(fileName);
    m_image = m_image.scaled(image_width, image_height);

    // Создаём вектор кусков картинки
    QVector<QPixmap> pieces;

    // Задаём размеры куска
    const int piece_width = image_width / m_row_count;
    const int piece_height = image_height / m_column_count;

    // В цикле генерируем 16 кусков
    for(int i = 0; i < m_row_count; i++){
        for (int j = 0; j < image_width; j += piece_width) {
            pieces.push_back(QPixmap::fromImage(QImage(createSubImage
                                                       (&m_image, QRect(QPoint(j, i * piece_height), QPoint(j + piece_width, (i + 1) * piece_height))))));
        }
    }

    // Выставляем каждой плитке иконку и задаём ей размер
    for (int i = 0; i < m_tiles.size(); ++i) {
        m_tiles[i]->setIcon(pieces[i]);
        m_tiles[i]->setIconSize(QSize(pieces[i].size()));
    }
 }

QImage MainWindow::createSubImage(QImage *image, const QRect &rect)
{
    // https://stackoverflow.com/questions/12681554/dividing-qimage-to-smaller-pieces

    size_t offset = rect.x() * image->depth() / 8
            + rect.y() * image->bytesPerLine();
    return QImage(image->bits() + offset, rect.width(), rect.height(),
                  image->bytesPerLine(), image->format());
}

void MainWindow::updateTimer()
{
    m_time_passed = m_time_passed.addSecs(1);
    ui->time_label->setText(m_time_passed.toString("mm:ss"));
}

