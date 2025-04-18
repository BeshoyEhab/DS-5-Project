#include "autocompleteapp.h"
#include "inputfield.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRegularExpression>
#include <QStyle>
#include <QSizePolicy>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>

AutoCompleteApp::AutoCompleteApp(QWidget *parent)
    : QMainWindow(parent)
    , selectedIndex(-1)
{
    setStyleSheet(R"(
        QMainWindow {
            background-color: #0d1117;
        }
        QTextEdit {
            margin: 0;
            padding: 8px 15px;
            font-size: 18px;
            border: none;
            border-radius: 15px;
            min-height: 36px;
            color: #ffffff;
            background-color: #2d2d2d;
        }
        QTextEdit[placeholder="true"] {
            color: #484f58;
        }
        QPushButton {
            background-color: #21262d;
            color: rgba(255, 255, 255, 0.9);
            padding: 4px 12px;
            border: 1px solid #30363d;
            font-size: 14px;
            margin: 0;
            border-radius: 6px;
            height: 24px;
            line-height: 24px;
        }
        QPushButton:hover {
            background-color: #2f81f7;
            color: white;
            border: 1px solid #ffffff;
        }
        QLabel#separator {
            color:rgb(105, 106, 107);
            font-size: 14px;
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
            border-top-left-radius: 12px;
            border-top-right-radius: 12px;
            border-bottom-left-radius: 0px;
            border-bottom-right-radius: 0px;
            margin-left: 100px;
            margin-right: 100px;
            padding: 4px 12px;
            min-height: 36px;
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

    // Add stretch to push content to vertical center
    mainLayout->addStretch(1);

    // Main content container (input + suggestions)
    QWidget *contentWidget = new QWidget();
    contentWidget->setObjectName("inputContainer");
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(40, 0, 40, 0);
    contentLayout->setSpacing(0);

    // Title Label
    titleLabel = new QLabel("let me help you to write fast");
    titleLabel->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(titleLabel);
    contentLayout->addSpacing(20);

    // Create a container for suggestions that's always present
    QWidget *suggestionsWrapper = new QWidget();
    QVBoxLayout *suggestionsWrapperLayout = new QVBoxLayout(suggestionsWrapper);
    suggestionsWrapperLayout->setContentsMargins(0, 0, 0, 0);
    suggestionsWrapperLayout->setSpacing(0);

    // Create a fixed-size spacer widget to reserve space for suggestions
    QWidget *suggestionsSpacer = new QWidget();
    suggestionsSpacer->setFixedHeight(36); // Same height as suggestion container
    suggestionsSpacer->setVisible(false);
    suggestionsWrapperLayout->addWidget(suggestionsSpacer);

    // Suggestions Container with animation setup
    suggestionContainer = new QWidget();
    suggestionContainer->setObjectName("suggestionContainer");
    suggestionContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    QHBoxLayout *suggestionsLayout = new QHBoxLayout(suggestionContainer);
    suggestionsLayout->setContentsMargins(4, 4, 4, 4);
    suggestionsLayout->setSpacing(4);
    suggestionsLayout->setAlignment(Qt::AlignVCenter);

    contentLayout->addWidget(suggestionContainer);

    // Setup opacity effect
    opacityEffect = new QGraphicsOpacityEffect(suggestionContainer);
    opacityEffect->setOpacity(0.0);
    suggestionContainer->setGraphicsEffect(opacityEffect);

    // Setup slide animation
    slideAnimation = new QPropertyAnimation(suggestionContainer, "pos");
    slideAnimation->setDuration(150);

    // Initialize animation group pointer
    currentAnimGroup = nullptr;

    // Add suggestion container to wrapper
    suggestionsWrapperLayout->addWidget(suggestionContainer);

    // Input Field
    inputField = new InputField();
    inputField->setObjectName("inputField");
    inputField->setPlaceholderText("Start typing Here");

    // Create the main content stack in correct order
    contentLayout->addWidget(suggestionsWrapper);
    contentLayout->addWidget(inputField);

    // Hide suggestions initially
    suggestionContainer->hide();

    // Center the content widget horizontally
    QHBoxLayout *horizontalWrapper = new QHBoxLayout();
    horizontalWrapper->addStretch();
    horizontalWrapper->addWidget(contentWidget, 8);
    horizontalWrapper->addStretch();

    mainLayout->addLayout(horizontalWrapper);

    // Add stretch after content for vertical centering
    mainLayout->addStretch(1);

    // Update showSuggestions and hideSuggestions methods to handle the spacer
    connect(this, &AutoCompleteApp::suggestionsVisibilityChanged, 
            suggestionsSpacer, &QWidget::setVisible);

    setCentralWidget(centralWidget);
    connect(inputField, &InputField::navigationKeyPressed,
            this, &AutoCompleteApp::handleNavigationKeys);
    connect(inputField, &QTextEdit::textChanged,
            this, &AutoCompleteApp::updateUI);
}

void AutoCompleteApp::setupAutocomplete()
{
    wordDatabase["alex"] = QStringList() << "Alex" << "Alexa" << "Alexis" << "Alexander" << "Alexandra";
    wordDatabase["arg"] = QStringList() << "Argon" << "Argo" << "Argue" << "Argument" << "Argueable";
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
                                                "background-color: #2f81f7; color: #ffffff; border-radius: 8px;" :
                                                "background-color: #21262d; color: rgba(255, 255, 255, 0.9); border-radius: 8px;");
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
        emit suggestionsVisibilityChanged(true);
        opacityEffect->setOpacity(0.0);

        QPropertyAnimation *fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
        fadeAnimation->setDuration(150);
        fadeAnimation->setStartValue(0.0);
        fadeAnimation->setEndValue(1.0);
        fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);
        connect(fadeAnimation, &QPropertyAnimation::finished, fadeAnimation, &QPropertyAnimation::deleteLater);
        fadeAnimation->start();
    }
}

void AutoCompleteApp::hideSuggestions()
{
    if (suggestionContainer->isVisible()) {
        QPropertyAnimation *fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
        fadeAnimation->setDuration(100);
        fadeAnimation->setStartValue(opacityEffect->opacity());
        fadeAnimation->setEndValue(0.0);
        fadeAnimation->setEasingCurve(QEasingCurve::InCubic);
        
        connect(fadeAnimation, &QPropertyAnimation::finished, [this, fadeAnimation]() {
            suggestionContainer->hide();
            emit suggestionsVisibilityChanged(false);
            fadeAnimation->deleteLater();
        });
        
        fadeAnimation->start();
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
            btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            btn->setMinimumHeight(26);

            // Calculate the width needed for the text
            QFontMetrics fm(btn->font());
            int textWidth = fm.horizontalAdvance(displayText);
            // Add padding (16px on each side from the stylesheet + 5px extra on each side)
            int totalWidth = textWidth + 42;  // 16px + 5px padding on each side
            btn->setMinimumWidth(totalWidth);

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
                separator->setFixedWidth(8); // Reduced separator width
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
