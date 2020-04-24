#ifndef MEMORY_ARENA_H
#define MEMORY_ARENA_H

/** Allocate memory on the stack
 * Caveat: should only use for small kBs. Deallocated when the function returns.
 */
#define ALLOCA(TYPE, COUNT) (TYPE *)alloca((COUNT) * sizeof(TYPE))

#ifndef L1_CACHE_LINE_SIZE
#define L1_CACHE_LINE_SIZE 64
#endif

#define IS_WINDOWS 0
#define IS_OSX 1

#include <list>
#include <utility>

/** Allocate cache-aligned memory */
static void* AllocAligned(size_t size)
{
#if defined(IS_WINDOWS)
    //return _aligned_malloc(size, L1_CACHE_LINE_SIZE);
#elif defined(IS_OSX)
    void* ptr;
    if (posix_memalign(&ptr, L1_CACHE_LINE_SIZE, size) != 0)
    {
        ptr = nullptr;
    }
    return ptr;
#else
    return memalign(L1_CACH_LINE_SIZE, size);
#endif
}


/** Convenient method to allocate cache-aligned array of objects */
static template<typename T>
T* AllocAligned(size_t count)
{
    return (T*)AllocAligned(sizeof(T) * count);
}

static void FreeAligned(void* ptr)
{
    if (ptr != nullptr) free(ptr);
}


class MemoryArena
{
public:
    /** Default block size is 256kB */
    MemoryArena(size_t blockSize = 262144) :
        blockSize(blockSize)
    {}
    
    ~MemoryArena()
    {
        FreeAligned(currentBlock);
        for (auto& block: usedBlocks)
        {
            FreeAligned(block.second);
        }
        
        for (auto& block: availableBlocks)
        {
            FreeAligned(block.second);
        }
    }
    
    void* Alloc(size_t nBytes)
    {
        // Round up to minimum machine alignment
        nBytes = ((nBytes + 15) & (~15));
        
        if (currentBlockPos + nBytes > currentAllocSize)
        {
            // Add current block to usedBlocks list
            if (currentBlock)
            {
                usedBlocks.push_back(std::make_pair(currentAllocSize, currentBlock));
                currentBlock = nullptr;
            }
            
            // Get new block memory
            // Try to get memory block from available blocks
            for (auto iter = availableBlocks.begin(); iter != availableBlocks.end(); iter++)
            {
                if (iter->first >= nBytes)
                {
                    currentAllocSize = iter->first;
                    currentBlock = iter->second;
                    
                    // Erase the element at this reference
                    availableBlocks.erase(iter);
                    break;
                }
            }
            
            if (!currentBlock)
            {
                currentAllocSize = std::max(nBytes, blockSize);
                currentBlock = AllocAligned<uint8_t>(currentAllocSize);
            }
            
            currentBlockPos = 0;
        }
        
        void* ret = currentBlock + currentBlockPos;
        currentBlockPos += nBytes;
        return ret;
    }
    
    template<typename T>
    T* Alloc(size_t n = 1, bool runConstructor = true)
    {
        T* ret = (T*)Alloc(n * sizeof(T));
        if (runConstructor)
        {
            for (size_t i = 0; i < n; ++i)
            {
                new (&ret[i]) T();
            }
        }
        return ret;
    }
    
    void Reset()
    {
        currentBlockPos = 0;
        availableBlocks.splice(availableBlocks.begin(), usedBlocks);
    }
    
    size_t TotalAllocated() const
    {
        size_t total = currentAllocSize;
        for (const auto& alloc : usedBlocks)
        {
            total += alloc.first;
        }
        
        for (const auto& alloc : availableBlocks)
        {
            total += alloc.first;
        }
        
        return total;
    }
    
private:
    const size_t blockSize;
    size_t currentBlockPos = 0, currentAllocSize = 0;
    uint8_t* currentBlock = nullptr;
    
    /** 2 lists: memory that are fully used, and memory allocated but not used */
    std::list<std::pair<size_t, uint8_t*>> usedBlocks, availableBlocks;
};


/** Blocked 2D array breaks memory alignment into blocks
 * so that spatially coherent array position will also have
 * memory access coherence.
 * logBlockSize is used to ensure the block size is a power of 2
 */
template <typename T, int logBlockSize>
class BlockedArray
{
public:
    BlockedArray(int uRes, int vRes, const T* d = nullptr) :
        uRes(uRes), vRes(vRes), uBlocks(RoundUp(uRes) >> logBlockSize)
    {
        int nAlloc = RoundUp(uRes) * RoundUp(vRes);
        data = AllocAligned<T>(nAlloc);
        for (int i = 0; i < nAlloc; ++i)
        {
            new (&data[i]) T();
        }
        
        if (d)
        {
            for (int v = 0; v < vRes; ++v)
            {
                for (int u = 0; u < uRes; ++u)
                {
                    (*this)(u, v) = d[v * uRes + u];
                }
            }
        }
    }
    
    
    constexpr int BlockSize() const { return 1 << logBlockSize; }
    int RoundUp(int x) const
    {
        return (x + BlockSize() - 1) & ~(BlockSize() - 1);
    }
    
    inline int uSize() const { return uRes; }
    inline int vSize() const { return vRes; }
    
    ~BlockedArray()
    {
        for (int i = 0; i < uRes * vRes; ++i)
        {
            data[i].~T();
        }
        FreeAligned(data);
    }
    
    int Block(int a) const { return a >> logBlockSize; }
    int Offset(int a) const { return (a & BlockSize() - 1); }
    T& operator()(int u, int v)
    {
        int bu = Block(u), bv = Block(v);
        int ou = Offset(u), ov = Offset(v);
        int offset = BlockSize() * BlockSize() * (uBlocks * bv + bu);
        offset += BlockSize() * ov + ou;
        return data[offset];
    }
    
    const T& operator()(int u, int v) const
    {
        int bu = Block(u), bv = Block(v);
        int ou = Offset(u), ov = Offset(v);
        int offset = BlockSize() * BlockSize() * (uBlocks * bv + bu);
        offset += BlockSize() * ov + ou;
        return data[offset];
    }
    
    void GetLinearArray(T* a) const
    {
        for (int v = 0; v < vRes; ++v)
        {
            for (int u = 0; u < uRes; ++u)
            {
                *a++ = (*this)(u, v);
            }
        }
    }
    
private:
    T* data;
    const int uRes, vRes, uBlocks;
};

#endif
