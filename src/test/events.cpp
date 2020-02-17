#define BOOST_TEST_MODULE EventsTest

#include <boost/test/included/unit_test.hpp>

#include "../channel.hpp"

BOOST_AUTO_TEST_CASE(UnbufferedChannelString)
{
	events::Channel<std::string> c(0);
	// TODO Complete the test
}
