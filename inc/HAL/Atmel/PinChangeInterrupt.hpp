#ifndef HAL_ATMEL_PINCHANGEINTERRUPT_HPP_
#define HAL_ATMEL_PINCHANGEINTERRUPT_HPP_

#include "HAL/Atmel/InterruptVectors.hpp"
#include "AtomicScope.hpp"

extern uint8_t intInvocations;

namespace HAL {
namespace Atmel {

template <typename pcintInfo>
class PinChangeSupport {
    static constexpr uint8_t PCIE = _BV(pcintInfo::PCIE);

    static uint8_t last;
    static uint8_t rising;
    static uint8_t directional;

    template <uint8_t bitmask>
    static bool shouldInvoke(uint8_t now) {
        uint8_t changed = now ^ last;
        if (!(changed & bitmask)) return false;
        if (!(directional & bitmask)) return true;
        return (now & bitmask) == (rising & bitmask);
    }

    static void enablePCINT() {
        AtomicScope _;

        if ((PCICR & PCIE) == 0) {
            last = *pcintInfo::pin;
            PCICR |= PCIE;
        }
    }

    static void disablePCINTIfNeeded() {
        AtomicScope _;

        if ((PCICR & PCIE) != 0) {
            if (*pcintInfo::pcmsk == 0) { // no more handlers are registered
                PCICR &= ~PCIE;
            }
        }
    }

public:
    template <uint8_t bitmask, typename body_t>
    static void wrap(body_t body) {
        uint8_t now = *pcintInfo::pin;
        if (shouldInvoke<bitmask>(now)) {
            body();
        }
        last = now;
    }

    template <uint8_t bitmask>
    static void interruptOnChange() {
        intInvocations++;
        directional &= ~bitmask;
        enablePCINT();
        *pcintInfo::pcmsk |= bitmask;
    }

    template <uint8_t bitmask>
    static void interruptOnRising() {
        rising |= bitmask;
        directional |= bitmask;
        enablePCINT();
        *pcintInfo::pcmsk |= bitmask;
    }

    template <uint8_t bitmask>
    static void interruptOnFalling() {
        rising &= ~bitmask;
        directional |= bitmask;
        enablePCINT();
        *pcintInfo::pcmsk |= bitmask;
    }

    template <uint8_t bitmask>
    static void interruptOff() {
        *pcintInfo::pcmsk &= ~bitmask;
        disablePCINTIfNeeded();
    }
};

template <typename pcintInfo> uint8_t PinChangeSupport<pcintInfo>::last = 0;
template <typename pcintInfo> uint8_t PinChangeSupport<pcintInfo>::rising = 0;
template <typename pcintInfo> uint8_t PinChangeSupport<pcintInfo>::directional = 0;

template <typename pcintInfo, uint8_t bitmask>
struct PinChangeVector {
    typedef typename pcintInfo::PCINT VECT;
    typedef PinChangeSupport<pcintInfo> support;

    template <typename body_t>
    static void wrap(body_t body) {
        intInvocations++;
        support::wrap(body);
    }
};

template <typename pcintInfo, uint8_t bitmask>
class PinChangeInterrupt {
public:
    typedef PinChangeVector<pcintInfo, bitmask> INT;

    /**
     * Invokes an attached interrupt handler whenever the pin changes value.
     */
    void interruptOnChange() {
        INT::support::template interruptOnChange<bitmask>();
    }

    /**
     * Invokes an attached interrupt handler whenever the pin goes from low to high.
     */
    void interruptOnRising() {
        INT::support::template interruptOnRising<bitmask>();
    }

    /**
     * Invokes an attached interrupt handler whenever the pin goes from high to low.
     */
    void interruptOnFalling() {
        INT::support::template interruptOnFalling<bitmask>();
    }

    void interruptOff() {
        INT::support::template interruptOff<bitmask>();
    }
};

}
}


#endif /* HAL_ATMEL_PINCHANGEINTERRUPT_HPP_ */
