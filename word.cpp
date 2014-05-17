/*
    Copyright 2014 Vanniktech - Niklas Baudy

    This file is part of SpeedReader.

    SpeedReader is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    SpeedReader is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpeedReader. If not, see <http://www.gnu.org/licenses/>.
*/

#include "word.h"

Word::Word() {
}

Word::Word(QString word, bool stopWord, bool breakWord, int delayWord) {
    this->word = word;
    this->stopWord = stopWord;
    this->breakWord = breakWord;
    this->delayWord = delayWord;
}

bool Word::equals(Word word) {
    return word.word == this->word && word.stopWord == this->stopWord && word.breakWord == this->breakWord && word.delayWord == this->delayWord;
}
