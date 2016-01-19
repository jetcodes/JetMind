/**
* Copyright (c) 2012 JetMind. All rights reserved.
*/
#include <core/CGlobal>

#include "CWorkbench.h"
#include "CApplication.h"

CApplication::CApplication(int argc, char **argv)
    : QApplication(argc, argv),
      m_bench(NULL)
{
}

CApplication::~CApplication()
{
}

bool CApplication::event(QEvent *evt)
{
    bool            r = false;
    QString         filePath;
    QFileOpenEvent  *fileOpenEvent = NULL;

    switch (evt->type())
    {
    case QEvent::FileOpen:
        fileOpenEvent = static_cast<QFileOpenEvent *>(evt);
        if (fileOpenEvent)
        {
            filePath = fileOpenEvent->file();
            if (m_bench)
            {
                QMessageBox::information(NULL, tr("test"), filePath, QMessageBox::Ok);
                m_bench->loadFile(filePath);
            }
        }
        r = true;
        break;

    default:
        break;
    };

    if (!r)
         r = QApplication::event(evt);

    return r;
}

void CApplication::setWorkbench(CWorkbench *bench)
{
    m_bench = bench;
}
