#pragma once

#include <map>
#include <string>
#include <unordered_map>

#include "igneous/engine/Vec2.hpp"
#include "igneous/input/InputEvent.hpp"
#include "igneous/input/InputLayer.hpp"
#include "SDL3/SDL_events.h"

namespace Engine
{
    /**
     * @class Input
     * @brief Manages input processing with priority-based layer system.
     *
     * The Input class provides a centralized system for handling keyboard, mouse,
     * and window events. It implements a priority-based input layer system where
     * higher priority layers (like UI) can consume inputs to prevent lower priority
     * layers (like gameplay) from responding.
     *
     * ## Input Layer System
     *
     * Input layers are processed in descending priority order. When a layer handles
     * an input (by calling HandleKey() or HandleButton()), that input is marked as
     * "handled" and subsequent layers can choose to skip it using the skipIfHandled
     * parameter in query methods.
     *
     * Default layers:
     * - "ui" (priority 1): For UI elements and menus
     * - "gameplay" (priority 0): For gameplay input
     *
     * ## ImGui Integration
     *
     * The Input class integrates with ImGui to prevent input from affecting the game
     * when ImGui wants to capture it (e.g., when typing in a text field).
     *
     * ## Frame Lifecycle
     *
     * Each frame:
     * 1. ResetEvents() clears per-frame state (velocities, handled flags)
     * 2. HandleEvent() processes SDL events as they arrive
     * 3. Query methods (IsKeyDown, etc.) check input state
     * 4. HandleKey()/HandleButton() mark inputs as consumed
     *
     * @note This is a static utility class and should not be instantiated.
     * @note Automatically integrates with ImGui's input capture system.
     *
     * @see InputLayer
     * @see InputEvent
     */
    class Input
    {
      private:
        /**
         * @brief Multimap of input layers sorted by priority (descending).
         *
         * Layers are stored with priority as the key, allowing multiple layers
         * with the same priority. Uses std::greater<> for descending order.
         */
        static inline std::multimap<int, InputLayer, std::greater<>> layers{};

        /**
         * @brief Lookup map for quick access to layer iterators by name.
         *
         * Allows O(1) layer removal and existence checks by name.
         */
        static inline std::unordered_map<std::string, std::multimap<int, InputLayer, std::greater<>>::iterator> layerLookup{};

        /**
         * @brief Map of keyboard key states.
         *
         * Only contains entries for keys that have been pressed at least once.
         */
        static inline std::unordered_map<SDL_Keycode, InputEvent> keyEvents{};

        /**
         * @brief Map of mouse button states.
         *
         * Only contains entries for buttons that have been pressed at least once.
         */
        static inline std::unordered_map<SDL_MouseButtonFlags, InputEvent> mouseEvents{};

        /**
         * @brief Current mouse X position in screen space (pixels).
         */
        static inline float mouseX = 0;

        /**
         * @brief Current mouse Y position in screen space (pixels).
         */
        static inline float mouseY = 0;

        /**
         * @brief Mouse X velocity (relative movement) for this frame.
         *
         * Reset to 0 at the start of each frame.
         */
        static inline float mouseVelX = 0;

        /**
         * @brief Mouse Y velocity (relative movement) for this frame.
         *
         * Reset to 0 at the start of each frame.
         */
        static inline float mouseVelY = 0;

        /**
         * @brief Mouse wheel X velocity (horizontal scroll) for this frame.
         *
         * Reset to 0 at the start of each frame.
         */
        static inline float mouseWheelVelX = 0;

        /**
         * @brief Mouse wheel Y velocity (vertical scroll) for this frame.
         *
         * Reset to 0 at the start of each frame.
         */
        static inline float mouseWheelVelY = 0;

        /**
         * @brief Whether the window was resized this frame.
         *
         * Reset to false at the start of each frame.
         */
        static inline bool wasWindowResized = false;

      public:
        /**
         * @brief Initializes the input system with default layers.
         *
         * Creates the default input layers:
         * - "gameplay" (priority 0)
         * - "ui" (priority 1)
         *
         * @return true if initialization succeeded, false otherwise.
         *
         * @note Called automatically by the Engine during initialization.
         */
        static bool Init();

        /**
         * @brief Resets per-frame input state.
         *
         * Performs the following resets:
         * - Clears window resize flag
         * - Resets mouse velocity to 0
         * - Resets mouse wheel velocity to 0
         * - Updates pressedLastFrame for all keys and buttons
         * - Clears handled flags for all keys and buttons
         *
         * @note Called once per frame by the Engine before processing events.
         */
        static void ResetEvents();

        /**
         * @brief Processes an SDL event and updates input state.
         *
         * Handles the following event types:
         * - SDL_EVENT_KEY_DOWN / SDL_EVENT_KEY_UP: Updates keyboard state
         * - SDL_EVENT_MOUSE_BUTTON_DOWN / SDL_EVENT_MOUSE_BUTTON_UP: Updates mouse button state
         * - SDL_EVENT_MOUSE_MOTION: Updates mouse position and velocity
         * - SDL_EVENT_MOUSE_WHEEL: Updates mouse wheel velocity
         * - SDL_EVENT_WINDOW_RESIZED: Sets window resize flag
         *
         * @param event The SDL event to process.
         *
         * @note Called automatically by the Engine for each SDL event.
         */
        static void HandleEvent(SDL_Event& event);

        /**
         * @brief Adds a new input layer with the specified priority.
         *
         * Input layers are processed in descending priority order (highest first).
         * Higher priority layers can consume inputs to prevent lower priority
         * layers from responding.
         *
         * @param layerName Unique name for the layer.
         * @param priority Priority value. Higher values process input first.
         *
         * @return true if the layer was added, false if a layer with that name
         *         already exists.
         *
         * @note Logs an error if the layer already exists.
         *
         * Example:
         * @code
         * Input::AddInputLayer("pause_menu", 2); // Highest priority
         * Input::AddInputLayer("ui", 1);
         * Input::AddInputLayer("gameplay", 0);
         * @endcode
         */
        static bool AddInputLayer(const std::string& layerName, int priority);

        /**
         * @brief Removes an input layer by name.
         *
         * @param layerName The name of the layer to remove.
         *
         * @return true if the layer was removed, false if it doesn't exist.
         *
         * @note Logs an error if the layer doesn't exist.
         */
        static bool RemoveInputLayer(const std::string& layerName);

        /**
         * @brief Gets a copy of all input layers.
         *
         * Returns the layers sorted by priority in descending order (highest first).
         * Used by the Engine to iterate through layers during event processing.
         *
         * @return Multimap of input layers sorted by priority (descending).
         *
         * @note Returns a copy, not a reference.
         */
        static std::multimap<int, InputLayer, std::greater<>> GetInputLayers();

        /**
         * @brief Marks a key as handled (consumed).
         *
         * Sets the handled flag for the specified key, preventing lower priority
         * layers from responding to it when they use skipIfHandled=true.
         *
         * @param key The SDL keycode to mark as handled.
         *
         * @note Safe to call even if the key hasn't been pressed.
         * @note Typically called automatically by IsKeyDown() and IsKeyJustPressed().
         */
        static void HandleKey(SDL_Keycode key);

        /**
         * @brief Marks a mouse button as handled (consumed).
         *
         * Sets the handled flag for the specified button, preventing lower priority
         * layers from responding to it when they use skipIfHandled=true.
         *
         * @param mouseButton The SDL mouse button to mark as handled.
         *
         * @note Safe to call even if the button hasn't been pressed.
         * @note Typically called automatically by IsMouseButtonDown() and IsButtonJustPressed().
         */
        static void HandleButton(SDL_MouseButtonFlags mouseButton);

        /**
         * @brief Checks if a key has been handled (consumed) this frame.
         *
         * @param key The SDL keycode to check.
         * @return true if the key has been handled, false otherwise.
         *
         * @note Returns false if the key hasn't been pressed yet.
         */
        static bool IsKeyHandled(SDL_Keycode key);

        /**
         * @brief Checks if a mouse button has been handled (consumed) this frame.
         *
         * @param mouseButton The SDL mouse button to check.
         * @return true if the button has been handled, false otherwise.
         *
         * @note Returns false if the button hasn't been pressed yet.
         */
        static bool IsButtonHandled(SDL_MouseButtonFlags mouseButton);

        /**
         * @brief Checks if a key is currently held down.
         *
         * Returns false if:
         * - ImGui wants to capture keyboard input
         * - The key hasn't been pressed yet
         * - The key is handled and skipIfHandled is true
         * - The key is not currently pressed
         *
         * If the check succeeds and the key is down, it is automatically marked
         * as handled.
         *
         * @param key The SDL keycode to check.
         * @param skipIfHandled If true, returns false if the key is already handled.
         *                      Default is true.
         *
         * @return true if the key is down and passes all checks, false otherwise.
         *
         * @note Automatically marks the key as handled when returning true.
         * @note Respects ImGui's input capture state.
         */
        static bool IsKeyDown(SDL_Keycode key, bool skipIfHandled = true);

        /**
         * @brief Checks if a key was just pressed this frame.
         *
         * Returns true only on the first frame the key is pressed (not held).
         * Returns false if:
         * - ImGui wants to capture keyboard input
         * - The key hasn't been pressed yet
         * - The key is handled and skipIfHandled is true
         * - The key is not pressed or was already pressed last frame
         *
         * If the check succeeds, the key is automatically marked as handled.
         *
         * @param key The SDL keycode to check.
         * @param skipIfHandled If true, returns false if the key is already handled.
         *                      Default is true.
         *
         * @return true if the key was just pressed this frame, false otherwise.
         *
         * @note Automatically marks the key as handled when returning true.
         * @note Respects ImGui's input capture state.
         */
        static bool IsKeyJustPressed(SDL_Keycode key, bool skipIfHandled = true);

        /**
         * @brief Checks if a key was just released this frame.
         *
         * Returns true only on the first frame the key is released.
         * Returns false if:
         * - ImGui wants to capture keyboard input
         * - The key hasn't been pressed yet
         * - The key is still pressed or wasn't pressed last frame
         *
         * @param key The SDL keycode to check.
         * @return true if the key was just released this frame, false otherwise.
         *
         * @note Does not automatically mark the key as handled.
         * @note Respects ImGui's input capture state.
         */
        static bool IsKeyJustReleased(SDL_Keycode key);

        /**
         * @brief Checks if a mouse button is currently held down.
         *
         * Returns false if:
         * - ImGui wants to capture mouse input
         * - The button hasn't been pressed yet
         * - The button is handled and skipIfHandled is true
         * - The button is not currently pressed
         *
         * If the check succeeds and the button is down, it is automatically marked
         * as handled.
         *
         * @param mouseButton The SDL mouse button to check.
         * @param skipIfHandled If true, returns false if the button is already handled.
         *                      Default is true.
         *
         * @return true if the button is down and passes all checks, false otherwise.
         *
         * @note Automatically marks the button as handled when returning true.
         * @note Respects ImGui's input capture state.
         */
        static bool IsMouseButtonDown(SDL_MouseButtonFlags mouseButton, bool skipIfHandled = true);

        /**
         * @brief Checks if a mouse button was just pressed this frame.
         *
         * Returns true only on the first frame the button is pressed (not held).
         * Returns false if:
         * - ImGui wants to capture mouse input
         * - The button hasn't been pressed yet
         * - The button is handled and skipIfHandled is true
         * - The button is not pressed or was already pressed last frame
         *
         * If the check succeeds, the button is automatically marked as handled.
         *
         * @param mouseButton The SDL mouse button to check.
         * @param skipIfHandled If true, returns false if the button is already handled.
         *                      Default is true.
         *
         * @return true if the button was just pressed this frame, false otherwise.
         *
         * @note Automatically marks the button as handled when returning true.
         * @note Respects ImGui's input capture state.
         */
        static bool IsButtonJustPressed(SDL_MouseButtonFlags mouseButton, bool skipIfHandled = true);

        /**
         * @brief Checks if a mouse button was just released this frame.
         *
         * Returns true only on the first frame the button is released.
         * Returns false if:
         * - ImGui wants to capture mouse input
         * - The button hasn't been pressed yet
         * - The button is still pressed or wasn't pressed last frame
         *
         * @param mouseButton The SDL mouse button to check.
         * @return true if the button was just released this frame, false otherwise.
         *
         * @note Does not automatically mark the button as handled.
         * @note Respects ImGui's input capture state.
         */
        static bool IsButtonJustReleased(SDL_MouseButtonFlags mouseButton);

        /**
         * @brief Checks if the window was resized this frame.
         *
         * @return true if the window was resized this frame, false otherwise.
         *
         * @note Reset to false at the start of each frame.
         */
        static bool IsWindowResized();

        /**
         * @brief Gets the current mouse position in screen space.
         *
         * @return Vec2 containing the mouse position in pixels (x, y).
         *
         * @note Position is in screen/window coordinates, not world coordinates.
         */
        static Vec2<float> GetMouseScreenPosition();

        /**
         * @brief Gets the mouse velocity (relative movement) for this frame.
         *
         * @return Vec2 containing the mouse velocity in pixels (xrel, yrel).
         *
         * @note Velocity is relative movement since the last frame.
         * @note Reset to (0, 0) at the start of each frame.
         */
        static Vec2<float> GetMouseVelocity();

        /**
         * @brief Gets the mouse wheel velocity for this frame.
         *
         * @return Vec2 containing the wheel velocity (x = horizontal, y = vertical).
         *
         * @note Positive Y typically means scrolling up/away from user.
         * @note Reset to (0, 0) at the start of each frame.
         */
        static Vec2<float> GetMouseWheelVelocity();
    };
}