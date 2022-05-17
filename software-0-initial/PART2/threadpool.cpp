// https://stackoverflow.com/questions/15752659/thread-pooling-in-c11
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include "tinyraytracer.hh"

class ThreadPool
{
    
struct imgQueue
{
    sf::Image img;
    int order;
    float angle_v;
    float angle_h;
    float angle_logo;
    bool isCalc = false;
};

public:
    void Start(Tinyraytracer raytracer, sf::RenderWindow& window, sf::Clock& clock, sf::Texture& texture, sf::Sprite& sprite);
    void notifyDisplay();
    void addImage(float angle_v, float angle_h, float angle_logo, int order);
    void Stop();


private:
    std::queue<ThreadPool::imgQueue> imageBuffer;
    
    void renderer(Tinyraytracer raytracer);
    void displayer(sf::RenderWindow& window, sf::Clock& clock, sf::Texture& texture, sf::Sprite& sprite);



    bool should_terminate = false;           // Tells threads to stop looking for jobs
    std::mutex queue_mutex;                  // Prevents data races to the job queue
    std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination
    std::vector<std::thread> threads;
};

void ThreadPool::Start(Tinyraytracer raytracer, sf::RenderWindow& window, sf::Clock& clock, sf::Texture& texture, sf::Sprite& sprite)
{
    
    const size_t num_threads = std::thread::hardware_concurrency(); // Max # of threads the system supports
    threads.resize(num_threads);
    // threads.at(0) = std::thread(&ThreadPool::displayer, std::ref(window), clock, texture, sprite);
    for (size_t i = 1; i < num_threads; i++)
    {
         threads.at(i) = std::thread(&ThreadPool::renderer, raytracer);
    }
}

void ThreadPool::notifyDisplay()
{
}

void ThreadPool::displayer(sf::RenderWindow& window, sf::Clock& clock, sf::Texture& texture, sf::Sprite& sprite)
{
    while (true)
    {
        ThreadPool::imgQueue curr_img;
        static unsigned framecount = 0;
        std::unique_lock<std::mutex> lock(queue_mutex);
        mutex_condition.wait(lock, [this]
                             { return !imageBuffer.empty() || should_terminate; });
        curr_img = imageBuffer.front();
        imageBuffer.pop();
        texture.loadFromImage(curr_img.img);
        window.clear();
        window.draw(sprite);
        window.display();
        framecount++;
        sf::Time currentTime = clock.getElapsedTime();
        if (currentTime.asSeconds() > 1.0)
        {
            float fps = framecount / currentTime.asSeconds();
            std::cout << "fps: " << fps << std::endl;
            clock.restart();
            framecount = 0;
        }
    }
}

void ThreadPool::addImage(float angle_v, float angle_h, float angle_logo, int order)
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    ThreadPool::imgQueue newImage;
    newImage.angle_v = angle_v;
    newImage.angle_h = angle_h;
    newImage.angle_logo = angle_logo;
    newImage.order = order;
    imageBuffer.push(newImage);
    mutex_condition.notify_one();
}

void ThreadPool::renderer(Tinyraytracer raytracer)
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        mutex_condition.wait(lock, [this]
                             { return !imageBuffer.empty() || should_terminate; });
        if (should_terminate)
        {
            return;
        }
        ThreadPool::imgQueue toCompute = this->imageBuffer.front();
        toCompute.img = raytracer.render(toCompute.angle_v, toCompute.angle_h, toCompute.angle_logo);
        toCompute.isCalc = true;
    }
}
// void ThreadPool::ThreadLoop() {
//     while (true) {
//         std::function<void()> job;
//         {
//             std::unique_lock<std::mutex> lock(queue_mutex);
//             mutex_condition.wait(lock, [this] {
//                 return !jobs.empty() || should_terminate;
//             });
//             if (should_terminate) {
//                 return;
//             }
//             job = jobs.front();
//             jobs.pop();
//         }
//         job();
//     }
// }

// void ThreadPool::QueueJob(const std::function<void()>& job) {
//     {
//         std::unique_lock<std::mutex> lock(queue_mutex);
//         jobs.push(job);
//     }
//     mutex_condition.notify_one();
// }

void ThreadPool::Stop()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        should_terminate = true;
    }
    mutex_condition.notify_all();
    for (std::thread &active_thread : threads)
    {
        active_thread.join();
    }
    threads.clear();
}