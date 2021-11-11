// iniConfig.hpp
// Loads/Serialises INI config values. Allows for easy, STL-like interface for retrieving values
#pragma once
#include <string>
#include <string_view>
#include <robin_hood.h>

class iniConfig
    {
        private:
            struct value
                {
                    private:
                        std::string m_value = "";

                        // if we can infer the stored type, we store it here for fast conversions
                        union {
                            bool BOOL;
                            char CHAR;
                            short SHORT;
                            int INT;
                            unsigned int UNSIGNED_INT;
                            long long LONG_LONG;
                            unsigned long long UNSIGNED_LONG_LONG;
                            float FLOAT;
                            double DOUBLE;
                        } m_storedValueData;

                        enum class valueType
                            {
                                UNKNOWN				= 0,
                                STRING				= 1 << 0,
                                BOOL				= 1 << 1,
                                CHAR				= 1 << 2,
                                SHORT				= 1 << 3,
                                INT					= 1 << 4,
                                UNSIGNED_INT		= 1 << 5,
                                LONG_LONG			= 1 << 6,
                                UNSIGNED_LONG_LONG	= 1 << 7,
                                FLOAT				= 1 << 8,
                                DOUBLE				= 1 << 9
                            } m_storedValue = valueType::UNKNOWN;

                        template<typename T>
                        T convertType() const;

                        friend class iniConfig;

                    public:
                        // set and infer the type from a string. Attempts to parse the string and determine most likely type
                        void setAndInfer(std::string_view value);

                        std::string_view asString() const;
                        bool asBool() const;
                        char asChar() const;
                        short asShort() const;
                        int asInt() const;
                        unsigned int asUnsignedInt() const;
                        long long asLongLong() const;
                        unsigned long long asUnsignedLongLong() const;
                        float asFloat() const;
                        double asDouble() const;

                        operator std::string_view() const;
                        operator bool() const;
                        operator char() const;
                        operator short() const;
                        operator int() const;
                        operator unsigned int() const;
                        operator long long() const;
                        operator unsigned long long() const;
                        operator float() const;
                        operator double() const;

                        value &operator=(const value &rhs);
                        value &operator=(std::string_view rhs);
                        value &operator=(const char *rhs);
                        value &operator=(bool rhs);
                        value &operator=(char rhs);
                        value &operator=(short rhs);
                        value &operator=(int rhs);
                        value &operator=(unsigned int rhs);
                        value &operator=(long long rhs);
                        value &operator=(unsigned long long rhs);
                        value &operator=(float rhs);
                        value &operator=(double rhs);
                };

            struct section
                {
                    private:
                        robin_hood::unordered_flat_map<std::string, value> m_keyValuePairs;

                        friend class iniConfig;

                    public:
                        const robin_hood::unordered_flat_map<std::string, value> &keyValuePairs = m_keyValuePairs;

                        section() = default;
                        section(const section &rhs);
                        section(const section &&rhs);

                        section &operator=(const section &rhs);
                        section &operator=(const section &&rhs);

                        value &operator[](const std::string& key);
                        const value &operator[](const std::string& key) const;

                        bool has(const std::string &key) const;
                };

            robin_hood::unordered_map<std::string, section> m_sections;

        public:
            const robin_hood::unordered_map<std::string, section> &sections = m_sections;
            unsigned char indentLength = 2;

            iniConfig() = default;
            iniConfig(std::string_view file);

            void load(std::string_view file);
            void save(std::string_view file) const;

            bool has(const std::string &key) const;

            section &operator[](const std::string& key);
            const section &operator[](const std::string &key) const;
    };

template<typename T>
inline T iniConfig::value::convertType() const
    {
        switch (m_storedValue)
            {
                case iniConfig::value::valueType::BOOL:
                    return static_cast<T>(m_storedValueData.BOOL);
                case iniConfig::value::valueType::CHAR:
                    return static_cast<T>(m_storedValueData.CHAR);
                case iniConfig::value::valueType::SHORT:
                    return static_cast<T>(m_storedValueData.SHORT);
                case iniConfig::value::valueType::INT:
                    return static_cast<T>(m_storedValueData.INT);
                case iniConfig::value::valueType::UNSIGNED_INT:
                    return static_cast<T>(m_storedValueData.UNSIGNED_INT);
                case iniConfig::value::valueType::LONG_LONG:
                    return static_cast<T>(m_storedValueData.LONG_LONG);
                case iniConfig::value::valueType::UNSIGNED_LONG_LONG:
                    return static_cast<T>(m_storedValueData.UNSIGNED_LONG_LONG);
                case iniConfig::value::valueType::FLOAT:
                    return static_cast<T>(m_storedValueData.FLOAT);
                case iniConfig::value::valueType::DOUBLE:
                    return static_cast<T>(m_storedValueData.DOUBLE);
                default:
                    break;
            }
        return T();
    }
