#ifndef AUTOCOMPLETEAPP_H
#define AUTOCOMPLETEAPP_H

#include <QMainWindow>
#include <QMap>
#include <QStringList>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QTimer>
#include "../trie.h"




class InputField;
class QLabel;

class AutoCompleteApp : public QMainWindow {
    Q_OBJECT

public:
    Trie* t  ;
    explicit AutoCompleteApp(Trie* t,QWidget *parent = nullptr);

signals:
    void suggestionsVisibilityChanged(bool visible);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    InputField *inputField;
    QWidget *suggestionContainer;
    QList<QPushButton *> suggestionButtons;
    int selectedIndex;
    QMap<QString, QStringList> wordDatabase;
    QLabel *titleLabel;
    QPropertyAnimation *slideAnimation;
    QGraphicsOpacityEffect *opacityEffect;
    QParallelAnimationGroup *currentAnimGroup;
    QTimer *searchDelayTimer;
    bool isHoveringSuggestion;

    void setupUI();
    void setupAutocomplete();
    void updateInputHeight();
    QString getCurrentWord();
    void clearSelection();
    void selectNext();
    void selectPrevious();
    void updateSelection();
    void activateSelected();
    void updateUI();
    void showSuggestions();
    void hideSuggestions();
    void updateSuggestions();
    void replaceCurrentWord(const QString &replacement);

private slots:
    void handleNavigationKeys(QKeyEvent *event);
};

#endif // AUTOCOMPLETEAPP_H 
