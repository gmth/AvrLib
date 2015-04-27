/*
 * Timers.hpp
 *
 *  Created on: Dec 25, 2014
 *      Author: jan
 */

#ifndef TIMERS_HPP_
#define TIMERS_HPP_

#include "InterruptHandler.hpp"
#include "AtomicScope.hpp"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "gcc_limits.h"

namespace TimeUnits {
    template <typename t>
    constexpr t pow(t base, int exp) {
      return (exp > 0) ? base * pow(base, exp-1) : 1;
    };

    template <char...> struct literal;
    template <> struct literal<> {
      static constexpr uint32_t to_uint32 = 0;
    };
    template <char c, char ...cv> struct literal<c, cv...> {
      static constexpr uint32_t to_uint32 = (c - '0') * pow(10, sizeof...(cv)) + literal<cv...>::to_uint32;
    };

    template<typename Base, int percentage>
    class MultipliedTimeUnit;

    template <char... cv>
    class TimeUnit {
        typedef literal<cv...> value;
    public:
        static constexpr uint32_t to_uint32 = value::to_uint32;
    };

    template<typename Base, int percentage>
    class MultipliedTimeUnit {
    public:
        MultipliedTimeUnit() {
            static_assert(percentage > 0, "percentage must be larger than zero");
        }
        template <typename prescaled_t, uint32_t value = Base::to_uint32>
        static constexpr uint16_t toCounts() {
            return Base::template toCounts<prescaled_t, uint32_t(value) * percentage / 100>();
        }
    };

    template <char... cv>
    class Microseconds: public TimeUnit<cv...> {
    public:
        template <typename prescaled_t, uint32_t value = TimeUnit<cv...>::to_uint32>
        static constexpr uint16_t toCounts() {
            return prescaled_t::template microseconds2counts<value>();
        }

        template <int percentage>
        static constexpr MultipliedTimeUnit<Microseconds<cv...>,percentage> percent() {
            return MultipliedTimeUnit<Microseconds<cv...>,percentage>();
        }
    };

    template <char... cv>
    class Milliseconds: public TimeUnit<cv...> {
    public:
        template <typename prescaled_t, uint32_t value = TimeUnit<cv...>::to_uint32>
        static constexpr uint16_t toCounts() {
            return prescaled_t::template milliseconds2counts<value>();
        }

        template <int percentage>
        static constexpr MultipliedTimeUnit<Milliseconds<cv...>,percentage> percent() {
            return MultipliedTimeUnit<Milliseconds<cv...>,percentage>();
        }
    };

    template <char ...cv>
    constexpr Microseconds<cv...> operator "" _us() { return Microseconds<cv...>(); }

    template <char ...cv>
    constexpr Milliseconds<cv...> operator "" _ms() { return Milliseconds<cv...>(); }
};

ISR(TIMER0_OVF_vect);
ISR(TIMER1_OVF_vect);
ISR(TIMER2_OVF_vect);

enum class ExtPrescaler: uint8_t {
    _1 = _BV(CS00),
    _8 = _BV(CS01),
    _64 = _BV(CS00) | _BV(CS01),
    _256 = _BV(CS02),
    _1024 = _BV(CS02) | _BV(CS00)
};

enum class IntPrescaler: uint8_t {
    _1 = _BV(CS00),
    _8 = _BV(CS01),
    _32 = _BV(CS00) | _BV(CS01),
    _64 = _BV(CS02),
    _128 = _BV(CS02) | _BV(CS00),
    _256 = _BV(CS02) | _BV(CS01),
    _1024 = _BV(CS02) | _BV(CS01) | _BV(CS00)
};

extern InterruptChain tm0int;
extern InterruptChain tm0ocra;
extern InterruptChain tm0ocrb;
extern InterruptChain tm1int;
extern InterruptChain tm1ocra;
extern InterruptChain tm1ocrb;
extern InterruptChain tm2int;
extern InterruptChain tm2ocra;
extern InterruptChain tm2ocrb;

template<typename prescaler_t, prescaler_t prescaler>
struct PrescalerMeta {};

template<> struct PrescalerMeta<ExtPrescaler,ExtPrescaler::_1> {
    constexpr static uint8_t power2 = 0;
};

template<> struct PrescalerMeta<ExtPrescaler,ExtPrescaler::_8> {
    constexpr static uint8_t power2 = 3;
};

template<> struct PrescalerMeta<ExtPrescaler,ExtPrescaler::_64> {
    constexpr static uint8_t power2 = 6;
};

template<> struct PrescalerMeta<ExtPrescaler,ExtPrescaler::_256> {
    constexpr static uint8_t power2 = 8;
};

template<> struct PrescalerMeta<ExtPrescaler,ExtPrescaler::_1024> {
    constexpr static uint8_t power2 = 10;
};

template<> struct PrescalerMeta<IntPrescaler,IntPrescaler::_1> {
    constexpr static uint8_t power2 = 0;
};

template<> struct PrescalerMeta<IntPrescaler,IntPrescaler::_8> {
    constexpr static uint8_t power2 = 3;
};

template<> struct PrescalerMeta<IntPrescaler,IntPrescaler::_32> {
    constexpr static uint8_t power2 = 5;
};

template<> struct PrescalerMeta<IntPrescaler,IntPrescaler::_64> {
    constexpr static uint8_t power2 = 6;
};

template<> struct PrescalerMeta<IntPrescaler,IntPrescaler::_128> {
    constexpr static uint8_t power2 = 7;
};

template<> struct PrescalerMeta<IntPrescaler,IntPrescaler::_256> {
    constexpr static uint8_t power2 = 8;
};

template<> struct PrescalerMeta<IntPrescaler,IntPrescaler::_1024> {
    constexpr static uint8_t power2 = 10;
};

template <typename _value_t, typename prescaler_t, prescaler_t _prescaler>
class Prescaled {
    typedef PrescalerMeta<prescaler_t,_prescaler> Meta;
public:
    typedef _value_t value_t;
    static constexpr prescaler_t prescaler = _prescaler;
    static constexpr uint8_t prescalerPower2 = Meta::power2;
    template <uint32_t usecs>
    static constexpr value_t microseconds2counts() {
        static_assert((uint32_t(F_CPU) >> prescalerPower2) / 1000 * usecs / 1000 > 1,
                "Number of counts for microseconds is so low that it rounds to 0 or 1, you might want to decrease the timer prescaler.");
        static_assert((uint32_t(F_CPU) >> prescalerPower2) / 1000 * usecs / 1000 <= std::numeric_limits<value_t>::max(),
                "Number of counts for microseconds does not fit in value_t, you might want to increase the timer prescaler.");
        return (F_CPU >> prescalerPower2) / 1000 * usecs / 1000;
    }
    template <uint32_t msecs>
    static constexpr value_t milliseconds2counts() {
        static_assert((uint32_t(F_CPU) >> prescalerPower2) / 1000 * msecs > 1,
                "Number of counts for milliseconds is so low that it rounds to 0 or 1, you might want to decrease the timer prescaler.");
        static_assert((uint32_t(F_CPU) >> prescalerPower2) / 1000 * msecs <= std::numeric_limits<value_t>::max(),
                "Number of counts for milliseconds does not fit in value_t, you might want to increase the timer prescaler.");
        return (F_CPU >> prescalerPower2) / 1000 * msecs;
    }
};

template <typename info>
class TimerComparator {
public:

    inline typename info::value_t getValue() const {
        return *info::tcnt;
    }
    inline InterruptChain &interrupt() const {
        return *info::handler;
    }
    inline void interruptOn() const {
        *info::tifr |= _BV(info::tifr_bit); // Datasheet: "OCF is cleared by writing logic 1 to the bit"
        *info::timsk |= _BV(info::timsk_bit);
    }
    inline void interruptOff() const {
        *info::timsk &= ~_BV(info::timsk_bit);
    }
};

enum class NonPWMOutputMode: uint8_t {
    disconnected = 0,
    toggle_on_match = 1,
    low_on_match = 2,
    high_on_match = 3
};

template <typename info, typename prescaler_t, prescaler_t prescaler>
class NonPWMTimerComparator: public TimerComparator<info>, public Prescaled<typename info::value_t, prescaler_t, prescaler> {
public:
    /**
     * Sets the pin output mode, i.e. what should happen to this comparator's linked
     * pin whenever the comparator matches.
     */
    void output(NonPWMOutputMode mode) const {
        *info::tccra = (*info::tccra & ~(info::output_mode_bitmask)) | (static_cast<uint8_t>(mode) << info::output_mode_bitstart);
    }
    /**
     * Sets the target at which the next comparator match event is to take place.
     * Takes effect immediately.
     */
    inline void setTarget(typename info::value_t value) {
        *info::ocr = value;
    }
};

enum class FastPWMOutputMode: uint8_t {
    disconnected = 0,
    connected = 2,
    connected_inverting = 3
};

template <typename info, typename prescaler_t, prescaler_t prescaler>
class FastPWMTimerComparator: public TimerComparator<info>, public Prescaled<typename info::value_t, prescaler_t, prescaler> {
public:
    /**
     * Sets the pin output mode, i.e. what should happen to this comparator's linked
     * pin whenever the comparator matches.
     */
    void output(FastPWMOutputMode mode) const {
        *info::tccra = (*info::tccra & ~(info::output_mode_bitmask)) | (static_cast<uint8_t>(mode) << info::output_mode_bitstart);
    }
    /**
     * Sets the target at which the next comparator match event is to take place.
     * Takes effect at the start of the next timer run (i.e. after the next overflow).
     */
    inline void setTargetFromNextRun(typename info::value_t value) {
        *info::ocr = value;
    }
};

template <typename info, typename comparator_a_t, typename comparator_b_t>
class Timer {
public:
    typedef info timer_info_t;
    typedef comparator_a_t comparatorA_t;
    typedef comparator_b_t comparatorB_t;
private:
    comparator_a_t comparator_a;
    comparator_b_t comparator_b;
public:
    inline InterruptChain &interruptOnOverflow() const {
        return *info::intHandler;
    }
    inline void interruptOnOverflowOn() const {
        *info::tifr |= _BV(TOV0); // Datasheet: "[...] is cleared by writing logic 1 to the bit"
        *info::timsk |= _BV(TOIE0);
    }
    inline void interruptOnOverflowOff() const {
        *info::timsk &= ~_BV(TOIE0);
    }
    inline typename info::value_t getValue() const {
        return *info::tcnt;
    }
    inline bool isOverflow() const {
        return *info::tifr & _BV(TOV0);
    }
    inline comparator_a_t &comparatorA() {
        return comparator_a;
    }
    inline comparator_b_t &comparatorB() {
        return comparator_b;
    }

    static constexpr typename info::value_t maximum = std::numeric_limits<typename info::value_t>::max();
    /** 8 for 8-bit timer, 16 for 16-bit timer */
    static constexpr uint8_t maximumPower2 = sizeof(typename info::value_t) * 8;
};

template <typename info, typename info::prescaler_t _prescaler, typename comparator_a_t, typename comparator_b_t>
class PrescaledTimer : public Timer<info, comparator_a_t, comparator_b_t>,
                       public Prescaled<typename info::value_t, typename info::prescaler_t, _prescaler>
{};

/**
 * In fast PWM mode, changes to the comparator values apply on the next timer run.
 * However, hardware PWM can apply those comparator values.
 */
template <typename info, typename info::prescaler_t _prescaler>
class FastPWMTimer: public PrescaledTimer<info, _prescaler,
                      FastPWMTimerComparator<typename info::ComparatorA, typename info::prescaler_t, _prescaler>,
                      FastPWMTimerComparator<typename info::ComparatorB, typename info::prescaler_t, _prescaler>> {
public:
    inline FastPWMTimer() {
        info::configureFastPWM(_prescaler);
    }
    typedef void is_fastpwm;

};

/**
 * In normal mode, changes to the comparator values apply immediately.
 * However, no hardware PWM can be performed.
 */
template <typename info, typename info::prescaler_t _prescaler>
class NormalTimer: public PrescaledTimer<info, _prescaler,
                     NonPWMTimerComparator<typename info::ComparatorA, typename info::prescaler_t, _prescaler>,
                     NonPWMTimerComparator<typename info::ComparatorB, typename info::prescaler_t, _prescaler>> {
public:
    inline NormalTimer() {
        info::configureNormal(_prescaler);
    }
    typedef void is_normal;

};

struct Timer0Info {
    static constexpr volatile uint8_t *tccra = &TCCR0A;
    static constexpr volatile uint8_t *tccrb = &TCCR0B;
    static constexpr volatile uint8_t *tcnt = &TCNT0;
    static constexpr volatile uint8_t *timsk = &TIMSK0;
    static constexpr volatile uint8_t *tifr = &TIFR0;

    static constexpr InterruptChain* intHandler = &tm0int;

    typedef uint8_t value_t;
    typedef ExtPrescaler prescaler_t;

    inline static void configureNormal(prescaler_t p) {
        // Existing settings for COM0A1 etc. should be kept
        AtomicScope::SEI _;
        *tccra &= ~_BV(WGM00);
        *tccra &= ~_BV(WGM01); // Mode 0, normal
        *tccrb = static_cast<uint8_t>(p);
    }

    inline static void configureFastPWM(prescaler_t p) {
        // Existing settings for COM0A1 etc. should be kept
        AtomicScope::SEI _;
        *tccra |= _BV(WGM00);
        *tccra |= _BV(WGM01); // Mode 3, count up to 0xFF
        *tccrb = static_cast<uint8_t>(p);
    }

    struct Comparator {
        typedef uint8_t value_t;
        static constexpr volatile uint8_t *tcnt = &TCNT0;
        static constexpr volatile uint8_t *timsk = &TIMSK0;
        static constexpr volatile uint8_t *tifr = &TIFR0;
        static constexpr volatile uint8_t *tccra = &TCCR0A;
    };

    struct ComparatorA: public Comparator {
        static constexpr volatile uint8_t *ocr = &OCR0A;
        static constexpr uint8_t timsk_bit = OCIE0A;
        static constexpr uint8_t tifr_bit = OCF0A;
        static constexpr InterruptChain* handler = &tm0ocra;
        static constexpr uint8_t output_mode_bitmask = (1 << COM0A0) | (1 << COM0A1);
        static constexpr uint8_t output_mode_bitstart = COM0A0;

    };
    struct ComparatorB: public Comparator {
        static constexpr volatile uint8_t *ocr = &OCR0B;
        static constexpr uint8_t timsk_bit = OCIE0B;
        static constexpr uint8_t tifr_bit = OCF0B;
        static constexpr InterruptChain* handler = &tm0ocrb;
        static constexpr uint8_t output_mode_bitmask = (1 << COM0B0) | (1 << COM0B1);
        static constexpr uint8_t output_mode_bitstart = COM0B0;
    };
};

struct Timer1Info {
    static constexpr volatile uint8_t *tccra = &TCCR1A;
    static constexpr volatile uint8_t *tccrb = &TCCR1B;
    static constexpr volatile uint16_t *tcnt = &TCNT1;
    static constexpr volatile uint8_t *timsk = &TIMSK1;
    static constexpr volatile uint8_t *tifr = &TIFR1;

    static constexpr InterruptChain* intHandler = &tm1int;

    static constexpr uint16_t maximum = 65535;
    static constexpr uint8_t maximumPower2 = 16;

    typedef uint16_t value_t;
    typedef ExtPrescaler prescaler_t;

    inline static void configureNormal(prescaler_t p) {
        // Existing settings for COM0A1 etc. should be kept
        AtomicScope::SEI _;
        *tccra &= ~_BV(WGM10);
        *tccra &= ~_BV(WGM11);
        *tccrb = static_cast<uint8_t>(p);
    }

    inline static void configureFastPWM(prescaler_t p) {
        // Existing settings for COM0A1 etc. should be kept
        AtomicScope::SEI _;
        *tccra &= ~_BV(WGM10);
        *tccra |= _BV(WGM11);
        *tccrb = _BV(WGM13) | _BV(WGM12) | static_cast<uint8_t>(p);
        ICR1 = 0xFFFF; // Mode 14, count up to ICR1, which we always set at 0xFFFF.
    }

    struct Comparator {
        typedef uint16_t value_t;
        static constexpr volatile uint16_t *tcnt = &TCNT1;
        static constexpr volatile uint8_t *timsk = &TIMSK1;
        static constexpr volatile uint8_t *tifr = &TIFR1;
        static constexpr volatile uint8_t *tccra = &TCCR1A;
    };

    struct ComparatorA: public Comparator {
        static constexpr volatile uint16_t *ocr = &OCR1A;
        static constexpr uint8_t timsk_bit = OCIE1A;
        static constexpr uint8_t tifr_bit = OCF1A;
        static constexpr InterruptChain* handler = &tm1ocra;
        static constexpr uint8_t output_mode_bitmask = (1 << COM1A0) | (1 << COM1A1);
        static constexpr uint8_t output_mode_bitstart = COM1A0;
    };
    struct ComparatorB: public Comparator {
        static constexpr volatile uint16_t *ocr = &OCR1B;
        static constexpr uint8_t timsk_bit = OCIE1B;
        static constexpr uint8_t tifr_bit = OCF1B;
        static constexpr InterruptChain* handler = &tm1ocrb;
        static constexpr uint8_t output_mode_bitmask = (1 << COM1B0) | (1 << COM1B1);
        static constexpr uint8_t output_mode_bitstart = COM1B0;
    };
};

struct Timer2Info {
    static constexpr volatile uint8_t *tccra = &TCCR2A;
    static constexpr volatile uint8_t *tccrb = &TCCR2B;
    static constexpr volatile uint8_t *tcnt = &TCNT2;
    static constexpr volatile uint8_t *timsk = &TIMSK2;
    static constexpr volatile uint8_t *tifr = &TIFR2;

    static constexpr InterruptChain* intHandler = &tm2int;

    static constexpr uint8_t maximum = 255;
    static constexpr uint8_t maximumPower2 = 8;

    typedef uint8_t value_t;
    typedef IntPrescaler prescaler_t;

    inline static void configureNormal(prescaler_t p) {
        // Existing settings for COM0A1 etc. should be kept
        AtomicScope::SEI _;
        *tccra &= ~_BV(WGM20);
        *tccra &= ~_BV(WGM21); // Mode 0, normal
        *tccrb = static_cast<uint8_t>(p);
    }

    inline static void configureFastPWM(prescaler_t p) {
        // Existing settings for COM0A1 etc. should be kept
        AtomicScope::SEI _;
        *tccra |= _BV(WGM20);
        *tccra |= _BV(WGM21); // Mode 3, count up to 0xFF
        *tccrb = static_cast<uint8_t>(p);
    }

    struct Comparator {
        typedef uint8_t value_t;
        static constexpr volatile uint8_t *tcnt = &TCNT2;
        static constexpr volatile uint8_t *timsk = &TIMSK2;
        static constexpr volatile uint8_t *tifr = &TIFR2;
        static constexpr volatile uint8_t *tccra = &TCCR2A;
    };

    struct ComparatorA: public Comparator {
        static constexpr volatile uint8_t *ocr = &OCR2A;
        static constexpr uint8_t timsk_bit = OCIE2A;
        static constexpr uint8_t tifr_bit = OCF2A;
        static constexpr InterruptChain* handler = &tm2ocra;
        static constexpr uint8_t output_mode_bitmask = (1 << COM2A0) | (1 << COM2A1);
        static constexpr uint8_t output_mode_bitstart = COM2A0;
    };
    struct ComparatorB: public Comparator {
        static constexpr volatile uint8_t *ocr = &OCR2B;
        static constexpr uint8_t timsk_bit = OCIE2B;
        static constexpr uint8_t tifr_bit = OCF2B;
        static constexpr InterruptChain* handler = &tm2ocrb;
        static constexpr uint8_t output_mode_bitmask = (1 << COM2B0) | (1 << COM2B1);
        static constexpr uint8_t output_mode_bitstart = COM2B0;
    };
};

template <ExtPrescaler prescaler> using Timer0_Normal = NormalTimer<Timer0Info,prescaler>;
template <ExtPrescaler prescaler> using Timer1_Normal = NormalTimer<Timer1Info,prescaler>;
template <IntPrescaler prescaler> using Timer2_Normal = NormalTimer<Timer2Info,prescaler>;
template <ExtPrescaler prescaler> using Timer0_FastPWM = FastPWMTimer<Timer0Info,prescaler>;
template <ExtPrescaler prescaler> using Timer1_FastPWM = FastPWMTimer<Timer1Info,prescaler>;
template <IntPrescaler prescaler> using Timer2_FastPWM = FastPWMTimer<Timer2Info,prescaler>;

#endif /* TIMERS_HPP_ */
