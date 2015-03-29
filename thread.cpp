#include "thread.h"

Thread::Thread(QObject *parent) : QThread(parent) {
}

Thread::~Thread() {
  quit();
  #if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
  requestInterruption();
  #endif
  wait();
}
