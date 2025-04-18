#include "autocompleteapp.h"
#include "inputfield.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRegularExpression>
#include <QStyle>
#include <QSizePolicy>

AutoCompleteApp::AutoCompleteApp(QWidget *parent)
    : QMainWindow(parent)
    , selectedIndex(-1)
{
    setStyleSheet(R"(
        QMainWindow {
            background-color: #1e1e1e;
        }
        QTextEdit {
            margin: 0;
            padding: 15px;
            font-size: 18px;
            border: none;
            border-radius: 15px;
            min-height: 60px;
            color: #ffffff;
            background-color: #2d2d2d;
        }
        QTextEdit[placeholder="true"] {
            color: #666666;
        }
        QPushButton {
            background-color: transparent;
            color: rgba(255, 255, 255, 0.9);
            padding: 8px 16px;
            border: 1px solid transparent;
            font-size: 18px;
            margin: 0;
            border-radius: 8px;
        }
        QPushButton:hover {
            background-color: #4b89bf;
            color: white;
            border: 1px solid #ffffff;
        }
        QLabel#separator {
            color: rgba(255, 255, 255, 0.2);
            font-size: 18px;
            margin: 0;
            padding: 0 4px;
        }
        QLabel {
            color: #ffffff;
            font-size: 32px;
            font-weight: normal;
        }
        QWidget#suggestionContainer {
            background-color: #3d3d3d;
            border-radius: 12px;
            margin-bottom: -15px;
            margin-left: 40px;
            margin-right: 40px;
            padding: 8px 12px;
        }
        QWidget#inputContainer {
            margin: 0;
            padding: 0;
        }
    )");

    setupUI();
    setupAutocomplete();
    resize(800, 600);
    setWindowTitle("Fast Writer Pro");
}

void AutoCompleteApp::keyPressEvent(QKeyEvent *event)
{
    handleNavigationKeys(event);
}

void AutoCompleteApp::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Add stretch before content to push it down
    mainLayout->addStretch();

    // Title Label with proper spacing
    titleLabel = new QLabel("let me help you to write fast");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel, 0, Qt::AlignHCenter);
    mainLayout->addSpacing(20);

    // Main content container (input + suggestions)
    QWidget *contentWidget = new QWidget();
    contentWidget->setObjectName("inputContainer");
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(40, 0, 40, 0);
    contentLayout->setSpacing(0);

    // Suggestions Container with animation setup
    suggestionContainer = new QWidget();
    suggestionContainer->setObjectName("suggestionContainer");
    suggestionContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    // Setup opacity effect
    opacityEffect = new QGraphicsOpacityEffect(suggestionContainer);
    opacityEffect->setOpacity(0.0);
    suggestionContainer->setGraphicsEffect(opacityEffect);

    // Setup slide animation
    slideAnimation = new QPropertyAnimation(suggestionContainer, "pos");
    slideAnimation->setDuration(150);

    // Initialize animation group pointer
    currentAnimGroup = nullptr;

    QHBoxLayout *suggestionsLayout = new QHBoxLayout(suggestionContainer);
    suggestionsLayout->setContentsMargins(0, 0, 0, 0);
    suggestionsLayout->setSpacing(2);

    contentLayout->addWidget(suggestionContainer);

    // Hide suggestions initially
    suggestionContainer->hide();

    // Input Field
    inputField = new InputField();
    inputField->setObjectName("inputField");
    inputField->setPlaceholderText("Start typing Here");
    contentLayout->addWidget(inputField);

    // Center the content widget horizontally and vertically
    QHBoxLayout *horizontalWrapper = new QHBoxLayout();
    horizontalWrapper->addStretch();
    horizontalWrapper->addWidget(contentWidget, 8);
    horizontalWrapper->addStretch();

    mainLayout->addLayout(horizontalWrapper);

    // Add stretch after content to push it up
    mainLayout->addStretch();

    setCentralWidget(centralWidget);
    connect(inputField, &InputField::navigationKeyPressed,
            this, &AutoCompleteApp::handleNavigationKeys);
    connect(inputField, &QTextEdit::textChanged,
            this, &AutoCompleteApp::updateUI);
}

void AutoCompleteApp::setupAutocomplete()
{
    wordDatabase["alex"] = QStringList() << "Alex" << "Alexa" << "Alexis" << "Alexander";
    wordDatabase["arg"] = QStringList() << "Argon" << "Argo" << "Argue" << "Argument";
}

void AutoCompleteApp::updateInputHeight()
{
    int docHeight = inputField->document()->size().height();
    inputField->setMinimumHeight(qMin(qMax(60, int(docHeight) + 30), int(this->height() * 0.7)));
}

QString AutoCompleteApp::getCurrentWord()
{
    QTextCursor cursor = inputField->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    return cursor.selectedText().replace(QRegularExpression("\\W+"), "");
}

void AutoCompleteApp::clearSelection()
{
    selectedIndex = -1;
    for(QPushButton *btn : suggestionButtons) {
        btn->setProperty("selected", false);
        btn->style()->polish(btn);
    }
}

void AutoCompleteApp::selectNext()
{
    if(suggestionButtons.isEmpty()) return;
    selectedIndex = (selectedIndex + 1) % suggestionButtons.size();
    updateSelection();
}

void AutoCompleteApp::selectPrevious()
{
    if(suggestionButtons.isEmpty()) return;
    selectedIndex = (selectedIndex - 1 + suggestionButtons.size()) % suggestionButtons.size();
    updateSelection();
}

void AutoCompleteApp::updateSelection()
{
    for(int i = 0; i < suggestionButtons.size(); i++) {
        bool selected = (i == selectedIndex);
        suggestionButtons[i]->setStyleSheet(selected ?
                                                "background-color: #4b89bf; color: #ffffff; border-radius: 8px;" :
                                                "background-color: transparent; color: rgba(255, 255, 255, 0.9);");
    }
}

void AutoCompleteApp::activateSelected()
{
    if(selectedIndex >= 0 && selectedIndex < suggestionButtons.size()) {
        suggestionButtons[selectedIndex]->click();
    }
}

void AutoCompleteApp::updateUI()
{
    updateInputHeight();
    updateSuggestions();
}

void AutoCompleteApp::showSuggestions()
{
    if (!suggestionContainer->isVisible()) {
        suggestionContainer->show();

        // Reset position for slide animation
        QPoint startPos = suggestionContainer->pos();
        QPoint endPos = startPos;
        startPos.setY(startPos.y() + 20); // Start 20px below

        // Setup and start animations
        slideAnimation->setStartValue(startPos);
        slideAnimation->setEndValue(endPos);
        slideAnimation->setEasingCurve(QEasingCurve::OutCubic);

        QPropertyAnimation *fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity");
        fadeAnimation->setDuration(200);
        fadeAnimation->setStartValue(0.0);
        fadeAnimation->setEndValue(1.0);
        fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);

        slideAnimation->start();
        fadeAnimation->start();
    }
}

void AutoCompleteApp::hideSuggestions()
{
    if (suggestionContainer->isVisible()) {
        // Create new animations for hiding
        QPropertyAnimation *hideSlide = new QPropertyAnimation(suggestionContainer, "pos");
        QPropertyAnimation *hideFade = new QPropertyAnimation(opacityEffect, "opacity");

        // Setup slide animation
        QPoint startPos = suggestionContainer->pos();
        QPoint endPos = startPos;
        endPos.setY(startPos.y() + 20); // Slide 20px down

        hideSlide->setStartValue(startPos);
        hideSlide->setEndValue(endPos);
        hideSlide->setEasingCurve(QEasingCurve::InCubic);
        hideSlide->setDuration(150);

        // Setup fade animation
        hideFade->setDuration(150);
        hideFade->setStartValue(1.0);
        hideFade->setEndValue(0.0);
        hideFade->setEasingCurve(QEasingCurve::InCubic);

        // Create parallel animation group
        QParallelAnimationGroup *hideGroup = new QParallelAnimationGroup(this);
        hideGroup->addAnimation(hideSlide);
        hideGroup->addAnimation(hideFade);

        // Hide container and cleanup when animations finish
        connect(hideGroup, &QParallelAnimationGroup::finished, [this, hideGroup]() {
            suggestionContainer->hide();
            hideGroup->deleteLater();
        });

        hideGroup->start();
    }
}

void AutoCompleteApp::updateSuggestions()
{
    clearSelection();
    suggestionButtons.clear();
    QLayoutItem* child;
    while ((child = suggestionContainer->layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QString currentWord = getCurrentWord();
    if(currentWord.isEmpty()) {
        hideSuggestions();
        return;
    }

    QString baseWord = currentWord.toLower();
    bool capitalize = currentWord.length() > 0 && currentWord[0].isUpper();
    bool allCaps = currentWord == currentWord.toUpper();

    if(wordDatabase.contains(baseWord)) {
        QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(suggestionContainer->layout());
        layout->addStretch();

        QStringList suggestions = wordDatabase[baseWord].mid(0,4);
        for(int i = 0; i < suggestions.size(); ++i) {
            QString suggestion = suggestions[i];
            QString displayText = suggestion;

            if(capitalize) {
                displayText = suggestion.left(1).toUpper() + suggestion.mid(1).toLower();
            } else if(allCaps) {
                displayText = suggestion.toUpper();
            } else {
                displayText = suggestion.toLower();
            }

            QPushButton *btn = new QPushButton(displayText);
            btn->setCursor(Qt::PointingHandCursor);
            btn->setFixedHeight(36);

            // Calculate the width needed for the text
            QFontMetrics fm(btn->font());
            int textWidth = fm.horizontalAdvance(displayText);
            // Add padding (16px on each side from the stylesheet + 5px extra on each side)
            int totalWidth = textWidth + 42;  // 16px + 5px padding on each side
            btn->setFixedWidth(totalWidth);

            connect(btn, &QPushButton::clicked, [this, displayText]() {
                replaceCurrentWord(displayText);
            });

            layout->addWidget(btn);
            suggestionButtons.append(btn);

            // Add separator if not the last item
            if(i < suggestions.size() - 1) {
                QLabel *separator = new QLabel("|");
                separator->setObjectName("separator");
                separator->setAlignment(Qt::AlignCenter);
                separator->setFixedWidth(10); // Fixed width for separator
                layout->addWidget(separator);
            }
        }

        layout->addStretch();

        if(!suggestionButtons.isEmpty()) {
            selectedIndex = 0;
            updateSelection();
            showSuggestions();
        }
    } else {
        hideSuggestions();
    }
}

void AutoCompleteApp::replaceCurrentWord(const QString &replacement)
{
    QTextCursor cursor = inputField->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    cursor.insertText(replacement + " ");
    inputField->setFocus();
}

void AutoCompleteApp::handleNavigationKeys(QKeyEvent *event)
{
    if (suggestionButtons.isEmpty()) {
        event->ignore();
        return;
    }

    switch(event->key()) {
    case Qt::Key_Tab:
        if(event->modifiers() & Qt::ShiftModifier) {
            selectPrevious();
        } else {
            selectNext();
        }
        event->accept();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        activateSelected();
        event->accept();
        break;
    default:
        event->ignore();
    }
} 
