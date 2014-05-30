/*******************************************************************************
 * Copyright Daniele Pallastrelli 2014.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/
 
#ifndef FSM_H_
#define FSM_H_

#include <boost/shared_ptr.hpp>

namespace FSM_impl { class FSM_; }

class Fsm
{
public:
    Fsm();
    void DoSomething();
    void DoSomethingElse();
private:
    friend class FSM_impl::FSM_;
    bool Guard();
    void Action();
    // my state machine, hidden
    boost::shared_ptr<void> fsm;
};

#endif
