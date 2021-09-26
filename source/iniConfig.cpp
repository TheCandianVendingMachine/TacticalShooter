#include "iniConfig.hpp"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <charconv>
#include <cctype>
#include <cstdio>

iniConfig::iniConfig(std::string_view file)
	{
		load(file);
	}

void iniConfig::load(std::string_view file)
	{
		std::FILE *stream = nullptr;
		stream = std::fopen(file.data(), "r");
		if (!stream || std::ferror(stream))
			{
				spdlog::error("Cannot read file {} Reason: {}", file, std::strerror(errno));
				return;
			}

		enum class readState
			{
				PARSING,
				READING_SECTION,
				READING_KEY,
				READING_VALUE
			};

		std::string key;
		std::string currentData;

		// Value information
		constexpr int floatValues = 0b0'0'0'0000'11;
		constexpr int intValues = 0b0'1'0'1111'00;
		constexpr int numberValues = floatValues | intValues;
		constexpr int boolValues = 0b0'1'0'0000'00;
		constexpr int stringValues = 0b1'0'1'0000'00;

		int possibleValues = 0b1'1'1'1111'11;
		bool boolHasNonNumeric = false;
		bool hasDecimal = false;
		int power = 1;

		section *currentSection = nullptr;
		while (!std::feof(stream))
			{
				int next = 0;
				while ((next = std::getc(stream)) != EOF && (next == ' ' || next == '\t')) {}
				readState state = readState::PARSING;

				while (next != EOF && next != '\n')
					{
						switch (state)
							{
								case readState::PARSING:
									if (next == '[')
										{
											state = readState::READING_SECTION;
											break;
										}
									else if (next == ';') [[unlikely]]
										{
											while ((next = std::getc(stream)) != EOF && next != '\n') {}
											break;
										}
									else if (currentSection == nullptr) [[unlikely]]
										{
											spdlog::error("Unexpected character found before section while parsing file {}", file);
											std::fclose(stream);
											return;
										}
									else
										{
											state = readState::READING_KEY;
											break;
										}
									next = std::getc(stream);
									break;
								case readState::READING_SECTION:
									next = std::getc(stream);
									if (next == ']')
										{
											m_sections.emplace(currentData, section{});
											currentSection = &m_sections.at(currentData);
											currentData = "";

											if (currentSection == nullptr)
												{
													spdlog::error("Current section is nullptr while reading section {} in file {} -- this should never occur", currentData, file);
													std::fclose(stream);
													return;
												}

											next = std::getc(stream);
											state = readState::READING_KEY;
										}
									else if (next != '\n')
										{
											currentData += next;
										}
									else [[unlikely]]
										{
											spdlog::error("Found newline before section end while parsing file {} in partial-section name {}", file, currentData);
											std::fclose(stream);
											return;
										}
									break;
								case readState::READING_KEY: [[likely]]
									if (next == '=')
										{
											state = readState::READING_VALUE;

											possibleValues = 0b1'1'1'1111'11;
											boolHasNonNumeric = false;
											power = 1;
											hasDecimal = false;

											key = currentData;
											currentData = "";

											next = std::getc(stream);
											break;
										}

									currentData += next;
									next = std::getc(stream);
									break;
								case readState::READING_VALUE: [[likely]]
									// if the character is not a number, we are assuming the type isnt a number
									if ((possibleValues & numberValues) != 0)
										{
											if (!(next >= '0' && next <= '9') && !(currentData.size() == 0 && next == '-'))
												{
													if (next == '.' && !hasDecimal)
														{
															possibleValues ^= intValues;
															hasDecimal = true;
														}
													else
														{
															possibleValues ^= numberValues;
														}

													// if the character isn't in "false" or "true", or a number, it isnt a boolean
													if (next != 't' && next != 'r' && next != 'u' && next != 'e' &&
														next != 'f' && next != 'a' && next != 'l' && next != 's' && next != 'e')
														{
															possibleValues &= 0b1'0'1'1111'11;
															boolHasNonNumeric = true;
														}
												}
											power *= 10;
										}
									
									currentData += next;
									{
										next = std::getc(stream);
										if (next == EOF || next == '\n') [[unlikely]]
											{
												// push data

												value v;
												v.m_value = currentData;

												// attempt to infer data type for quick getting later
												if ((possibleValues & numberValues) != 0)
													{
														double numericValue = 0.0;
														int sign = 1;
														if (currentData[0] == '-')
															{
																sign = -1;
															}

														for (const auto &c : v.m_value)
															{
																if (c == '-' || c == '.') { continue; }
																power /= 10;
																numericValue += (c - '0') * power;
															}
														numericValue *= sign;

														if ((possibleValues & intValues) == intValues)
															{
																v.m_storedValue = value::valueType::LONG_LONG;
																v.m_storedValueData.LONG_LONG = static_cast<long long>(numericValue);
															}
														else if ((possibleValues & floatValues) == floatValues)
															{
																v.m_storedValue = value::valueType::DOUBLE;
																v.m_storedValueData.DOUBLE = numericValue;
															}
														else if ((possibleValues & boolValues) == boolValues)
															{
																v.m_storedValue = value::valueType::BOOL;
																if (boolHasNonNumeric)
																	{
																		v.m_storedValueData.BOOL = (currentData == "true");
																	}
																else
																	{
																		v.m_storedValueData.BOOL = (numericValue == 0);
																	}
						
															}
													}
												else
													{
														v.m_storedValue = value::valueType::STRING;
														if (currentData.size() == 1)
															{
																v.m_storedValue = value::valueType::CHAR;
																v.m_storedValueData.CHAR = currentData[0];
															}
													}

												(*currentSection)[key] = v;

												currentData = "";
											}
									}
									
									break;
								default:
									break;
							}
					}
			}

		std::fclose(stream);
	}

void iniConfig::save(std::string_view file) const
	{
		const std::string indentStr(indentLength, ' ');

		std::FILE *stream = nullptr;
		stream = std::fopen(file.data(), "w+");
		if (!stream)
			{
				spdlog::error("Cannot write file {}", file);
				return;
			}

		for (const auto &section : m_sections)
			{
				std::fprintf(stream, "[%s]\n", section.first.data());
				for (const auto &pair : section.second.m_keyValuePairs)
					{
						std::fprintf(stream, "%s%s=%s\n", indentStr.data(), pair.first.data(), pair.second.asString().data());
					}
				std::fprintf(stream, "\n");
			}

		std::fclose(stream);
	}

iniConfig::section &iniConfig::operator[](const std::string &key)
	{
		return m_sections[key];
	}

const iniConfig::section &iniConfig::operator[](const std::string &key) const
	{
		if (m_sections.find(key) == m_sections.end())
			{
				return section{};
			}
		return m_sections.at(key);
	}

iniConfig::section::section(const section &rhs)
	{
		*this = rhs;
	}

iniConfig::section::section(const section &&rhs)
	{
		*this = std::move(rhs);
	}

iniConfig::section &iniConfig::section::operator=(const section &rhs)
	{
		if (&rhs != this)
			{
				m_keyValuePairs = rhs.m_keyValuePairs;
			}

		return *this;
	}

iniConfig::section &iniConfig::section::operator=(const section &&rhs)
	{
		if (&rhs != this)
			{
				m_keyValuePairs = std::move(rhs.m_keyValuePairs);
			}

		return *this;
	}

iniConfig::value &iniConfig::section::operator[](const std::string &key)
	{
		return m_keyValuePairs[key];
	}

const iniConfig::value &iniConfig::section::operator[](const std::string &key) const
	{
		if (m_keyValuePairs.find(key) == m_keyValuePairs.end())
			{
				return value{};
			}
		return m_keyValuePairs.at(key);
	}

void iniConfig::value::setAndInfer(std::string_view valueStr)
	{
		m_value = valueStr;

		constexpr int floatValues = 0b0'0'0'0000'11;
		constexpr int intValues = 0b0'1'1'1111'00;
		constexpr int numberValues = floatValues | intValues;
		constexpr int boolValues = 0b0'1'0'0000'00;
		constexpr int stringValues = 0b1'0'1'0000'00;

		int possibleValues = 0b1'1'1'1111'11;

		bool hasNonNumeric = false;

		for (auto &c : m_value)
			{
				// if the character is not a number, we are assuming the type isnt a number
				if (!std::isdigit(c) && c != '.' && c != '-')
					{
						possibleValues &= 0b1'1'0'0000'00;
						hasNonNumeric = true;
					}

				if (c == '.')
					{
						possibleValues &= 0b1'0'1'0000'11;
					}

				// if the character isn't in "false" or "true", or a number, it isnt a boolean
				if (!std::isdigit(c) &&
					c != 't' && c != 'r' && c != 'u' && c != 'e' &&
					c != 'f' && c != 'a' && c != 'l' && c != 's' && c != 'e')
					{
						possibleValues &= 0b1'0'1'1111'11;
					}
			}

		if (!m_value.empty() && m_value != "." && m_value != "-" && (possibleValues & numberValues) != 0)
			{
				if ((possibleValues & intValues) == intValues)
					{
						long double value = 0.0;
						std::from_chars(m_value.data(), m_value.data() + m_value.size(), value);
						m_storedValue = valueType::LONG_LONG;
						m_storedValueData.LONG_LONG = static_cast<long long>(value);
					}
				else if ((possibleValues & floatValues) == floatValues)
					{
						long double value = 0.0;
						std::from_chars(m_value.data(), m_value.data() + m_value.size(), value);
						m_storedValue = valueType::DOUBLE;
						m_storedValueData.DOUBLE = value;
					}
				else if ((possibleValues & boolValues) == boolValues)
					{
						m_storedValue = valueType::BOOL;
						if (hasNonNumeric)
							{
								m_storedValueData.BOOL = (m_value == "true");
							}
						else
							{
								long double value = 0.0;
								std::from_chars(m_value.data(), m_value.data() + m_value.size(), value);
								m_storedValueData.BOOL = (value == 0);
							}
						
					}
			}
		else
			{
				m_storedValue = valueType::STRING;
				if (m_value.size() == 1)
					{
						m_storedValue = valueType::CHAR;
						m_storedValueData.CHAR = m_value[0];
					}
			}
	}

std::string_view iniConfig::value::asString() const
	{
		return m_value;
	}

bool iniConfig::value::asBool() const
	{
		if (m_storedValue != valueType::STRING && m_storedValue != valueType::UNKNOWN)
			{
				return convertType<bool>();
			}

		if (m_value == "true" || m_value == "false")
			{
				return m_value == "true";
			}

		long double value = 0.0;
		std::from_chars(m_value.data(), m_value.data() + m_value.size(), value);

		return static_cast<bool>(value);
	}

char iniConfig::value::asChar() const
	{
		if (m_storedValue != valueType::STRING && m_storedValue != valueType::UNKNOWN)
			{
				return convertType<char>();
			}

		long double value = 0.0;
		std::from_chars(m_value.data(), m_value.data() + m_value.size(), value);

		return static_cast<char>(value);
	}

short iniConfig::value::asShort() const
	{
		if (m_storedValue != valueType::STRING && m_storedValue != valueType::UNKNOWN)
			{
				return convertType<short>();
			}

		long double value = 0.0;
		std::from_chars(m_value.data(), m_value.data() + m_value.size(), value);

		return static_cast<short>(value);
	}

int iniConfig::value::asInt() const
	{
		if (m_storedValue != valueType::STRING && m_storedValue != valueType::UNKNOWN)
			{
				return convertType<int>();
			}

		long double value = 0.0;
		std::from_chars(m_value.data(), m_value.data() + m_value.size(), value);

		return static_cast<int>(value);
	}

unsigned int iniConfig::value::asUnsignedInt() const
	{
		if (m_storedValue != valueType::STRING && m_storedValue != valueType::UNKNOWN)
			{
				return convertType<unsigned int>();
			}

		long double value = 0.0;
		std::from_chars(m_value.data(), m_value.data() + m_value.size(), value);

		return static_cast<unsigned int>(value);
	}

long long iniConfig::value::asLongLong() const
	{
		if (m_storedValue != valueType::STRING && m_storedValue != valueType::UNKNOWN)
			{
				return convertType<long long>();
			}

		long double value = 0.0;
		std::from_chars(m_value.data(), m_value.data() + m_value.size(), value);

		return static_cast<long long>(value);
	}

unsigned long long iniConfig::value::asUnsignedLongLong() const
	{
		if (m_storedValue != valueType::STRING && m_storedValue != valueType::UNKNOWN)
			{
				return convertType<unsigned long long>();
			}

		long double value = 0.0;
		std::from_chars(m_value.data(), m_value.data() + m_value.size(), value);

		return static_cast<unsigned long long>(value);
	}

float iniConfig::value::asFloat() const
	{
		if (m_storedValue != valueType::STRING && m_storedValue != valueType::UNKNOWN)
			{
				return convertType<float>();
			}

		long double value = 0.0;
		std::from_chars(m_value.data(), m_value.data() + m_value.size(), value);

		return static_cast<float>(value);
	}

double iniConfig::value::asDouble() const
	{
		if (m_storedValue != valueType::STRING && m_storedValue != valueType::UNKNOWN)
			{
				return convertType<double>();
			}

		long double value = 0.0;
		std::from_chars(m_value.data(), m_value.data() + m_value.size(), value);

		return static_cast<double>(value);
	}

iniConfig::value::operator std::string_view() const
	{
		return asString();
	}

iniConfig::value::operator bool() const
	{
		return asBool();
	}

iniConfig::value::operator char() const
	{
		return asChar();
	}

iniConfig::value::operator short() const
	{
		return asShort();
	}

iniConfig::value::operator int() const
	{
		return asInt();
	}

iniConfig::value::operator unsigned int() const
	{
		return asUnsignedInt();
	}

iniConfig::value::operator long long() const
	{
		return asLongLong();
	}

iniConfig::value::operator unsigned long long() const
	{
		return asUnsignedLongLong();
	}

iniConfig::value::operator float() const
	{
		return asFloat();
	}

iniConfig::value::operator double() const
	{
		return asDouble();
	}

iniConfig::value &iniConfig::value::operator=(const value& rhs)
	{
		if (&rhs != this)
			{
				this->m_value = rhs.m_value;
				this->m_storedValue = rhs.m_storedValue;
				this->m_storedValueData = rhs.m_storedValueData;
			}
		return *this;
	}

iniConfig::value &iniConfig::value::operator=(std::string_view rhs)
	{
		m_value = rhs;
		m_storedValue = valueType::STRING;
		return *this;
	}

iniConfig::value &iniConfig::value::operator=(const char *rhs)
	{
		m_value = rhs;
		m_storedValue = valueType::STRING;
		return *this;
	}

iniConfig::value &iniConfig::value::operator=(bool rhs)
	{
		m_value = rhs ? "true" : "false";
		m_storedValue = valueType::BOOL;
		m_storedValueData.BOOL = rhs;
		return *this;
	}

iniConfig::value &iniConfig::value::operator=(char rhs)
	{
		m_value = rhs;
		m_storedValue = valueType::CHAR;
		m_storedValueData.CHAR = rhs;
		return *this;
	}

iniConfig::value &iniConfig::value::operator=(short rhs)
	{
		m_value = fmt::format_int(rhs).str();
		m_storedValue = valueType::SHORT;
		m_storedValueData.SHORT = rhs;
		return *this;
	}

iniConfig::value &iniConfig::value::operator=(int rhs)
	{
		m_value = fmt::format_int(rhs).str();
		m_storedValue = valueType::INT;
		m_storedValueData.INT = rhs;
		return *this;
	}

iniConfig::value &iniConfig::value::operator=(unsigned int rhs)
	{
		m_value = fmt::format_int(rhs).str();
		m_storedValue = valueType::UNSIGNED_INT;
		m_storedValueData.UNSIGNED_INT = rhs;
		return *this;
	}

iniConfig::value &iniConfig::value::operator=(long long rhs)
	{
		m_value = fmt::format_int(rhs).str();
		m_storedValue = valueType::LONG_LONG;
		m_storedValueData.LONG_LONG = rhs;

		return *this;
	}

iniConfig::value &iniConfig::value::operator=(unsigned long long rhs)
	{
		m_value = fmt::format_int(rhs).str();
		m_storedValue = valueType::UNSIGNED_LONG_LONG;
		m_storedValueData.UNSIGNED_LONG_LONG = rhs;
		return *this;
	}

iniConfig::value &iniConfig::value::operator=(float rhs)
	{
		m_value = fmt::to_string(rhs);
		m_storedValue = valueType::FLOAT;
		m_storedValueData.FLOAT = rhs;
		return *this;
	}

iniConfig::value &iniConfig::value::operator=(double rhs)
	{
		m_value = fmt::to_string(rhs);
		m_storedValue = valueType::DOUBLE;
		m_storedValueData.DOUBLE = rhs;
		return *this;
	}
