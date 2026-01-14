#pragma once

namespace Engine
{
    /**
     * @struct InputEvent
     * @brief Represents the state of a single input (key or mouse button).
     *
     * InputEvent tracks the current and previous frame state of an input,
     * as well as whether the input has been "handled" (consumed) by a higher
     * priority input layer.
     *
     * The handled flag implements an input consumption system where higher
     * priority layers (like UI) can prevent lower priority layers (like gameplay)
     * from responding to the same input.
     *
     * @see Input
     * @see InputLayer
     */
    struct InputEvent
    {
        /**
         * @brief Whether the input is currently pressed this frame.
         *
         * Set to true when the key/button is down, false when released.
         */
        bool pressed = false;

        /**
         * @brief Whether the input was pressed in the previous frame.
         *
         * Used to detect "just pressed" and "just released" events by
         * comparing with the current pressed state.
         *
         * Updated at the start of each frame by Input::ResetEvents().
         */
        bool pressedLastFrame = false;

        /**
         * @brief Whether this input has been handled (consumed) this frame.
         *
         * When an input layer handles an input, this flag is set to true,
         * preventing lower priority layers from responding to the same input.
         * This prevents UI clicks from also triggering gameplay actions.
         *
         * Reset to false at the start of each frame by Input::ResetEvents().
         */
        bool handled = false;
    };
}