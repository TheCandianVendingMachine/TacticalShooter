// inputHandler.hpp
// Allows the mapping of arbitrary keys to inputs
#pragma once
#include "typeDefines.hpp"
#include <glm/vec2.hpp>
#include <vector>
#include <string>
#include <string_view>
#include <robin_hood.h>

struct GLFWwindow;
class inputHandler
    {
        private:
            const GLFWwindow *m_window = nullptr;

            robin_hood::unordered_map<std::string, std::vector<std::string>> m_defaultInputGroups;
            robin_hood::unordered_map<fe::str, robin_hood::unordered_map<fe::str, int>> m_defaultInputs;

            robin_hood::unordered_map<std::string, std::vector<std::string>> m_inputGroups;
            robin_hood::unordered_map<fe::str, robin_hood::unordered_map<fe::str, int>> m_inputs;

        public:
            enum class inputState
                {
                    RELEASE,
                    PRESS,
                };

            inputHandler(GLFWwindow *window);
            inputHandler(GLFWwindow *window, const char *inputFilePath);

            void load(const char *filepath);
            void save(const char *filepath) const;

            void addKey(std::string_view group, std::string_view keyName, int keyCode);
            void addDefaultKey(std::string_view group, std::string_view keyName, int keyCode);

            int keyCode(std::string_view group, std::string_view keyName) const;
            int defaultKeyCode(std::string_view group, std::string_view keyName) const;

            inputState keyState(std::string_view group, std::string_view keyName) const;
            inputState keyState(int keyCode) const;

            glm::vec2 getCursorPosition() const;
    };

namespace globals
    {
        extern inputHandler *g_inputs;
    }
