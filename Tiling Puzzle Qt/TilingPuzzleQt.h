/*
This file is part of Tile Puzzles Forever.
	Tile Puzzles Forever is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	Tile Puzzles Forever is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with Tile Puzzles Forever.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TilingPuzzleQt.h"
#include "PuzzleDisplay.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QComboBox>
#include <QFileDialog>
#include <QShortcut>

class TilingPuzzleQt : public QMainWindow
{
    Q_OBJECT

public:
    TilingPuzzleQt(QWidget *parent = Q_NULLPTR);

private:
    Ui::TilingPuzzleQtClass ui;

	std::unique_ptr<QShortcut> shortcutFullscreenExit = std::make_unique<QShortcut>(QKeySequence(tr("ESC", "Exit Fullscreen")), this);
	enum class PreFullscreenWindowState { NORMAL, MAXIMIZED };
	PreFullscreenWindowState preFullscreenWindowState = PreFullscreenWindowState::NORMAL;
	bool preFullscreenVisibilityStateForBtnApplyChanges = false;

	// Note: This has no relationship with the directory path in the scene.
	// It's only used to store what directory the UI btn last opened from, for convenience.
	QString uiPuzzlePathLastOpened = QCoreApplication::applicationDirPath();

	std::unique_ptr<PuzzleDisplay> puzzleDisplay = std::make_unique<PuzzleDisplay>(this);
	std::unique_ptr<QGridLayout> layout = std::make_unique<QGridLayout>();

	std::unique_ptr<QGroupBox> uiGroup = std::make_unique<QGroupBox>();
	std::unique_ptr<QHBoxLayout> uiLayout = std::make_unique<QHBoxLayout>();

	std::unique_ptr<QPushButton> configBtnPuzzlePath = std::make_unique<QPushButton>();
	std::unique_ptr<QComboBox> configBtnPuzzleType = std::make_unique<QComboBox>();
	std::unique_ptr<QComboBox> configBtnPuzzleMultiplier = std::make_unique<QComboBox>();

	std::unique_ptr<QPushButton> configBtnApplyChanges = std::make_unique<QPushButton>();

	std::unique_ptr<QPushButton> configBtnHideUi = std::make_unique<QPushButton>();

	void enterFullscreen();
	void exitFullscreen();
};
