//
//  mutex.h
//  jupiter
//
//  Created by fumw on 2017/8/3.
//  Copyright © 2017年 wedoctor. All rights reserved.
//


#ifndef __UTILES_H__
#define __UTILES_H__

#include <errno.h>
#include <stdint.h>
#include <time.h>
#ifdef WIN32
//#include "utils.h"
#include <windows.h>
typedef int pthread_mutexattr_t;

#else
#include <pthread.h>
#include <sys/time.h>
#endif

#ifdef WIN32
static int
gettimeofday(struct timeval *tp, void *tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
    tp->tv_sec = clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}
#endif
/* Thread management macros */
#ifdef _WIN32
/* Win32 */
#define _WIN32_WINNT 0x500 /* WINBASE.H - Enable SignalObjectAndWait */
#include <process.h>
#include <windows.h>
#include <processthreadsapi.h>
#define pthread_t HANDLE
#define pthread_create(thhandle,attr,thfunc,tharg) (int)((*thhandle=(HANDLE)_beginthreadex(NULL,0,(_beginthreadex_proc_type)thfunc,tharg,0,NULL))==NULL) //NOLINT
#define pthread_join(thread, result) ((WaitForSingleObject((thread),INFINITE)!=WAIT_OBJECT_0) || !CloseHandle(thread))
#define pthread_detach(thread) if(thread!=NULL)CloseHandle(thread)
#define thread_sleep(nms) Sleep(nms)
#define pthread_cancel(thread) TerminateThread(thread,0)
#define ts_key_create(ts_key, destructor) {ts_key = TlsAlloc();};
#define pthread_getspecific(ts_key) TlsGetValue(ts_key)
#define pthread_setspecific(ts_key, value) TlsSetValue(ts_key, (void *)value) //NOLINT
#define pthread_self() GetCurrentThreadId()

#define pthread_setname_np(thhandle, name) SetThreadName(-1,name)


#else
/* pthreads */
/* Nearly everything is already defined */
#define THREAD_FUNCTION void *
#define THREAD_FUNCTION_RETURN void *
#define THREAD_SPECIFIC_INDEX pthread_key_t
#define thread_sleep(nms) sleep((nms+500)/1000)
#define ts_key_create(ts_key, destructor) pthread_key_create (&(ts_key), destructor);
#endif

/* Syncrhronization macros: Win32 and Pthreads */
#ifdef _WIN32
#define pthread_mutex_t HANDLE
#define pthread_cond_t HANDLE
#define pthread_mutex_lock(pobject) WaitForSingleObject(*pobject,INFINITE)
#define pthread_mutex_unlock(pobject) ReleaseMutex(*pobject)
#define pthread_mutex_init(pobject,pattr) (*pobject=CreateMutex(NULL,FALSE,NULL))
#define pthread_cond_init(pobject,pattr) (*pobject=CreateEvent(NULL,FALSE,FALSE,NULL))
#define pthread_mutex_destroy(pobject) CloseHandle(*pobject)
#define pthread_cond_destroy(pobject) CloseHandle(*pobject)
#define CV_TIMEOUT INFINITE /* Tunable value */
/* USE THE FOLLOWING FOR WINDOWS 9X */
/* For addtional explanation of the condition variable emulation and the use of the
* timeout, see the paper "Batons: A Sequential Synchronization Object"
* by Andrew Tucker and Johnson M Hart. (Windows Developer’s Journal,
* July, 2001, pp24 ff. www.wdj.com). */
//#define pthread_cond_wait(pcv,pmutex) {ReleaseMutex(*pmutex);WaitForSingleObject(*pcv,CV_TIMEOUT);WaitForSingleObject(*pmutex,INFINITE);};
/* You can use the following on Windows NT/2000/XP and avoid the timeout */
#define pthread_cond_wait(pcv,pmutex) {SignalObjectAndWait(*pmutex,*pcv,INFINITE,FALSE);WaitForSingleObject(*pmutex,INFINITE);};
//#define pthread_cond_timedwait(pcv,pmutex,time) SignalObjectAndWait(*pmutex,*pcv,(*time).tv_sec*1000*1000,FALSE)
#define pthread_cond_timedwait(pcv,pmutex,time) SignalObjectAndWait(*pmutex,*pcv,1000,FALSE)//1s

//#define pthread_cond_broadcast(pcv) PulseEvent(*pcv)
#define pthread_cond_signal(pcv) SetEvent(*pcv)
static int OnceFlag;
//static DWORD ThId; /* This is ugly, but is required on Win9x for _beginthreadex */
#else
/* Not Windows. Assume pthreads */

#endif


#include <assert.h>
//#include "time_utils.h"
#define ASSERT assert
#define ASSERT2(...)
#ifdef WIN32
#include <mutex> //NOLINT
typedef std::mutex Mutex;
#else
class Mutex {
  public:
    typedef pthread_mutex_t handle_type;
    Mutex(bool _recursive = false) //NOLINT
        : magic_(reinterpret_cast<uintptr_t>(this)), mutex_(), attr_() {
        //禁止重复加锁

        int ret = pthread_mutexattr_init(&attr_);

        if (ENOMEM == ret) ASSERT(0 == ENOMEM);
        else if (0 != ret) ASSERT(0 == ret);

        ret = pthread_mutexattr_settype(&attr_, _recursive ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_ERRORCHECK);

        if (EINVAL == ret) ASSERT(0 == EINVAL);
        else if (0 != ret) ASSERT(0 == ret);

        ret = pthread_mutex_init(&mutex_, &attr_);

        if (EAGAIN == ret) ASSERT(0 == EAGAIN);
        else if (ENOMEM == ret) ASSERT(0 == ENOMEM);
        else if (EPERM == ret) ASSERT(0 == EPERM);
        else if (EBUSY == ret) ASSERT(0 == EBUSY);
        else if (EINVAL == ret) ASSERT(0 == EINVAL);
        else if (0 != ret) ASSERT(0 == ret);
    }

    ~Mutex() {
        magic_ = 0;


        int ret = pthread_mutex_destroy(&mutex_);

        if (EBUSY == ret) ASSERT(0 == EBUSY);
        else if (EINVAL == ret) ASSERT(0 == EINVAL);
        else if (0 != ret) ASSERT(0 == ret);

        ret = pthread_mutexattr_destroy(&attr_);

        if (EINVAL == ret) ASSERT(0 == EINVAL);
        else if (0 != ret) ASSERT(0 == ret);

    }

    bool lock() {
        // 成功返回0，失败返回错误码
        ASSERT2(reinterpret_cast<uintptr_t>(this) == magic_ && 0 != magic_, "this:%p != mageic:%p", this, (void*)magic_);

        if (reinterpret_cast<uintptr_t>(this) != magic_) return false;

        int ret = pthread_mutex_lock(&mutex_);

        if (EINVAL == ret) ASSERT(0 == EINVAL);
        else if (EAGAIN == ret) ASSERT(0 == EAGAIN);
        else if (EDEADLK == ret) ASSERT(0 == EDEADLK);
        else if (0 != ret) ASSERT(0 == ret);

        return 0 == ret;
    }

    bool unlock() {
        ASSERT2(reinterpret_cast<uintptr_t>(this) == magic_ && 0 != magic_, "this:%p != mageic:%p", this, (void*)magic_);
        //        if (reinterpret_cast<uintptr_t>(this)!=m_magic) return false;

        int ret = pthread_mutex_unlock(&mutex_);

        if (EINVAL == ret) ASSERT(0 == EINVAL);
        else if (EAGAIN == ret) ASSERT(0 == EAGAIN);
        else if (EPERM == ret) ASSERT(0 == EPERM);
        else if (0 != ret) ASSERT(0 == ret);

        return 0 == ret;
    }


    bool trylock() {
        ASSERT2(reinterpret_cast<uintptr_t>(this) == magic_ && 0 != magic_, "this:%p != mageic:%p", this, (void*)magic_);

        if (reinterpret_cast<uintptr_t>(this) != magic_) return false;

        int ret = pthread_mutex_trylock(&mutex_);

        if (EBUSY == ret) return false;

        if (EINVAL == ret) ASSERT(0 == EINVAL);
        else if (EAGAIN == ret) ASSERT(0 == EAGAIN);
        else if (EDEADLK == ret) ASSERT(0 == EDEADLK);
        else if (0 != ret) ASSERT(0 == ret);

        return 0 == ret;
    }

#ifdef ANDROID
    bool timedlock(unsigned int _millisecond) {
        ASSERT2(reinterpret_cast<uintptr_t>(this) == magic_ && 0 != magic_, "this:%p != mageic:%p", this, (void*)magic_);

        if (reinterpret_cast<uintptr_t>(this) != magic_) return false;

        int ret = 0;
#if defined(ANDROID) && __ANDROID_API__ < 21 && !defined(__LP64__)
        ret = pthread_mutex_lock_timeout_np(&mutex_, (unsigned)_millisecond);
#else
        struct timespec ts;
        MakeTimeout(&ts, _millisecond);
        ret = pthread_mutex_timedlock(&mutex_, &ts);
#endif

        switch (ret) {
        case 0: return true;

        case ETIMEDOUT: return false;

        case EBUSY: return false;

        case EAGAIN: ASSERT2(false, "EAGAIN"); return false;

        case EDEADLK: ASSERT2(false, "EDEADLK"); return false;

        case EINVAL: ASSERT2(false, "EINVAL"); return false;

        default: ASSERT2(false, "%d", ret); return false;
        }

        return false;
    }
#elif defined(__linux__)
    bool timedlock(unsigned int _millisecond) {
        ASSERT2(reinterpret_cast<uintptr_t>(this) == magic_ && 0 != magic_, "this:%p != mageic:%p", this, (void*)magic_);

        if (reinterpret_cast<uintptr_t>(this) != magic_) return false;

        struct timespec ts;
        MakeTimeout(&ts, _millisecond);

        int ret = pthread_mutex_timedlock(&mutex_, &ts);

        switch (ret) {
        case 0: return true;

        case ETIMEDOUT: return false;

        case EAGAIN: ASSERT2(false, "EAGAIN"); return false;

        case EDEADLK: ASSERT2(false, "EDEADLK"); return false;

        case EINVAL: ASSERT2(false, "EINVAL"); return false;

        default: ASSERT2(false, "%d", ret); return false;
        }

        return false;
    }
#endif

    bool islocked() {
        ASSERT(reinterpret_cast<uintptr_t>(this) == magic_);

        int ret = pthread_mutex_trylock(&mutex_);

        if (0 == ret) unlock();

        return 0 != ret;
    }

    handle_type& internal() { return mutex_; }

  private:
    Mutex(const Mutex&);
    Mutex& operator = (const Mutex&);

  private:
    static void MakeTimeout(struct timespec* pts, unsigned int millisecond) {
        struct timeval tv;
        gettimeofday(&tv, 0);
        pts->tv_sec = millisecond / 1000 + tv.tv_sec;
        pts->tv_nsec = (millisecond % 1000) * 1000 * 1000 + tv.tv_usec * 1000;

        pts->tv_sec += pts->tv_nsec / (1000 * 1000 * 1000);
        pts->tv_nsec = pts->tv_nsec % (1000 * 1000 * 1000);
    }

  private:
    uintptr_t    magic_;  // Dangling pointer will dead lock, so check it!!!
    pthread_mutex_t mutex_;

    pthread_mutexattr_t attr_;
};
#endif

#endif 
