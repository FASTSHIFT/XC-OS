# Arduino Async Duplex

This library allows you to asynchronously interact with any serial device
having a call-and-response style interface.

If you've ever used one of the many modem-handling libraries that exist,
you're familiar with the frustration that is waiting for a response from
a long-running command.  Between sending your command and receiving a
response (or worse -- that command timing out), your program is halted,
and your microcontroller is wasiting valuable cycles.  This library
aims to fix that problem by allowing you to queue commands that will
be asynchronously sent to your device without blocking your
microcontroller loop.

## Requirements

* std::functional: This is available in the standard library for most
  non-AVR Arduino cores, but should you be attempting to use this
  on an AVR microcontroller (e.g. an atmega328p), you may find what you
  need in this repository: https://github.com/SGSSGene/StandardCplusplus
* Regexp (https://github.com/nickgammon/Regexp)

## Examples

The following examples are based upon interactions with a SIM7000 LTE modem.

### Simple

Sending a command is as easy as queueing it:

```c++
#include <AsyncDuplex.h>
#include <Regexp.h>

AsyncDuplex handler = AsyncDuplex();

void setup() {
    handler.begin(&Serial1);

    handler.execute("AT");
}

void loop() {
    handler.loop();
}
```

But that isn't much more useful than just writing to the stream directly;
for more useful applications, keep reading.

### Independent

When executing multiple independent commands, you can follow the below
pattern:

```c++
#include <AsyncDuplex.h>
#include <Regexp.h>

AsyncDuplex handler = AsyncDuplex();

void setup() {
    handler.begin(&Serial);

    handler.execute(
        "AT+CCLK?",
        "+CCLK:.*\n",
    );
    handler.execute(
        "AT+CIPSTATUS",
        "STATE:.*\n"
    );
}

void loop() {
    handler.loop();
}
```

This pattern will work great for independent commands like the above, but
for a few reasons this isn't the recommended pattern to follow for
sequential steps that are dependent upon one another:

1. If one of these commands' expectations are not met (i.e. `AT+CIPSTART`
   in the examples below returns `ERROR` instead of `OK`), the subsequent
   commands will still be executed.
2. No guarantee is made that these will be executed sequentially.  More
   commands could be queued and inserted between the above commands if
   another function queues a high-priority (`AsyncDuplex::Timing::NEXT`)
   command.
3. There are a limited number of independent queue slots (by default: 5,
   but this value can be adjusted by changing `COMMAND_QUEUE_SIZE`).


### Sequential (Nested Callbacks)

This is a simplified overview of connecting to a TCP server using
a SIM7000 LTE modem.

1. Send `AT+CIPSTART...`; wait for `OK` followed by the line ending.
2. Send `AT+CIPSEND...`; wait for a `>` to be printed.
3. Send the data you want to send followed by CTRL+Z (`\x1a`).

The below commands will be executed sequentially and should any command's
expectations not be met, subsequent commands will not be executed.

```c++
#include <AsyncDuplex.h>
#include <Regexp.h>

AsyncDuplex handler = AsyncDuplex();

void setup() {
    handler.begin(&Serial);

    handler.execute(
        "AT+CIPSTART=\"TCP\",\"mywebsite.com\",\"80\"", // Command
        "OK\r\n",  // Expectation regex
        [&handler](MatchState ms) -> void {
            Serial.println("Connected");

            handler.execute(
                "AT+CIPSEND",
                ">",
                NEXT,
                [&handler](MatchState ms) -> void {
                    handler.execute(
                        "abc\r\n\x1a"
                        "SEND OK\r\n"
                        NEXT,
                    );
                }
            )
        }
    );
}


void loop() {
    handler.loop();
}
```

### Sequential (Chained)

Most of the time, you probably just want to run a few commands in sequence,
and the callback structure above may become tedious.  For sequential commands
that should be chained together (like above: aborting subsequent steps
should any command's expectations not be met), there is a simpler way
of handling these:

```c++
#include <AsyncDuplex.h>
#include <Regexp.h>

AsyncDuplex handler = AsyncDuplex();

void setup() {
    handler.begin(&Serial);

    AsyncDuplex::Command commands[] = {
        AsyncDuplex::Command(
            "AT+CIPSTART=\"TCP\",\"mywebsite.com\",\"80\"", // Command
            "OK\r\n",  // Expectation regex
            [](MatchState ms){
                Serial.println("Connected");
            }
        },
        AsyncDuplex::Command(
            "AT+CIPSEND",
            ">",
        ),
        AsyncDuplex::Command(
            "abc\r\n\x1a"
            "SEND OK\r\n"
        )
    }
    handler.executeChain(commands, 3);
}


void loop() {
    handler.loop();
}
```

The above is identical in function to the "Nested Callbacks" example earlier,
but using this pattern allows you define callbacks that are automatically
prepended to any callback that might have originally been defined for every
member of the chain:

```c++
#include <AsyncDuplex.h>
#include <Regexp.h>

AsyncDuplex handler = AsyncDuplex();

void setup() {
    handler.begin(&Serial);

    AsyncDuplex::Command commands[] = {
        AsyncDuplex::Command(
            "AT+CIPSTART=\"TCP\",\"mywebsite.com\",\"80\"", // Command
            "OK\r\n",  // Expectation regex
            [](MatchState ms){
                Serial.println("Connected");
            }
        },
        AsyncDuplex::Command(
            "AT+CIPSEND",
            ">",
        ),
        AsyncDuplex::Command(
            "abc\r\n\x1a"
            "SEND OK\r\n"
        )
    }
    handler.executeChain(
        commands,
        3,
        [](MatchState ms) { // Common success function
            // This will cause a message to be printed
            // after the completion of every command in the chain
            Serial1.println("Success!");
        },
        [](Command*) { // Common failure function
            // This will cause a message to be printed
            // after any member of the chain fails; this
            // might be a good place to put retry logic!
            Serial1.println("Failed!");
        }
    );
}


void loop() {
    handler.loop();
}
```

### Capture Groups

The below example will execute the relevant commands and, when the response
is received, set local variables using captured data.

```c++
#include <AsyncDuplex.h>
#include <Regexp.h>

AsyncDuplex handler = AsyncDuplex();

void setup() {
    handler.begin(&Serial);

    // Get the current timestamp
    time_t currentTime;
    handler.execute(
        "AT+CCLK?",
        "+CCLK: \"([%d]+)/([%d]+)/([%d]+),([%d]+):([%d]+):([%d]+)([\\+\\-])([%d]+)\"",
        [&currentTime](MatchState ms) {
            char year_str[3];
            char month_str[3];
            char day_str[3];
            char hour_str[3];
            char minute_str[3];
            char second_str[3];
            char zone_dir_str[2];
            char zone_str[3];

            ms.GetCapture(year_str, 0);
            ms.GetCapture(month_str, 1);
            ms.GetCapture(day_str, 2);
            ms.GetCapture(hour_str, 3);
            ms.GetCapture(minute_str, 4);
            ms.GetCapture(second_str, 5);
            ms.GetCapture(zone_dir_str, 6);
            ms.GetCapture(zone_str, 7);

            tmElements_t timeEts;
            timeEts.Hour = atoi(hour_str);
            timeEts.Minute = atoi(minute_str);
            timeEts.Second = atoi(second_str);
            timeEts.Day = atoi(day_str);
            timeEts.Month = atoi(month_str);
            timeEts.Year = (2000 + atoi(year_str)) - 1970;

            currentTime = makeTime(timeEts);
        }
    );

    char connectionStatus[10];
    handler.execute(
        "AT+CIPSTATUS",
        "STATE: (.*)\n"
        [&connectionStatus](MatchState ms) {
            ms.GetCapture(connectionStatus, 0);
        }
    );
}

void loop() {
    handler.loop();
}
```

### Failure Handling

You can pass a second function parameter to be executed should the request
timeout.  If you would like to retry the command (and subsequent commands
chained with it), you are able to do so.

```c++
#include <AsyncDuplex.h>
#include <Regexp.h>

AsyncDuplex handler = AsyncDuplex();

void setup() {
    handler.begin(&Serial);
    handler.execute(
        "AT+CIPSTART=\"TCP\",\"mywebsite.com\",\"80\"", // Command
        "OK\r\n",  // Expectation regex
        [](MatchState ms) -> void {
            Serial.println("Connected");
        },
        [&handler](AsyncDuplex::Command* cmd) -> void {  // Run this function on failure
            Serial.println("Connection failed; retrying");

            // Retry this immediately
            handler.execute(cmd, AsyncDuplex::Timing::NEXT);
        }
    );
}

void loop() {
    handler.loop();
}
```

If you only want to print to the console that an error occurred, you can
use the `AsyncDuplex::printFailure` helper:

```c++
#include <AsyncDuplex.h>
#include <Regexp.h>

AsyncDuplex handler = AsyncDuplex();

void setup() {
    handler.begin(&Serial);
    handler.execute(
        "AT+CIPSTART=\"TCP\",\"mywebsite.com\",\"80\"", // Command
        "OK\r\n",  // Expectation regex
        [](MatchState ms) -> void {
            Serial.println("Connected");
        },
        AsyncDuplex::printFailure(&Serial1), // Will print "Command 'AT+CIPSTART...' failed."
    );
}

void loop() {
    handler.loop();
}
```

### Timeouts

By default, commands time out after 2.5s (see `COMMAND_TIMEOUT`); sometimes
you may need to run a command that needs extra time to complete:

```c++
#include <AsyncDuplex.h>
#include <Regexp.h>

AsyncDuplex handler = AsyncDuplex();

void setup() {
    handler.begin(&Serial);
    handler.execute(
        "AT+CIPSTART=\"TCP\",\"mywebsite.com\",\"80\"", // Command
        "OK\r\n",  // Expectation regex
        NULL,
        NULL,
        10000  // Extended Timeout
    );
}

void loop() {
    handler.loop();
}
```

### Delaying

Occasionally, especially when chaining commands, you may need to ensure
that a subsequent command isn't executed immediately; in these cases,
you are able to set a delay:

```c++
#include <AsyncDuplex.h>
#include <Regexp.h>

AsyncDuplex handler = AsyncDuplex();

void setup() {
    handler.begin(&Serial);

    AsyncDuplex::Command commands[] = {
        AsyncDuplex::Command(
            "AT+CIPSTART=\"TCP\",\"mywebsite.com\",\"80\"", // Command
            "OK\r\n",  // Expectation regex
            [](MatchState ms){
                Serial.println("Connected");
            }
        },
        AsyncDuplex::Command(
            "AT+CIPSEND",
            ">",
            NULL,
            NULL,
            COMMAND_TIMEOUT,
            1000  // Wait for 1s before running this command 
        ),
        AsyncDuplex::Command(
            "abc\r\n\x1a"
            "SEND OK\r\n"
        )
    }
    handler.executeChain(commands, 3);
}


void loop() {
    handler.loop();
}
```

# Note from the author

I'm not a particularly great C++ programmer, and all of the projects
I work on using this language are ones I work on for fun in my free time.
It's very likely that you, dear reader, will have a better understanding
of either C++ or programming for microcontrollers in general and will find
ways of improving this that I either wouldn't have the skills to pull off
on my own, or wouldn't even have the awareness of to know how much better
things could be.  If any of those situations occur, please either reach out
on freenode -- I'm `coddingtonbear` there, too, search the issues list
on Github or create a new issue if one doesn't exist, or, better yet,
post a pull request making things better.  Cheers!
