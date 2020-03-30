#include "../logger.h"
#include "../utils.h"

void set_thread_name(void *t, const char *name) {}

static JavaVM *g_jvm = nullptr;
void setGlobalJavaVM(JavaVM *vm) { g_jvm = vm; }

JavaVM *getGlobalJavaVM() { return g_jvm; }

void attachCurrentThreadToJvm() {
  JavaVM *vm = getGlobalJavaVM();
  if (vm) {
    JavaVMAttachArgs args;
    args.version = JNI_VERSION_1_6;
    args.name = 0;
    args.group = 0;
    JNIEnv *env = 0;
    int status = vm->AttachCurrentThread(&env, &args);
    if (JNI_OK != status) {
      LOGE() << "Failed to attach the thread";
    }
  }
}

void detachCurrentThreadFromJvm() {
  JavaVM *vm = getGlobalJavaVM();
  if (vm) {
    vm->DetachCurrentThread();
  }
}
