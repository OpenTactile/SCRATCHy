#ifndef DISPLAYDETACHABLE_H
#define DISPLAYDETACHABLE_H

#include <QThread>
#include <scratchy/graphicaldisplay.h>

class ExternalEventLoop : public QThread
{
protected:
    void run() { exec(); }
};

class DisplayDetachable : public QObject
{
    Q_OBJECT

public:
    DisplayDetachable();
    virtual ~DisplayDetachable();

    enum FontSize
    {
        Small, Medium, Big
    };

signals:
    void finished();

public slots:
    void clear();
    void setDisplay(int icon, std::string header, std::string body);
    void setText(const std::string& text);

protected:
    void setIcon(Icon icon);
    void drawText(const QString& text, unsigned int offset, unsigned int xOffset, FontSize size);

    bool initialized = false;
};

#endif // DISPLAYDETACHABLE_H
