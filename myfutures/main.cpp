#include <iostream>
#include <chrono>
#include <thread>
#include <cassert>

using namespace std;

/*

future<int> f_int = asynch() -> future<int>

f<string> f_string = f_int.then( [](int){ ... return future<string> } );    f_int.set(int)       f_int.then( [](int){ return R} )   continuation int->R

f<bool> f_bool = f_string.then( [](string){ ... return future<bool> } )

xxx = f_bool.then( [](bool){} )

*/

// forward declaration
template <typename T> class Future;

template <typename X>
struct TypeErasedContinuation
{
    virtual void Exec(X par) = 0;
};

template <typename R, typename X>
struct Cont : public TypeErasedContinuation<X>
{
    Cont(std::function<shared_ptr<Future<R>>(X)> c) : continuation(c), future(make_shared<Future<R>>()) {}

    shared_ptr<Future<R>> GetFuture() { return future; }
    
    void Exec(X par) override
    {
        if (!continuation) return;
        shared_ptr<Future<R>> futureResult = continuation(par);
        if (futureResult->terminated)
        {
            future->Set(futureResult->result);
        }
        else
        {
            auto cont = [this](R result)
                        {
                            auto f = make_shared<Future<R>>();
                            f->terminated = true;
                            future->Set(result);
                            return f;
                        };
            futureResult->continuation = make_shared<Cont<R,R>>(cont);
       }
    }
    
    std::function<shared_ptr<Future<R>>(X)> continuation;
    shared_ptr<Future<R>> future;
};


template <typename T>
class Future
{
public:
    using Type = T;

    // Continuation: T -> future<R>     [where R is determined by signature of Then]

    void Set(T value)
    {
        result = value;
        terminated = true;

        if (continuation) 
            continuation->Exec(value);
    }

    template <typename Continuation, typename R = typename std::result_of<Continuation&(T)>::type> // continuation : T -> future<R>
    R Then(Continuation _continuation) // (T->future<R>) -> future<R>
    {
        if (terminated) return _continuation(result);

        using Ret = typename R::element_type::Type;
        // not terminated yet
        // save the continuation
        auto c = make_shared<Cont<Ret,T>>(_continuation);
        continuation = c;
        return c->GetFuture();
    }

    T Get() const { assert(terminated); return result; }

    bool terminated = false;
    T result = {};
    shared_ptr<TypeErasedContinuation<T>> continuation;
};

#if 0
// an asynch function
template <typename T, typename F>
void async_call(T delay, F f)
{
    thread t( [delay, f](){
        this_thread::sleep_for(delay);
        f();
    } );
    t.detach();
}
#endif

// an asynch function with Future
template <typename T, typename F, typename R = typename std::result_of<F&()>::type>
shared_ptr<Future<R>> async_call_future(T delay, F f)
{
    cout << "-- async_call_future begin" << endl;
    auto result = make_shared<Future<R>>();
    thread t( [result, delay, f](){
        cout << "-- async_call_future thread begin" << endl;        
        this_thread::sleep_for(delay);
        auto i = f();
        result->Set(i);
        cout << "-- async_call_future thread end" << endl;
    } );
    t.detach();
    return result;
}

// a call immediately returning
template <typename F, typename R = typename std::result_of<F&()>::type>
shared_ptr<Future<R>> call_future(F f)
{
    auto result = make_shared<Future<R>>();
    auto i = f();
    result->Set(i);
    return result;
}

int main()
{
    cout << "START" << endl;

    using namespace std::chrono_literals;
    auto f = async_call_future(2s, [](){ cout << "Delayed hello!" << endl; return 42; });
    // f type = shared_ptr<Future<int>>
    f->Then( [](int x) // shared_ptr<Future<int>> -> Then( [](int){ return shared_ptr<Future<string>>; } )
        {
            cout << "Completed. Result = " << x << endl;
            return async_call_future(1s, [](){ cout << "Delayed hello 2!" << endl; return std::string("69"); });
        } // returns shared_ptr<Future<string>>
    )->Then( [](std::string x)  // problema abbiamo un oggetto di tipo shared_ptr<Future<int>> invochiamo Then( function<void(string)> )
        {
            cout << "Completed. Result = " << x << endl;
            return async_call_future(2s, [](){ cout << "Delayed hello 3!" << endl; return false; });
        }
    )->Then( [](bool x)
        {
            cout << "Completed. Result = " << x << endl;
            // return make_shared<Future<bool>>();
            return call_future( [](){ cout << "hello!" << endl; return true; } );
        }
    )->Then( [](bool x)
        {
            cout << "Completed. Result = " << x << endl;
            return async_call_future(1s, [](){ cout << "Delayed hello 4!" << endl; return false; });
        }
    );

    cin.get();

    cout << "END" << endl;
    
    return 0;
}
