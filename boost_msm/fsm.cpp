/*******************************************************************************
 * Copyright Daniele Pallastrelli 2014.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/
 
#include <iostream>
#include <cassert>
#include <boost/utility.hpp>

// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
// functors
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>

#include "fsm.h"

using namespace std;

//////////////////////

namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace boost::msm::front;

namespace FSM_impl {

// events:
struct Ev1 {};
struct Ev2 {};
struct Ev3 {};
struct Ev4 {};

class FSM_  : public msm::front::state_machine_def< FSM_ >
{
    public:

        // The list of FSM states

        struct State1 : public msm::front::state<> {};
        struct State2 : public msm::front::state<> {};
        struct State3 : public msm::front::state<> {};
        struct State4 : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& fsm) { cout << "entry State4/5" << endl; }

            template <class Event,class FSM>
            void on_exit(Event const&,FSM& fsm) { cout << "exit State4/5" << endl; }

            struct IntAct1
            {
                template <class EVT,class FSM,class SourceState,class TargetState>
                void operator()(EVT const& e,FSM& fsm,SourceState& ,TargetState& ) { cout << "internal action 1" << endl; }
            };

            struct IntAct2
            {
                template <class EVT,class FSM,class SourceState,class TargetState>
                void operator()(EVT const& e,FSM& fsm,SourceState& ,TargetState& ) { cout << "internal action 2" << endl; }
            };

            struct internal_transition_table : mpl::vector<
                     //    Event    Action     Guard
                Internal < Ev1    , IntAct1 ,  none  >,
                Internal < Ev2    , IntAct2 ,  none  >
                     //  +--------+---------+---------+
            > {};

        };

        struct State5 : public State4 {}; // this state is a copy of State4

        static std::string StateName( std::size_t index )
        {
            static char const* const state_names[] = { "State1", "State2", "State3", "State4", "State5" };
            assert( index < 5 );
            return state_names[ index ];
        }

         // the initial state of the SM. Must be defined
        typedef State1 initial_state;

        // transition actions
        template < class E > void Action1(E const&) { cout << "action 1" << endl; }
        template < class E > void Action2(E const&) { cout << "action 2" << endl; }
        template < class E > void Action3(E const&) { cout << "action 3" << endl; }
        template < class E > void Action4(E const&) { f -> Action(); }
        struct Action5
        {
            template <class EVT,class FSM,class SourceState,class TargetState>
            void operator()(EVT const& ,FSM& fsm,SourceState& ,TargetState& )
            {
                fsm.f -> Action();
            }
        };

        // guard conditions:
        template < class E > bool Predicate1( E const& ) { return f -> Guard(); }
        template < class E > bool Predicate2( E const& e ) { return !Predicate1( e ); }

        typedef FSM_ p; // makes transition table cleaner

        // Transition table:
        struct transition_table : mpl::vector<
            //      Start            Event              Next             Action               Guard
            //    +----------------+------------------+----------------+--------------------+------------------+
            row   < State1         , Ev1              , State2         , &p::Action1        , &p::Predicate1   >,
            row   < State1         , Ev1              , State4         , &p::Action2        , &p::Predicate2   >,
            row   < State1         , Ev2              , State2         , &p::Action3        , &p::Predicate1   >,
            row   < State1         , Ev2              , State5         , &p::Action4        , &p::Predicate2   >,
            //    +----------------+------------------+----------------+--------------------+------------------+
            //a_row < State2        , Ev3              , State1         , &p::Action4                           >,
            Row   < State2         , Ev3              , State1         , Action5            , none             >,
            //    +----------------+------------------+----------------+--------------------+------------------+
            _row  < State3         , Ev3              , State1                                                 >,
            //    +----------------+------------------+----------------+--------------------+------------------+
            a_row < State4         , Ev4              , State2         , &p::Action2                           >,
            _row  < State4         , Ev1              , State4                                                 >,
            //    +----------------+------------------+----------------+--------------------+------------------+
            a_row < State5         , Ev4              , State2         , &p::Action1                           >,
            _row  < State5         , Ev3              , State3                                                 >
            //    +----------------+------------------+----------------+--------------------+------------------+
        > {};

        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const& e, FSM&,int state )
        {
            cout << "FSM: no transition for event "
                 << typeid(e).name() << " in state " << state;
        }

        FSM_( Fsm* _f ) : f( _f ) {}
        
    private:
    
        Fsm* f;
};


typedef msm::back::state_machine< FSM_ > FSM;

} // namespace FSM_impl
    
//////////////////////

using namespace FSM_impl;

Fsm::Fsm() : fsm( new FSM( this ) )
{
    boost::static_pointer_cast< FSM > (fsm) -> start();
}

void Fsm::DoSomething()
{
    boost::static_pointer_cast< FSM > (fsm) -> process_event( Ev1() );
}

void Fsm::DoSomethingElse()
{
    boost::static_pointer_cast< FSM > (fsm) -> process_event( Ev2() );
}

bool Fsm::Guard()
{
    return true;
}

void Fsm::Action()
{
    cout << "action" << endl;
}

//////////////////////
