#pragma once

#include <cstdlib>
#include <stdint.h>

#include "Thread.h"
#include "ThreadController.h"
#include "concurrency/InterruptableDelay.h"

namespace concurrency
{

extern InterruptableDelay mainDelay;

#define RUN_SAME -1

class ExtendedThreadController : public ThreadController
{
  private:
    bool runASAP;

  public:
    ExtendedThreadController(String name);
    // If a thread does something that might need for it to be rescheduled ASAP it can call this function
    // This will suppress the current delay and instead try to run ASAP.
    void blockDelay(void);
    void unblockDelay(void);

    long runOrDelay();

    bool isDelayBlocked(void);
};

/**
 * @brief Base threading
 *
 * This is a pseudo threading layer that is super easy to port, well suited to our slow network and very ram & power efficient.
 *
 * TODO FIXME @geeksville
 *
 * move more things into OSThreads
 * remove lock/lockguard
 *
 * move typedQueue into concurrency
 * remove freertos from typedqueue
 */
class OSThread : public Thread
{
    /// Show debugging info for disabled threads
    static bool showDisabled;

    /// Show debugging info for threads when we run them
    static bool showRun;

    /// Show debugging info for threads we decide not to run;
    static bool showWaiting;

  public:
    /// For debug printing only (might be null)
    static const OSThread *currentThread;

    OSThread(const char *name, uint32_t period = 0);

    virtual ~OSThread();

    virtual bool shouldRun(unsigned long time);

    /**
     * Wait a specified number msecs starting from the current time (rather than the last time we were run)
     */
    void setIntervalFromNow(unsigned long _interval);

    static ExtendedThreadController &getController(void);

  protected:
    /**
     * The method that will be called each time our thread gets a chance to run
     *
     * Returns desired period for next invocation (or RUN_SAME for no change)
     */
    virtual int32_t runOnce() = 0;

    // Do not override this
    virtual void run() final;
};

/**
 * This flag is set **only** when setup() starts, to provide a way for us to check for sloppy static constructor calls.
 * Call assertIsSetup() to force a crash if someone tries to create an instance too early.
 *
 * it is super important to never allocate those object statically.  instead, you should explicitly
 *  new them at a point where you are guaranteed that other objects that this instance
 * depends on have already been created.
 *
 * in particular, for OSThread that means "all instances must be declared via new() in setup() or later" -
 * this makes it guaranteed that the global mainController is fully constructed first.
 */
extern bool hasBeenSetup;

void assertIsSetup();

} // namespace concurrency
