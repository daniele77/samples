#include <iostream>
#include "simulation.h"

// ---

class Customer
{
public:
    // arrivo di un cliente
    Customer( const Simulation* s ) : simulation( s )
    {
        enqueueTime = s -> GetTime();
        std::cout << enqueueTime << " - Nuovo customer (this=" << this << ")" << std::endl;
    }
    // entra in coda
    void Enqueue()
    {
        std::cout << enqueueTime << " - In coda customer (this=" << this << ")" << std::endl;
    }
    // inizia il servizio
    void Operation()
    { 
        ++customerServed; 
        queueSumTime += ( simulation -> GetTime() - enqueueTime );
        std::cout << simulation -> GetTime() << " - Operation (this=" << this << ")" << std::endl;
    }
    // finisce il servizio (esce dal sistema)
    ~Customer()
    {
        std::cout << simulation -> GetTime() << " - Fine servizio (this=" << this << ")" << std::endl;
    }
    static Time MeanQueueTime() { return queueSumTime / customerServed; }
private:
    const Simulation* simulation;
    Time enqueueTime;
    static Time queueSumTime;
    static size_t customerServed;
};

Time Customer::queueSumTime = 0.0;
size_t Customer::customerServed = 0;

typedef boost::shared_ptr< Customer > CustomerPtr;


class CustomerQueue
{
public:
    CustomerQueue( Simulation* s ) : simulation( s ), peak( 0 ), timeLastChange( 0.0 ), sum( 0.0 ) {}
    void Add( CustomerPtr customer )
    {
        UpdateStats();
        customer -> Enqueue();
        queue.push( customer );
        peak = std::max( peak, queue.size() );
    }
    CustomerPtr Next()
    {
        UpdateStats();
        CustomerPtr c = queue.front();
        queue.pop();
        return c;
    }
    bool Empty() const
    {
        return queue.empty();
    }
    void Dump() const
    {
        std::cout << "Queue max size: " << peak << std::endl;
        std::cout << "Queue mean size: " << sum / timeLastChange << std::endl;
    }
private:
    // dev'essere chiamata *prima* di cambiare la dimensione della coda!
    void UpdateStats()
    {
        const Time now = simulation -> GetTime();
        sum += ( ( now - timeLastChange ) * queue.size() );
        timeLastChange = now;
    }
    Simulation* simulation;
    std::queue< CustomerPtr > queue;
    size_t peak;
    Time timeLastChange;
    Time sum;
};

class Operator
{
public:
    Operator( Simulation* s, CustomerQueue* q ) : 
        simulation( s ),
        queue( q ),
        busyTime( 0.0 )
    {}
    void Serve( CustomerPtr customer )
    {
        std::cout << simulation -> GetTime() << " - Serve" << std::endl;
        startServiceTime = simulation -> GetTime();
        customer -> Operation();
        currentCustomer = customer;
        simulation -> Schedule( 
            boost::bind( &Operator::EndService, this ),
            simulation -> GetTime() + RandomTime::Instance().Service( startServiceRange, endServiceRange ) 
        );
    }
    bool Busy() const
    {
        return currentCustomer;
    }
    void Dump() const
    {
        std::cout << "Clerk Busy time = " << busyTime << std::endl;
        std::cout << "Clerk load = " << busyTime / simulation -> GetTime() << std::endl;
    }
private:
    void EndService()
    {
        std::cout << simulation -> GetTime() << " - EndService" << std::endl;
        busyTime += ( simulation -> GetTime() - startServiceTime );
        assert( currentCustomer );
        currentCustomer.reset();
        if ( ! queue -> Empty() )
            Serve( queue -> Next() );
    }
    Simulation* simulation;
    CustomerQueue* queue;
    CustomerPtr currentCustomer;
    Time busyTime;
    Time startServiceTime;
    static const Time startServiceRange;
    static const Time endServiceRange;
};

const Time Operator::startServiceRange = 2.0;
const Time Operator::endServiceRange = 6.0;


class Office
{
public:
    Office( Simulation* simulation ) : queue( simulation ), clerk( simulation, &queue ) {}
    void Arrive( CustomerPtr customer )
    {
        if ( clerk.Busy() )
            queue.Add( customer );
        else
            clerk.Serve( customer );
    }
    void Dump() const
    {
        queue.Dump();
        clerk.Dump();
        std::cout << "Mean customer wait time = " << Customer::MeanQueueTime() << std::endl;
    }
private:
    CustomerQueue queue;
    Operator clerk;
};


class OfficeSimulation : public Simulation
{
public:
    OfficeSimulation( Time endTime ) :
      Simulation( endTime ),
      office( this )
    {
        Schedule(
            boost::bind( &OfficeSimulation::Arrival, this ),
            RandomTime::Instance().Arrival( lambda )
        );
    }
    void Dump() const
    {
        office.Dump();
    }
private:
    void Arrival()
    {
        std::cout << GetTime() << " - Arrival" << std::endl;
        // inserisce il prossimo arrivo:
        Schedule(
            boost::bind( &OfficeSimulation::Arrival, this ),
            GetTime() + RandomTime::Instance().Arrival( lambda )
        );
        // gestisce questo arrivo:        
        CustomerPtr c = CustomerPtr( new Customer( this ) );
        office.Arrive( c );
    }

    Office office;
    static const double lambda;
};

const double OfficeSimulation::lambda = 0.1;


int main()
{
    OfficeSimulation s( 100000.0 );
    s.Run();
    s.Dump();
    system( "PAUSE" );
    return 0;
}

