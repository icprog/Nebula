#ifndef UICOMPOSER_H
#define UICOMPOSER_H

#include <list>
#include <functional>

#include <QObject>

#include "utils/common.h"
#include "ui/MainWindowUi.h"
#include "ui/DefaultUi.h"
#include "ui/DeviceUi.h"
#include "ui/Animation.h"
#include "ui/AnimationDrawIntf.h"

namespace nebula {

class UiComposer :
    public QObject,
    public AnimationDrawIntf
{
    Q_OBJECT

public:
    int32_t onDeviceAttached(QString &name);
    int32_t onDeviceRemoved(QString &name);
    int32_t drawAnimation(QString name, int32_t frameId) override;

public:
    explicit UiComposer(QMainWindow *window);
    ~UiComposer();
    int32_t construct();
    int32_t destruct();

signals:
    int32_t drawUi(std::function<int32_t ()> func);
    int32_t drawAnimationFrame(QString name, int32_t frameId);

private slots:
    int32_t onDrawUi(std::function<int32_t ()> func);
    int32_t onDrawAnimationFrame(QString name, int32_t frameId);

private:
    bool          mConstructed;
    QMainWindow  *mMainWindow;
    QWidget      *mBaseWidget;
    MainWindowUi *mMainWindowUi;
    DefaultUi    *mDefaultUi;
    std::list<DeviceUi *> mDeviceUi;
    std::list<Animation *> mAnimation;
};

}

#endif
