/**
 * File:    adrian_spi.hpp
 * Author:  padin.adrian@gmail.com
 *
 * Copyright 2019
 */


#ifndef ADRIAN_SPI_HPP_
#define ADRIAN_SPI_HPP_


/* ===== Includes ===== */
#include <stdint.h>


namespace adrian
{
    /**
     * SPI Interface
     * Implement this interface for each new platform.
     * This interface can then be used for any project
     * which uses the adrian::SPI class.
     */
    class SPI
    {
    public:
        /* ===== Constants ===== */
        static const uint32_t KHz = 1000;
        static const uint32_t MHz = 1000000;


        /* ===== Enums ===== */

        /** The SPI mode (CPOL and CPHA) */
        enum TransferMode
        {
            TRANSFER_MODE_0 = 0,    // CPOL = 0, CPHA = 0
            TRANSFER_MODE_1 = 1,    // CPOL = 0, CPHA = 1
            TRANSFER_MODE_2 = 2,    // CPOL = 1, CPHA = 0
            TRANSFER_MODE_3 = 3,    // CPOL = 1, CPHA = 1
        };

        /** The bit order (big endian or little endian) */
        enum BitOrder
        {
            BIT_ORDER_MSB_FIRST = 0,
            BIT_ORDER_LSB_FIRST = 1,
        };


        /* ===== Functions ===== */

        /** Obligatory virtual destructor */
        virtual ~SPI() {};

        /** Initialize the SPI interface (platform dependent) */
        virtual void Initialize(
            uint32_t clk_pin,
            uint32_t mosi_pin,
            uint32_t miso_pin) = 0;

        /** Release the SPI interface (platform dependent) */
        virtual void Release() = 0;

        /** Set the transfer mode (see SPI::TransferMode) */
        virtual void SetMode(const TransferMode mode) = 0;

        /** Set the frequency in Hertz */
        virtual void SetFrequency(const uint32_t frequency) = 0;

        /** Set the bit order (see SPI::BitOrder) */
        virtual void SetBitOrder(const BitOrder order) = 0;

        /** Perform a single full-duplex SPI transfer */
        virtual void Transfer(
            const uint8_t *tx_buf,
            uint8_t *rx_buf,
            const uint8_t num_bytes) = 0;
    };

}   // end namespace adrian


#endif  // ADRIAN_SPI_HPP_
