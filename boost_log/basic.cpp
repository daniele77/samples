/*******************************************************************************
 * Copyright Daniele Pallastrelli 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/
 
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

void Init()
{
    // add a file log
    logging::add_file_log
    (
        keywords::file_name = "basic_%N.log", // file name
        keywords::open_mode = std::ios_base::app, // append to old log file
        keywords::rotation_size = 10 * 1024 * 1024, // size rotation (10M)
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), // midnight rotation
        keywords::format = "[%TimeStamp%]: %Message%" // format
    );
    // add a console log
    logging::add_console_log
    (
        std::clog,
        keywords::format = "[%TimeStamp% - %ThreadID%]: %Message%", // format
        keywords::filter = ( logging::trivial::severity >= logging::trivial::warning ) // filter
    );

    // filters
    logging::core::get() -> set_filter
    (
        logging::trivial::severity >= logging::trivial::info
    );

    // adds the attributes "LineID", "TimeStamp", "ProcessID" and "ThreadID",
    // available in the format log
    logging::add_common_attributes();
}
 
int main(int, char*[])
{
    Init();

    BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
    BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
    BOOST_LOG_TRIVIAL(info) << "An informational severity message";
    BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
    BOOST_LOG_TRIVIAL(error) << "An error severity message";
    BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";

    return 0;
}