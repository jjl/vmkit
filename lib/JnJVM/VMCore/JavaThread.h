//===----------- JavaThread.h - Java thread description -------------------===//
//
//                              JnJVM
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef JNJVM_JAVA_THREAD_H
#define JNJVM_JAVA_THREAD_H

#include <csetjmp>

#include "mvm/Object.h"
#include "mvm/Threads/Cond.h"
#include "mvm/Threads/Locks.h"
#include "mvm/Threads/Thread.h"

#include "JavaObject.h"

namespace jnjvm {

class Class;
class JavaObject;
class Jnjvm;


#define BEGIN_NATIVE_EXCEPTION(level) \
  JavaThread* __th = JavaThread::get(); \
  __th->startNative(level); \
  try {

#define END_NATIVE_EXCEPTION \
  } catch(...) { \
    __th->throwFromNative(); \
  } \
  __th->endNative();

#define BEGIN_JNI_EXCEPTION \
  JavaThread* th = JavaThread::get(); \
  try {

#define END_JNI_EXCEPTION \
  } catch(...) { \
    th->throwFromJNI(); \
  }


/// JavaThread - This class is the internal representation of a Java thread.
/// It maintains thread-specific information such as its state, the current
/// exception if there is one, the layout of the stack, etc.
///
class JavaThread : public mvm::Thread {


public:
  
  /// jniEnv - The JNI environment of the thread.
  ///
  void* jniEnv;
  
  /// pendingException - The Java exception currently pending.
  ///
  JavaObject* pendingException;

  /// internalPendingException - The C++ exception currencty pending.
  ///
  void* internalPendingException;

  /// javaThread - The Java representation of this thread.
  ///
  JavaObject* javaThread;

  /// vmThread - The VMThread object of this thread.
  ///
  JavaObject* vmThread;

  /// lock - This lock is used when waiting or being notified or interrupted.
  ///
  mvm::LockNormal lock;

  /// varcond - Condition variable when the thread needs to be awaken from
  /// a wait.
  ///
  mvm::Cond varcond;

  /// interruptFlag - Has this thread been interrupted?
  ///
  uint32 interruptFlag;
  
  static const unsigned int StateRunning;
  static const unsigned int StateWaiting;
  static const unsigned int StateInterrupted;

  /// state - The current state of this thread: Running, Waiting or Interrupted.
  uint32 state;
  
  /// sjlj_buffers - Setjmp buffers pushed when entering a non-JVM native
  /// function and popped when leaving the function. The buffer is used when
  /// the native function throws an exception through a JNI throwException call.
  ///
  std::vector<jmp_buf*> sjlj_buffers;

  /// addresses - The list of return addresses which represent native/Java cross
  /// calls.
  ///
  std::vector<void*> addresses;

  /// tracer - Traces GC-objects pointed by this thread object.
  ///
  virtual void TRACER;

  /// JavaThread - Empty constructor, used to get the VT.
  ///
  JavaThread() {
#ifdef SERVICE
    replacedEIPs = 0;
#endif
  }

  /// ~JavaThread - Delete any potential malloc'ed objects used by this thread.
  ///
  ~JavaThread();
  
  /// JavaThread - Creates a Java thread.
  ///
  JavaThread(JavaObject* thread, JavaObject* vmThread, Jnjvm* isolate);

  /// get - Get the current thread as a JnJVM object.
  ///
  static JavaThread* get() {
    return (JavaThread*)mvm::Thread::get();
  }

  /// getJVM - Get the JnJVM in which this thread executes.
  ///
  Jnjvm* getJVM() {
    return (Jnjvm*)MyVM;
  }

  /// currentThread - Return the current thread as a Java object.
  ///
  JavaObject* currentThread() {
    return javaThread;
  }
 
  /// throwException - Throw the given exception in the current thread.
  ///
  void throwException(JavaObject* obj);

  /// throwPendingException - Throw a pending exception.
  ///
  void throwPendingException();
  
  /// getJavaException - Return the pending exception.
  ///
  JavaObject* getJavaException() {
    return pendingException;
  }

  /// throwFromJNI - Throw an exception after executing JNI code.
  ///
  void throwFromJNI() {
    assert(sjlj_buffers.size());
    internalPendingException = 0;
#if defined(__MACH__)
    longjmp((int*)sjlj_buffers.back(), 1);
#else
    longjmp((__jmp_buf_tag*)sjlj_buffers.back(), 1);
#endif
  }
  
  /// throwFromNative - Throw an exception after executing Native code.
  ///
  void throwFromNative() {
#ifdef DWARF_EXCEPTIONS
    addresses.pop_back();
    throwPendingException();
#endif
  }
  
  /// throwFromJava - Throw an exception after executing Java code.
  ///
  void throwFromJava() {
    addresses.pop_back();
    throwPendingException();
  }

  /// startNative - Record that we are entering native code.
  ///
  void startNative(int level) __attribute__ ((noinline));

  /// startJava - Record that we are entering Java code.
  ///
  void startJava() __attribute__ ((noinline));
  
  /// endNative - Record that we are leaving native code.
  ///
  void endNative() {
    //assert(!(addresses.size() % 2) && "Wrong stack");    
    addresses.pop_back();
  }

  /// endJava - Record that we are leaving Java code.
  ///
  void endJava() {
    //assert((addresses.size() % 2) && "Wrong stack");    
    addresses.pop_back();
  }

  /// getCallingClass - Get the Java class that called the last Java
  /// method that called the Java getCallingClass method.
  ///
  UserClass* getCallingClass(uint32 level);
  
  /// getCallingMethod - Get the Java method that called the last Java
  /// method on the stack.
  ///
  JavaMethod* getCallingMethod();
  
  /// getCallingClassLevel - Get the Java method in the stack at the
  /// specified level.
  ///
  UserClass* getCallingClassLevel(uint32 level);
  
  /// getNonNullClassLoader - Get the first non-null class loader on the
  /// stack.
  ///
  JavaObject* getNonNullClassLoader();
    
  /// printBacktrace - Prints the backtrace of this thread.
  ///
  void printBacktrace() __attribute__ ((noinline));
  
  /// printJavaBacktrace - Prints the backtrace of this thread. Only prints
  /// the Java methods on the stack.
  ///
  void printJavaBacktrace();

  /// getJavaFrameContext - Fill the vector with Java frames
  /// currently on the stack.
  ///
  void getJavaFrameContext(std::vector<void*>& context);

private:
  /// internalClearException - Clear the C++ and Java exceptions
  /// currently pending.
  ///
  virtual void internalClearException() {
    pendingException = 0;
    internalPendingException = 0;
  }

public:

#ifdef SERVICE
  /// ServiceException - The exception that will be thrown if a bundle is
  /// stopped.
  JavaObject* ServiceException;

  /// replacedEIPs - List of instruction pointers which must be replaced
  /// to a function that throws an exception. We maintain this list and update
  /// the stack correctly so that Dwarf unwinding does not complain.
  ///
  void** replacedEIPs;

  /// eipIndex - The current index in the replacedIPs list.
  ///
  uint32_t eipIndex;
#endif

};

} // end namespace jnjvm

#endif
