#include "../headers/autocompleteapp.h"
#include "../headers/inputfield.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRegularExpression>
#include <QStyle>
#include <QFile>
#include <QSizePolicy>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>
#include <QMessageBox>
#include <QDir>
#include <string>
#include "vector"
#include <QTimer>
using namespace std;



AutoCompleteApp::AutoCompleteApp(Trie *r,QWidget *parent): QMainWindow(parent), selectedIndex(-1), isHoveringSuggestion(false)
{
    t=r;

    QString baseDir = QCoreApplication::applicationDirPath();
    QString srcPath = QDir(baseDir + "/../../src").absolutePath();
    QFile styleFile(srcPath+"/Style.css");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        setStyleSheet(styleSheet);
        styleFile.close();
    }

    setupUI();
    //setupAutocomplete();
    resize(800, 600);
    setWindowTitle("Fast Writer Pro");

    // Initialize timer
    searchDelayTimer = new QTimer(this);
    searchDelayTimer->setSingleShot(true);
    searchDelayTimer->setInterval(50); // 50ms delay
    connect(searchDelayTimer, &QTimer::timeout, this, &AutoCompleteApp::updateSuggestions);
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
    // connect(inputField, &QTextEdit::textChanged,this, &AutoCompleteApp::updateUI);
    connect(inputField, &QTextEdit::textChanged, this, [this]() {
        updateInputHeight();
        searchDelayTimer->stop();  // Reset timer on new input
        searchDelayTimer->start();
    });
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
                "background-color:#e1e1e1; color:rgb(27, 27, 27) ; border-radius: 10px;" :
                "background-color:#262626; color: rgba(255, 255, 255, 0.9); border-radius: 10px;"
            );
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
        fadeAnimation->setDuration(1000);
        fadeAnimation->setStartValue(0.0);
        fadeAnimation->setEndValue(1.150);
        fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);
        connect(fadeAnimation, &QPropertyAnimation::finished, fadeAnimation, &QPropertyAnimation::deleteLater);
        fadeAnimation->start();
    }
}

QString currentWord;

void AutoCompleteApp::updateSuggestions() {
    if(!t) return;
    clearSelection();
    suggestionButtons.clear();
    currentWord = getCurrentWord();

    // fade out animation
    bool isInputEmpty = inputField->toPlainText().trimmed().isEmpty();

    if(isInputEmpty) {

        if (suggestionContainer->isVisible()) {

            QPropertyAnimation *fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);

            fadeAnimation->setDuration(200);
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
        return;
    }

    if(currentWord.isEmpty()) {
        // Clear buttons but keep container visible
        QLayoutItem* child;
        while ((child = suggestionContainer->layout()->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }
        return; // Exit early - no need to process empty word
    }



    // تنظيف الـ layout القديم
    QLayoutItem* child;

    while ((child = suggestionContainer->layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QString baseWord = currentWord.toLower();
    bool capitalize = currentWord.length() > 0 && currentWord[0].isUpper();
    bool allCaps = currentWord == currentWord.toUpper();

    t->printSuggestions(baseWord.toStdString());
    vector<string>& sugs = t->V;

    if(!sugs.empty()) {
        QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(suggestionContainer->layout());
        layout->addStretch();

        for(const auto &it : sugs) {
            QString suggestion = QString::fromStdString(it);

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
            int totalWidth = textWidth + 50;  // 16px + 5px padding on each side
            btn->setMinimumWidth(totalWidth);
            btn->installEventFilter(this);
            connect(btn, &QPushButton::clicked, [this, displayText]() {
                replaceCurrentWord(displayText);


            });

            layout->addWidget(btn);
            suggestionButtons.append(btn);
        }

        layout->addStretch();
        showSuggestions();
    }
}


void AutoCompleteApp::replaceCurrentWord(const QString &replacement)
{
    t->increaseF(replacement.toLower().toStdString());

    QTextCursor cursor = inputField->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    cursor.insertText(replacement + " ");
    inputField->setFocus();
}

void AutoCompleteApp::handleNavigationKeys(QKeyEvent *event)
{
    QString currentWord = getCurrentWord();

    if (suggestionButtons.isEmpty()) {
        event->ignore();
        return;
    }

    switch(event->key()) {
    case Qt::Key_Tab:
        if (!suggestionButtons.isEmpty()) {
            if(event->modifiers() & Qt::ShiftModifier) {
                selectPrevious();
            } else {
                selectNext();
            }
            event->accept();
        }
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (selectedIndex != -1 || isHoveringSuggestion && !currentWord.isEmpty()) {
            string wordLower = currentWord.toLower().toStdString();
            if (t->contain(wordLower)) {
                t->increaseF(wordLower);
            } else {
                t->autosave(wordLower);
            }
            activateSelected();
            event->accept();
        } else {
            event->ignore(); // Allow normal Enter behavior
            string wordLower = currentWord.toLower().toStdString();
            if (t->contain(wordLower)) {
                t->increaseF(wordLower);
            } else {
                t->autosave(wordLower);
            }
            activateSelected();
        }
        break;
    case Qt::Key_Space:
        if (!currentWord.isEmpty()) {
            string wordLower = currentWord.toLower().toStdString();
            if (t->contain(wordLower)) {
                t->increaseF(wordLower);
            } else {
                t->autosave(wordLower);
            }
        }
        event->accept();
        break;
    default:
        event->ignore();
    }
}

void AutoCompleteApp::closeEvent(QCloseEvent *event) {
    // Create a message box with custom buttons
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Exit Confirmation");
    msgBox.setText("You have unsaved changes. What would you like to do?");

    // Add custom buttons
    QPushButton *saveButton = msgBox.addButton("Save", QMessageBox::AcceptRole);
    QPushButton *discardButton = msgBox.addButton("Discard", QMessageBox::DestructiveRole);
    QPushButton *cancelButton = msgBox.addButton("Cancel", QMessageBox::RejectRole);

    msgBox.exec();  // Show the dialog

    // Determine which button was clicked
    if (msgBox.clickedButton() == saveButton) {    // Call your save function
        event->accept();  // Close the window
    } else if (msgBox.clickedButton() == discardButton) {
        event->accept();  // Close without saving
    } else if (msgBox.clickedButton() == cancelButton) {
        event->ignore();   // Cancel closing
    }
}
