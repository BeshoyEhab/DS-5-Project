#include "autocompleteapp.h"
#include "inputfield.h"
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
#include <QMenuBar>
#include <settingsdialog.h>
#include <string>
#include "vector"
#include <QTimer>
#include <QScrollArea>
using namespace std;

AutoCompleteApp::AutoCompleteApp(Trie *t, QWidget *parent)
    : QMainWindow(parent)
    , selectedIndex(-1)
    , isHoveringSuggestion(false)
{
    trie = t;

    QString baseDir = QCoreApplication::applicationDirPath();
    QString srcPath = QDir(baseDir + "/../../src").absolutePath();
    QFile styleFile(srcPath + "/Style.css");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        setStyleSheet(styleSheet);
        styleFile.close();
    }


    setupUI();
    resize(800, 600);
    setWindowTitle("Fast Writer Pro");

    // Initialize timer
    searchDelayTimer = new QTimer(this);
    searchDelayTimer->setSingleShot(true);
    searchDelayTimer->setInterval(50);
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

    mainLayout->addStretch(1);
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

    QWidget *suggestionsWrapper = new QWidget();
    QVBoxLayout *suggestionsWrapperLayout = new QVBoxLayout(suggestionsWrapper);
    suggestionsWrapperLayout->setContentsMargins(0, 0, 0, 0);
    suggestionsWrapperLayout->setSpacing(0);

    QWidget *suggestionsSpacer = new QWidget();
    suggestionsSpacer->setFixedHeight(36);
    suggestionsSpacer->setVisible(false);
    suggestionsWrapperLayout->addWidget(suggestionsSpacer);

    suggestionContainer = new QWidget();
    suggestionContainer->setObjectName("suggestionContainer");
    suggestionContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    suggestionFlowLayout = new QGridLayout(suggestionContainer);
    suggestionFlowLayout->setContentsMargins(10, 10, 10, 10);
    suggestionFlowLayout->setHorizontalSpacing(8);
    suggestionFlowLayout->setVerticalSpacing(8);
    suggestionFlowLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    contentLayout->addWidget(suggestionContainer);

    opacityEffect = new QGraphicsOpacityEffect(suggestionContainer);
    opacityEffect->setOpacity(0.0);
    suggestionContainer->setGraphicsEffect(opacityEffect);

    slideAnimation = new QPropertyAnimation(suggestionContainer, "pos");
    slideAnimation->setDuration(150);

    currentAnimGroup = nullptr;

    suggestionsWrapperLayout->addWidget(suggestionContainer);

    inputField = new InputField();
    inputField->setObjectName("inputField");
    inputField->setPlaceholderText("Start typing Here");

    contentLayout->addWidget(suggestionsWrapper);
    contentLayout->addWidget(inputField);

    suggestionContainer->hide();

    QHBoxLayout *horizontalWrapper = new QHBoxLayout();
    horizontalWrapper->addStretch();
    horizontalWrapper->addWidget(contentWidget, 8);
    horizontalWrapper->addStretch();

    mainLayout->addLayout(horizontalWrapper);

    mainLayout->addStretch(1);

    connect(this,
            &AutoCompleteApp::suggestionsVisibilityChanged,
            suggestionsSpacer,
            &QWidget::setVisible);

    setCentralWidget(centralWidget);
    connect(inputField,
            &InputField::navigationKeyPressed,
            this,
            &AutoCompleteApp::handleNavigationKeys);
    connect(inputField, &QTextEdit::textChanged, this, [this]() {
        updateInputHeight();
        searchDelayTimer->stop();
        searchDelayTimer->start();
    });
    QMenuBar *menuBar = new QMenuBar();
    QMenu *settingsMenu = menuBar->addMenu("Settings");
    QAction *prefsAction = settingsMenu->addAction("Preferences...");
    connect(prefsAction, &QAction::triggered, [this]() {
        SettingsDialog dlg(trie, this);
        connect(&dlg, &SettingsDialog::settingsChanged, this, &AutoCompleteApp::onSettingsChanged);
        dlg.exec();
    });
    this->setMenuBar(menuBar);
}

void AutoCompleteApp::onSettingsChanged(bool bfs, int maxSug, bool usefreq, bool highlightFirst)
{
    useBFS = bfs;
    maxSuggestions = maxSug;
    useFreq = usefreq;
    highlight = highlightFirst;
    updateSuggestions();
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
    for (QPushButton *btn : suggestionButtons) {
        btn->setProperty("selected", false);
        btn->style()->polish(btn);
    }
}

void AutoCompleteApp::selectNext()
{
    if (suggestionButtons.isEmpty())
        return;
    selectedIndex = (selectedIndex + 1) % suggestionButtons.size();
    updateSelection();
}

void AutoCompleteApp::selectPrevious()
{
    if (suggestionButtons.isEmpty())
        return;
    selectedIndex = (selectedIndex - 1 + suggestionButtons.size()) % suggestionButtons.size();
    updateSelection();
}

void AutoCompleteApp::updateSelection()
{
    for (int i = 0; i < suggestionButtons.size(); i++) {
        bool selected = (i == selectedIndex);
        suggestionButtons[i]->setStyleSheet(
            selected ? "background-color:#e1e1e1; color:rgb(27, 27, 27) ; border-radius: 10px;"
                     : "background-color:#262626; color: rgba(255, 255, 255, 0.9); border-radius: "
                       "10px;");
    }
}

void AutoCompleteApp::activateSelected()
{
    if (selectedIndex >= 0 && selectedIndex < suggestionButtons.size()) {
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
        connect(fadeAnimation,
                &QPropertyAnimation::finished,
                fadeAnimation,
                &QPropertyAnimation::deleteLater);
        fadeAnimation->start();
    }
}




QString currentWord;
void AutoCompleteApp::updateSuggestions()
{
    if (!trie)
        return;
    clearSelection();
    suggestionButtons.clear();
    currentWord = getCurrentWord();

    bool isInputEmpty = inputField->toPlainText().trimmed().isEmpty();

    if (isInputEmpty) {
        if (suggestionContainer->isVisible()) {
            QPropertyAnimation *fadeAnimation = new QPropertyAnimation(opacityEffect,
                                                                       "opacity",
                                                                       this);

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

    if (currentWord.isEmpty()) {
        QLayoutItem *child;
        while ((child = suggestionContainer->layout()->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }
        return;
    }

    QLayoutItem *child;

    while ((child = suggestionContainer->layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }


    QString baseWord = currentWord.toLower();
    bool capitalize = currentWord.length() > 0 && currentWord[0].isUpper();
    bool allCaps = currentWord == currentWord.toUpper();

    trie->generateSuggestions(baseWord.toStdString(), maxSuggestions, useBFS, useFreq);

    vector<string> &sugs = trie->suggestionsVector;
    if (!(std::find(sugs.begin(), sugs.end(), currentWord.toStdString()) != sugs.end())) {
        if (!sugs.empty())
            sugs.pop_back();
        sugs.insert(sugs.begin(), currentWord.toStdString());
    }

    if (!sugs.empty()) {
        int availableWidth = suggestionContainer->width() - 20;
        int maxButtonsPerRow = max(5, maxSuggestions/2);

        const int avgButtonWidth = 120;

        int row = 0;
        int col = 0;

        for (const auto &it : sugs) {
            QString suggestion = QString::fromStdString(it);

            QString displayText = suggestion;

            if (capitalize) {
                displayText = suggestion.left(1).toUpper() + suggestion.mid(1).toLower();
            } else if (allCaps) {
                displayText = suggestion.toUpper();
            } else {
                displayText = suggestion.toLower();
            }

            QPushButton *btn = new QPushButton(displayText);
            btn->setCursor(Qt::PointingHandCursor);
            btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            btn->setMinimumHeight(28);

            QFontMetrics fm(btn->font());
            int textWidth = fm.horizontalAdvance(displayText);
            int totalWidth = textWidth + 50;
            btn->setMinimumWidth(totalWidth);
            btn->installEventFilter(this);
            connect(btn, &QPushButton::clicked, [this, displayText]() {
                replaceCurrentWord(displayText);
            });

            suggestionFlowLayout->addWidget(btn, row, col);
            suggestionButtons.append(btn);

            col++;
            if (col >= maxButtonsPerRow) {
                col = 0;
                row++;
            }
        }

        showSuggestions();
    }
    if (!suggestionButtons.isEmpty() && highlight) {
        selectedIndex = 0;
        updateSelection();
    }
}

void AutoCompleteApp::replaceCurrentWord(const QString &replacement)
{
    string text = replacement.toLower().toStdString();
    if (trie->contain(text))
        trie->increaseFrequency(text);
    else
        trie->insert(text);

    QTextCursor cursor = inputField->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    cursor.insertText(replacement + " ");
    inputField->setFocus();
}



void AutoCompleteApp::handleNavigationKeys(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Tab:
        if (!suggestionButtons.isEmpty()) {
            if (event->modifiers() & Qt::ShiftModifier) {
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
            event->accept();
        } else {
            string wordLower = currentWord.toLower().toStdString();
            if (trie->contain(wordLower)) {
                trie->increaseFrequency(wordLower);
            } else {
                trie->autosave(wordLower);
            }
            event->ignore();
        }
        activateSelected();
        break;
    case Qt::Key_Colon:
    case Qt::Key_Comma:
    case Qt::Key_Semicolon:
    case Qt::Key_Period:
    case Qt::Key_Space:
        if (!currentWord.isEmpty()) {
            string wordLower = currentWord.toLower().toStdString();
            if (trie->contain(wordLower)) {
                trie->increaseFrequency(wordLower);
            } else {
                trie->autosave(wordLower);
            }
        }
        event->accept();
        break;
    default:
        event->ignore();
    }

}

void AutoCompleteApp::closeEvent(QCloseEvent *event) {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Exit Confirmation");
    msgBox.setText("Do you want to save before exiting?");

    QPushButton *saveButton = msgBox.addButton("Save", QMessageBox::AcceptRole);
    QPushButton *discardButton = msgBox.addButton("Discard", QMessageBox::DestructiveRole);
    QPushButton *cancelButton = msgBox.addButton("Cancel", QMessageBox::RejectRole);

    msgBox.exec();
    QSettings settings;

    if (msgBox.clickedButton() == saveButton) {
        emit aboutToClose();
        event->accept();
        settings.clear();
    }
    else if (msgBox.clickedButton() == discardButton) {
        event->accept();
        settings.clear();
    }
    else {
        event->ignore();
    }
}
