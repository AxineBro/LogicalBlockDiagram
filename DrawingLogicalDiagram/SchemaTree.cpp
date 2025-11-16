#include <SchemaTree.h>
#include <QDebug>

// Реализация конструктора
SchemaTree::SchemaTree(const QString& text){
    SchemaTree::buildTree(text);
    height = calculateHeight(root);
    width = calculateWidth(root);
}

// Получить высоту дерева
int  SchemaTree::getHeight() const{
    return height;
}

// Получить высоту узла
int  SchemaTree::getHeightNode(const std::unique_ptr<Node>& node) const{
    return SchemaTree::calculateHeight(node);
}

// Получить ширину дерева
int SchemaTree::getWidth() const{
    return width;
}

// Получить ширину узла
int  SchemaTree::getWidthNode(const std::unique_ptr<Node>& node) const{
    return SchemaTree::calculateWidth(node);
}

// Получить корень
const SchemaTree::Node* SchemaTree::getRoot() const {
    return root.get();
}

// Посчитать высоту
int SchemaTree::calculateHeight(const std::unique_ptr<SchemaTree::Node>& node) const{
    if (!node) return 0;

    int maxHeight = 0;

    for (const auto& child : node->children)
        maxHeight = std::max(maxHeight, calculateHeight(child));

    return maxHeight + 1;
}

// Посчитать ширину
int SchemaTree::calculateWidth(const std::unique_ptr<SchemaTree::Node>& node) const{
    if (!node) return 0;
    if (node->children.empty()) return 1;

    int total = 0;

    for (const auto& child : node->children)
        total += calculateWidth(child);

    return total;
}

// Построить дерево
void SchemaTree::buildTree(const QString& text) {
    if (text.isEmpty()) {
        root.reset();
        return;
    }

    QString expr = text;
    expr.remove(' ');

    if (expr.startsWith('!')) {
        QString inside = expr.mid(1);

        bool isGlobalNot = false;

        if (inside.startsWith('(')) {
            int depth = 0;
            for (int i = 0; i < inside.size(); ++i) {
                if (inside[i] == '(') depth++;
                else if (inside[i] == ')') depth--;
                if (depth == 0 && i < inside.size() - 1) {
                    isGlobalNot = false;
                    break;
                }
                if (depth == 0 && i == inside.size() - 1) {
                    isGlobalNot = true;
                }
            }
        }

        if (isGlobalNot) {
            root = std::make_unique<Node>(NodeType::NOT, "!");
            inside = inside.mid(1, inside.length() - 2);
            root->children = buildTreeHelper(inside);
            return;
        }
    }

    std::vector<std::unique_ptr<Node>> result = buildTreeHelper(expr);
    if (!result.empty())
        root = std::move(result.front());
    else
        root.reset();
}

// Вспомогательная функция для построения дерева
std::vector<std::unique_ptr<SchemaTree::Node>> SchemaTree::buildTreeHelper(const QString& expr)
{
    std::vector<std::unique_ptr<Node>> nodes;
    QString expression = expr.trimmed();
    expression.remove(' ');

    if (expression.isEmpty())
        return nodes;

    int depth = 0;
    std::vector<int> opPositions;
    std::vector<QChar> opChars;

    for (int i = 0; i < expression.size(); ++i) {
        QChar c = expression[i];
        if (c == '(') depth++;
        else if (c == ')') depth--;
        else if (depth == 0 && (c == '&' || c == '|'|| c == '^')) {
            opPositions.push_back(i);
            opChars.push_back(c);
        }
    }

    if (opPositions.empty()) {
        if (expression.startsWith('!')) {
            auto notNode = std::make_unique<Node>(NodeType::NOT, "!");
            QString inside = expression.mid(1);
            if (inside.startsWith('(') && inside.endsWith(')'))
                inside = inside.mid(1, inside.length() - 2);
            notNode->children = buildTreeHelper(inside);
            nodes.push_back(std::move(notNode));
        }
        else if (expression.startsWith('(') && expression.endsWith(')')) {
            nodes = buildTreeHelper(expression.mid(1, expression.length() - 2));
        }
        else {
            nodes.push_back(std::make_unique<Node>(NodeType::VAR, expression));
        }
        return nodes;
    }

    QChar mainOp = opChars.front();

    std::vector<QString> parts;
    int lastPos = 0;
    for (int pos : opPositions) {
        parts.push_back(expression.mid(lastPos, pos - lastPos));
        lastPos = pos + 1;
    }
    parts.push_back(expression.mid(lastPos));

    auto opNode = std::make_unique<Node>(NodeType::OP, QString(mainOp));

    for (auto &part : parts) {
        auto children = buildTreeHelper(part);
        for (auto &ch : children)
            opNode->children.push_back(std::move(ch));
    }

    nodes.push_back(std::move(opNode));
    return nodes;
}

// Печать дерева в консоль
void SchemaTree::printTree() const {
    if (!root) {
        qDebug().noquote() << "(пустое дерево)";
        return;
    }
    printNode(root.get(), "", true);
}

// Рекурсивная печать одного узла
void SchemaTree::printNode(const Node* node, const QString& prefix, bool isLast) const {
    if (!node) return;

    QString line = prefix;
    if (!prefix.isEmpty()) {
        line += isLast ? "└─ " : "├─ ";
    }

    QString typeStr;
    switch (node->type) {
    case NodeType::VAR: typeStr = "VAR"; break;
    case NodeType::OP:  typeStr = "OP";  break;
    case NodeType::NOT: typeStr = "NOT"; break;
    }

    qDebug().noquote() << line + node->value + " (" + typeStr + ")";

    QString childPrefix = prefix + (isLast ? "   " : "│  ");

    for (size_t i = 0; i < node->children.size(); ++i) {
        bool last = (i == node->children.size() - 1);
        printNode(node->children[i].get(), childPrefix, last);
    }
}

// Получить количество элементов типа VAR
int SchemaTree::countVarNodes(const std::unique_ptr<Node>& node) const
{
    if (!node) return 0;

    int count = 0;

    if (node->type == NodeType::VAR)
        ++count;

    for (const auto& child : node->children)
        count += countVarNodes(child);

    return count;
}

