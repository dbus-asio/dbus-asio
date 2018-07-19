#include "dbus.h"
#include <chrono>
#include <mutex>
#include <iostream>

// Invoke:
//    pummel-client [stubname] [iterations_per_thread] [threads]

using namespace std::chrono;

size_t gTotalSuccesses = 0;
std::mutex gMutexOutput;
std::mutex gMutexSuccessCount;
std::mutex gMutexReplyErrorCount;
std::mutex gMutexReplyResultCount;

bool testClient(const std::string& stubname, size_t iterations)
{
    DBus::Log::setLevel(DBus::Log::INFO);

    DBus::Native native(DBus::Platform::getSessionBus());
    std::this_thread::sleep_for(std::chrono::seconds(1));

    native.BeginAuth(DBus::AuthenticationProtocol::AUTH_BASIC);

    native.callHello([](const DBus::Message::MethodReturn& msg) { std::cerr << "Ready. Client on " << DBus::Type::asString(msg.getParameter(0)) << std::endl; },
        [](const DBus::Message::Error& msg) {
            std::cerr << "Failed to get hello message. Aborting early" << std::endl;
            return -1;
        });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    milliseconds ms_start = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch());

    size_t replies = 0;
    size_t correct = 0;
    size_t errors = 0;
    size_t sent = 0;
    milliseconds message_sleep = std::chrono::milliseconds(18);
    for (; sent < iterations; ++sent) {
        std::string expected(stubname);
        std::string number(std::to_string(sent));
        expected += " ";
        expected += number;

        DBus::Message::MethodCallParametersIn params;
        params.add(stubname);
        params.add(number);

        DBus::Message::MethodCall concat(DBus::Message::MethodCallIdentifier("/", "biz.brightsign.test", "concat"), params);
        native.sendMethodCall("biz.brightsign", concat,
            [&replies, &correct, expected](const DBus::Message::MethodReturn& msg) {
                std::lock_guard<std::mutex> guard(gMutexReplyResultCount);
                std::string result = DBus::Type::asString(msg.getParameter(0));
                ++replies;
                if (result == expected) {
                    ++correct;
                }
            },
            [&errors](const DBus::Message::Error& msg) {
                std::lock_guard<std::mutex> guard(gMutexReplyErrorCount);
                ++errors;
            });

        // If we have a short delay (e.g. 1ms) between messages, everything is happy for 10-10-100
        // If this is 10ms, then the timeouts below happen a lot more frequently.
        // ATM, I can't tell if this is coincidence, or intentional.
        std::this_thread::sleep_for(message_sleep);
    }

    // We don't get a callback upon timeout. We use a separate loop to check for that
    // posibility, so we don't hang in this method.
    // libdbus (as used by the daemon) will timeout a request after 25 seconds. So, if there's anything left
    // as that time, we're not going to get a reply, so timeout that and everything else.
    milliseconds timeout = std::chrono::seconds(45);
    milliseconds ms_timeout_starts = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    milliseconds ms_end;

    do {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        ms_end = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

        if (ms_end - ms_timeout_starts > timeout) {
            std::cerr << "Timing out..." << std::endl;
            break;
        }
    } while (sent != replies + errors);

    std::lock_guard<std::mutex> guard(gMutexOutput);

    std::cout << (correct == sent ? "SUCCESS" : "FAILURE");
    std::cout << " " << stubname << ":: Results ::";
    std::cout << "  Sent: " << sent;
    std::cout << "  Replies: " << replies;
    std::cout << "  Correct: " << correct;
    std::cout << "  Errors: " << errors;
    std::cout << "  Duration: " << std::to_string((ms_end - ms_start).count()) << "ms";
    std::cout << std::endl;

    std::cout << "  Parameters: " << std::endl;
    std::cout << "    Timeout: " << std::to_string(timeout.count()).c_str() << "ms" << std::endl;
    std::cout << "    Message sleep: " << std::to_string(message_sleep.count()) << "ms" << std::endl;
    std::cout << "    Iterations: " << iterations;
    std::cout << std::endl;

    std::cout << native.getStats();
    std::cout << std::endl;

    return correct == sent;
}

void threadTestClient(const std::string& stubname, int iterations)
{
    if (testClient(stubname, iterations)) {
        std::lock_guard<std::mutex> guard(gMutexSuccessCount);
        ++gTotalSuccesses;
    }
}

int main(int argc, const char* argv[])
{
    std::vector<std::thread> threads;
    std::string stubname(argc < 2 ? std::string("TEST") : (argv[1]));
    size_t iterations = argc < 3 ? 100 : atoi(argv[2]);
    size_t numThreads = argc < 4 ? 10 : atoi(argv[3]);

    for (size_t i = 0; i < numThreads; ++i) {
        std::string fullname(stubname);
        fullname += "_thread_";
        fullname += std::to_string(i);
        threads.emplace_back(std::thread(threadTestClient, fullname, iterations));
    }

    for (auto& th : threads) {
        th.join();
    }

    return gTotalSuccesses == numThreads ? 0 : -1;
}
