#include "utils/common.h"
#include "core/Common.h"
#include "algorithm/Config.h"
#include "core/DeviceControl.h"
#include "algorithm/Algorithm.h"

namespace nebula {

int32_t DeviceControl::doTask()
{
    int32_t rc = NO_ERROR;
    int32_t result = false;
    Semaphore sem;

    if (SUCCEED(rc)) {
        rc = mUi->updateUiResult(mName, DEVICE_UI_TYPE_1, true);
        if (!SUCCEED(rc)) {
            showError("Failed to update ui, 1");
        }
    }

    if (SUCCEED(rc)) {
        mCtl = new RemoteControl(mPath, mName);
        if (ISNULL(mCtl)) {
            showError("Failed to create remote controller");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mCtl->setCb(
            [&sem]() -> int32_t {
                sem.signal();
                return NO_ERROR;
            }
        );
        if (!SUCCEED(rc)) {
            showError("Failed to set remote controller cb");
        }
    }

    if (SUCCEED(rc)) {
        rc = mCtl->startController();
        if (!SUCCEED(rc)) {
            showError("Failed to start remote controller");
        }
    }

    if (SUCCEED(rc)) {
        sem.wait();
        rc = mCtl->exitController();
        if (!SUCCEED(rc)) {
            showError("Failed to exit remote controller");
        } else {
            SECURE_DELETE(mCtl);
        }
    }

    if (SUCCEED(rc)) {
        rc = mUi->updateUiResult(mName, DEVICE_UI_TYPE_2, true);
        if (!SUCCEED(rc)) {
            showError("Failed to update ui, 2");
        }
    }

    if (SUCCEED(rc)) {
        mAlg = new Algorithm(mPath, mName);
        if (ISNULL(mAlg)) {
            showError("Failed to create algorithm");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mAlg->init();
        if (!SUCCEED(rc)) {
            showError("Failed to init algorithm");
        }
    }

    if (SUCCEED(rc)) {
        result = mAlg->process();
        if (!SUCCEED(result)) {
            qDebug() << "Failed to process algorithm";
        }
    }

    if (SUCCEED(rc)) {
        rc = mUi->updateUiResult(mName, DEVICE_UI_TYPE_3, result);
        if (!SUCCEED(rc)) {
            showError("Failed to update ui, 3");
        }
    }

    if (SUCCEED(rc)) {
        rc = mAlg->deinit();
        if (!SUCCEED(rc)) {
            showError("Failed to deinit algorithm");
        }
    }

    if (SUCCEED(rc)) {
        SECURE_DELETE(mAlg);
    }

    if (SUCCEED(rc)) {
        rc = mUi->updateUiResult(mName, DEVICE_UI_TYPE_4, result);
        if (!SUCCEED(rc)) {
            showError("Failed to update ui, 4");
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        rc = mUi->updateUiResult(mName,
            DEVICE_UI_TYPE_RESULT, result && SUCCEED(rc));
        if (!SUCCEED(rc)) {
            showError("Failed to update ui, result");
        }
    }

    return rc;
}

void DeviceControl::run()
{
    doTask();
}

QString DeviceControl::getName()
{
    return mName;
}

DeviceControl::DeviceControl(QString &name,
    UpdateUiIntf *ui, DebugIntf *debug) :
    mExit(false),
    mName(name),
    mUi(ui),
    mDebug(debug),
    mCtl(nullptr),
    mAlg(nullptr)
{
}

DeviceControl::~DeviceControl()
{
    if (isRunning()) {
        int32_t rc = stopThread();
        if (!SUCCEED(rc)) {
            showError("Failed to destruct device controller.");
        }
    }
    quit();
    wait();
}

int32_t DeviceControl::startThread()
{
    int32_t rc = NO_ERROR;

    if (!isRunning()) {
        start();
    } else {
        showError("Device controller already started.");
    }

    return rc;
}

int32_t DeviceControl::stopThread()
{
    mExit = true;
    mExitSem.wait();

    return NO_ERROR;
}

void DeviceControl::onNewPathSelected(QString path)
{
    mPath = path;
}

}
