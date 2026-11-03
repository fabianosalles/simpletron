#include <gtest/gtest.h>
#include "events.h"


TEST(EventBus, SubscribeAndPublishLogEvent) {
	EventBus bus;
	bool eventReceived = false;
	LogEvent testEvent{ LogLevel::INFO, "Test log message" };

	bus.subscribe<LogEvent>([&eventReceived, &testEvent](const LogEvent& event) {		
		EXPECT_EQ(event.level, testEvent.level);
		EXPECT_EQ(event.message, testEvent.message);

		eventReceived = true;
	});


	bus.publish(testEvent);

	EXPECT_TRUE(eventReceived);
}