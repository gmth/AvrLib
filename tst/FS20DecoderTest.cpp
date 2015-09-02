#include <gtest/gtest.h>
#include <type_traits>
#include "FS20/FS20Decoder.hpp"

using namespace FS20;

namespace FS20Tests_prescaler64 {

struct FS20MockPulseCounter {
    typedef uint8_t count_t;

    struct comparator_t {
        typedef uint8_t value_t;
        static constexpr uint8_t prescalerPower2 = 6;

        template <uint32_t usecs,typename return_t>
        static constexpr return_t microseconds2counts() {
            return (F_CPU >> prescalerPower2) / 1000 * usecs / 1000;
        }
    };
};

template <typename T>
void sendData(T &decoder, const uint8_t *seq, uint16_t length) {
    bool high = true;
    for (uint16_t i = 0; i < length; i++) {
        Pulse event = { high, seq[i] };
        decoder.apply(event);
        high = !high;
    }
    decoder.apply(Pulse::empty());
}

TEST(FS20DecoderTest, fs20_decoder_can_decode_correct_bit_sequence) {
    FS20Decoder<FS20MockPulseCounter> decoder;

    const uint8_t seq1[] = { 115, 85, 107, 93, 101, 95, 100, 97, 103, 97, 99, 96, 97, 101, 96, 102, 94, 108, 89, 106, 92, 103, 142, 154, 93, 106, 95, 102, 94, 104, 139, 160, 137, 153, 93, 105, 141, 155, 141, 154, 92, 106, 140, 155, 141, 154, 143, 153, 142, 154, 141, 156, 140, 155, 140, 157, 139, 155, 93, 105, 91, 106, 91, 106, 93, 104, 93, 104, 92, 107, 90, 106, 92, 106, 92, 104, 92, 107, 91, 105, 91, 106, 92, 105, 92, 105, 92, 105, 92, 105, 93, 105, 91, 106, 93, 104, 92, 106, 92, 104, 142, 155, 92, 104, 92, 105, 92, 106, 91, 105, 92, 106, 141, 155 };
    sendData(decoder, seq1, std::extent<decltype(seq1)>::value);

    FS20Packet pkt;
    EXPECT_TRUE(decoder.in() >> pkt);
    EXPECT_EQ(27, pkt.houseCodeHi);
    EXPECT_EQ(255, pkt.houseCodeLo);
    EXPECT_EQ(0, pkt.address);
    EXPECT_EQ(0, pkt.command);
}


TEST(FS20DecoderTest, fs20_decoder_can_decode_correct_bit_sequence_2) {
    FS20Decoder<FS20MockPulseCounter> decoder;

    const uint8_t seq1[] = { 10, 9, 31, 49, 8, 12, 23, 19, 9, 13, 41, 31, 27, 63, 38, 36, 13, 20, 51, 39, 49, 52, 19, 32, 75, 31, 9, 24, 34, 9, 10, 19, 12, 15, 44, 37, 79, 15, 63, 41, 11, 21, 11, 105, 22, 76, 33, 27, 8, 16, 33, 10, 13, 9, 48, 26, 24, 33, 20, 56, 62, 10, 24, 27, 9, 10, 32, 33, 31, 42, 9, 13, 18, 14, 9, 13, 18, 14, 10, 12, 34, 14, 127, 35, 52, 10, 51, 37, 19, 39, 11, 45, 35, 37, 41, 50, 52, 31, 44, 33, 39, 16, 29, 61, 23, 71, 14, 11, 8, 10, 79, 42, 97, 12, 48, 9, 60, 16, 45, 47, 27, 46, 16, 64, 9, 24, 17, 31, 30, 35, 15, 22, 8, 13, 17, 48, 29, 11, 59, 17, 44, 12, 12, 15, 10, 9, 14, 117, 13, 10, 70, 20, 8, 10, 11, 23, 36, 10, 45, 9, 16, 8, 10, 19, 19, 43, 27, 13, 29, 11, 63, 11, 64, 37, 9, 45, 8, 9, 14, 28, 18, 16, 37, 11, 43, 11, 8, 12, 16, 12, 58, 28, 13, 13, 22, 17, 30, 68, 9, 82, 31, 14, 29, 48, 116, 49, 10, 38, 8, 10, 58, 19, 36, 46, 118, 81, 109, 90, 104, 94, 101, 97, 98, 101, 95, 102, 95, 102, 95, 103, 93, 105, 92, 105, 93, 105, 92, 105, 91, 108, 138, 156, 91, 107, 90, 108, 89, 107, 139, 157, 138, 158, 90, 107, 138, 158, 138, 158, 89, 108, 138, 157, 139, 157, 138, 157, 139, 158, 138, 157, 139, 157, 138, 158, 138, 158, 89, 109, 89, 107, 90, 107, 90, 109, 88, 108, 89, 108, 90, 107, 90, 107, 90, 108, 89, 108, 90, 107, 90, 107, 89, 109, 88, 108, 90, 108, 89, 107, 90, 109, 88, 108, 89, 108, 90, 108, 89, 107, 139, 157, 90, 107, 90, 108, 89, 108, 90, 107, 90, 106, 139, 158, 89, 0, 212, 60, 115, 83, 109, 89, 105, 93, 101, 98, 98, 100, 97, 101, 95, 103, 93, 106, 91, 105, 92, 106, 92, 105, 91, 106, 140, 156, 92, 106, 90, 107, 90, 107, 138, 158, 138, 159, 88, 108, 138, 158, 138, 158, 89, 108, 138, 158, 138, 156, 139, 157, 140, 156, 140, 156, 138, 158, 138, 159, 137, 160, 87, 109, 88, 108, 90, 107, 90, 107, 90, 109, 88, 108, 90, 107, 90, 107, 90, 108, 89, 108, 89, 107, 91, 107, 90, 106, 91, 108, 89, 107, 90, 107, 91, 106, 90, 108, 89, 108, 90, 107, 90, 108, 138, 157, 90, 108, 89, 109, 88, 108, 89, 108, 90, 106, 139, 158, 90, 0, 21, 116, 82, 108, 90, 105 };
    sendData(decoder, seq1, std::extent<decltype(seq1)>::value);

    FS20Packet pkt;
    EXPECT_TRUE(decoder.in() >> pkt);
    EXPECT_EQ(27, pkt.houseCodeHi);
    EXPECT_EQ(255, pkt.houseCodeLo);
    EXPECT_EQ(0, pkt.address);
    EXPECT_EQ(0, pkt.command);
}


TEST(FS20DecoderTest, fs20_decoder_ignores_packet_with_parity_error) {
    FS20Decoder<FS20MockPulseCounter> decoder;

    const uint8_t seq1[] = { 115, 85, 107, 93, 101, 95, 100, 97, 103, 97, 99, 96, 97, 101, 96, 102, 94, 108, 89, 106, 92, 103, 142, 154, 93, 106, 95, 102, 94, 104, 139, 160, 137, 153, 93, 105, 141, 155, 141, 154, 92, 106, 100, 100, 141, 154, 143, 153, 142, 154, 141, 156, 140, 155, 140, 157, 139, 155, 93, 105, 91, 106, 91, 106, 93, 104, 93, 104, 92, 107, 90, 106, 92, 106, 92, 104, 92, 107, 91, 105, 91, 106, 92, 105, 92, 105, 92, 105, 92, 105, 93, 105, 91, 106, 93, 104, 92, 106, 92, 104, 142, 155, 92, 104, 92, 105, 92, 106, 91, 105, 92, 106, 141, 155 };
    sendData(decoder, seq1, std::extent<decltype(seq1)>::value);

    FS20Packet pkt;
    EXPECT_FALSE(decoder.in() >> pkt);
}

TEST(FS20DecoderTest, fs20_decoder_ignores_packet_with_checksum_error_but_correct_parity) {
    FS20Decoder<FS20MockPulseCounter> decoder;

    const uint8_t seq1[] = { 115, 85, 107, 93, 101, 95, 100, 97, 103, 97, 99, 96, 97, 101, 96, 102, 94, 108, 89, 106, 92, 103, 142, 154, 93, 106, 95, 102, 94, 104, 139, 160, 137, 153, 93, 105, 141, 155, 141, 154, 92, 106, 140, 155, 141, 154, 143, 153, 142, 154, 141, 156, 140, 155, 140, 157, 139, 155, 93, 105, 91, 106, 91, 106, 93, 104, 93, 104, 92, 107, 90, 106, 92, 106, 92, 104, 92, 107, 91, 105, 91, 106, 92, 105, 92, 105, 92, 105, 92, 105, 93, 105, 91, 106, 93, 104, 92, 106, 92, 104, 142, 155, 92, 104, 92, 105, 92, 106, 91, 105, 141, 155, 92, 106 };
    sendData(decoder, seq1, std::extent<decltype(seq1)>::value);

    FS20Packet pkt;
    EXPECT_FALSE(decoder.in() >> pkt);
}

}

namespace FS20Tests_prescaler8 {

struct FS20MockPulseCounter {
    typedef uint16_t count_t;

    struct comparator_t {
        typedef uint16_t value_t;
        static constexpr uint8_t prescalerPower2 = 3;

        template <uint32_t usecs,typename return_t>
        static constexpr return_t microseconds2counts() {
            return (F_CPU >> prescalerPower2) / 1000 * usecs / 1000;
        }
    };
};

template <typename T>
void sendData(T &decoder, const uint16_t *seq, uint16_t length) {
    bool high = true;
    for (uint16_t i = 0; i < length; i++) {
        Pulse event = { high, seq[i] };
        decoder.apply(event);
        high = !high;
    }
    decoder.apply(Pulse::empty());
}

TEST(FS20DecoderTest, fs20_decoder_can_decode_correct_bit_sequence_on_prescaler_8) {
    FS20Decoder<FS20MockPulseCounter> decoder;

    const uint16_t seq1[] = { 267, 528, 100, 62, 108, 62, 134, 302, 91, 251, 62, 61, 79, 153, 149, 126, 376, 62, 422, 404, 965, 638, 892, 694, 855, 740, 827, 767, 806, 782, 793, 792, 782, 805, 776, 815, 764, 826, 756, 824, 756, 829, 753, 835, 1139, 1239, 748, 834, 745, 839, 745, 841, 1133, 1243, 1132, 1244, 740, 844, 1127, 1247, 1129, 1246, 744, 842, 1129, 1243, 1130, 1246, 1129, 1253, 1125, 1241, 1132, 1245, 1129, 1247, 1130, 1245, 1129, 1249, 741, 841, 739, 845, 743, 840, 738, 848, 738, 847, 737, 846, 736, 849, 734, 845, 738, 852, 734, 847, 734, 846, 738, 847, 739, 843, 737, 846, 738, 847, 739, 844, 739, 847, 734, 851, 737, 842, 741, 849, 734, 842, 1127, 1249, 740, 844, 739, 847, 738, 843, 740, 844, 738, 845, 1127, 1252, 735, 18587, 951, 644, 892, 702, 852, 743, 825, 768, 805, 780, 791, 800, 782, 805, 772, 817, 762, 823, 760, 826, 754, 834, 750, 835, 1133, 1240, 747, 839, 743, 846, 737, 844, 1131, 1241, 1131, 1247, 740, 838, 1136, 1241, 1133, 1248, 737, 847, 1127, 1244, 1129, 1247, 1130, 1246, 1133, 1242, 1130, 1243, 1132, 1247, 1128, 1246, 1129, 1252, 736, 844, 738, 845, 738, 847, 738, 842, 741, 843, 738, 848, 741, 844, 737, 846, 738, 844, 740, 849, 735, 844, 740, 847, 733, 850, 734, 849, 734, 846, 738, 844, 740, 847, 735, 848, 738, 848, 735, 846, 737, 843, 1130, 1247, 741, 844, 737, 848, 737, 843, 741, 844, 738, 844, 1127, 1249, 739, 18585, 947, 649, 888, 709, 846, 744, 824, 768, 809, 780, 793, 795, 783, 808, 772, 813, 765, 824, 757, 828, 753, 833, 749, 831, 1143, 1238, 747, 837, 744, 842, 739, 843, 1132, 1244, 1129, 1246, 740, 843, 1133, 1239, 1134, 1246, 740, 849

};
    sendData(decoder, seq1, std::extent<decltype(seq1)>::value);

    FS20Packet pkt;
    EXPECT_TRUE(decoder.in() >> pkt);
    EXPECT_EQ(27, pkt.houseCodeHi);
    EXPECT_EQ(255, pkt.houseCodeLo);
    EXPECT_EQ(0, pkt.address);
    EXPECT_EQ(0, pkt.command);
}

}

/*
10, 9, 31, 49, 8, 12, 23, 19, 9, 13, 41, 31, 27, 63, 38, 36, 13, 20, 51, 39, 49, 52, 19, 32, 75, 31, 9, 24, 34, 9, 10, 19, 12, 15, 44, 37, 79, 15, 63, 41, 11, 21, 11, 105, 22, 76, 33, 27, 8, 16, 33, 10, 13, 9, 48, 26, 24, 33, 20, 56, 62, 10, 24, 27, 9, 10, 32, 33, 31, 42, 9, 13, 18, 14, 9, 13, 18, 14, 10, 12, 34, 14, 127, 35, 52, 10, 51, 37, 19, 39, 11, 45, 35, 37, 41, 50, 52, 31, 44, 33, 39, 16, 29, 61, 23, 71, 14, 11, 8, 10, 79, 42, 97, 12, 48, 9, 60, 16, 45, 47, 27, 46, 16, 64, 9, 24, 17, 31, 30, 35, 15, 22, 8, 13, 17, 48, 29, 11, 59, 17, 44, 12, 12, 15, 10, 9, 14, 117, 13, 10, 70, 20, 8, 10, 11, 23, 36, 10, 45, 9, 16, 8, 10, 19, 19, 43, 27, 13, 29, 11, 63, 11, 64, 37, 9, 45, 8, 9, 14, 28, 18, 16, 37, 11, 43, 11, 8, 12, 16, 12, 58, 28, 13, 13, 22, 17, 30, 68, 9, 82, 31, 14, 29, 48, 116, 49, 10, 38, 8, 10, 58, 19, 36, 46, 118, 81, 109, 90, 104, 94, 101, 97, 98, 101, 95, 102, 95, 102, 95, 103, 93, 105, 92, 105, 93, 105, 92, 105, 91, 108, 138, 156, 91, 107, 90, 108, 89, 107, 139, 157, 138, 158, 90, 107, 138, 158, 138, 158, 89, 108, 138, 157, 139, 157, 138, 157, 139, 158, 138, 157, 139, 157, 138, 158, 138, 158, 89, 109, 89, 107, 90, 107, 90, 109, 88, 108, 89, 108, 90, 107, 90, 107, 90, 108, 89, 108, 90, 107, 90, 107, 89, 109, 88, 108, 90, 108, 89, 107, 90, 109, 88, 108, 89, 108, 90, 108, 89, 107, 139, 157, 90, 107, 90, 108, 89, 108, 90, 107, 90, 106, 139, 158, 89, 0, 212, 60, 115, 83, 109, 89, 105, 93, 101, 98, 98, 100, 97, 101, 95, 103, 93, 106, 91, 105, 92, 106, 92, 105, 91, 106, 140, 156, 92, 106, 90, 107, 90, 107, 138, 158, 138, 159, 88, 108, 138, 158, 138, 158, 89, 108, 138, 158, 138, 156, 139, 157, 140, 156, 140, 156, 138, 158, 138, 159, 137, 160, 87, 109, 88, 108, 90, 107, 90, 107, 90, 109, 88, 108, 90, 107, 90, 107, 90, 108, 89, 108, 89, 107, 91, 107, 90, 106, 91, 108, 89, 107, 90, 107, 91, 106, 90, 108, 89, 108, 90, 107, 90, 108, 138, 157, 90, 108, 89, 109, 88, 108, 89, 108, 90, 106, 139, 158, 90, 0, 21, 116, 82, 108, 90, 105,

 *
 *
 */

/*

267, 528, 100, 62, 108, 62, 134, 302, 91, 251, 62, 61, 79, 153, 149, 126, 376, 62, 422, 404, 965, 638, 892, 694, 855, 740, 827, 767, 806, 782, 793, 792, 782, 805, 776, 815, 764, 826, 756, 824, 756, 829, 753, 835, 1139, 1239, 748, 834, 745, 839, 745, 841, 1133, 1243, 1132, 1244, 740, 844, 1127, 1247, 1129, 1246, 744, 842, 1129, 1243, 1130, 1246, 1129, 1253, 1125, 1241, 1132, 1245, 1129, 1247, 1130, 1245, 1129, 1249, 741, 841, 739, 845, 743, 840, 738, 848, 738, 847, 737, 846, 736, 849, 734, 845, 738, 852, 734, 847, 734, 846, 738, 847, 739, 843, 737, 846, 738, 847, 739, 844, 739, 847, 734, 851, 737, 842, 741, 849, 734, 842, 1127, 1249, 740, 844, 739, 847, 738, 843, 740, 844, 738, 845, 1127, 1252, 735, 18587, 951, 644, 892, 702, 852, 743, 825, 768, 805, 780, 791, 800, 782, 805, 772, 817, 762, 823, 760, 826, 754, 834, 750, 835, 1133, 1240, 747, 839, 743, 846, 737, 844, 1131, 1241, 1131, 1247, 740, 838, 1136, 1241, 1133, 1248, 737, 847, 1127, 1244, 1129, 1247, 1130, 1246, 1133, 1242, 1130, 1243, 1132, 1247, 1128, 1246, 1129, 1252, 736, 844, 738, 845, 738, 847, 738, 842, 741, 843, 738, 848, 741, 844, 737, 846, 738, 844, 740, 849, 735, 844, 740, 847, 733, 850, 734, 849, 734, 846, 738, 844, 740, 847, 735, 848, 738, 848, 735, 846, 737, 843, 1130, 1247, 741, 844, 737, 848, 737, 843, 741, 844, 738, 844, 1127, 1249, 739, 18585, 947, 649, 888, 709, 846, 744, 824, 768, 809, 780, 793, 795, 783, 808, 772, 813, 765, 824, 757, 828, 753, 833, 749, 831, 1143, 1238, 747, 837, 744, 842, 739, 843, 1132, 1244, 1129, 1246, 740, 843, 1133, 1239, 1134, 1246, 740, 849,

1038, 634, 896, 695, 860, 738, 829, 758, 812, 781, 794, 796, 784, 809, 770, 814, 765, 820, 759, 826, 755, 832, 750, 837, 1138, 1233, 752, 835, 747, 836, 747, 836, 1136, 1240, 1135, 1244, 741, 840, 1131, 1247, 1129, 1244, 746, 838, 1132, 1243, 1132, 1241, 1135, 1243, 1132, 1247, 1128, 1244, 1132, 1242, 1135, 1244, 1131, 1245, 739, 842, 742, 842, 740, 846, 740, 841, 742, 843, 738, 845, 742, 847, 734, 846, 739, 842, 739, 847, 737, 850, 736, 844, 738, 845, 739, 844, 739, 845, 740, 843, 738, 847, 738, 847, 738, 841, 742, 844, 740, 844, 1129, 1246, 738, 848, 738, 842, 738, 846, 738, 847, 738, 844, 1129, 1249, 737, 18586, 947, 646, 893, 705, 850, 740, 827, 765, 807, 782, 793, 794, 784, 807, 774, 811, 769, 818, 760, 828, 754, 834, 748, 834, 1141, 1236, 750, 838, 741, 840, 743, 840, 1133, 1244, 1131, 1245, 741, 838, 1135, 1239, 1136, 1245, 740, 844, 1128, 1244, 1132, 1241, 1136, 1245, 1130, 1244, 1132, 1243, 1130, 1244, 1131, 1245, 1131, 1248, 738, 844, 743, 842, 740, 842, 739, 848, 737, 843, 741, 844, 738, 851, 733, 845, 738, 848, 736, 842, 741, 848, 735, 847, 740, 843, 738, 843, 740, 847, 738, 844, 740, 846, 736, 848, 737, 846, 741, 841, 741, 839, 1130, 1248, 738, 851, 736, 847, 734, 846, 738, 847, 738, 844, 1129, 1243, 742, 18582, 948, 651, 886, 705, 853, 743, 823, 766, 810, 779, 798, 796, 776, 810, 773, 816, 763, 829, 751, 829, 753, 832, 749, 834, 1139, 1236, 749, 836, 748, 836, 747, 837, 1133, 1242, 1136, 1250, 734, 843, 1133, 1238, 1136, 1244, 742, 841, 1131, 1243, 1131, 1244, 1132, 1251, 1124, 1244, 1134, 1239, 1134, 1245, 1128, 1249, 1127, 1246, 743, 840, 743, 846,


950, 639, 892, 703, 853, 733, 832, 762, 811, 781, 793, 798, 780, 807, 774, 812, 766, 816, 761, 828, 755, 834, 749, 830, 1141, 1237, 748, 840, 742, 842, 742, 843, 1127, 1245, 1131, 1245, 742, 838, 1135, 1246, 1129, 1244, 742, 839, 1134, 1247, 1127, 1244, 1131, 1243, 1135, 1241, 1136, 1244, 1127, 1245, 1132, 1246, 1127, 1247, 744, 841, 740, 841, 741, 848, 737, 842, 742, 843, 739, 846, 738, 842, 740, 846, 739, 843, 740, 846, 737, 847, 739, 847, 734, 847, 734, 847, 740, 845, 737, 845, 739, 845, 740, 843, 739, 850, 734, 845, 738, 844, 1131, 1248, 737, 842, 742, 845, 739, 850, 735, 844, 737, 846, 1126, 1249, 740, 18583, 942, 651, 888, 711, 849, 746, 821, 764, 807, 787, 789, 796, 785, 804, 773, 814, 766, 820, 761, 828, 754, 828, 754, 832, 1140, 1236, 748, 839, 744, 836, 748, 833, 1138, 1239, 1135, 1245, 743, 842, 1129, 1243, 1131, 1250, 739, 839, 1133, 1241, 1132, 1250, 1126, 1244, 1134, 1243, 1132, 1244, 1128, 1247, 1130, 1245, 1129, 1251, 740, 839, 743, 840, 740, 850, 736, 845, 737, 846, 737, 847, 738, 846, 737, 845, 739, 845, 736, 848, 740, 843, 739, 842, 742, 844, 738, 846, 738, 853, 732, 842, 741, 846, 736, 845, 740, 842, 740, 846, 737, 842, 1132, 1245, 741, 844, 738, 843, 742, 845, 739, 844, 737, 846, 1127, 1246, 741, 18586, 945, 653, 880, 715, 848, 742, 828, 762, 807, 783, 793, 798, 784, 801, 772, 818, 762, 822, 761, 828, 755, 829, 752, 834, 1137, 1239, 748, 837, 744, 840, 743, 837, 1137, 1242, 1130, 1246, 740, 840, 1134, 1241, 1135, 1246, 738, 843, 1129, 1244, 1133, 1242, 1136, 1241, 1130, 1247, 1128, 1247, 1130, 1244, 1134, 1244, 1129, 1250, 737, 839, 742, 844,


389, 613, 956, 641, 870, 725, 832, 767, 806, 780, 788, 803, 774, 820, 760, 825, 749, 846, 738, 842, 739, 851, 730, 855, 729, 854, 1114, 1263, 720, 872, 712, 865, 721, 862, 1109, 1266, 1106, 1273, 719, 861, 1110, 1266, 1105, 1274, 714, 867, 715, 870, 714, 874, 708, 870, 1104, 1272, 1101, 1277, 712, 872, 1098, 1270, 1104, 1275, 713, 871, 716, 867, 712, 874, 712, 869, 714, 869, 714, 871, 710, 870, 714, 871, 715, 869, 715, 871, 709, 871, 713, 872, 714, 867, 1102, 1280, 707, 869, 716, 869, 1102, 1276, 712, 872, 711, 871, 716, 864, 1106, 1274, 713, 867, 714, 873, 1098, 1279, 1103, 1267, 1104, 1277, 710, 868, 714, 872, 714, 18602, 928, 664, 869, 725, 833, 766, 799, 788, 785, 808, 766, 826, 756, 830, 744, 848, 734, 851, 732, 853, 730, 853, 727, 860, 1112, 1264, 720, 869, 716, 866, 714, 867, 1106, 1271, 1104, 1273, 712, 868, 1105, 1273, 1101, 1284, 702, 874, 715, 868, 714, 871, 707, 879, 1095, 1275, 1101, 1281, 705, 871, 1102, 1278, 1098, 1275, 710, 872, 712, 874, 711, 870, 712, 870, 711, 878, 709, 870, 712, 871, 713, 869, 713, 876, 708, 875, 707, 881, 701, 879, 708, 875, 1097, 1274, 713, 870, 712, 869, 1103, 1279, 709, 874, 708, 876, 710, 870, 1101, 1277, 706, 878, 707, 877, 1096, 1275, 1100, 1281, 1096, 1274, 710, 878, 710, 870, 712, 18603, 918, 677, 871, 721, 831, 767, 797, 797, 779, 806, 768, 823, 752, 841, 740, 843, 739, 849, 730, 857, 727, 863, 717, 863, 1110, 1268, 715, 870, 719, 870, 710, 868, 1103, 1271, 1103, 1280, 707, 870, 1101, 1274, 1099, 1280, 708, 874, 711, 870, 713, 874, 707, 876, 1099, 1273, 1101, 1279, 707, 877, 1095, 1277, 1099, 1277, 712, 872
709, 874, 713, 871, 711, 870, 711, 876, 710, 872, 714, 867, 714, 878, 703, 876, 707, 875, 710, 873, 710, 874, 710, 872, 1102, 1274, 712, 869, 723, 863, 1099, 1273, 717, 872, 709, 872, 712, 869, 1102, 1282, 703, 878, 707, 871, 1104, 1266, 1109, 1274, 1102, 1275, 707, 875, 710, 874, 707, 16641, 61

v: 4 p: 124475
v: 4 p: 135505
v: 4 p: 146459
v: 4 p: 157391
v: 4 p: 167837
v: 4 p: 178101
v: 4 p: 188666
FS20: 27, 27, 0, 18
FS20: 27, 27, 0, 18
v: 4 p: 197830
300: 974 626 908 685 867 725 839 757 816 770 808 782 791 797 786 800 778 811 768 819 765 821 758 823 1150 1228 760 825 756 829 751 837 1139 1228 1145 1232 758 829 1139 1236 1141 1237 748 833 750 831 752 842 740 837 1139 1234 1141 1236 748 833 1141 1234 1140 1236 752 833 751 831 750 838 747 838 744 835 747 838 747 833 748 838 746 834 749 834 748 839 750 837 743 835 1138 1238 748 835 750 835 1139 1235 750 834 746 842 745 835 1136 1236 752 830 753 833 1137 1239 1137 1238 1139 1234 751 838 744 835 749 18575 960 640 896 694 863 732 832 762 811 775 802 786 787 808 772 811 770 816 767 820 763 819 762 819 1152 1232 752 831 750 833 752 831 1141 1230 1149 1235 748 833 1137 1239 1138 1237 748 834 749 841 742 836 749 830 1141 1243 1134 1238 749 834 1139 1232 1144 1233 751 831 752 837 749 832 746 837 750 836 746 835 748 835 750 835 745 840 748 833 751 834 746 840 742 838 1137 1235 750 835 752 830 1141 1238 747 838 746 833 749 839 1133 1240 750 833 748 832 1140 1241 1137 1233 1139 1249 738 833 750 836 747 18578 947 648 898 695 860 734 832 759 816 773 801 787 791 798 779 813 769 815 767 813 768 820 760 828 1143 1232 755 828 752 831 753 830 1145 1234 1140 1233 750 833 1142 1235 1138 1237 748 837 751 832 749 834 750 834 1139 1235 1138 1239 749 834 1138 1231 1147 1234 749 838 744 838
55: 748 833 748 838 745 838 748 835 749 837 748 833 749 837 746 835 747 836 749 836 746 838 1134 1241 748 832 751 830 1144 1236 746 841 742 838 747 850 1121 1238 750 834 751 832 1137 1245 1134 1231 1142 1241 747 832 751 835 746 21800 102

 */
