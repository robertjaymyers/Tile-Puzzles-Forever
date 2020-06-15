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

#include "TilingPuzzleQt.h"

TilingPuzzleQt::TilingPuzzleQt(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	setWindowState(Qt::WindowMaximized);

	setStyleSheet("QMainWindow, QGraphicsView, QGroupBox { background: #FFFFFF; border: none; }");

	ui.centralWidget->setLayout(layout.get());
	layout.get()->setMargin(10);
	layout.get()->addWidget(puzzleDisplay.get(), 0, 0);
	puzzleDisplay.get()->show();

	layout.get()->addWidget(uiGroup.get(), 1, 0);
	uiGroup.get()->setLayout(uiLayout.get());

	configBtnPuzzlePath.get()->setText("Load New Puzzle Set From...");
	configBtnPuzzlePath.get()->setFocusPolicy(Qt::NoFocus);
	configBtnPuzzlePath.get()->setMaximumWidth(200);

	configBtnPuzzleType.get()->addItem(tr("Puzzle Type: "));
	configBtnPuzzleType.get()->addItem(tr("Puzzle Type: Rearrangement"));
	configBtnPuzzleType.get()->addItem(tr("Puzzle Type: Sliding"));
	configBtnPuzzleType.get()->setFocusPolicy(Qt::NoFocus);
	configBtnPuzzleType.get()->setMaximumWidth(200);
	configBtnPuzzleType.get()->setCurrentIndex(0);

	configBtnPuzzleMultiplier.get()->addItem(tr("Puzzle Pieces: "));
	configBtnPuzzleMultiplier.get()->addItem(tr("Puzzle Pieces: 4"));
	configBtnPuzzleMultiplier.get()->addItem(tr("Puzzle Pieces: 9"));
	configBtnPuzzleMultiplier.get()->addItem(tr("Puzzle Pieces: 16"));
	configBtnPuzzleMultiplier.get()->addItem(tr("Puzzle Pieces: 25"));
	configBtnPuzzleMultiplier.get()->addItem(tr("Puzzle Pieces: 36"));
	configBtnPuzzleMultiplier.get()->addItem(tr("Puzzle Pieces: 49"));
	configBtnPuzzleMultiplier.get()->addItem(tr("Puzzle Pieces: 64"));
	configBtnPuzzleMultiplier.get()->addItem(tr("Puzzle Pieces: 81"));
	configBtnPuzzleMultiplier.get()->addItem(tr("Puzzle Pieces: 100"));
	configBtnPuzzleMultiplier.get()->setFocusPolicy(Qt::NoFocus);
	configBtnPuzzleMultiplier.get()->setMaximumWidth(200);
	configBtnPuzzleMultiplier.get()->setCurrentIndex(0);

	configBtnApplyChanges.get()->setText("Apply Changes");
	configBtnApplyChanges.get()->setToolTip("This will remove the current puzzle.");
	configBtnApplyChanges.get()->setFocusPolicy(Qt::NoFocus);
	configBtnApplyChanges.get()->setMaximumWidth(200);
	configBtnApplyChanges.get()->setVisible(false);

	uiLayout.get()->setAlignment(Qt::AlignLeft);
	uiLayout.get()->addWidget(configBtnPuzzlePath.get(), 0);
	uiLayout.get()->addWidget(configBtnPuzzleType.get(), 1);
	uiLayout.get()->addWidget(configBtnPuzzleMultiplier.get(), 2);
	uiLayout.get()->addWidget(configBtnApplyChanges.get(), 3);

	connect(configBtnPuzzlePath.get(), &QPushButton::clicked, this, [=]() {
		QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Images Folder"), uiPuzzlePathLastOpened, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
		if (!dir.isEmpty())
		{
			puzzleDisplay.get()->scene.get()->setPuzzleListLoadNew(dir);
			configBtnApplyChanges.get()->setVisible(true);
			uiPuzzlePathLastOpened = dir;
		}
	});

	connect(configBtnPuzzleType.get(), QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
		if (configBtnPuzzleType.get()->currentIndex() > 0)
		{
			puzzleDisplay.get()->scene.get()->setPuzzleType(configBtnPuzzleType.get()->itemText(index));
			configBtnApplyChanges.get()->setVisible(true);
		}
	});

	connect(configBtnPuzzleMultiplier.get(), QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
		if (configBtnPuzzleMultiplier.get()->currentIndex() > 0)
		{
			puzzleDisplay.get()->scene.get()->setPuzzleMultipler(configBtnPuzzleMultiplier.get()->itemText(index));
			configBtnApplyChanges.get()->setVisible(true);
		}
	});

	connect(configBtnApplyChanges.get(), &QPushButton::clicked, this, [=]() {
		puzzleDisplay.get()->scene.get()->setPuzzleApplyChanges();
		configBtnApplyChanges.get()->setVisible(false);
	});
}
