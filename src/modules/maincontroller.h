#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
/**
 * @brief The mainController class  for connections between other controllers
 */
class mainController : public QObject
{
    Q_OBJECT
public:
    explicit mainController(QObject *parent = nullptr);

signals:

public slots:
};

#endif // MAINCONTROLLER_H
