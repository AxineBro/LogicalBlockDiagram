#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {

/**
 * @class MainWindow
 * @brief Главное окно приложения для построения и отображения логических схем
 *
 * Класс представляет основное окно интерфейса пользователя,
 * где вводится логическое выражение, строится дерево и схема,
 * а также предоставляется возможность сохранения схемы в изображение.
 *
 * @details
 * - Использует Qt Designer для UI (ui_mainwindow.h).
 * - Обрабатывает нажатия кнопок "Выполнить" и "Сохранить".
 * - Интегрируется с классами SchemaTree и SchemaProgram для парсинга и отрисовки.
 */
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    /**
     * @brief Конструктор главного окна
     * @param parent Родительский виджет (по умолчанию nullptr)
     *
     * Инициализирует UI и устанавливает соединения.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Деструктор главного окна
     *
     * Освобождает ресурсы UI.
     */
    ~MainWindow();

private slots:

    /**
     * @brief Обработчик нажатия кнопки "Выполнить"
     *
     * Получает текст из inputEdit, строит SchemaTree,
     * выводит дерево в консоль и создаёт схему через SchemaProgram.
     */
    void on_executeButton_clicked();

    /**
     * @brief Обработчик нажатия кнопки "Сохранить"
     *
     * Открывает диалог сохранения файла, рендерит сцену graphicsView
     * в QImage и сохраняет как изображение (PNG, JPG, BMP).
     */
    void on_saveButton_clicked();

private:
    Ui::MainWindow *ui;   ///< Указатель на UI, сгенерированный Qt Designer
};
#endif // MAINWINDOW_H
