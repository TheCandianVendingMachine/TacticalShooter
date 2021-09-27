#include "inputHandler.hpp"
#include "iniConfig.hpp"
#include "str.hpp"
#include <GLFW/glfw3.h>

inputHandler *globals::g_inputs = nullptr;

inputHandler::inputHandler(GLFWwindow *window) :
	m_window(window)
	{
	}

inputHandler::inputHandler(GLFWwindow *window, const char *inputFilePath) :
	m_window(window)
	{
		load(inputFilePath);
	}

void inputHandler::load(const char *filepath)
	{
		m_inputGroups = m_defaultInputGroups;
		m_inputs = m_defaultInputs;

		iniConfig config(filepath);
		for (const auto &pair : config.sections)
			{
				for (const auto &[key, keyCode] : pair.second.keyValuePairs)
					{
						addKey(pair.first, key, keyCode);
					}
			}
	}

void inputHandler::save(const char *filepath) const
	{
		iniConfig config;

		for (auto &[group, keys] : m_inputGroups)
			{
				for (auto &key : keys)
					{
						int keyCode = m_inputs.at(FE_STR(group.data())).at(FE_STR(key.c_str()));
						config[group][key] = keyCode;
					}
			}

		for (auto &[group, keys] : m_defaultInputGroups)
			{
				for (auto &key : keys)
					{
						int keyCode = m_defaultInputs.at(FE_STR(group.data())).at(FE_STR(key.c_str()));
						if (m_inputs.find(FE_STR(group.data())) == m_inputs.end() ||
							m_inputs.at(FE_STR(group.data())).find(FE_STR(key.c_str())) == m_inputs.at(FE_STR(group.data())).end()
							) 
							{
								config[group][key] = keyCode;
							}
					}
			}

		config.save(filepath);
	}

void inputHandler::addKey(std::string_view group, std::string_view keyName, int keyCode)
	{
		m_inputGroups[group.data()].push_back(keyName.data());
		m_inputs[FE_STR(group.data())][FE_STR(keyName.data())] = keyCode;
	}

void inputHandler::addDefaultKey(std::string_view group, std::string_view keyName, int keyCode)
	{
		m_defaultInputGroups[group.data()].push_back(keyName.data());
		m_defaultInputs[FE_STR(group.data())][FE_STR(keyName.data())] = keyCode;
	}

int inputHandler::keyCode(std::string_view group, std::string_view keyName) const
	{
		fe::str groupStr = FE_STR(group.data());
		fe::str keyStr = FE_STR(keyName.data());

		if (m_inputs.find(groupStr) == m_inputs.end() ||
			m_inputs.at(groupStr).find(keyStr) == m_inputs.at(groupStr).end()
			)
			{
				return defaultKeyCode(group, keyName);
			}
		return m_inputs.at(groupStr).at(keyStr);
	}

int inputHandler::defaultKeyCode(std::string_view group, std::string_view keyName) const
	{
		fe::str groupStr = FE_STR(group.data());
		fe::str keyStr = FE_STR(keyName.data());

		if (m_defaultInputs.find(groupStr) == m_defaultInputs.end() ||
			m_defaultInputs.at(groupStr).find(keyStr) == m_defaultInputs.at(groupStr).end()
			)
			{
				return -1;
			}
		return m_defaultInputs.at(groupStr).at(keyStr);
	}

bool inputHandler::keyState(std::string_view group, std::string_view keyName) const
	{
		return keyState(keyCode(group, keyName));
	}

bool inputHandler::keyState(int keyCode) const
	{
		return glfwGetKey(const_cast<GLFWwindow*>(m_window), keyCode);
	}
