#pragma once

#include <string>

namespace Engine
{
    /**
     * @class InputLayer
     * @brief Represents a priority-based input processing layer.
     *
     * Input layers allow different systems to process input in a defined order.
     * Higher priority layers (like UI) process input first and can "consume"
     * inputs to prevent lower priority layers (like gameplay) from responding.
     *
     * Common layer priorities:
     * - UI/Menus: High priority (e.g., 1 or higher)
     * - Gameplay: Low priority (e.g., 0)
     *
     * Layers are processed in descending priority order (highest first).
     *
     * Example usage:
     * @code
     * Input::AddInputLayer("ui", 1);        // UI processes input first
     * Input::AddInputLayer("gameplay", 0);  // Gameplay processes input second
     * @endcode
     *
     * @see Input
     * @see InputEvent
     */
    class InputLayer
    {
      private:
        /**
         * @brief The unique name of this input layer.
         *
         * Used to identify and retrieve the layer.
         */
        std::string name{};

        /**
         * @brief The priority of this input layer.
         *
         * Higher priority layers process input first. Layers are sorted
         * in descending order (highest priority first).
         */
        int priority = 0;

      public:
        /**
         * @brief Constructs an input layer with a name and priority.
         *
         * @param _name The unique name for this layer.
         * @param _priority The priority value. Higher values process input first.
         *
         * @note Typically called internally by Input::AddInputLayer().
         */
        InputLayer(const std::string& _name, int _priority);

        /**
         * @brief Checks if this layer matches the given name.
         *
         * Used to identify the current layer during input processing,
         * allowing different behavior based on which layer is active.
         *
         * @param layerName The name to check against.
         * @return true if this layer's name matches layerName, false otherwise.
         *
         * Example:
         * @code
         * void HandleEvents(InputLayer& layer)
         * {
         *     if (layer.Is("gameplay"))
         *     {
         *         // Handle gameplay input
         *     }
         * }
         * @endcode
         */
        bool Is(const std::string& layerName);
    };
}