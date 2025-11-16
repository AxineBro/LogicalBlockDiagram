#ifndef DRAWINGDIAGRAM_H
#define DRAWINGDIAGRAM_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "SchemaTree.h"
#include "NameGenerator.h"

/**
 * @class DrawingDiagram
 * @brief Класс, отвечающий за построение графической схемы по дереву SchemaTree.
 *
 * Класс принимает указатель на корень дерева логического выражения
 * и динамически формирует QGraphicsScene, содержащую:
 *
 *  - центральный прямоугольник (главная группа оператора)
 *  - глобальный NOT (кружок справа), если есть
 *  - рекурсивную отрисовку дочерних узлов
 *  - отдельный уровень переменных слева
 *  - выход группы справа (линия, квадрат, обозначения)
 *
 * Вычисления размеров выполняются автоматически.
 */
class DrawingDiagram : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса DrawingDiagram
     * @param root Корневой узел дерева логического выражения
     * @param view QGraphicsView, определяющий размеры сцены
     * @param parent Родительский объект QObject
     */
    DrawingDiagram(const SchemaTree::Node* root,
                   QGraphicsView* view,
                   QObject* parent = nullptr);

    /**
     * @brief Построить QGraphicsScene и вернуть её
     * @return Указатель на новую QGraphicsScene
     */
    QGraphicsScene* buildScene();

private:
    NameGenerator generator;               ///< Генератор имён выходов
    qreal varLevelX;                       ///< X-координата уровня переменных слева
    const SchemaTree::Node* root;          ///< Корень логического дерева
    QGraphicsView* view;                   ///< View для отображения сцены
    qreal sceneWidth;                      ///< Итоговая ширина сцены
    qreal sceneHeight;                     ///< Итоговая высота сцены

    /**
     * @brief Подсчитывает количество узлов в поддереве
     * @param node Указатель на корень поддерева
     * @return Число узлов
     */
    int countAllNodes(const SchemaTree::Node* node) const;

    /**
     * @brief Вычисляет максимальную глубину дерева
     * @param node Текущий узел
     * @return Глубина (высота)
     */
    int calculateHeight(const SchemaTree::Node* node) const;

    /**
     * @brief Вычисляет размер квадрата/прямоугольника на основе коэффициента
     * @param coefficient Масштабирующий коэффициент
     * @return Размер стороны квадрата
     */
    qreal computeBoxSize(qreal coefficient) const;

    /**
     * @brief Добавляет текст в сцену
     * @param scene Сцена
     * @param txt Строка текста
     * @param color Цвет текста
     * @return QGraphicsTextItem*
     */
    QGraphicsTextItem* addText(QGraphicsScene* scene,
                               const QString& txt,
                               const QColor& color);

    /**
     * @brief Добавляет прямоугольник в сцену
     * @param scene Сцена
     * @param x Левый X
     * @param y Верхний Y
     * @param w Ширина
     * @param h Высота
     * @param pen Обводка
     * @param brush Заливка
     */
    void addRectRaw(QGraphicsScene* scene,
                    qreal x, qreal y,
                    qreal w, qreal h,
                    const QPen& pen,
                    const QBrush& brush = QBrush());

    /**
     * @brief Добавляет окружность/овал в сцену
     * @param scene Сцена
     * @param x Левый X
     * @param y Верхний Y
     * @param w Ширина
     * @param h Высота
     * @param pen Обводка
     */
    void addEllipseRaw(QGraphicsScene* scene,
                       qreal x, qreal y,
                       qreal w, qreal h,
                       const QPen& pen);

    /**
     * @brief Добавляет линию в сцену
     * @param scene Сцена
     * @param x1 Начало X
     * @param y1 Начало Y
     * @param x2 Конец X
     * @param y2 Конец Y
     * @param pen Обводка линии
     */
    void addLineRaw(QGraphicsScene* scene,
                    qreal x1, qreal y1,
                    qreal x2, qreal y2,
                    const QPen& pen);

    /**
     * @brief Рисует центральный прямоугольник и глобальный NOT при необходимости
     *
     * @param scene Сцена
     * @param centralNode Узел, отображаемый внутри прямоугольника
     * @param isGlobalNot Есть ли глобальное отрицание
     * @param rectX X левого края прямоугольника
     * @param rectY Y верхнего края прямоугольника
     * @param centralWidth Ширина прямоугольника
     * @param centralHeight Высота прямоугольника
     * @param coefficient Масштабирующий коэффициент
     * @param widthFactor Дополнительный множитель ширины
     * @param rightmostX Сюда записывается крайняя правая координата элемента
     */
    void drawCentralRectAndMaybeGlobalNot(QGraphicsScene* scene,
                                          const SchemaTree::Node* centralNode,
                                          bool isGlobalNot,
                                          qreal rectX, qreal rectY,
                                          qreal centralWidth, qreal centralHeight,
                                          qreal coefficient,
                                          qreal widthFactor,
                                          qreal& rightmostX);

    /**
     * @brief Рисует выход группы справа: линию, квадрат, Y, номер и логические обозначения
     *
     * @param scene Сцена
     * @param rightmostX Правая точка, откуда должен начинаться выход
     * @param centerY Y-координата центра группы
     * @param coefficient Масштабирующий коэффициент
     */
    void drawOutputGroup(QGraphicsScene* scene,
                         qreal rightmostX,
                         qreal centerY,
                         qreal coefficient);

    /**
     * @brief Рекурсивно рисует узел дерева и соединяет его с родителем
     *
     * @param scene Сцена
     * @param node Узел дерева
     * @param connectX X точки соединения с родителем
     * @param connectY Y точки соединения
     * @param allocHeight Вертикальная область, выделенная под поддерево
     * @param coefficient Масштаб отображения
     * @param treeDepth Полная глубина всего дерева
     * @param currentLevel Текущий уровень
     * @param widthFactor Множитель ширины уровней
     */
    void drawNode(QGraphicsScene* scene,
                  const SchemaTree::Node* node,
                  qreal connectX,
                  qreal connectY,
                  qreal allocHeight,
                  qreal coefficient,
                  int treeDepth,
                  int currentLevel,
                  qreal widthFactor);
};

#endif // DRAWINGDIAGRAM_H
