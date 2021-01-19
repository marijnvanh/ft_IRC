#ifndef UUID_HPP
#define UUID_HPP

#include <random>
#include <sstream>

namespace ft_irc {

    const auto hex_digits = "0123456789abcdef";

    class UUID {
        std::uint64_t _low;
        std::uint64_t _high;
    public:
        UUID(std::uint64_t low, std::uint64_t high) : _low(low), _high(high) {};

        auto ToString() -> std::string {
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

        auto Generate() -> UUID {
            uint64_t low = _dis(_engine);
            uint64_t high = _dis(_engine);
            return UUID(low, high);
        }
    };
}

#endif // UUID_HPP
