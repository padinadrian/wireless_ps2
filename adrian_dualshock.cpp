/**
 * File:    adrian_dualshock.cpp
 * Author:  padin.adrian@gmail.com
 *
 * Copyright 2019
 */

/* ===== Includes ===== */
#include "adrian_dualshock.hpp"

namespace adrian
{
    /* ===== Constants ===== */
    static const uint8_t digital_poll[5] = { 0x01, 0x42, 0x00, 0x00, 0x00 };
    static const uint8_t enter_config_mode[5] = { 0x01, 0x43, 0x00, 0x01, 0x00 };
    static const uint8_t enable_analog_mode[9] = { 0x01, 0x44, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00 };
    static const uint8_t enable_motor_command[9] = { 0x01, 0x4D, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF };
    static const uint8_t config_pressure_values[9] = { 0x01, 0x4F, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00 };
    static const uint8_t exit_config_mode[9] = { 0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A };
    static const uint8_t analog_poll[21] = { 0x01, 0x42, 0x00, 0x00, 0x00 };

    /* ===== Enums ===== */

    enum DualShockAnalogByte
    {
        // Analog stick values
        DUALSHOCK_ANALOG_RX = 5,
        DUALSHOCK_ANALOG_RY = 6,
        DUALSHOCK_ANALOG_LX = 7,
        DUALSHOCK_ANALOG_LY = 8,

        // Analog button values
        DUALSHOCK_ANALOG_D_RIGHT  = 9,
        DUALSHOCK_ANALOG_D_LEFT   = 10,
        DUALSHOCK_ANALOG_D_UP     = 11,
        DUALSHOCK_ANALOG_D_DOWN   = 12,
        DUALSHOCK_ANALOG_TRIANGLE = 13,
        DUALSHOCK_ANALOG_CIRCLE   = 14,
        DUALSHOCK_ANALOG_CROSS    = 15,
        DUALSHOCK_ANALOG_SQUARE   = 16,
        DUALSHOCK_ANALOG_L1       = 17,
        DUALSHOCK_ANALOG_R1       = 18,
        DUALSHOCK_ANALOG_L2       = 19,
        DUALSHOCK_ANALOG_R2       = 20,
    };

    /* ===== DualShock Functions ===== */

    // Constructor - assumes SPI interface is already initialized.
    DualShock::DualShock(SPI* const spi_interface) :
        m_spi_ptr(spi_interface),
        m_is_connected(false),
        m_analog_enabled(false)
    {
        // Set up SPI configuration
        m_spi_ptr->SetMode(SPI::TRANSFER_MODE_3);
        m_spi_ptr->SetBitOrder(SPI::BIT_ORDER_MSB_FIRST);
        m_spi_ptr->SetFrequency(250 * SPI::KHz);
    }

    // Attempt to establish communication with the controller.
    bool DualShock::Connect()
    {
        m_is_connected = false;

        return m_is_connected;
    }

    // Disconnect from the controller (actually does nothing).
    bool DualShock::Disconnect()
    {
        m_is_connected = false;
    }

    // Check if controller is connected.
    bool DualShock::IsConnected() const
    {
        return m_is_connected;
    }

    // Just poll for the button states.
    void DualShock::Poll(ButtonState& current_button_states)
    {
        this->Poll(0, 0, current_button_states);
    }

    // Poll for the button states and activate the controller rumble.
    void DualShock::Poll(uint8_t left_rumble,
                         uint8_t right_rumble,
                         ButtonState& current_button_states)
    {
        // Set these to false for now.
        // They will be set to true if the parsing is successful.
        current_button_states.digital_valid = false;
        current_button_states.analog_valid = false;

        if (m_analog_enabled)
        {
            // Analog poll
            uint8_t rx_buffer[sizeof(analog_poll)] = { 0 };
            m_spi_ptr->Transfer(+analog_poll, +rx_buffer, sizeof(analog_poll));

            // Parse results
            if (rx_buffer[1] == 0x73)
            {
                DualShock::ParseDigitalButtons(rx_buffer, current_button_states);
                DualShock::ParseAnalogButtons(rx_buffer, current_button_states);
            }
            else
            {
                // Unexpected response
            }
        }
        else
        {
            // Digital poll
            uint8_t rx_buffer[sizeof(digital_poll)] = { 0 };
            m_spi_ptr->Transfer(+digital_poll, +rx_buffer, sizeof(digital_poll));

            // Parse results
            if (rx_buffer[1] == 0x41)
            {
                DualShock::ParseDigitalButtons(rx_buffer, current_button_states);
            }
            else
            {
                // Unexpected response
            }
        }
    }

    // Enter Analog Mode and enable reading pressure values.
    bool DualShock::EnableAnalog()
    {
        // Send a series of commands to enable Analog Mode
        uint8_t rx_buffer[sizeof(analog_poll)] = {};

        // Enter Config Mode
        m_spi_ptr->Transfer(+enter_config_mode, +rx_buffer, sizeof(enter_config_mode));
        // TODO@adrian: error checking
        // TODO@adrian: wait?

        // Enable Analog Mode
        m_spi_ptr->Transfer(+enable_analog_mode, +rx_buffer, sizeof(enable_analog_mode));
        // TODO@adrian: error checking
        // TODO@adrian: wait?

        // Enable Motor Commands
        m_spi_ptr->Transfer(+enable_motor_command, +rx_buffer, sizeof(enable_motor_command));
        // TODO@adrian: error checking
        // TODO@adrian: wait?

        // Configure Pressure Sensing
        m_spi_ptr->Transfer(+config_pressure_values, +rx_buffer, sizeof(config_pressure_values));
        // TODO@adrian: error checking
        // TODO@adrian: wait?

        // Exit Config Mode
        m_spi_ptr->Transfer(+exit_config_mode, +rx_buffer, sizeof(exit_config_mode));
        // TODO@adrian: error checking

        return false;   // TODO@adrian: not fully implemented
    }

    // Enter Digital Mode and disable reading pressure values.
    bool DualShock::DisableAnalog()
    {
        // Not implemented
    }

    // Parse the digital buttons from the response.
    // Buttons are active high - if bit is low, button is pressed.
    void DualShock::ParseDigitalButtons(
        const uint8_t* digital_bytes,
        DualShock::ButtonState& button_state_out)
    {
        button_state_out.digital_valid = true;

        const uint8_t fourth = digital_bytes[3];
        button_state_out.select   = !ISBITQ(0, fourth);
        button_state_out.left3    = !ISBITQ(1, fourth);
        button_state_out.right3   = !ISBITQ(2, fourth);
        button_state_out.start    = !ISBITQ(3, fourth);
        button_state_out.d_up     = !ISBITQ(4, fourth);
        button_state_out.d_right  = !ISBITQ(5, fourth);
        button_state_out.d_down   = !ISBITQ(6, fourth);
        button_state_out.d_left   = !ISBITQ(7, fourth);

        const uint8_t fifth = digital_bytes[4];
        button_state_out.left2    = !ISBITQ(0, fifth);
        button_state_out.right2   = !ISBITQ(1, fifth);
        button_state_out.left1    = !ISBITQ(2, fifth);
        button_state_out.right1   = !ISBITQ(3, fifth);
        button_state_out.triangle = !ISBITQ(4, fifth);
        button_state_out.circle   = !ISBITQ(5, fifth);
        button_state_out.cross    = !ISBITQ(6, fifth);
        button_state_out.square   = !ISBITQ(7, fifth);
    }

    // Parse the analog buttons from the response.
    void DualShock::ParseAnalogButtons(
        const uint8_t* analog_bytes,
        DualShock::ButtonState& button_state_out)
    {
        button_state_out.analog_valid = true;

        // Analog sticks
        button_state_out.analog_left_x   = analog_bytes[DUALSHOCK_ANALOG_LX];
        button_state_out.analog_left_y   = analog_bytes[DUALSHOCK_ANALOG_LY];
        button_state_out.analog_right_x  = analog_bytes[DUALSHOCK_ANALOG_RX];
        button_state_out.analog_right_y  = analog_bytes[DUALSHOCK_ANALOG_RY];

        // Analog button pressures
        button_state_out.d_right_pressure  = analog_bytes[DUALSHOCK_ANALOG_D_RIGHT];
        button_state_out.d_left_pressure   = analog_bytes[DUALSHOCK_ANALOG_D_LEFT];
        button_state_out.d_up_pressure     = analog_bytes[DUALSHOCK_ANALOG_D_UP];
        button_state_out.d_down_pressure   = analog_bytes[DUALSHOCK_ANALOG_D_DOWN];
        button_state_out.triangle_pressure = analog_bytes[DUALSHOCK_ANALOG_TRIANGLE];
        button_state_out.circle_pressure   = analog_bytes[DUALSHOCK_ANALOG_CIRCLE];
        button_state_out.cross_pressure    = analog_bytes[DUALSHOCK_ANALOG_CROSS];
        button_state_out.square_pressure   = analog_bytes[DUALSHOCK_ANALOG_SQUARE];
        button_state_out.left1_pressure    = analog_bytes[DUALSHOCK_ANALOG_L1];
        button_state_out.right1_pressure   = analog_bytes[DUALSHOCK_ANALOG_R1];
        button_state_out.left2_pressure    = analog_bytes[DUALSHOCK_ANALOG_L2];
        button_state_out.right2_pressure   = analog_bytes[DUALSHOCK_ANALOG_R2];
    }

}   // end namespace ps2


/* // Unused stuff...

enum DeviceMode
{
    DEVICE_MODE_DIGITAL = 0x4,
    DEVICE_MODE_ANALOG  = 0x7,
    DEVICE_MODE_NATIVE  = 0xF,
};

enum CommandMode
{
    COMMAND_CONFIG_POLL   = 0x41,
    COMMAND_DATA_POLL     = 0x42,
    COMMAND_CONFIG_MODE   = 0x43,
    COMMAND_SWITCH_MODEs  = 0x44,
    COMMAND_STATUS_INFO   = 0x45,
    COMMAND_CONFIG_ANALOG = 0x4F,
};

struct DigitalButtonState
{
    // fourth byte
    bool select;    // 4.0
    bool left3;     // 4.1
    bool right3;    // 4.2
    bool start;     // 4.3
    bool d_up;      // 4.4
    bool d_right;   // 4.5
    bool d_down;    // 4.6
    bool d_left;    // 4.7
    // fifth byte
    bool left2;     // 5.0
    bool right2;    // 5.1
    bool left1;     // 5.2
    bool right1;    // 5.3
    bool triangle;  // 5.4
    bool circle;    // 5.5
    bool cross;     // 5.6
    bool square;    // 5.7
};

/** Parse
DigitalButtonState ParseDigitalButtons(uint8_t fourth, uint8_t fifth)
{
    DigitalButtonState btn_state = {};
    btn_state.select    = !BITGET(0, fourth);
    btn_state.left3     = !BITGET(1, fourth);
    btn_state.right3    = !BITGET(2, fourth);
    btn_state.start     = !BITGET(3, fourth);
    btn_state.d_up      = !BITGET(4, fourth);
    btn_state.d_right   = !BITGET(5, fourth);
    btn_state.d_down    = !BITGET(6, fourth);
    btn_state.d_left    = !BITGET(7, fourth);
    btn_state.left2     = !BITGET(0, fifth);
    btn_state.right2    = !BITGET(1, fifth);
    btn_state.left1     = !BITGET(2, fifth);
    btn_state.right1    = !BITGET(3, fifth);
    btn_state.triangle  = !BITGET(4, fifth);
    btn_state.circle    = !BITGET(5, fifth);
    btn_state.cross     = !BITGET(6, fifth);
    btn_state.square    = !BITGET(7, fifth);

    return btn_state;
}

*/
