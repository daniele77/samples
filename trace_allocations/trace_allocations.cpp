#include <cstdlib>
#include <iostream>

namespace
{
    class AllocationCount
    {
    public:
        ~AllocationCount() { std::cout << "Number of allocations: " << count << std::endl; }
        void NewCall() { ++count; }
    private:
        std::size_t count = 0;
    };

    AllocationCount allocationCount;
}

void* operator new(std::size_t sz)
{
    std::cout << "global op new called, size = " << sz << std::endl;    
    allocationCount.NewCall();
    return std::malloc(sz);
}

void operator delete(void* ptr) noexcept
{
    std::cout << "global op delete called" << std::endl;
    std::free(ptr);
}

void* operator new[](std::size_t sz)
{
    std::cout << "global op new[] called, size = " << sz << std::endl;
    allocationCount.NewCall();    
    return std::malloc(sz);
}
void operator delete[](void* ptr) noexcept
{
    std::cout << "global op delete[] called" << std::endl;
    std::free(ptr);
}