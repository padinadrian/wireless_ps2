/**
 *
 */

/* ===== Includes ===== */
#include <iostream>
#include <gtest/gtest.h>
#include "adrian_dualshock.hpp"
#include "adrian_helpers.hpp"
using adrian::BITGET;
using adrian::BITSET;


/* ===== Tests ===== */

TEST(PS2Test, Bitget1)
{
    EXPECT_EQ(BITGET(0xA5, 0), 1);
    EXPECT_EQ(BITGET(0xA5, 1), 0);
    EXPECT_EQ(BITGET(0xA5, 2), 1);
    EXPECT_EQ(BITGET(0xA5, 3), 0);
    EXPECT_EQ(BITGET(0xA5, 4), 0);
    EXPECT_EQ(BITGET(0xA5, 5), 1);
    EXPECT_EQ(BITGET(0xA5, 6), 0);
    EXPECT_EQ(BITGET(0xA5, 7), 1);
}

TEST(PS2Test, Bitset1)
{
    uint8_t byte = 0x00;
    EXPECT_EQ(BITSET(byte, 0), 0x01);
    EXPECT_EQ(BITSET(byte, 1), 0x03);
    EXPECT_EQ(BITSET(byte, 2), 0x07);
    EXPECT_EQ(BITSET(byte, 3), 0x0F);
    EXPECT_EQ(BITSET(byte, 4), 0x1F);
    EXPECT_EQ(BITSET(byte, 5), 0x3F);
    EXPECT_EQ(BITSET(byte, 6), 0x7F);
    EXPECT_EQ(BITSET(byte, 7), 0xFF);
}

class DebugSPI : public adrian::SPI
{
public:
    // Initialize
    virtual void Initialize(
        uint32_t clk_pin,
        uint32_t mosi_pin,
        uint32_t miso_pin)
    {
        std::cout << "init SPI" << std::endl;
        std::cout << "clk:  " << clk_pin  << std::endl;
        std::cout << "mosi: " << mosi_pin << std::endl;
        std::cout << "miso: " << miso_pin << std::endl;
    }

    // Release
    virtual void Release()
    {
        std::cout << "release SPI" << std::endl;
    }

    // Set mode
    virtual void SetMode(const TransferMode mode)
    {
        std::cout << "SPI mode: " << mode << std::endl;
    }

    // Set frequency
    virtual void SetFrequency(const uint32_t frequency)
    {
        std::cout << "SPI frequency: " << frequency << std::endl;
    }

    // Set bit order
    virtual void SetBitOrder(const BitOrder order)
    {
        std::cout << "SPI order: " << order << std::endl;
    }

    // Transfer
    virtual void Transfer(
        const uint8_t *tx_buf,
        uint8_t *rx_buf,
        const uint8_t num_bytes)
    {
        std::cout << "transfer bytes: " << (int)num_bytes << std::endl;
        for (uint8_t i = 0; i < num_bytes; ++i)
        {
            std::cout << "byte [" << (int)i << "]: " << std::hex << (int)tx_buf[i] << std::endl;
        }
    }

};

TEST(DualShock, WithDebugSpi)
{
    DebugSPI spi;
    spi.Initialize(1, 2, 3);

    adrian::DualShock controller1(&spi);

    adrian::DualShock::ButtonState buttons = {};
    controller1.Poll(5, 10, buttons);
}
