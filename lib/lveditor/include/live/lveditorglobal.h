#include "../../src/lveditorglobal.h"

#define LV_ACCURACY 1e-7
#define lv_qmlobject_connect(Sender, SenderType, Signal, Receiver, ReceiverType, Method) \
{ \
    SenderType *sender = (Sender); \
    ReceiverType *receiver = (Receiver); \
    const char *signal = (Signal); \
    const char *method = (Method); \
    static int signalIdx = -1; \
    static int methodIdx = -1; \
    if (signalIdx < 0) { \
        Q_ASSERT((static_cast<int>(*signal) - '0') == QSIGNAL_CODE); \
        signalIdx = SenderType::staticMetaObject.indexOfSignal(signal+1); \
    } \
    if (methodIdx < 0) { \
        int code = (static_cast<int>(*method) - '0'); \
        Q_ASSERT(code == QSLOT_CODE || code == QSIGNAL_CODE); \
        if (code == QSLOT_CODE) \
            methodIdx = ReceiverType::staticMetaObject.indexOfSlot(method+1); \
        else \
            methodIdx = ReceiverType::staticMetaObject.indexOfSignal(method+1); \
    } \
    Q_ASSERT(signalIdx != -1 && methodIdx != -1); \
    QMetaObject::connect(sender, signalIdx, receiver, methodIdx, Qt::DirectConnection); \
}
#define LV_IS_SIGNAL_CONNECTED(Sender, SenderType, Name, Arguments) \
do { \
    QObject *sender = (Sender); \
    void (SenderType::*signal)Arguments = &SenderType::Name; \
    static QMetaMethod method = QMetaMethod::fromSignal(signal); \
    static uint signalIdx = static_cast<uint>(QMetaObjectPrivate::signalIndex(method)); \
    return QObjectPrivate::get(sender)->isSignalConnected(signalIdx); \
} while (0)
