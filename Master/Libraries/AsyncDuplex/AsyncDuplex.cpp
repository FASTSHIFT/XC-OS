#include <functional>

#include <Arduino.h>
#undef min
#undef max
#include <Regexp.h>

#include "AsyncDuplex.h"

#ifdef ASYNC_DUPLEX_DEBUG_COUT
    #include <iostream>
#endif

AsyncDuplex::Command::Command() {}

AsyncDuplex::Command::Command(
    const char* _cmd,
    const char* _expect,
    std::function<void(MatchState)> _success,
    std::function<void(Command*)> _failure,
    uint16_t _timeout,
    uint32_t _delay
) {
    strncpy(command, _cmd, MAX_COMMAND_LENGTH - 1);
    command[MAX_COMMAND_LENGTH - 1] = '\0';
    strncpy(expectation, _expect, MAX_EXPECTATION_LENGTH - 1);
    command[MAX_EXPECTATION_LENGTH - 1] = '\0';
    success = _success;
    failure = _failure;
    timeout = _timeout;
    delay = _delay;
}

AsyncDuplex::Hook::Hook() {}

AsyncDuplex::Hook::Hook(
    const char* _expect,
    std::function<void(MatchState)> _success
) {
    strncpy(expectation, _expect, MAX_EXPECTATION_LENGTH - 1);
    success = _success;
}

AsyncDuplex::AsyncDuplex(){}

bool AsyncDuplex::begin(Stream* _stream, Stream* _errorStream) {
    stream = _stream;
    errorStream = _errorStream;
    began = true;

    // Sublcasses may perform additional actions here that may not
    // be successful; in this base class' case, this isn't variable
    return true;
}

bool AsyncDuplex::wait(uint32_t _timeout, std::function<void()> feed_watchdog) {
    uint32_t started = millis();

    while(queueLength > 0) {
        if(_timeout && (millis() > started + _timeout)) {
            // Wait timeout
            return false;
        }
        if(feed_watchdog) {
            feed_watchdog();
        }
        loop();
    }
    return true;
}

bool AsyncDuplex::abort() {
    if(queueLength > 0) {
        #ifdef ASYNC_DUPLEX_DEBUG
            AsyncDuplex::debugMessage("\t<Command Aborted>");
        #endif

        shiftLeft();
        inputBuffer[0] = '\0';
        bufferPos = 0;
        processing=false;

        return true;
    } else {
        return false;
    }
}

bool AsyncDuplex::execute(
    const char *_command,
    const char *_expectation,
    AsyncDuplex::Timing _timing,
    std::function<void(MatchState)> _success,
    std::function<void(Command*)> _failure,
    uint16_t _timeout,
    uint32_t _delay
) {
    if(queueLength == COMMAND_QUEUE_SIZE) {
        return false;
    }

    uint8_t position = 0;
    if(_timing == ANY) {
        position = queueLength;
        queueLength++;
    } else {
        shiftRight();
    }

    if(strlen(_command) > MAX_COMMAND_LENGTH - 1) {
        #ifdef ASYNC_DUPLEX_DEBUG
            AsyncDuplex::debugMessage("\t<Command Rejected>");
        #endif
        return false;
    }
    if(strlen(_expectation) > MAX_EXPECTATION_LENGTH - 1) {
        #ifdef ASYNC_DUPLEX_DEBUG
            AsyncDuplex::debugMessage("\t<Expectation Rejected>");
        #endif
        return false;
    }

    strcpy(commandQueue[position].command, _command);
    strcpy(commandQueue[position].expectation, _expectation);
    commandQueue[position].success = _success;
    commandQueue[position].failure = _failure;
    commandQueue[position].timeout = _timeout;

    // Once queued, the delay signifies the point in time at
    // which this task can begin being processed
    commandQueue[position].delay = _delay + millis();

    return true;
}

bool AsyncDuplex::execute(
    const char *_command,
    const char *_expectation,
    std::function<void(MatchState)> _success,
    std::function<void(Command*)> _failure,
    uint16_t _timeout,
    uint32_t _delay
) {
    return AsyncDuplex::execute(
        _command,
        _expectation,
        AsyncDuplex::Timing::ANY,
        _success,
        _failure,
        _timeout,
        _delay
    );
}

bool AsyncDuplex::execute(
    const Command* cmd,
    Timing _timing
) {
    return AsyncDuplex::execute(
        cmd->command,
        cmd->expectation,
        _timing,
        cmd->success,
        cmd->failure,
        cmd->timeout
    );
}

bool AsyncDuplex::executeChain(
    const Command* cmdArray,
    uint16_t count,
    Timing _timing,
    std::function<void(MatchState)> _success,
    std::function<void(Command*)> _failure
) {
    if(count < 2) {
        return false;
    }

    Command scratch;
    Command chain = cmdArray[count - 1];
    AsyncDuplex::prependCallback(&chain, _success, _failure);

    for(int16_t i = count - 2; i >= 0; i--) {
        AsyncDuplex::copyCommand(
            &scratch,
            &cmdArray[i]
        );
        AsyncDuplex::prependCallback(&scratch, _success, _failure);
        AsyncDuplex::createChain(
            &scratch,
            &chain
        );
        AsyncDuplex::copyCommand(
            &chain,
            &scratch
        );
    }
    return AsyncDuplex::execute(
        &chain,
        _timing
    );
}

bool AsyncDuplex::executeChain(
    const Command* cmdArray,
    uint16_t count,
    std::function<void(MatchState)> _success,
    std::function<void(Command*)> _failure
) {
    AsyncDuplex::executeChain(
        cmdArray,
        count,
        AsyncDuplex::Timing::ANY,
        _success,
        _failure
    );
}

void AsyncDuplex::createChain(Command* dest, const Command* toChain) {
    Command chained;
    copyCommand(&chained, toChain);

    std::function<void(MatchState)> originalSuccess = dest->success;
    dest->success = [this, chained, originalSuccess](MatchState ms){
        if(originalSuccess) {
            originalSuccess(ms);
        }
        AsyncDuplex::execute(
            &chained,
            Timing::NEXT
        );
    };
}

void AsyncDuplex::copyCommand(Command* dest, const Command* src) {
    strcpy(dest->command, src->command);
    strcpy(dest->expectation, src->expectation);
    dest->success = src->success;
    dest->failure = src->failure;
    dest->timeout = src->timeout;
    dest->delay = src->delay;
}

void AsyncDuplex::prependCallback(
    Command* cmd, 
    std::function<void(MatchState)> _success,
    std::function<void(Command*)> _failure
) {
    if(_success) {
        std::function<void(MatchState)> originalFn = cmd->success;
        cmd->success = [_success, originalFn](MatchState ms){
            _success(ms);
            if(originalFn) {
                originalFn(ms);
            }
        };
    }
    if(_failure) {
        std::function<void(Command*)> originalFn = cmd->failure;
        cmd->failure = [_failure, originalFn](Command* cmd){
            _failure(cmd);
            if(originalFn) {
                originalFn(cmd);
            }
        };
    }
}

void AsyncDuplex::loop(){
    if(!began) {
        return;
    }
    if(processing && (timeout < millis())) {
        #ifdef ASYNC_DUPLEX_DEBUG
            String nonMatching = String(inputBuffer);
            nonMatching.trim();
            AsyncDuplex::debugMessage(
                "\t<-- " + nonMatching
            );
            AsyncDuplex::debugMessage("\t<Command Timeout>");
        #endif

        Command failedCommand;
        AsyncDuplex::copyCommand(&failedCommand, &commandQueue[0]);
        std::function<void(Command*)> fn = failedCommand.failure;
        if(fn) {
            // Clear delay settings before handing to error
            // handler callback to prevent erroneously delaying
            // for forty years if the error handler tries to retry
            failedCommand.delay = 0;
            fn(&failedCommand);
        }

        shiftLeft();
        inputBuffer[0] = '\0';
        bufferPos = 0;
        processing=false;
    }
    while(stream->available()) {
        bool shouldRunHooks = false;
        uint8_t received = stream->read();
        if(received != '\0') {
            if(received == '\n') {
                // If we've found a line ending, we should plan to run
                // any registered hooks so they can check for unsolicited
                // data that might be useful.
                shouldRunHooks = true;
            }
            if(bufferPos + 1 == INPUT_BUFFER_LENGTH) {
                for(int32_t i = INPUT_BUFFER_LENGTH - 1; i > 0; i--) {
                    inputBuffer[i-1] = inputBuffer[i];
                }
                bufferPos--;
            }
            inputBuffer[bufferPos++] = received;
            inputBuffer[bufferPos] = '\0';
        }
        #ifdef ASYNC_DUPLEX_DEBUG
        #ifdef ASYNC_DUPLEX_DEBUG_VERBOSE
            AsyncDuplex::debugMessage(
                "\t  = (" + String(bufferPos) + ") \"" + String(inputBuffer) + "\""
            );
        #endif
        #endif

        if(processing) {
            MatchState ms;
            ms.Target(inputBuffer);
            char result = ms.Match(commandQueue[0].expectation);
            if(result) {
                #ifdef ASYNC_DUPLEX_DEBUG
                    String src = String(ms.src);
                    src.trim();
                    AsyncDuplex::debugMessage(
                        "\t<-- " + src
                    );
                    AsyncDuplex::debugMessage(
                        "\t<Expectation Matched>"
                    );
                #endif

                processing=false;

                std::function<void(MatchState)> fn = commandQueue[0].success;
                shiftLeft();
                if(fn) {
                    fn(ms);
                }
                stripMatchFromInputBuffer(ms);
            }
        }

        if(shouldRunHooks) {
            runHooks();
        }
    }
    if(!processing && queueLength > 0 && commandQueue[0].delay <= millis()) {
        #ifdef ASYNC_DUPLEX_DEBUG
            AsyncDuplex::debugMessage("\t--> " + String(commandQueue[0].command));
        #endif
        inputBuffer[0] = '\0';
        bufferPos = 0;
        stream->println(commandQueue[0].command);
        stream->flush();
        processing = true;
        timeout = millis() + commandQueue[0].timeout;
    }
}

bool AsyncDuplex::registerHook(
    const char *_expectation,
    std::function<void(MatchState)> _success
) {
    if(hookCount == MAX_HOOK_COUNT) {
        #ifdef ASYNC_DUPLEX_DEBUG
            AsyncDuplex::debugMessage("\t<Hook Rejected>");
        #endif
        return false;
    }
    if(strlen(_expectation) + 1 > MAX_EXPECTATION_LENGTH) {
        #ifdef ASYNC_DUPLEX_DEBUG
            AsyncDuplex::debugMessage("\t<Expectation Rejected>");
        #endif
        return false;
    }
    strcpy(hooks[hookCount].expectation, _expectation);
    hooks[hookCount].success = _success;
    hookCount++;

    return true;
}

void AsyncDuplex::runHooks() {
    for(uint8_t i = 0; i < hookCount; i++) {
        Hook hook = hooks[i];

        MatchState ms;
        ms.Target(inputBuffer);

        char result = ms.Match(hook.expectation);
        if(result) {
            #ifdef ASYNC_DUPLEX_DEBUG
                String src = String(ms.src);
                src.trim();
                AsyncDuplex::debugMessage(
                    "\t<-- " + src
                );
                AsyncDuplex::debugMessage(
                    "\t<Hook Triggered>"
                );
            #endif
            hook.success(ms);
        }
    }
}

uint8_t AsyncDuplex::getQueueLength() {
    return queueLength;
}

void AsyncDuplex::getResponse(char* buffer, uint16_t length) {
    strncpy(buffer, inputBuffer, length);
}

void AsyncDuplex::shiftRight() {
    for(int8_t i = 0; i < queueLength - 1; i++) {
        AsyncDuplex::copyCommand(&commandQueue[i+1], &commandQueue[i]);
    }
    queueLength++;
}

void AsyncDuplex::shiftLeft() {
    for(int8_t i = queueLength - 1; i > 0; i--) {
        AsyncDuplex::copyCommand(&commandQueue[i-1], &commandQueue[i]);
    }
    queueLength--;
}

std::function<void(AsyncDuplex::Command*)> AsyncDuplex::printFailure(Stream* stream) {
    return [stream](AsyncDuplex::Command* cmd) {
        stream->println(
            "Command '" + String(cmd->command) + "' failed."
        );
    };
}

void AsyncDuplex::stripMatchFromInputBuffer(MatchState ms) {
    uint16_t offset = ms.MatchStart + ms.MatchLength;
    for(uint16_t i = offset; i < INPUT_BUFFER_LENGTH; i++) {
        inputBuffer[i - offset] = inputBuffer[i];
        bufferPos = i - offset;
        // If we reached the end of the capture, we
        // do not need to copy anything further
        if(inputBuffer[i] == '\0') {
            break;
        }
    }
}

void AsyncDuplex::emitErrorMessage(const char *msg) {
    if(errorStream != NULL) {
        errorStream->println(msg);
        errorStream->flush();
    }
}

#ifdef ASYNC_DUPLEX_DEBUG
void AsyncDuplex::debugMessage(String msg) {
    #ifdef ASYNC_DUPLEX_DEBUG_COUT
        std::cout << msg;
        std::cout << "\n";
    #endif
    #ifdef ASYNC_DUPLEX_DEBUG_STREAM
        AsyncDuplex::emitErrorMessage(msg.c_str());
    #endif
}

void AsyncDuplex::debugMessage(const char *msg) {
    #ifdef ASYNC_DUPLEX_DEBUG_COUT
        std::cout << msg;
        std::cout << "\n";
    #endif
    #ifdef ASYNC_DUPLEX_DEBUG_STREAM
        AsyncDuplex::emitErrorMessage(msg);
    #endif
}
#endif

inline int AsyncDuplex::available() {
    return stream->available();
}

inline size_t AsyncDuplex::write(uint8_t bt) {
    return stream->write(bt);
}

inline int AsyncDuplex::read() {
    return stream->read();
}

inline int AsyncDuplex::peek() {
    return stream->peek();
}

inline void AsyncDuplex::flush() {
    return stream->flush();
}
