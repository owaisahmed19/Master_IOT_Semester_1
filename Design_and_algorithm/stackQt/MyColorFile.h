#ifndef MYCOLORFILE_H
#define MYCOLORFILE_H

#include <QPair>
#include <QString>
#include <QColor>

const QPair<QString, QColor> colors[10] = {
    qMakePair(QString("Crimson"), QColor(0xDC143C)),
    qMakePair(QString("Amber"), QColor(0xFFBF00)),
    qMakePair(QString("Lime"), QColor(0x32CD32)),
    qMakePair(QString("Teal"), QColor(0x008080)),
    qMakePair(QString("Cyan"), QColor(0x00FFFF)),
    qMakePair(QString("Violet"), QColor(0x8A2BE2)),
    qMakePair(QString("Salmon"), QColor(0xFA8072)),
    qMakePair(QString("Chocolate"), QColor(0xD2691E)),
    qMakePair(QString("Ivory"), QColor(0xFFFFF0)),
    qMakePair(QString("Fuchsia"), QColor(0xFF77FF)),
};
#endif // MYCOLORFILE_H
