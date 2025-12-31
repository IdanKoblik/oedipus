#pragma once

namespace listener {

    struct IListenerBase {
        virtual ~IListenerBase() = default;
    };

    template<typename E>
    struct IListener : IListenerBase {
        virtual void handle(const E&) = 0;
    };

}
