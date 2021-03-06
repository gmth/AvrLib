/*
 * FS20Packet.hpp
 *
 *  Created on: Apr 16, 2015
 *      Author: jan
 */

#ifndef FS20PACKET_HPP_
#define FS20PACKET_HPP_

#include "Serial/SerialTx.hpp"
#include "Streams/Protocol.hpp"

namespace FS20 {

using namespace Streams;
using namespace Serial;

struct FS20Packet {
    static const uint8_t prefix[];
    static const uint8_t postfix[];

    template <typename prescaled_t>
    static constexpr SerialConfig serialConfig() {
        using namespace Time;

        return SerialConfig {
            /* highOnIdle */ false,
            prefix, 24,
            highPulseOn<prescaled_t>(400_us),
            lowPulseOn<prescaled_t>(400_us),
            highPulseOn<prescaled_t>(600_us),
            lowPulseOn<prescaled_t>(600_us),
            SerialParity::EVEN,
            SerialBitOrder::MSB_FIRST,
            postfix, 2 };
    }

    uint8_t houseCodeHi = 0;
    uint8_t houseCodeLo = 0;
    uint8_t address = 0;
    uint8_t command = 0;
    uint8_t commandExt = 0;
    uint8_t checksum = 0;

    FS20Packet() {}

    FS20Packet(uint8_t _houseCodeHi, uint8_t _houseCodeLo, uint8_t _address, uint8_t _command, uint8_t _commandExt):
        houseCodeHi(_houseCodeHi), houseCodeLo(_houseCodeLo), address(_address), command(_command), commandExt(_commandExt) {
        checksum = getExpectedChecksum();
    }

    bool hasCommandExt() const;
    uint8_t getExpectedChecksum() const;
    bool isChecksumCorrect() const;

    typedef Protocol<FS20Packet> P;
    typedef P::Seq<
        P::Binary<uint8_t, &FS20Packet::houseCodeHi>,
        P::Binary<uint8_t, &FS20Packet::houseCodeLo>,
        P::Binary<uint8_t, &FS20Packet::address>,
        P::Binary<uint8_t, &FS20Packet::command>,
        P::Conditional<&FS20Packet::hasCommandExt,
            P::Binary<uint8_t, &FS20Packet::commandExt>
        >,
        P::Binary<uint8_t, &FS20Packet::checksum>
    > DefaultProtocol;
};

}

#endif /* FS20PACKET_HPP_ */
