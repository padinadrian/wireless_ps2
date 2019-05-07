/**
 * File:    adrian_dualshock.hpp
 * Author:  padin.adrian@gmail.com
 *
 * Copyright 2019
 */

#ifndef ADRIAN_DUALSHOCK_HPP_
#define ADRIAN_DUALSHOCK_HPP_

/* ===== Includes ===== */
#include "adrian_helpers.hpp"
#include "adrian_spi.hpp"

namespace adrian
{
    using adrian::SPI;

    /** Represents a DualShock controller. */
    class DualShock
    {
    public:
        /* ===== Classes ===== */

        /** Holds the state of all buttons, including analog and digital values. */
        struct ButtonState
        {
            /** Constructor */
            ButtonState() : digital_valid(false), analog_valid(false) {}

            // Validity flags
            bool digital_valid;
            bool analog_valid;

            // Digital buttons
            bool select : 1;    // 4.0
            bool left3 : 1;     // 4.1
            bool right3 : 1;    // 4.2
            bool start : 1;     // 4.3
            bool d_up : 1;      // 4.4
            bool d_right : 1;   // 4.5
            bool d_down : 1;    // 4.6
            bool d_left : 1;    // 4.7
            bool left2 : 1;     // 5.0
            bool right2 : 1;    // 5.1
            bool left1 : 1;     // 5.2
            bool right1 : 1;    // 5.3
            bool triangle : 1;  // 5.4
            bool circle : 1;    // 5.5
            bool cross : 1;         // 5.6
            bool square : 1;    // 5.7

            // Analog sticks
            uint8_t analog_left_x;
            uint8_t analog_left_y;
            uint8_t analog_right_x;
            uint8_t analog_right_y;

            // Analog button pressure
            // uint8_t select_pressure;
            // uint8_t left3_pressure;
            // uint8_t right3_pressure;
            // uint8_t start_pressure;
            uint8_t d_up_pressure;
            uint8_t d_right_pressure;
            uint8_t d_down_pressure;
            uint8_t d_left_pressure;
            uint8_t left2_pressure;
            uint8_t right2_pressure;
            uint8_t left1_pressure;
            uint8_t right1_pressure;
            uint8_t triangle_pressure;
            uint8_t circle_pressure;
            uint8_t cross_pressure;
            uint8_t square_pressure;
        };

        /* ===== Functions ===== */

        /**
         * Constructor - configure SPI interface for DualShock settings.
         * Assumes SPI interface is already initialized.
         * @param[in] spi_interface - Pointer to SPI device.
         */
        DualShock(SPI* const spi_interface);

        /** Attempt to establish communication with the controller. */
        bool Connect();

        /** Disconnect from the controller (actually does nothing). */
        bool Disconnect();

        /** Check if controller is connected. */
        bool IsConnected() const;

        /**
         * Just poll for the button states.
         * @param[out] current_button_states - Digital and analog button values.
         */
        void Poll(ButtonState& current_button_states);

        /**
         * Poll for the button states and activate the controller rumble.
         * @param[in] left_rumble - Rumble value for left-side motor (between 0 and 256).
         * @param[in] right_rumble - Rumble value for right-side motor (between 0 and 256).
         * @param[out] current_button_states - Digital and analog button values.
         */
        void Poll(uint8_t left_rumble,
                  uint8_t right_rumble,
                  ButtonState& current_button_states);

        /**
         * Enter Analog Mode to enable reading pressure values.
         * You must call this function in order to read pressure values!
         */
        bool EnableAnalog();

        /**
         * Enter Digital Mode to disable reading pressure values.
         * The controller will be in digital mode at startup by default.
         */
        bool DisableAnalog();

        /** Parse the digital buttons from the response */
        static void ParseDigitalButtons(
            const uint8_t* digital_bytes,
            ButtonState& button_state_out);

        /** Parse the analog buttons from the response */
        static void ParseAnalogButtons(
            const uint8_t* analog_bytes,
            ButtonState& button_state_out);

    private:
        SPI* const m_spi_ptr;
        bool m_is_connected;
        bool m_analog_enabled;
    };

}   // end namespace ps2

#endif  // ADRIAN_DUALSHOCK_HPP_
