#ifndef UUID_HPP
#define UUID_HPP

#include <random>
#include <sstream>
#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>

namespace ft_irc {

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

    std::ostream& operator<<(std::ostream& os, const UUID& uuid) {
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

        UUIDGenerator(const UUIDGenerator& other) = delete;

        auto Generate() -> UUID {
            uint64_t low = _dis(_engine);
            uint64_t high = _dis(_engine);
            return UUID(low, high);
        }
    };
}

namespace std {
    template<>
    struct hash<ft_irc::UUID> {
        // TODO: is this okay? It discards upper 64 bits for hashing
        // But operator== ensures equality *won't* be preserved.
        std::size_t operator()(const ft_irc::UUID& uuid) const {
            return uuid.Low();
        }
    };
}

#endif // UUID_HPP
