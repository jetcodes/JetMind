/**
* Copyright (c) 2012 JetMind. All rights reserved.
*/

#ifndef PRODRAW_CAPPLICATION_H
#define PRODRAW_CAPPLICATION_H

#include <QtGui>
#include <QApplication>

class CWorkbench;

class CApplication : public QApplication
{
    Q_OBJECT

public:
    CApplication(int argc, char **argv);
    virtual ~CApplication();

    void setWorkbench(CWorkbench *bench);

protected:
    bool event(QEvent *evt);

private:
    CApplication();
    Q_DISABLE_COPY(CApplication)

    CWorkbench			*m_bench;

}; // End of class CApplication


#endif // PRODRAW_CAPPLICATION_H
