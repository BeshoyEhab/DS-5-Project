#include "inputfield.h"
#include <QFontMetrics>
#include <QTextDocument>

InputField::InputField(QWidget *parent) : QTextEdit(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setLineWrapMode(QTextEdit::WidgetWidth);
    setAcceptRichText(false);
    setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

    QFontMetrics fm(font());
    setMinimumHeight(fm.height() + 30);
    setMaximumHeight(fm.height() + 30);

    connect(this, &QTextEdit::textChanged, this, &InputField::adjustHeight);
}

void InputField::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit navigationKeyPressed(event);
        if (!event->isAccepted()) {
            QTextEdit::keyPressEvent(event);
        }
        return;
    }

    if (event->key() == Qt::Key_Tab) {
        emit navigationKeyPressed(event);
        if (!event->isAccepted()) {
            QTextEdit::keyPressEvent(event);
        }
        return;
    }

    if (event->key() == Qt::Key_Space) {
        emit navigationKeyPressed(event);
    }


    QTextEdit::keyPressEvent(event);
}

void InputField::adjustHeight()
{
    int docHeight = document()->size().height();
    QFontMetrics fm(font());
    int contentHeight = docHeight + 30;

    QWidget* window = this->window();
    int maxHeight = window ? int(window->height() * 0.7) : 400;

    int newHeight = qMin(maxHeight, qMax(fm.height() + 30, contentHeight));
    setMaximumHeight(newHeight);
}
