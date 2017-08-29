#include "mouseevents.h"

#include <linux/input.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>

#include <QMap>
#include <QDir>
#include <QDebug>

namespace MouseEvents
{
    int fd = 0;
    bool initDone = false;
    QMap<QString, QString> deviceMap;

    void init(const QString& devname)
    {
        assert(!initDone);

        fd = open(deviceMap[devname].toUtf8().constData(), O_RDONLY);
        //fd = open("/dev/input/mice", O_RDONLY);


        assert(fd != -1);

        initDone = true;
    }

    QStringList getDevices()
    {
        QStringList devices;

        QDir directory("/dev/input/","event*",QDir::Name,QDir::System);

        for(QFileInfo fileInfo : directory.entryInfoList()) {

            char name[128];

            QString device = fileInfo.absoluteFilePath();
            fd = open(device.toUtf8().constData(), O_RDONLY);

            if(fd != -1)
            {
                ioctl(fd, EVIOCGNAME(sizeof(name)), name);
                QString sName = name;
                if (sName.contains("Mouse")) {
                    devices << name;
                    deviceMap[sName] = device;
                }
            }

            close(fd);
        }
        return devices;
    }

    void getPos(int& dx, int& dy, bool& button)
    {
        assert(initDone);
        dx = 0;
        dy = 0;

//        struct input_event ie;
//        unsigned char *ptr = (unsigned char*)&ie;

//        if(read(fd, &ie, sizeof(struct input_event))!=-1) {
//            dx = (char) ptr[1];
//            dy = (char) ptr[2];
//        }

//        fd_set set;
//        FD_ZERO(&set);
//        FD_SET(fd, &set);
//        if(select(FD_SETSIZE, &set, NULL, NULL, NULL) > 0)
//        {
                int bytes;
                int events = 1;
                struct input_event event[events];
//            if(FD_ISSET(fd, &set))
//            {
                while(true) {
                    bytes = read(fd, &event, sizeof(event));


                    if(bytes == static_cast<int>(sizeof(event)))
                    {
                        for(int i=0; i<events; i++) {
                            if(event[i].type == EV_KEY) // Button Pressed
                            {
                                button = !button;
                                return;
                            }

                            if(event[i].type == EV_REL) //|| event.type == EV_ABS)
                            {
                                if (event[i].code == REL_X) dx += event[i].value;
                                if (event[i].code == REL_Y) dy += event[i].value;                            
                                return;
                            }                            
                        }
                    } else {
                        assert(-1);
                    }
                }
//            }
//        }
    }

    void free()
    {
        assert(initDone);
        initDone = false;
        close(fd);
    }
}
