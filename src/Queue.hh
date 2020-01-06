#ifndef _QUEUE_HH_
#define _QUEUE_HH_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace src
{

/**
 * class ConsumerProducerQueue
 * A queue for multi-threaded consumer producer environment
 */
template<typename T> class ConsumerProducerQueue
{
    private:
        std::condition_variable cond;
        std::mutex mutex;
        std::queue<T> cpq;

    public:
        ConsumerProducerQueue()  = default;

        void add(T request)
        {
            std::unique_lock<std::mutex> lock(mutex);
            cond.wait(lock, [this]()
                    { return !isFull(); });
            cpq.push(request);
            lock.unlock();
            cond.notify_all();
        }

        T top()
        {
            std::unique_lock<std::mutex> lock(mutex);
            cond.wait(lock, [this]()
                    { return !isEmpty(); });
            auto ret = cpq.front();
            //cpq.pop();
            lock.unlock();
            cond.notify_all();
            return ret;

        }

        T pop()
        {
            std::unique_lock<std::mutex> lock(mutex);
            cond.wait(lock, [this]()
                    { return !isEmpty(); });
            auto ret = cpq.front();
            cpq.pop();
            lock.unlock();
            cond.notify_all();
            return ret;
        }

        /* it will never full! */
        bool isFull() const { return false; }

        bool isEmpty() const
        {
            return cpq.size() == 0;
        }

        int length() const
        {
            return cpq.size();
        }

        void clear()
        {
            std::unique_lock<std::mutex> lock(mutex);
            while (!isEmpty())
            {
                cpq.pop();
            }
            lock.unlock();
            cond.notify_all();
        }
};

} // namespace src

#endif
