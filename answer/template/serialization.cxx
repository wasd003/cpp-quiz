#include <quiz/base.h>
#include <concepts>
#include <cstring>
#include <span>

constexpr auto succ_code = std::errc{};

constexpr auto success(std::errc code)
{
    return std::errc{} == code;
}

constexpr auto failure(std::errc code)
{
    return std::errc{} != code;
}

struct input {};
struct output {};

struct address_book {
    enum class phone_type : int {
        home,
        work,
    };
    struct phone_number {
        static auto serialize(auto& archive, auto& self) {
            return archive(self.number, self.type);
        }
        phone_number(const std::string& number, phone_type type) : number(number), type(type) {}
        phone_number() = default;
        std::string number;
        phone_type type;
    };
    struct person {
        static auto serialize(auto& archive, auto& self) {
            return archive(self.name, self.id, self.email, self.phones);
        }
        person(const std::string& name, int id, const std::string& email, std::vector<phone_number> phones)
            : name(name), id(id), email(email), phones(std::move(phones)) {}
        person() = default;
        std::string name;
        int id;
        std::string email;
        std::vector<phone_number> phones;
    };
    static auto serialize(auto& archive, auto& self) {
        return archive(self.people);
    }
    std::vector<person> people;

    friend std::ostream& operator<<(std::ostream& os, const address_book& book) {
        for (const auto& person : book.people) {
            os << "Name: " << person.name << ", ID: " << person.id << ", Email: " << person.email << '\n';
            for (const auto& phone : person.phones) {
                os << "Phone: " << phone.number << ", Type: " << (phone.type == phone_type::work ? "work" : "home") << '\n';
            }
        }
        return os;
    }
};

/// Kind can be either input or output
template <typename ByteView, typename Kind>
class archive {
private:
    ByteView& m_buf;
    std::size_t m_position{};

public:
    explicit archive(ByteView& data, Kind) : m_buf(data) {}

    auto position() const { return m_position; }

    auto operator()(auto&&... objects) { return archive_many(objects...); }

private:
    auto archive_many(auto& first, auto&&... remains) {
        if (auto result = archive_one(first); failure(result)) {
            return result;
        }
        return archive_many(remains...);
    }
    auto archive_many() { return succ_code; }

    auto archive_one(auto&& object) {
        using type = std::remove_cvref_t<decltype(object)>;

        if constexpr (std::is_fundamental_v<type> || std::is_enum_v<type>) {
            /// serialize trivial copyable types
            return archive_trivially(object);
        } else if constexpr (requires {
                                 type{}.data();
                                 type{}.size();
                             }) {
            /// serialize contiguous memory containers
            auto size = object.size();
            if (auto result = archive_one(size); failure(result)) {
                // write size
                return result;
            }
            if constexpr (std::same_as<Kind, input>) {
                object.resize(size);
            }
            for (auto& element : object) {
                // write each element
                if (auto result = archive_one(element); failure(result)) {
                    return result;
                }
            }
            return succ_code;
        } else if constexpr (requires { type::serialize(*this, object); }) {
            return type::serialize(*this, object);
        } else {
            static_assert(std::is_void_v<type>, "Currently Unsupported");
        }
    }

    auto archive_trivially(auto&& object) {
        if (sizeof(object) > m_buf.size() - m_position) {
            return std::errc::result_out_of_range;
        }
        if constexpr (std::same_as<Kind, output>) {
            memcpy(m_buf.data() + m_position, &object, sizeof(object));
        } else if constexpr (std::same_as<Kind, input>) {
            memcpy(&object, m_buf.data() + m_position, sizeof(object));
        }
        m_position += sizeof(object);
        return succ_code;
    }
};

std::size_t serialize(const address_book& book, std::span<std::byte> data) {
    archive archive{data, output{}};
    if (failure(archive(book))) {
        return 0;
    }
    return archive.position();
}

std::size_t deserialize(address_book& book, std::span<const std::byte> data) {
    archive archive{data, input{}};
    if (failure(archive(book))) {
        return 0;
    }
    return archive.position();
}

int main() {
    address_book book;
    book.people.emplace_back("John Doe", 1234, "abc.gmail.com", 
        std::vector { 
            address_book::phone_number {"123", address_book::phone_type::home},
            address_book::phone_number {"456", address_book::phone_type::work},
        }
    );

    std::cout << book << '\n';
    std::vector<std::byte> data(1024);
    if (auto ret = serialize(book, data); !ret) {
        std::cout << "Failed to serialize\n";
    } else {
        std::cout << "Serialized " << ret << " bytes\n";
    }

    std::cout << "=====================================" << std::endl;

    address_book book2;
    if (auto ret = deserialize(book2, data); !ret) {
        std::cout << "Failed to deserialize\n";
    } else {
        std::cout << "Deserialized " << ret << " bytes\n";
    }
    std::cout << book2 << '\n';
    return 0;
}