#pragma once
#include <QString>

// For all those blasphemous global variables that are going to bring down society.

// We make these available globally because UI button text uses them and so does extraction in scene function.
// Want to ensure that if the text lead in is changed, it doesn't need to be changed anywhere else.
const QString configBtnTextLeadPuzzleType = "Puzzle Type: ";
const QString configBtnTextLeadPuzzleMultiplier = "Puzzle Pieces: ";