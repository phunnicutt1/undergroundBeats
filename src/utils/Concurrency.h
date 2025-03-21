/*
 * Underground Beats
 * Concurrency.h
 * 
 * Utility classes for thread-safe operations
 */

#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace UndergroundBeats {
namespace Concurrency {

/**
 * @class LockFreeQueue
 * @brief A lock-free queue for single producer, single consumer operations
 * 
 * This queue is safe for use between the audio thread and other threads.
 */
template<typename T, int Size = 1024>
class LockFreeQueue
{
public:
    LockFreeQueue()
      : writeIndex(0), readIndex(0)
    {
        static_assert(Size > 1 && ((Size & (Size - 1)) == 0), "Size must be a power of 2");
    }
    
    /**
     * @brief Push an item to the queue
     * 
     * @param item The item to push
     * @return true if successful, false if the queue is full
     */
    bool push(const T& item)
    {
        const int writePos = writeIndex.load(std::memory_order_relaxed);
        const int nextWritePos = (writePos + 1) % Size;
        
        if (nextWritePos == readIndex.load(std::memory_order_acquire))
        {
            // Queue is full
            return false;
        }
        
        data[writePos] = item;
        writeIndex.store(nextWritePos, std::memory_order_release);
        return true;
    }
    
    /**
     * @brief Try to pop an item from the queue
     * 
     * @param item Reference to store the popped item
     * @return true if an item was popped, false if the queue is empty
     */
    bool pop(T& item)
    {
        const int readPos = readIndex.load(std::memory_order_relaxed);
        
        if (readPos == writeIndex.load(std::memory_order_acquire))
        {
            // Queue is empty
            return false;
        }
        
        item = data[readPos];
        readIndex.store((readPos + 1) % Size, std::memory_order_release);
        return true;
    }
    
    /**
     * @brief Check if the queue is empty
     * 
     * @return true if the queue is empty
     */
    bool isEmpty() const
    {
        return readIndex.load(std::memory_order_acquire) == 
               writeIndex.load(std::memory_order_acquire);
    }
    
    /**
     * @brief Get the number of items in the queue
     * 
     * @return Number of items in the queue
     */
    int getNumReady() const
    {
        int readPos = readIndex.load(std::memory_order_acquire);
        int writePos = writeIndex.load(std::memory_order_acquire);
        
        if (writePos >= readPos)
            return writePos - readPos;
        else
            return Size - readPos + writePos;
    }
    
    /**
     * @brief Clear the queue
     */
    void clear()
    {
        readIndex.store(0, std::memory_order_release);
        writeIndex.store(0, std::memory_order_release);
    }
    
private:
    T data[Size];
    std::atomic<int> writeIndex;
    std::atomic<int> readIndex;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LockFreeQueue)
};

/**
 * @class ParameterQueue
 * @brief Queue for thread-safe parameter updates from UI to audio thread
 */
class ParameterQueue
{
public:
    struct ParameterUpdate
    {
        int nodeId;
        int paramIndex;
        float value;
    };
    
    ParameterQueue() {}
    
    /**
     * @brief Add a parameter update to the queue
     * 
     * @param nodeId The ID of the processor node
     * @param paramIndex The index of the parameter
     * @param value The new parameter value
     * @return true if the update was queued
     */
    bool enqueue(int nodeId, int paramIndex, float value)
    {
        ParameterUpdate update = { nodeId, paramIndex, value };
        return queue.push(update);
    }
    
    /**
     * @brief Process all queued parameter updates
     * 
     * @param callback Function to process each update
     */
    void processUpdates(std::function<void(int, int, float)> callback)
    {
        ParameterUpdate update;
        while (queue.pop(update))
        {
            callback(update.nodeId, update.paramIndex, update.value);
        }
    }
    
private:
    LockFreeQueue<ParameterUpdate, 256> queue;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterQueue)
};

/**
 * @class ThreadSafeValue
 * @brief A simple thread-safe wrapper for a value
 */
template<typename T>
class ThreadSafeValue
{
public:
    ThreadSafeValue(const T& initialValue = T())
      : value(initialValue)
    {
    }
    
    /**
     * @brief Set the value
     * 
     * @param newValue The new value
     */
    void set(const T& newValue)
    {
        std::lock_guard<std::mutex> lock(mutex);
        value = newValue;
    }
    
    /**
     * @brief Get the value
     * 
     * @return The current value
     */
    T get() const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return value;
    }
    
private:
    T value;
    mutable std::mutex mutex;
};

} // namespace Concurrency
} // namespace UndergroundBeats
