#include <iostream>
#include <unistd.h>
#include <queue>
#include <functional>
#include <pthread.h>
#include <vector>

class pool
{
private:
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool runflag = true;
    std::queue<std::function<void *()>> works; // 修改为无参函数队列
    std::vector<pthread_t> pth;
    int thread_count;

public:
    pool(int n = 4) : thread_count(n)
    {
        pthread_cond_init(&cond, nullptr);
        pthread_mutex_init(&mutex, nullptr);
        pth.resize(n);
        for (int i = 0; i < n; ++i)
        {
            pthread_create(&pth[i], nullptr, &pool::staticTask, this);
        }
    }

    ~pool()
    {
        end_thread();
        for (pthread_t &tid : pth)
        {
            pthread_join(tid, nullptr);
        }
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }

    static void *staticTask(void *arg)
    {
        pool *self = static_cast<pool *>(arg);
        return self->task();
    }

    void *task()
    {
        pthread_mutex_lock(&mutex);
        while (runflag || !works.empty())
        {
            while (works.empty() && runflag)
            {
                pthread_cond_wait(&cond, &mutex);
            }
            if (!works.empty())
            {
                auto func = works.front();
                works.pop();
                pthread_mutex_unlock(&mutex);
                func();
                pthread_mutex_lock(&mutex);
            }
        }
        pthread_mutex_unlock(&mutex);
        return nullptr;
    }

    void enqueue(std::function<void *(void *)> callback, void *arg)
    {
        pthread_mutex_lock(&mutex);
        works.emplace([callback, arg]()
                      { return callback(arg); }); // 绑定参数
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    void end_thread()
    {
        pthread_mutex_lock(&mutex);
        runflag = false;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }

    void finish()
    {
        pthread_mutex_lock(&mutex);
        while (!works.empty())
        {
            pthread_mutex_unlock(&mutex);
            sleep(1);
            pthread_mutex_lock(&mutex);
        }
        pthread_mutex_unlock(&mutex);
    }
};