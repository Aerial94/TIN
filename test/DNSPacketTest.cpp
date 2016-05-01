#include "catch.hpp"
#include "../src/DNSPacket.hpp"

SCENARIO("DNS Packet should create header fields by parsing raw data buffer") {
    DNSPacket dnsPacket;
    GIVEN("Raw response DNS packet") {
        unsigned char data[] = {0x40, 0x11, 0x81, 0x80, 0x00, 0x01, 0x00, 0x01,
                                0x00,
                                0x00, 0x00, 0x00, 0x06, 0x67, 0x6f, 0x6f, 0x67,
                                0x6c,
                                0x65, 0x03, 0x63, 0x6f, 0x6d, 0x00, 0x00, 0x01,
                                0x00,
                                0x01, 0xc0, 0x0c, 0x00, 0x01, 0x00, 0x01, 0x00,
                                0x00,
                                0x00, 0x01, 0x00, 0x04, 0xd8, 0x3a, 0xd1, 0x4e};
        int datasize = sizeof data;
        WHEN("dns Packet is parsing data") {
            dnsPacket.parseRawBuffer(data, datasize);
            THEN("id is correct") {
                REQUIRE(dnsPacket.getIdentifier() == 0x4011);
            }
            AND_THEN("packet is response") {
                REQUIRE(dnsPacket.isResponse());
            }
            AND_THEN("packet answer is not authoritative") {
                REQUIRE(dnsPacket.isAnswerAuthoritative() == false);
            }
            AND_THEN("packet recurion was desired") {
                REQUIRE(dnsPacket.isRecursionDesired() == true);
            }
            AND_THEN("recurion is avaiable on server") {
                REQUIRE(dnsPacket.isRecursionAvailable() == true);
            }
            AND_THEN("response code is OK (0)") {
                REQUIRE(dnsPacket.getResponseCode() == 0);
            }
            AND_THEN("question count is 1") {
                REQUIRE(dnsPacket.getQuestionCount() == 1);
            }
            AND_THEN("answer count is 1") {
                REQUIRE(dnsPacket.getAnswerCount() == 1);
            }
            AND_THEN("authority count is 0") {
                REQUIRE(dnsPacket.getAuthorityCount() == 0);
            }
            AND_THEN("additional count is 0") {
                REQUIRE(dnsPacket.getAdditionalCount() == 0);
            }
        }
    }
}

SCENARIO("DNSQuestion conversion") {
    GIVEN("www.google.com domain name") {
        std::string domain = "www.google.com";
        WHEN("DNSQuestion converts it to packet format")
        {
            DNSQuestion question;
            question.toPacketFormat(domain);
            THEN("name is correctly converted") {
                char * name = question.getRawName();
                REQUIRE(name[0] == 3);
                REQUIRE(name[1] == 'w');
                REQUIRE(name[2] == 'w');
                REQUIRE(name[3] == 'w');
            }
        }
    }
}
