#ifndef LISTENER_H
#define LISTENER_H

namespace listener {

    struct IListenerBase {
        virtual ~IListenerBase() = default;
    };

    template<typename E>
    struct IListener : IListenerBase {
        virtual void handle(const E&) = 0;
    };

}

#endif // LISTENER_H