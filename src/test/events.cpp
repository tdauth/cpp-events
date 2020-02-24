#define BOOST_TEST_MODULE EventsTest

#include <boost/test/included/unit_test.hpp>

#include "../channel.hpp"

BOOST_AUTO_TEST_CASE(ChannelString)
{
	events::Channel<std::string> c;

	// Thread 1
	std::thread t1([&c] {
		auto e = c.sendEvt("Hello world!");

		bool result = e.sync();

		BOOST_REQUIRE(result);
	});

	auto e = c.recvEvt();
	const std::optional<std::string> &result = e.sync();

	BOOST_REQUIRE(result.has_value());
	BOOST_REQUIRE_EQUAL(result.value(), "Hello world!");

	t1.join();
}

BOOST_AUTO_TEST_CASE(ChannelStringClosed)
{
	events::Channel<std::string> c;
	c.close();

	auto e1 = c.recvEvt();
	const std::optional<std::string> &result1 = e1.sync();

	BOOST_REQUIRE(!result1.has_value());

	auto e2 = c.sendEvt("Hello world!");
	bool result2 = e2.sync();
	BOOST_REQUIRE(!result2);
}

BOOST_AUTO_TEST_CASE(EventAlways)
{
	events::Event<std::string> e1 = events::always<std::string>("Hello world!");
	BOOST_REQUIRE_EQUAL(e1.sync(), "Hello world!");

	events::Event<void> e2 = events::always();
	e2.sync();
}

BOOST_AUTO_TEST_CASE(EventWrap)
{
	events::Event<std::string> e1 = events::always<std::string>("Hello world!");
	events::Event<std::string> e2 = e1.wrap<std::string>(
	    [](const std::string &v) { return v + " It is a great day!"; });
	BOOST_REQUIRE_EQUAL(e2.sync(), "Hello world! It is a great day!");

	events::Event<void> e3 = events::always();
	events::Event<std::string> e4 =
	    e3.wrap<std::string>([] { return "It is a great day!"; });
	BOOST_REQUIRE_EQUAL(e4.sync(), "It is a great day!");
}

BOOST_AUTO_TEST_CASE(EventChooseString)
{
	events::Event<std::string> e1 = events::always<std::string>("A");
	events::Event<std::string> e2 = events::never<std::string>();
	events::Event<std::string> e3 = e1.choose(e2);

	BOOST_REQUIRE_EQUAL(e3.sync(), "A");
}

BOOST_AUTO_TEST_CASE(EventChooseVoid)
{
	events::Event<void> e1 = events::always();
	events::Event<void> e2 = events::never<void>();
	events::Event<void> e3 = e1.choose(e2);

	e3.sync();
}
