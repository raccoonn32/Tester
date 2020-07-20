#ifndef SAVERESULTS_H
#define SAVERESULTS_H

#include <QObject>

#include <QFile>
#include <QIODevice>
#include "backworkthread.h"

class saveResults : public QObject
{
    Q_OBJECT
public:
    explicit saveResults(QObject *parent = NULL);

    void saveType(BackWorkThread::eWorkType type);

signals:

public slots:
};

#endif // SAVERESULTS_H
