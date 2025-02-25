#include "time_utils.h"

#include <stdint.h>

namespace TimeUtils {

#ifdef ANDROID
#include <time.h>
#include <errno.h>
#include <linux/ioctl.h>

//#if __ANDROID_API__< 21 && !defined(__LP64__)
//#include <sys/atomics.h>
//#else
#include <stdatomic.h>
//#endif

#include "android_alarm.h"
#include <fcntl.h>
#include <unistd.h>

//ms
uint64_t rtc_gettickcount() {
    static int s_fd = -1;
    static int errcode  = 0;
    if (s_fd == -1 && EACCES != errcode) {
        int fd = open("/dev/alarm", O_RDONLY);
        if (-1 == fd) errcode = errno;
//#if __ANDROID_API__< 21 && !defined(__LP64__)
//        if (__atomic_cmpxchg(-1, fd, &s_fd)) {
//            if(fd != -1) close(fd);
//        }
//#else
        atomic_int x = ATOMIC_VAR_INIT(s_fd);
        int expect = -1;
        if (!atomic_compare_exchange_strong(&x, &expect, fd)) {
            if (fd >=0) close(fd);
        }
        s_fd = atomic_load(&x);
//#endif
    }

    struct timespec ts;
    int result = ioctl(s_fd, ANDROID_ALARM_GET_TIME(ANDROID_ALARM_ELAPSED_REALTIME), &ts);

    if (result != 0) {
        // XXX: there was an error, probably because the driver didn't
        // exist ... this should return
        // a real error, like an exception!
        clock_gettime(CLOCK_BOOTTIME, &ts);
    }
    return (uint64_t)ts.tv_sec*1000 + (uint64_t)ts.tv_nsec/1000000;
}

#elif defined __APPLE__

#include <TargetConditionals.h>

#if 0 //defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR) //gettimeofday will rollback, KERN_BOOTTIME up late
#include <sys/sysctl.h>
#include <assert.h>
#include <mach/kern_return.h>

uint64_t rtc_gettickcount() {
    // On iOS mach_absolute_time stops while the device is sleeping. Instead use
    // now - KERN_BOOTTIME to get a time difference that is not impacted by clock
    // changes. KERN_BOOTTIME will be updated by the system whenever the system
    // clock change.
    struct timeval boottime;
    int mib[2] = {CTL_KERN, KERN_BOOTTIME};
    size_t size = sizeof(boottime);
    int kr = sysctl(mib, sizeof(mib)/sizeof(mib[0]), &boottime, &size, NULL, 0);
    assert(KERN_SUCCESS==kr);

    struct timeval now;
    gettimeofday(&now,NULL);

    return (uint64_t)(now.tv_sec-boottime.tv_sec) * 1000 + (uint64_t)(now.tv_usec-boottime.tv_usec)/ 1000;
}

#else

#include <mach/mach_time.h>

uint64_t rtc_gettickcount() {
    static mach_timebase_info_data_t timebase_info = {0};

    // Convert to nanoseconds - if this is the first time we've run, get the timebase.
    if (timebase_info.denom == 0 )
    {
        (void) mach_timebase_info(&timebase_info);
    }

    // Convert the mach time to milliseconds
    uint64_t mach_time = mach_absolute_time();
    uint64_t millis = (mach_time * timebase_info.numer) / (timebase_info.denom * 1000000);
    return millis;
}
#endif


#elif defined Q_OS_BLACKBERRY

#include <time.h>

uint64_t rtc_gettickcount() {//todoyy
    struct timespec ts;
    if (0==clock_gettime(CLOCK_MONOTONIC, &ts)){
        return (ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000);
    }
    return 0;
}

#elif defined __linux__

#include <time.h>

uint64_t rtc_gettickcount() {//todoyy
    struct timespec ts;
    if (0==clock_gettime(CLOCK_MONOTONIC, &ts)){
        return (ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000);
    }
    return 0;
}

#elif defined _WIN32
//#define NOMINMAX
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>

uint64_t rtc_gettickcount() {
    return GetTickCount();
}
#elif WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP || UWP

#include "unistd.h"
#include <stdint.h>

uint64_t rtc_gettickcount() {
    return GetTickCount64();
}

#else
#endif


//ms
uint64_t TimeMillis(){
    return rtc_gettickcount();
}

int64_t TimeDiff(int64_t later, int64_t earlier){
    return later - earlier;
}

int64_t TimeAfter(int64_t elapsed){
  return TimeMillis() + elapsed;
}

}
