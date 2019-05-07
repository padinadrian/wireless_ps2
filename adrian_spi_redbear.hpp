/**
 * File:    adrian_spi_nrf.hpp
 * Author:  padin.adrian@gmail.com
 *
 * Copyright 2019
 */


#ifndef ADRIAN_SPI_REDBEAR_HPP_
#define ADRIAN_SPI_REDBEAR_HPP_


/* ===== Includes ===== */
#include "adrian_spi.hpp"
#include "spi_master_redbear.hpp"


namespace adrian
{
    /**
     *
     */
    class RedbearSPI : public adrian::SPI
    {
    public:
        /**
         * Constructor: pass either NRF_SPI0 or NRF_SPI1
         * See "mbed.h" for more details.
         */
        RedbearSPI(NRF_SPI_Type *_spi) : spi_impl(_spi)
        {
            // Leave empty
        }

        /** Configure SPI interface with CLK, MOSI, and MISO pins. */
        virtual void Initialize(
            uint32_t clk_pin,
            uint32_t mosi_pin,
            uint32_t miso_pin)
        {
            readbear_spi.begin(clock_pin, mosi_pin, miso_pin);
        }

        /** Release the SPI interface */
        virtual void Release()
        {
            readbear_spi.endTransfer();
        }

        /** Set the transfer mode (see SPI::TransferMode) */
        virtual void SetMode(const SPI::TransferMode mode)
        {
            readbear_spi.setSPIMode(static_cast<uint8_t>(mode));
        }

        /**
         * Set the frequency in Hertz.
         * Supported frequencies:
         *    125000   (125 KHz)
         *    250000   (250 KHz)
         *    500000   (500 KHz)
         *    1000000  (1 MHz)
         *    2000000  (2 MHz)
         *    4000000  (4 MHz)
         *    8000000  (8 MHz)
         */
        virtual void SetFrequency(const uint32_t frequency)
        {
            // Round down to the closest available frequency
            if      (frequency < 250 * KHz)
            {
                redbear_spi.setFrequency(SPI_125K);
            }
            else if (frequency < 500 * KHz)
            {
                redbear_spi.setFrequency(SPI_250K);
            }
            else if (frequency < 1 * MHz)
            {
                redbear_spi.setFrequency(SPI_500K);
            }
            else if (frequency < 2 * MHz)
            {
                redbear_spi.setFrequency(SPI_1M);
            }
            else if (frequency < 4 * MHz)
            {
                redbear_spi.setFrequency(SPI_2M);
            }
            else if (frequency < 8 * MHz)
            {
                redbear_spi.setFrequency(SPI_4M);
            }
            else    // frequency >= 8 * MHz
            {
                redbear_spi.setFrequency(SPI_8M);
            }
        }

        /** Set the bit order (see SPI::BitOrder) */
        virtual void SetBitOrder(const SPI::BitOrder order)
        {
            switch (order)
            {
                case BIT_ORDER_LSB_FIRST:
                    redbear_spi.setBitORDER(LSBFIRST);
                    break;
                case BIT_ORDER_MSB_FIRST:
                default:    // fall through
                    redbear_spi.setBitORDER(MSBFIRST);
                    break;
            }
        }

    private:
        // RedBear SPI interface
        SPIClass redbear_spi;
    };

}   // end namespace adrian


#endif  // ADRIAN_SPI_REDBEAR_HPP_
