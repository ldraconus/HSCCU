#pragma once

#include "sheet.h"

#define NOTES     Sheet::ref().getUi()->notes

template <typename T>
constexpr auto DBG(T&& x) {
    NOTES->setPlainText(NOTES->toPlainText() + "\n" + x);
}
constexpr auto DBGVar(auto x) {
// NOLINTNEXTLINE
#define y(x) #x + QString(": %1")
    DBG(QString(y(x)).arg(x));
}

