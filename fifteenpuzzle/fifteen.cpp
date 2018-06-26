// fifteen puzzle

#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>

// #define DUMP

template <class State>
class BreadthFirst
{
public:
    BreadthFirst(const State& start, const State& _end) : end(_end)
    {
        open.push(start);        
    }
    bool Solve()
    {
        do
        {
            auto current = open.front();
            open.pop();
            if ( NextStep(current) )
            {
                solution = current;
                return true;
            }
        }
        while(!open.empty());

        return false;
    }
    State Solution() const { return solution; }
private:
    bool NextStep(const State& current)
    {
#ifdef DUMP        
        std::cout << "\n********* new iteration *************\n" << std::endl;
        std::cout << "current:\n";
        current.Print();
#endif

        if (current == end)
            return true; // found
        closed.push_back(current);
        auto nextStates = current.Next();

#ifdef DUMP
        std::cout << "\nnext:\n";
#endif

        for (auto s: nextStates)
        {
            if ( std::find(closed.begin(), closed.end(), s) == closed.end() ) // not visited yet
            {
#ifdef DUMP
                s.Print();
#endif
                open.push(s);
            }
        }
        return false;
    }

    const State end;
    State solution;
    std::queue<State> open;
    std::vector<State> closed;
};

//

#include <array>
#include <cassert>
#include <iomanip>

class FState
{
public:
    FState() : configuration( {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0} )
    {
    }    
    FState(std::array<int, 16>&& s) : configuration(std::move(s)) {}
    FState(const FState& s) : configuration(s.configuration), moves(s.moves) {}
    FState(FState&& s) : configuration(std::move(s.configuration)), moves(std::move(s.moves)) {}
    FState& operator = (const FState& s)
    { 
        if (&s != this)
        {
            configuration = s.configuration;
            moves = s.moves;
        }
        return *this;
    }    
    std::vector<FState> Next() const
    {
        auto emptyIt = std::find(configuration.begin(), configuration.end(), 0);
        auto emptyPosition = emptyIt - configuration.begin();
        using V = std::vector<FState>;
        switch (emptyPosition)
        {
            case 0: return V{ Move(0, 1), Move(0, 4) }; break;
            case 1: return V{ Move(1, 0), Move(1, 5), Move(1, 2) }; break;
            case 2: return V{ Move(2, 1), Move(2, 6), Move(2, 3) }; break;
            case 3: return V{ Move(3, 2), Move(3, 7) }; break;
            case 4: return V{ Move(4, 0), Move(4, 5), Move(4, 8) }; break;
            case 5: return V{ Move(5, 1), Move(5, 4), Move(5, 6), Move(5, 9) }; break;
            case 6: return V{ Move(6, 2), Move(6, 5), Move(6, 7), Move(6, 10) }; break;
            case 7: return V{ Move(7, 3), Move(7, 6), Move(7, 11) }; break;
            case 8: return V{ Move(8, 4), Move(8, 9), Move(8, 12) }; break;
            case 9: return V{ Move(9, 5), Move(9, 8), Move(9, 10), Move(9, 13) }; break;
            case 10: return V{ Move(10, 6), Move(10, 9), Move(10, 11), Move(10, 14) }; break;
            case 11: return V{ Move(11, 7), Move(11, 10), Move(11, 15) }; break;
            case 12: return V{ Move(12, 8), Move(12, 13)}; break;
            case 13: return V{ Move(13, 12), Move(13, 9), Move(13, 14) }; break;
            case 14: return V{ Move(14, 13), Move(14, 10), Move(14, 15) }; break;
            case 15: return V{ Move(15, 11), Move(15, 14)}; break;
            default: assert(false);
        }
        return {};
    }
    bool operator == (const FState& other) const
    {
        return configuration == other.configuration;
    }
    void Print() const
    {
        std::cout << std::endl;
        PrintItem(0);
        PrintItem(1);
        PrintItem(2);
        PrintItem(3);
        std::cout << std::endl;
        PrintItem(4);
        PrintItem(5);
        PrintItem(6);
        PrintItem(7);
        std::cout << std::endl;
        PrintItem(8);
        PrintItem(9);
        PrintItem(10);
        PrintItem(11);        
        std::cout << std::endl;
        PrintItem(12);
        PrintItem(13);
        PrintItem(14);
        PrintItem(15);
        std::cout << std::endl;
    }
    void PrintMoves() const
    {
        for (auto move: moves)
            std::cout << move << ' ';
        std::cout << std::endl;
    }
private:
    void PrintItem(std::size_t pos) const
    {
        std::cout << std::setw(3);
        std::cout << configuration[pos];
    }
    FState Move(std::size_t pivot, std::size_t item) const
    {
        auto cfg = *this;
        std::swap(cfg.configuration[pivot], cfg.configuration[item]);
        cfg.moves.push_back(item);
        return cfg;
    }

    std::array<int, 16> configuration;
    std::vector<int> moves;
};

#include <boost/chrono/chrono.hpp>
#include <boost/chrono/process_cpu_clocks.hpp>

using namespace boost::chrono;

int main()
{
    //const FState start( std::array<int, 16>({1,12,6,4,9,7,11,10,15,3,2,13,5,8,14,0}) );
    const FState start( std::array<int, 16>({2,3,7,4,1,0,11,8,5,6,10,12,9,13,14,15}) );
    const FState goal;
    BreadthFirst<FState> search(start, goal);
    
    auto t0 = process_user_cpu_clock::now(); // boost
    
    bool found = search.Solve();

    auto t1 = process_user_cpu_clock::now(); // boost   
    using ms = boost::chrono::milliseconds;
    ms d = boost::chrono::duration_cast<ms>(t1-t0); 
    std::cout << d.count() << " ms" << std::endl;

    if (found)
    {
        std::cout << "Solution found:" << std::endl;
        search.Solution().PrintMoves();        
    }
    else
    {
        std::cout << "Solution not found!" << std::endl;
    }

    return 0;
}