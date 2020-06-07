#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TilingPuzzleQt.h"
#include "PuzzleDisplay.h"
#include <QGridLayout>

class TilingPuzzleQt : public QMainWindow
{
    Q_OBJECT

public:
    TilingPuzzleQt(QWidget *parent = Q_NULLPTR);

private:
    Ui::TilingPuzzleQtClass ui;

	std::unique_ptr<PuzzleDisplay> puzzleDisplay = std::make_unique<PuzzleDisplay>(this);
	std::unique_ptr<QGridLayout> layout = std::make_unique<QGridLayout>();
};
