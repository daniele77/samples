#ifndef SIMULATION_H_
#define SIMULATION_H_

#include <string>
#include <queue>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/exponential_distribution.hpp>

// ***************

typedef double Time;

class Simulation;

class Event
{
public:
    Event( boost::function<void (void)> e, Time t ) :
      time( t ),
      task( e )
    {}
    void Simulate( Simulation& ) { task(); }
    Time GetTime() const { return time; }
private:
    const Time time;
    boost::function<void (void)> task;
};

typedef boost::shared_ptr< Event > EventPtr;

struct CompareEvent
{
    bool operator()( EventPtr e1, EventPtr e2 )
    {
        return e1 -> GetTime() > e2 -> GetTime();
    }
};

class Simulation
{
public:
    Simulation( Time et ) : time( 0 ), endTime( et )
    {
    }
    virtual ~Simulation() {}
    void Run()
    {
        while ( ! events.empty() && time < endTime )
        {
            EventPtr e = events.top();
            time = e -> GetTime();
            events.pop();
            e -> Simulate( *this );
        }
    }
    Time GetTime() const
    {
        return time;
    }
    void Schedule( boost::function<void (void)> event, Time t )
    {
        assert( t > time );
        events.push( EventPtr( new Event( event, t ) ) );
    }
private:
    typedef std::priority_queue< EventPtr, std::vector< EventPtr >, CompareEvent > EventQueue;
    EventQueue events;
    Time time;
    const Time endTime;
};

class RandomTime
{
public:
    static RandomTime& Instance() { return instance; }
    Time Arrival( double lambda )
    {
        boost::random::exponential_distribution<> distribution( lambda );
        return distribution( randomGenerator );
    }
    Time Service( Time min, Time max )
    {
        boost::random::uniform_real_distribution<> distribution( min, max );
        return distribution( randomGenerator );
    }
private:
    RandomTime() {}
    static RandomTime instance;
    //boost::random::mt19937 randomGenerator;
    boost::random::random_device randomGenerator;
};

#endif
