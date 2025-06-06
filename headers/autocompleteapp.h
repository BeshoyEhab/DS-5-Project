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
#include <QGridLayout>

class InputField;
class QLabel;

class AutoCompleteApp : public QMainWindow {
    Q_OBJECT

public:
    Trie* trie;
    explicit AutoCompleteApp(Trie* t,QWidget *parent = nullptr);
    bool useBFS = false;
    int maxSuggestions = 4;
    bool useFreq = true;
    bool highlight = true;

signals:
    void aboutToClose();
    void suggestionsVisibilityChanged(bool visible);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    QGridLayout *suggestionFlowLayout;
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

    void onSettingsChanged(bool bfs, int maxSug, bool usefreq, bool highlightFirst);
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
