#include "DrawingDiagram.h"
#include <QDebug> // Добавлено для qDebug, если используется
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <algorithm>
#include "NamingType.h"

DrawingDiagram::DrawingDiagram(const SchemaTree::Node* root, QGraphicsView* view, QObject* parent)
    : QObject(parent), root(root), view(view), sceneWidth(0), sceneHeight(0)
{}

QGraphicsScene* DrawingDiagram::buildScene()
{
    auto* scene = new QGraphicsScene();
    if (view) {
        QSize viewSize = view->viewport()->size();
        sceneWidth = viewSize.width();
        sceneHeight = viewSize.height();
    } else {
        sceneWidth = 800;
        sceneHeight = 600;
    }
    scene->setSceneRect(0, 0, sceneWidth, sceneHeight);
    if (!root) {
        qDebug() << "Корень дерева не задан";
        return scene;
    }
    bool isGlobalNot = (root->type == NodeType::NOT);
    const SchemaTree::Node* centralNode = isGlobalNot ? root->children[0].get() : root;
    int totalNodes = countAllNodes(centralNode);
    qreal coefficient = (sceneHeight - 50) / totalNodes;
    qreal widthFactor = 2.0; // Фактор для увеличения ширины (можно настроить)
    qreal widthCoefficient = coefficient * widthFactor;
    int treeDepth = calculateHeight(centralNode);
    qreal centralHeight = totalNodes * coefficient;
    qreal centralWidth = treeDepth * widthCoefficient;
    qreal centerX = sceneWidth / 2;
    qreal centerY = sceneHeight / 2;
    qreal rectX = centerX - centralWidth / 2;
    qreal rectY = centerY - centralHeight / 2;
    varLevelX = rectX - 100;  // единый уровень для всех переменных
    scene->addRect(rectX, rectY, centralWidth, centralHeight, QPen(Qt::darkBlue, 1), QBrush(Qt::NoBrush));
    qreal diameter = coefficient * 0.4 * widthFactor; // Масштабируем диаметр для пропорций
    qreal rightmostX;
    if (isGlobalNot) {
        qreal circleX = rectX + centralWidth;
        qreal circleY = centerY - diameter / 2;
        scene->addEllipse(circleX, circleY, diameter, diameter, QPen(Qt::black, 2));
        rightmostX = circleX + diameter;
    } else {
        rightmostX = rectX + centralWidth;
    }
    if (centralNode->type == NodeType::VAR) {
        auto* text = scene->addText(centralNode->value);
        text->setDefaultTextColor(Qt::green);
        qreal textW = text->boundingRect().width();
        qreal textH = text->boundingRect().height();
        text->setPos(rectX + centralWidth / 2 - textW / 2, centerY - textH / 2);
    } else if (centralNode->type == NodeType::OP) {
        auto* opText = scene->addText(centralNode->value);
        opText->setDefaultTextColor(Qt::blue);
        qreal opW = opText->boundingRect().width();
        opText->setPos(rectX + centralWidth - opW - 5, rectY + 5);
        size_t n = centralNode->children.size();
        int totalChildNodes = totalNodes - 1;
        qreal currentY = rectY;
        qreal childConnectX = rectX;
        for (const auto& ch : centralNode->children) {
            int childCount = countAllNodes(ch.get());
            qreal childAlloc = (totalChildNodes > 0) ? (static_cast<qreal>(childCount) / totalChildNodes) * centralHeight : centralHeight;
            qreal childConnectY = currentY + childAlloc / 2;
            drawNode(scene, ch.get(), childConnectX, childConnectY, childAlloc, coefficient, treeDepth, 1, widthFactor);
            currentY += childAlloc;
        }
    }
    qreal outputLineLength = 100;
    qreal outputEndX = rightmostX + outputLineLength;
    scene->addLine(rightmostX, centerY, outputEndX, centerY, QPen(Qt::black, 2));
    auto* yText = scene->addText("Y");
    auto* outText = scene->addText(generator.generateName(NameFormat::NUMERIC_PREFIX));
    auto* logicText = scene->addText(generator.generateName(NameFormat::LOGIC_SUFFIX));
    auto* letterText = scene->addText(generator.generateName(NameFormat::LETTER_PREFIX));
    yText->setDefaultTextColor(Qt::black);
    outText->setDefaultTextColor(Qt::darkGreen);
    logicText->setDefaultTextColor(Qt::black);
    letterText->setDefaultTextColor(Qt::black);
    qreal yW = yText->boundingRect().width();
    qreal yH = yText->boundingRect().height();
    yText->setPos(rectX + centralWidth - 5 - yW, centerY - yH / 2);
    qreal boxSize = coefficient * 0.35;  // хороший масштаб
    if (boxSize < 10) boxSize = 10;
    qreal boxX  = outputEndX - (boxSize/2);   // единый X
    qreal boxY  = centerY - (boxSize/2);
    scene->addRect(boxX, boxY, boxSize, boxSize, QPen(Qt::black, 2), QBrush(Qt::NoBrush));
    outText->setPos(outputEndX + boxSize + 10, centerY - yH / 2);
    qreal loigcH = yText->boundingRect().height();
    logicText->setPos(rectX + centralWidth, centralHeight + 25 + (25-loigcH) / 2);
    qreal letterH = yText->boundingRect().height();
    letterText->setPos(rectX + centralWidth, (25-letterH) / 2);
    return scene;
}

void DrawingDiagram::drawDemoElements(QGraphicsScene* scene)
{
    qreal centerX = sceneWidth / 2.0;
    qreal centerY = sceneHeight / 2.0;
    qreal rectWidth = 120;
    qreal rectHeight = 80;
    QRectF rect(centerX - rectWidth / 2, centerY - rectHeight / 2, rectWidth, rectHeight);
    scene->addRect(rect, QPen(Qt::black, 2), QBrush(Qt::lightGray));
    QPen greenPen(Qt::green, 3);
    scene->addLine(rect.right() + 20, centerY, rect.right() + 150, centerY, greenPen);
    auto* text1 = scene->addText("Центральный прямоугольник");
    text1->setDefaultTextColor(Qt::blue);
    text1->setPos(centerX - 100, centerY - rectHeight - 40);
    auto* text2 = scene->addText("Зелёная линия справа");
    text2->setDefaultTextColor(Qt::red);
    text2->setPos(centerX + 50, centerY + rectHeight + 20);
}

int DrawingDiagram::countAllNodes(const SchemaTree::Node* node) const
{
    if (!node) return 0;
    int count = 1;
    for (const auto& ch : node->children) {
        count += countAllNodes(ch.get());
    }
    return count;
}

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
    qreal diameter = coefficient * 0.4 * widthFactor;

    // ================================
    // ------- ПЕЧАТЬ ПЕРЕМЕННЫХ ------
    // ================================
    if (node->type == NodeType::VAR)
    {
        // Текст ВСЕГДА ПЕЧАТАЕТСЯ НА ОДНОМ УРОВНЕ
        auto* text = scene->addText(node->value);
        auto* textN = scene->addText(generator.generateName(NameFormat::NUMERIC_PREFIX));
        qreal boxSize = coefficient * 0.35;  // хороший масштаб
        if (boxSize < 10) boxSize = 10;
        text->setDefaultTextColor(Qt::black);
        textN->setDefaultTextColor(Qt::darkGreen);

        qreal textNW = textN->boundingRect().width();
        qreal textH = text->boundingRect().height();

        qreal boxX  = varLevelX - (boxSize/2);   // единый X
        qreal boxY  = connectY - (boxSize/2);

        qreal textX  = varLevelX + 105;   // единый X
        qreal textY = connectY - textH / 2;

        qreal textNX  = varLevelX - 10 - textNW - boxSize;   // единый X

        text->setPos(textX, textY);
        textN->setPos(textNX, textY);

        scene->addRect(boxX, boxY, boxSize, boxSize, QPen(Qt::black, 2), QBrush(Qt::NoBrush));

        // ЛИНИЯ ВСЕГДА ОТ connectX К varLevelX
        scene->addLine(connectX, connectY, varLevelX, connectY, linePen);
        return;
    }

    // ================================
    // ------------ NOT --------------
    // ================================
    if (node->type == NodeType::NOT)
    {
        // Кружок NOT на линии (без смещения ребёнка)
        qreal circleX = connectX - diameter;
        qreal circleY = connectY - diameter / 2;

        scene->addEllipse(circleX, circleY, diameter, diameter, linePen);

        // Линия от кружка k точке подключения переменной или поддерева
        qreal childConnectX = circleX;  // немного левее кружка

        // РЕКУРСИЯ — но ребёнок НЕ сдвигает свой уровень!
        drawNode(scene, node->children[0].get(),
                 childConnectX, connectY,
                 allocHeight,
                 coefficient, treeDepth, currentLevel,
                 widthFactor);
        return;
    }

    // ================================
    // --------- ОПЕРАЦИИ AND/OR -----
    // ================================
    if (node->type == NodeType::OP)
    {
        qreal rectWidth = (treeDepth - currentLevel) * widthCoefficient;
        qreal rectHeight = allocHeight;

        qreal rectX = connectX;
        qreal rectY = connectY - rectHeight / 2;

        scene->addRect(rectX, rectY, rectWidth, rectHeight, QPen(Qt::darkBlue, 1));

        QString displayText;
        if (node->value == "|") {
            displayText = "1";
        } else if (node->value == "^") {
            displayText = "=1";
        } else {
            displayText = node->value;
        }
        auto* opText = scene->addText(displayText);
        opText->setDefaultTextColor(Qt::blue);
        qreal opW = opText->boundingRect().width();
        opText->setPos(rectX + rectWidth - opW - 5, rectY + 5);

        int totalChildNodes = countAllNodes(node) - 1;

        qreal currentY = rectY;
        qreal childConnectX = rectX; // слева от прямоугольника

        for (const auto& ch : node->children)
        {
            int childCount = countAllNodes(ch.get());
            qreal childAlloc = (totalChildNodes > 0)
                                   ? (static_cast<qreal>(childCount) / totalChildNodes) * rectHeight
                                   : rectHeight;

            qreal childConnectY = currentY + childAlloc / 2;

            drawNode(scene, ch.get(),
                     childConnectX, childConnectY,
                     childAlloc,
                     coefficient, treeDepth, currentLevel + 1,
                     widthFactor);

            currentY += childAlloc;
        }
    }
}

