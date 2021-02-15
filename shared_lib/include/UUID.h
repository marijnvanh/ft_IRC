#ifndef UUID_HPP
#define UUID_HPP

#include <random>
#include <sstream>
#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>

namespace IRC {

    const auto hex_digits = "0123456789abcdef";

    class UUID {
        std::uint64_t _low;
        std::uint64_t _high;
    public:
        UUID(std::uint64_t low, std::uint64_t high) : _low(low), _high(high) {};
        UUID(const UUID& other) = default;
        ~UUID() = default;

        constexpr auto Low() const -> std::uint64_t {
            return _low;
        }

        constexpr auto High() const -> std::uint64_t {
            return _high;
        }

        auto operator==(const UUID other) const -> bool {
            return (other.Low() == Low() && other.High() == High());
        }

        auto ToString() const -> std::string {
            size_t i;
            std::ostringstream ss;
            
            for (i = 0; i < 64; i += 4) {
                if (i == 32 || i == 48)
                    ss << "-";
                ss << hex_digits[_low >> i & 0xf];
            }
            ss << "-";
            for (i = 0; i < 64; i += 4) {
                if (i == 16)
                    ss << "-";
                ss << hex_digits[_high >> i & 0xf];
            }

            return ss.str();
        }

    };

    inline std::ostream& operator<<(std::ostream& os, const UUID& uuid) {
        os << "UUID(" << uuid.ToString() << ")";
        return os;
    }

    class UUIDGenerator {
        std::uniform_int_distribution<std::uint64_t> _dis;
        std::random_device _device;
        std::mt19937 _engine;
    public:
        UUIDGenerator() { 
            _dis = std::uniform_int_distribution<std::uint64_t>(
                std::numeric_limits<std::uint64_t>::min(),
                std::numeric_limits<std::uint64_t>::max()
            );
            _engine = std::mt19937(_device());
        }

        static auto GetInstance() -> UUIDGenerator& {
            static UUIDGenerator instance;

            return instance;
        }

        UUIDGenerator(const UUIDGenerator& other) = delete;
        UUIDGenerator& operator=(const UUIDGenerator& other) = delete;

        auto Generate() -> UUID {
            uint64_t low = _dis(_engine);
            uint64_t high = _dis(_engine);
            return UUID(low, high);
        }
    };
}

/*
 * What this actually does:
 * 
 * This allows us to use std::unordered_map<UUID, ...> because
 * it will under the hood hash the UUIDs using the function below.
 *
 * There is one caveat: it currently discards upper 64 bits for hashing.
 * However, operator== ensures equality *won't* be preserved, so collisions 
 * *might* still get detected
 */
namespace std {
    template<>
    struct hash<IRC::UUID> {
        std::size_t operator()(const IRC::UUID& uuid) const {
            return uuid.Low();
        }
    };
}

#endif // UUID_HPP
