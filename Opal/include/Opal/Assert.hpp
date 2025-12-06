#pragma once

#include <filesystem>

#ifdef OPAL_ENABLE_ASSERTS

    // The Implementation: Uses OPAL_ERROR with the passed 'logger'
    #define OPAL_INTERNAL_ASSERT_IMPL(logger, check, msg, ...) { \
        if(!(check)) { \
            OPAL_ERROR(logger, msg, __VA_ARGS__); \
            OPAL_DEBUGBREAK(); \
        } \
    }

    // Case: With Message (3+ arguments: logger, check, message, ...)
    //     We wrap the user's message in "Assertion failed: {0}"
    #define OPAL_INTERNAL_ASSERT_WITH_MSG(logger, check, ...) \
        OPAL_INTERNAL_ASSERT_IMPL(logger, check, "Assertion failed: {0}", __VA_ARGS__)

    // Case: No Message (2 arguments: logger, check)
    //     We generate a default message with file and line info
    #define OPAL_INTERNAL_ASSERT_NO_MSG(logger, check) \
        OPAL_INTERNAL_ASSERT_IMPL(logger, check, "Assertion '{0}' failed at {1}:{2}", OPAL_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

    // The Selector Logic
    //     We need to count arguments to decide between WITH_MSG and NO_MSG.
    //     We shifted the arguments by one compared to your old code because 'logger' is now always Arg 1.
    //     - If 2 args (logger, check) -> Arg3 matches NO_MSG
    //     - If 3 args (logger, check, msg) -> Arg3 matches msg, Arg4 matches WITH_MSG
    #define OPAL_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, arg3, macro, ...) macro

    #define OPAL_INTERNAL_ASSERT_GET_MACRO(...) \
        OPAL_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, OPAL_INTERNAL_ASSERT_WITH_MSG, OPAL_INTERNAL_ASSERT_NO_MSG)

    // The Main Macro
    //     Usage: OPAL_ASSERT(myLogger, x > 5);
    //     Usage: OPAL_ASSERT(myLogger, x > 5, "Value was {}", x);
    #define OPAL_ASSERT(...) OPAL_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(__VA_ARGS__)

#else

    #define OPAL_ASSERT(...)

#endif
