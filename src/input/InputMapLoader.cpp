#include "igneous/input/InputMapLoader.hpp"

#include "igneous/input/GamepadAxis.hpp"
#include "igneous/input/InputBinding.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "SDL3/SDL.h"
#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_gamepad.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_mouse.h"

namespace Engine
{
    namespace
    {
        class JsonReader
        {
          public:

            explicit JsonReader(std::string source)
                : source(std::move(source))
            {
            }

            bool IsDone() const
            {
                return index >= source.size();
            }

            void SkipWhitespace()
            {
                while (!IsDone() && std::isspace(static_cast<unsigned char>(source[index])))
                    ++index;
            }

            char Peek() const
            {
                size_t peekIndex = index;
                while (peekIndex < source.size() && std::isspace(static_cast<unsigned char>(source[peekIndex])))
                    ++peekIndex;
                if (peekIndex >= source.size())
                    return '\0';
                return source[peekIndex];
            }

            char Consume()
            {
                SkipWhitespace();
                if (IsDone())
                    throw std::runtime_error("Unexpected end of JSON input");
                return source[index++];
            }

            void Expect(char expected)
            {
                char actual = Consume();
                if (actual != expected)
                    throw std::runtime_error(std::string("Expected '") + expected + "' in JSON");
            }

            std::string ReadString()
            {
                Expect('"');
                std::string value;
                while (!IsDone())
                {
                    char c = source[index++];
                    if (c == '"')
                        return value;
                    if (c == '\\')
                    {
                        if (IsDone())
                            throw std::runtime_error("Invalid escape in JSON string");
                        char escaped = source[index++];
                        switch (escaped)
                        {
                            case '"':
                            case '\\':
                            case '/':
                                value.push_back(escaped);
                                break;
                            case 'b':
                                value.push_back('\b');
                                break;
                            case 'f':
                                value.push_back('\f');
                                break;
                            case 'n':
                                value.push_back('\n');
                                break;
                            case 'r':
                                value.push_back('\r');
                                break;
                            case 't':
                                value.push_back('\t');
                                break;
                            default:
                                throw std::runtime_error("Unsupported escape in JSON string");
                        }
                        continue;
                    }
                    value.push_back(c);
                }
                throw std::runtime_error("Unterminated JSON string");
            }

            void SkipValue()
            {
                SkipWhitespace();
                if (IsDone())
                    return;

                char c = source[index];
                if (c == '"')
                {
                    ReadString();
                    return;
                }
                if (c == '{')
                {
                    ReadObject([](const std::string&, JsonReader& reader)
                    {
                        reader.SkipValue();
                    });
                    return;
                }
                if (c == '[')
                {
                    ReadArray([](JsonReader& reader)
                    {
                        reader.SkipValue();
                    });
                    return;
                }

                while (!IsDone() && source[index] != ',' && source[index] != '}' && source[index] != ']')
                    ++index;
            }

            template<typename ObjectHandler>
            void ReadObject(ObjectHandler&& handler)
            {
                Expect('{');
                SkipWhitespace();
                if (Peek() == '}')
                {
                    Consume();
                    return;
                }

                while (true)
                {
                    std::string key = ReadString();
                    Expect(':');
                    handler(key, *this);

                    SkipWhitespace();
                    char next = Consume();
                    if (next == '}')
                        break;
                    if (next != ',')
                        throw std::runtime_error("Expected ',' or '}' in JSON object");
                }
            }

            template<typename ArrayHandler>
            void ReadArray(ArrayHandler&& handler)
            {
                Expect('[');
                SkipWhitespace();
                if (Peek() == ']')
                {
                    Consume();
                    return;
                }

                while (true)
                {
                    handler(*this);
                    SkipWhitespace();
                    char next = Consume();
                    if (next == ']')
                        break;
                    if (next != ',')
                        throw std::runtime_error("Expected ',' or ']' in JSON array");
                }
            }

          private:

            std::string source;
            size_t index = 0;
        };

        std::string ResolveInputMapPath(const std::string& path)
        {
            std::vector<std::string> candidates{};
            candidates.push_back(path);

            if (!std::filesystem::path(path).is_absolute())
            {
                if (const char* basePath = SDL_GetBasePath())
                    candidates.push_back((std::filesystem::path(basePath) / path).string());
            }

            for (const std::string& candidate: candidates)
            {
                std::ifstream file(candidate);
                if (file)
                    return candidate;
            }

            return path;
        }

        std::string ReadFileToString(const std::string& path)
        {
            const std::string resolvedPath = ResolveInputMapPath(path);
            std::ifstream file(resolvedPath);
            if (!file)
                throw std::runtime_error("Failed to open input map file: " + path);
            std::ostringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        }

        void WriteFileFromString(const std::string& path, const std::string& contents)
        {
            std::ofstream file(path);
            if (!file)
                throw std::runtime_error("Failed to write input map file: " + path);
            file << contents;
        }

        std::string EscapeJsonString(const std::string& value)
        {
            std::string escaped;
            escaped.reserve(value.size());
            for (char c: value)
            {
                switch (c)
                {
                    case '"':
                        escaped += "\\\"";
                        break;
                    case '\\':
                        escaped += "\\\\";
                        break;
                    case '\n':
                        escaped += "\\n";
                        break;
                    case '\r':
                        escaped += "\\r";
                        break;
                    case '\t':
                        escaped += "\\t";
                        break;
                    default:
                        escaped.push_back(c);
                        break;
                }
            }
            return escaped;
        }

        SDL_Keycode ParseKeyName(const std::string& name)
        {
            SDL_Keycode key = SDL_GetKeyFromName(name.c_str());
            if (key != SDLK_UNKNOWN)
                return key;

            if (name.size() == 1)
            {
                std::string upper = name;
                upper[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(upper[0])));
                key = SDL_GetKeyFromName(upper.c_str());
                if (key != SDLK_UNKNOWN)
                    return key;
            }

            return SDLK_UNKNOWN;
        }

        SDL_MouseButtonFlags ParseMouseButtonName(const std::string& name)
        {
            std::string lower = name;
            std::ranges::transform(lower, lower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

            if (lower == "left")
                return SDL_BUTTON_LEFT;
            if (lower == "right")
                return SDL_BUTTON_RIGHT;
            if (lower == "middle")
                return SDL_BUTTON_MIDDLE;
            if (lower == "x1")
                return SDL_BUTTON_X1;
            if (lower == "x2")
                return SDL_BUTTON_X2;
            return static_cast<SDL_MouseButtonFlags>(0);
        }

        std::string MouseButtonToString(SDL_MouseButtonFlags button)
        {
            if (button == SDL_BUTTON_LEFT)
                return "left";
            if (button == SDL_BUTTON_RIGHT)
                return "right";
            if (button == SDL_BUTTON_MIDDLE)
                return "middle";
            if (button == SDL_BUTTON_X1)
                return "x1";
            if (button == SDL_BUTTON_X2)
                return "x2";
            return "left";
        }

        SDL_GamepadButton ParseGamepadButtonName(const std::string& name)
        {
            SDL_GamepadButton button = SDL_GetGamepadButtonFromString(name.c_str());
            if (button != SDL_GAMEPAD_BUTTON_INVALID)
                return button;

            std::string lower = name;
            std::ranges::transform(lower, lower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

            struct ButtonAlias
            {
                const char* alias;
                SDL_GamepadButton button;
            };

            static constexpr ButtonAlias kAliases[] = {
                {"south", SDL_GAMEPAD_BUTTON_SOUTH},
                {"east", SDL_GAMEPAD_BUTTON_EAST},
                {"west", SDL_GAMEPAD_BUTTON_WEST},
                {"north", SDL_GAMEPAD_BUTTON_NORTH},
                {"cross", SDL_GAMEPAD_BUTTON_SOUTH},
                {"circle", SDL_GAMEPAD_BUTTON_EAST},
                {"square", SDL_GAMEPAD_BUTTON_WEST},
                {"triangle", SDL_GAMEPAD_BUTTON_NORTH},
            };

            for (const ButtonAlias& alias: kAliases)
            {
                if (lower == alias.alias)
                    return alias.button;
            }

            return SDL_GAMEPAD_BUTTON_INVALID;
        }

        std::unique_ptr<InputBinding> ParseBinding(JsonReader& reader)
        {
            std::string type;
            std::string keyName;
            std::string buttonName;
            std::string axisName;

            reader.ReadObject([&](const std::string& field, JsonReader& objectReader)
            {
                if (field == "type")
                    type = objectReader.ReadString();
                else if (field == "key")
                    keyName = objectReader.ReadString();
                else if (field == "button")
                    buttonName = objectReader.ReadString();
                else if (field == "axis")
                    axisName = objectReader.ReadString();
                else
                    objectReader.SkipValue();
            });

            std::string lowerType = type;
            std::ranges::transform(lowerType, lowerType.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

            if (lowerType == "key")
            {
                SDL_Keycode key = ParseKeyName(keyName);
                if (key == SDLK_UNKNOWN)
                    throw std::runtime_error("Unknown key name in input map: " + keyName);
                return MakeKeyBinding(key);
            }

            if (lowerType == "mouse")
            {
                SDL_MouseButtonFlags button = ParseMouseButtonName(buttonName);
                if (button == static_cast<SDL_MouseButtonFlags>(0))
                    throw std::runtime_error("Unknown mouse button in input map: " + buttonName);
                return MakeMouseButtonBinding(button);
            }

            if (lowerType == "gamepad_button")
            {
                SDL_GamepadButton button = ParseGamepadButtonName(buttonName);
                if (button == SDL_GAMEPAD_BUTTON_INVALID)
                    throw std::runtime_error("Unknown gamepad button in input map: " + buttonName);
                return MakeGamepadButtonBinding(button);
            }

            if (lowerType == "gamepad_axis")
            {
                GamepadAxis axis{};
                if (!GamepadAxisFromString(axisName.c_str(), axis))
                    throw std::runtime_error("Unknown gamepad axis in input map: " + axisName);
                return MakeGamepadAxisBinding(axis);
            }

            throw std::runtime_error("Unknown binding type in input map: " + type);
        }

        void AppendBindingJson(std::ostringstream& output, const InputBinding& binding)
        {
            output << "        {\n";
            output << "          \"type\": \"";

            switch (binding.GetType())
            {
                case InputBindingType::Key:
                {
                    const auto& keyBinding = static_cast<const KeyBinding&>(binding);
                    output << "key\",\n";
                    output << "          \"key\": \"" << EscapeJsonString(SDL_GetKeyName(keyBinding.GetKey())) << "\"\n";
                    break;
                }
                case InputBindingType::Mouse:
                {
                    const auto& mouseBinding = static_cast<const MouseButtonBinding&>(binding);
                    output << "mouse\",\n";
                    output << "          \"button\": \"" << MouseButtonToString(mouseBinding.GetButton()) << "\"\n";
                    break;
                }
                case InputBindingType::GamepadButton:
                {
                    const auto& gamepadBinding = static_cast<const GamepadButtonBinding&>(binding);
                    output << "gamepad_button\",\n";
                    output << "          \"button\": \"" << EscapeJsonString(SDL_GetGamepadStringForButton(gamepadBinding.GetButton())) << "\"\n";
                    break;
                }
                case InputBindingType::GamepadAxis:
                {
                    const auto& axisBinding = static_cast<const GamepadAxisBinding&>(binding);
                    output << "gamepad_axis\",\n";
                    output << "          \"axis\": \"" << GamepadAxisToString(axisBinding.GetAxis()) << "\"\n";
                    break;
                }
            }

            output << "        }";
        }
    }

    std::shared_ptr<InputMap> InputMapLoader::LoadFromFile(const std::string& path)
    {
        return LoadFromString(ReadFileToString(path));
    }

    std::shared_ptr<InputMap> InputMapLoader::LoadFromString(const std::string& json)
    {
        auto inputMap = std::make_shared<InputMap>();
        JsonReader reader(json);

        reader.ReadObject([&](const std::string& field, JsonReader& objectReader)
        {
            if (field == "id")
                inputMap->SetId(objectReader.ReadString());
            else if (field == "name")
                inputMap->SetName(objectReader.ReadString());
            else if (field == "actions")
            {
                objectReader.ReadArray([&](JsonReader& actionReader)
                {
                    std::string actionName;
                    std::vector<std::unique_ptr<InputBinding>> bindings;

                    actionReader.ReadObject([&](const std::string& actionField, JsonReader& actionObjectReader)
                    {
                        if (actionField == "name")
                            actionName = actionObjectReader.ReadString();
                        else if (actionField == "bindings")
                        {
                            actionObjectReader.ReadArray([&](JsonReader& bindingReader)
                            {
                                bindings.push_back(ParseBinding(bindingReader));
                            });
                        }
                        else
                            actionObjectReader.SkipValue();
                    });

                    if (actionName.empty())
                        throw std::runtime_error("Action without name in input map");

                    InputAction& action = inputMap->AddAction(actionName);
                    for (std::unique_ptr<InputBinding>& binding: bindings)
                        action.AddBinding(std::move(binding));
                });
            }
            else
                objectReader.SkipValue();
        });

        return inputMap;
    }

    std::string InputMapLoader::SaveToString(const InputMap& inputMap)
    {
        std::ostringstream output;
        output << "{\n";
        output << "  \"id\": \"" << EscapeJsonString(inputMap.GetId()) << "\",\n";
        output << "  \"name\": \"" << EscapeJsonString(inputMap.GetName()) << "\",\n";
        output << "  \"actions\": [\n";

        const auto& actions = inputMap.GetActions();
        for (size_t actionIndex = 0; actionIndex < actions.size(); ++actionIndex)
        {
            const InputAction& action = *actions[actionIndex];
            output << "    {\n";
            output << "      \"name\": \"" << EscapeJsonString(action.GetName()) << "\",\n";
            output << "      \"bindings\": [\n";

            const auto& bindings = action.GetBindings();
            for (size_t bindingIndex = 0; bindingIndex < bindings.size(); ++bindingIndex)
            {
                AppendBindingJson(output, *bindings[bindingIndex]);
                if (bindingIndex + 1 < bindings.size())
                    output << ",";
                output << "\n";
            }

            output << "      ]\n";
            output << "    }";
            if (actionIndex + 1 < actions.size())
                output << ",";
            output << "\n";
        }

        output << "  ]\n";
        output << "}\n";
        return output.str();
    }

    bool InputMapLoader::SaveToFile(const InputMap& inputMap, const std::string& path)
    {
        WriteFileFromString(path, SaveToString(inputMap));
        return true;
    }
}
