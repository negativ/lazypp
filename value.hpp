#pragma once

#include <functional>
#include <optional>

namespace lazy {

template <typename F>
class value final {
public:
    using type = std::invoke_result_t<F>;

    explicit value(F&& f):
        func_{std::forward<F>(f)}
    {}

    value(value&&) = default;
    value& operator=(value&&) = default;
    value(const value&) = delete;
    value& operator=(const value&) = delete;
    ~value() = default;

    template <typename T = type, std::enable_if_t<std::is_void_v<T>, int> = 0>
    void operator()() {
        if (holder_.has_value())
            return;

        func_();

        holder_.emplace(empty{});
    }

    template <typename T = type, std::enable_if_t<!std::is_void_v<T>, int> = 0>
    operator T() {
        if (holder_.has_value())
            return holder_.value();

        return holder_.emplace(func_());
    }

    template <typename T = type, std::enable_if_t<!std::is_void_v<T>, int> = 0>
    T operator()() {
        return this->operator T();
    }

    bool has_value() const {
        return holder_.has_value();
    }

private:
    struct empty{};

    using holder_t = std::conditional_t<std::is_void_v<type>,
                                        std::optional<empty>,
                                        std::conditional_t<std::is_reference_v<type>,
                                                           std::optional<std::reference_wrapper<std::remove_reference_t<type>>>,
                                                           std::optional<type>>>;
    F func_;
    holder_t holder_{};
};

}
