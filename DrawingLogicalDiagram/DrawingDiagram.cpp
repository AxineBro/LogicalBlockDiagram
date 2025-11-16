#include "DrawingDiagram.h"
#include <QDebug>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <algorithm>

// Константы.
static constexpr qreal DEFAULT_SCENE_W = 800.0;
static constexpr qreal DEFAULT_SCENE_H = 600.0;
static constexpr qreal SCENE_MARGIN = 50.0;
static constexpr qreal WIDTH_FACTOR_DEFAULT = 2.0;
static constexpr qreal DIAMETER_COEFF = 0.4;
static constexpr qreal BOXSIZE_COEFF = 0.35;
static constexpr qreal MIN_BOX_SIZE = 10.0;
static constexpr qreal OUTPUT_LINE_LEN = 100.0;
static constexpr qreal CENTRAL_RECT_RIGHT_TEXT_OFFSET = 5.0;
static constexpr qreal RECT_TEXT_PADDING = 5.0;
static constexpr qreal LETTER_TEXT_OFFSET_Y = 25.0;
static constexpr qreal VAR_TEXT_OFFSET_X = 105.0;
static constexpr qreal VAR_TEXT_NUMBER_OFFSET = 10.0;

// Конструктор класса DrawingDiagram.
DrawingDiagram::DrawingDiagram(const SchemaTree::Node* root, QGraphicsView* view, QObject* parent)
    : QObject(parent)
    , varLevelX(0)
    , root(root)
    , view(view)
    , sceneWidth(0)
    , sceneHeight(0)
{}

// Добавляет текст в сцену.
QGraphicsTextItem* DrawingDiagram::addText(QGraphicsScene* scene, const QString& txt, const QColor& color)
{
    auto* item = scene->addText(txt);
    item->setDefaultTextColor(color);
    return item;
}

// Добавляет прямоугольник в сцену.
void DrawingDiagram::addRectRaw(QGraphicsScene* scene, qreal x, qreal y, qreal w, qreal h, const QPen& pen, const QBrush& brush)
{
    scene->addRect(x, y, w, h, pen, brush);
}
// Добавляет окружность/овал в сцену.
void DrawingDiagram::addEllipseRaw(QGraphicsScene* scene, qreal x, qreal y, qreal w, qreal h, const QPen& pen)
{
    scene->addEllipse(x, y, w, h, pen);
}

// Добавляет линию в сцену.
void DrawingDiagram::addLineRaw(QGraphicsScene* scene, qreal x1, qreal y1, qreal x2, qreal y2, const QPen& pen)
{
    scene->addLine(x1, y1, x2, y2, pen);
}

// Вычисляет размер квадрата/прямоугольника на основе коэффициента.
qreal DrawingDiagram::computeBoxSize(qreal coefficient) const
{
    qreal boxSize = coefficient * BOXSIZE_COEFF;
    return (boxSize < MIN_BOX_SIZE) ? MIN_BOX_SIZE : boxSize;
}

// Построить QGraphicsScene и вернуть её.
QGraphicsScene* DrawingDiagram::buildScene()
{
    auto* scene = new QGraphicsScene();

    if (view) {
        QSize viewSize = view->viewport()->size();
        sceneWidth = viewSize.width();
        sceneHeight = viewSize.height();
    } else {
        sceneWidth = DEFAULT_SCENE_W;
        sceneHeight = DEFAULT_SCENE_H;
    }

    scene->setSceneRect(0, 0, sceneWidth, sceneHeight);

    if (!root) {
        qDebug() << "Корень дерева не задан";
        return scene;
    }

    bool isGlobalNot = (root->type == NodeType::NOT);
    const SchemaTree::Node* centralNode = isGlobalNot ? root->children[0].get() : root;

    int totalNodes = countAllNodes(centralNode);
    if (totalNodes <= 0) {
        qDebug() << "Нет узлов для отрисовки";
        return scene;
    }

    qreal coefficient = (sceneHeight - SCENE_MARGIN) / totalNodes;
    qreal widthFactor = WIDTH_FACTOR_DEFAULT;
    qreal widthCoefficient = coefficient * widthFactor;
    int treeDepth = calculateHeight(centralNode);

    qreal centralHeight = totalNodes * coefficient;
    qreal centralWidth = treeDepth * widthCoefficient;

    qreal centerX = sceneWidth / 2.0;
    qreal centerY = sceneHeight / 2.0;
    qreal rectX = centerX - centralWidth / 2.0;
    qreal rectY = centerY - centralHeight / 2.0;

    varLevelX = rectX - 100.0;
    qreal rightmostX = rectX + centralWidth;

    drawCentralRectAndMaybeGlobalNot(scene, centralNode, isGlobalNot, rectX, rectY, centralWidth, centralHeight, coefficient, widthFactor, rightmostX);

    if (centralNode->type == NodeType::VAR) {
        auto* text = addText(scene, centralNode->value, Qt::green);
        qreal textW = text->boundingRect().width();
        qreal textH = text->boundingRect().height();
        text->setPos(rectX + centralWidth / 2.0 - textW / 2.0, centerY - textH / 2.0);
    }
    else if (centralNode->type == NodeType::OP) {
        auto* opText = addText(scene, centralNode->value, Qt::blue);
        qreal opW = opText->boundingRect().width();
        opText->setPos(rectX + centralWidth - opW - RECT_TEXT_PADDING, rectY + RECT_TEXT_PADDING);

        int totalChildNodes = totalNodes - 1;
        qreal currentY = rectY;
        qreal childConnectX = rectX;
        for (const auto& ch : centralNode->children) {
            int childCount = countAllNodes(ch.get());
            qreal childAlloc = (totalChildNodes > 0)
                                   ? (static_cast<qreal>(childCount) / totalChildNodes) * centralHeight
                                   : centralHeight;
            qreal childConnectY = currentY + childAlloc / 2.0;
            drawNode(scene, ch.get(), childConnectX, childConnectY, childAlloc, coefficient, treeDepth, 1, widthFactor);
            currentY += childAlloc;
        }
    }

    drawOutputGroup(scene, rightmostX, centerY, coefficient);

    return scene;
}

// Рисует центральный прямоугольник и глобальный NOT при необходимости.
void DrawingDiagram::drawCentralRectAndMaybeGlobalNot(QGraphicsScene* scene,
                                                      const SchemaTree::Node* centralNode,
                                                      bool isGlobalNot,
                                                      qreal rectX, qreal rectY,
                                                      qreal centralWidth, qreal centralHeight,
                                                      qreal coefficient,
                                                      qreal widthFactor,
                                                      qreal& rightmostX)
{
    addRectRaw(scene, rectX, rectY, centralWidth, centralHeight, QPen(Qt::darkBlue, 1), QBrush(Qt::NoBrush));

    qreal diameter = coefficient * DIAMETER_COEFF * widthFactor;

    if (isGlobalNot) {
        qreal circleX = rectX + centralWidth;
        qreal circleY = (rectY + rectY + centralHeight) / 2.0 - diameter / 2.0;
        addEllipseRaw(scene, circleX, circleY, diameter, diameter, QPen(Qt::black, 2));
        rightmostX = circleX + diameter;
    } else {
        rightmostX = rectX + centralWidth;
    }
}

// Рисует выход группы справа: линию, квадрат, Y, номер и логические обозначения.
void DrawingDiagram::drawOutputGroup(QGraphicsScene* scene,
                                     qreal rightmostX,
                                     qreal centerY,
                                     qreal coefficient)
{
    const qreal outputEndX = rightmostX + OUTPUT_LINE_LEN;
    addLineRaw(scene,
               rightmostX, centerY,
               outputEndX, centerY,
               QPen(Qt::black, 2));

    auto* yText      = addText(scene, "Y", Qt::black);
    auto* outText    = addText(scene, generator.generateName(NameFormat::NUMERIC_PREFIX), Qt::darkGreen);
    auto* logicText  = addText(scene, generator.generateName(NameFormat::LOGIC_SUFFIX), Qt::black);
    auto* letterText = addText(scene, generator.generateName(NameFormat::LETTER_PREFIX), Qt::black);

    const qreal notDiameter = coefficient * DIAMETER_COEFF * WIDTH_FACTOR_DEFAULT;
    const qreal yW = yText->boundingRect().width();
    const qreal yH = yText->boundingRect().height();

    const qreal yPosX = rightmostX - CENTRAL_RECT_RIGHT_TEXT_OFFSET - yW - notDiameter;
    const qreal yPosY = centerY - yH / 2.0;

    yText->setPos(yPosX, yPosY);

    const qreal boxSize = computeBoxSize(coefficient);
    const qreal boxX    = outputEndX - boxSize / 2.0;
    const qreal boxY    = centerY - boxSize / 2.0;

    addRectRaw(scene,
               boxX, boxY,
               boxSize, boxSize,
               QPen(Qt::black, 2),
               QBrush(Qt::NoBrush));

    outText->setPos(outputEndX + boxSize + 10.0,
                    centerY - yH / 2.0);

    const qreal logicH = logicText->boundingRect().height();
    const qreal logicX = rightmostX;
    const qreal logicY = centerY * 2.0 - LETTER_TEXT_OFFSET_Y +
                        (LETTER_TEXT_OFFSET_Y - logicH) / 2.0;

    logicText->setPos(logicX, logicY);

    const qreal letterH = letterText->boundingRect().height();
    const qreal letterX = rightmostX;
    const qreal letterY = (LETTER_TEXT_OFFSET_Y - letterH) / 2.0;

    letterText->setPos(letterX, letterY);
}

// Подсчитывает количество узлов в поддереве.
int DrawingDiagram::countAllNodes(const SchemaTree::Node* node) const
{
    if (!node) return 0;
    int count = 1;
    for (const auto& ch : node->children) {
        count += countAllNodes(ch.get());
    }
    return count;
}

// Вычисляет максимальную глубину дерева
int DrawingDiagram::calculateHeight(const SchemaTree::Node* node) const
{
    if (!node) return 0;
    if (node->type == NodeType::NOT) {
        return calculateHeight(node->children[0].get());
    }
    int maxH = 0;
    for (const auto& ch : node->children) {
        maxH = std::max(maxH, calculateHeight(ch.get()));
    }
    return maxH + 1;
}

// Рекурсивно рисует узел дерева и соединяет его с родителем.
void DrawingDiagram::drawNode(QGraphicsScene* scene,
                              const SchemaTree::Node* node,
                              qreal connectX,
                              qreal connectY,
                              qreal allocHeight,
                              qreal coefficient,
                              int treeDepth,
                              int currentLevel,
                              qreal widthFactor)
{
    if (!node) return;

    QPen linePen(Qt::black, 2);
    qreal widthCoefficient = coefficient * widthFactor;
    qreal diameter = coefficient * DIAMETER_COEFF * widthFactor;

    if (node->type == NodeType::VAR)
    {
        auto* text = addText(scene, node->value, Qt::black);
        auto* textN = addText(scene, generator.generateName(NameFormat::NUMERIC_PREFIX), Qt::darkGreen);
        qreal boxSize = computeBoxSize(coefficient);

        qreal textNW = textN->boundingRect().width();
        qreal textH = text->boundingRect().height();

        qreal boxX  = varLevelX - (boxSize / 2.0);
        qreal boxY  = connectY - (boxSize / 2.0);

        qreal textX  = varLevelX + VAR_TEXT_OFFSET_X;
        qreal textY = connectY - textH / 2.0;

        qreal textNX  = varLevelX - VAR_TEXT_NUMBER_OFFSET - textNW - boxSize;

        text->setPos(textX, textY);
        textN->setPos(textNX, textY);

        addRectRaw(scene, boxX, boxY, boxSize, boxSize, QPen(Qt::black, 2), QBrush(Qt::NoBrush));
        addLineRaw(scene, connectX, connectY, varLevelX, connectY, linePen);
        return;
    }

    if (node->type == NodeType::NOT)
    {
        qreal circleX = connectX - diameter;
        qreal circleY = connectY - diameter / 2.0;
        addEllipseRaw(scene, circleX, circleY, diameter, diameter, linePen);

        qreal childConnectX = circleX;
        drawNode(scene, node->children[0].get(),
                 childConnectX, connectY,
                 allocHeight,
                 coefficient, treeDepth, currentLevel,
                 widthFactor);
        return;
    }

    if (node->type == NodeType::OP)
    {
        qreal rectWidth = (treeDepth - currentLevel) * widthCoefficient;
        qreal rectHeight = allocHeight;

        qreal rectX = connectX;
        qreal rectY = connectY - rectHeight / 2.0;

        addRectRaw(scene, rectX, rectY, rectWidth, rectHeight, QPen(Qt::darkBlue, 1), QBrush(Qt::NoBrush));

        QString displayText;
        if (node->value == "|") {
            displayText = "1";
        } else if (node->value == "^") {
            displayText = "=1";
        } else {
            displayText = node->value;
        }
        auto* opText = addText(scene, displayText, Qt::blue);
        qreal opW = opText->boundingRect().width();
        opText->setPos(rectX + rectWidth - opW - RECT_TEXT_PADDING, rectY + RECT_TEXT_PADDING);

        int totalChildNodes = countAllNodes(node) - 1;

        qreal currentY = rectY;
        qreal childConnectX = rectX;

        for (const auto& ch : node->children)
        {
            int childCount = countAllNodes(ch.get());
            qreal childAlloc = (totalChildNodes > 0)
                                   ? (static_cast<qreal>(childCount) / totalChildNodes) * rectHeight
                                   : rectHeight;

            qreal childConnectY = currentY + childAlloc / 2.0;

            drawNode(scene, ch.get(),
                     childConnectX, childConnectY,
                     childAlloc,
                     coefficient, treeDepth, currentLevel + 1,
                     widthFactor);

            currentY += childAlloc;
        }
    }
}
