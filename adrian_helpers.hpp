/**
 * File:    adrian_helpers.hpp
 * Author:  padin.adrian@gmail.com
 *
 * Copyright 2019
 */


#ifndef ADRIAN_HELPERS_HPP_
#define ADRIAN_HELPERS_HPP_


/* ===== Includes ===== */
#include <stdint.h>


namespace adrian
{
    /* ===== Functions ===== */

    /* Bit Manipulation */

    // Return 1 if the bit is set, 0 otherwise
    inline uint8_t BITGET(const uint8_t byte, const uint8_t bitnum)
    {
        return (byte & (1 << bitnum)) >> bitnum;
    }

    // Set a particular bit within the byte
    inline uint8_t BITSET(uint8_t& byte, const uint8_t bitnum)
    {
        return (byte |= (1 << bitnum));
    }

    // Return true if the bit is set, false otherwise
    inline bool ISBITQ(const uint8_t byte, const uint8_t bitnum)
    {
        return static_cast<bool>(byte & (1 << bitnum));
    }

}   // end namespace adrian


#endif  // ADRIAN_HELPERS_HPP_
