#include "catch.hpp"
#include "../src/DNSPacket.hpp"
#include "../src/UDPSocket.hpp"

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

SCENARIO("DNS Packet should crate raw data buffer") {
}

SCENARIO("DNSQuestion to raw conversion") {
    GIVEN("www.google.com domain name") {
        std::string domain = "www.google.com";
        WHEN("DNSQuestion converts it to packet format") {
            DNSQuestion question(domain);
            THEN("name is correctly converted") {
                char *name = question.getRaw();
                REQUIRE(name[0] == 3);
                REQUIRE(name[1] == 'w');
                REQUIRE(name[2] == 'w');
                REQUIRE(name[3] == 'w');
                REQUIRE(name[4] == 6);
                REQUIRE(name[5] == 'g');
                REQUIRE(name[6] == 'o');
                REQUIRE(name[7] == 'o');
                REQUIRE(name[8] == 'g');
                REQUIRE(name[9] == 'l');
                REQUIRE(name[10] == 'e');
                REQUIRE(name[11] == 3);
                REQUIRE(name[12] == 'c');
                REQUIRE(name[13] == 'o');
                REQUIRE(name[14] == 'm');
                REQUIRE(name[15] == '\0');
                REQUIRE(name[16] == 0);
                REQUIRE(name[17] == 1);
                REQUIRE(name[18] == 0);
                REQUIRE(name[19] == 1);
                free(name);
            }
        }
    }
    GIVEN("elka.pw.edu.pl domain name") {
        std::string domain = "elka.pw.edu.pl";
        WHEN("DNSQuestion converts it to packet format") {
            DNSQuestion question(domain);
            THEN("name is correctly converted") {
                char *name = question.getRaw();
                REQUIRE(name[0] == 4);
                REQUIRE(name[1] == 'e');
                REQUIRE(name[2] == 'l');
                REQUIRE(name[3] == 'k');
                REQUIRE(name[4] == 'a');
                REQUIRE(name[5] == 2);
                REQUIRE(name[6] == 'p');
                REQUIRE(name[7] == 'w');
                REQUIRE(name[8] == 3);
                REQUIRE(name[9] == 'e');
                REQUIRE(name[10] == 'd');
                REQUIRE(name[11] == 'u');
                REQUIRE(name[12] == 2);
                REQUIRE(name[13] == 'p');
                REQUIRE(name[14] == 'l');
                REQUIRE(name[15] == '\0');
                REQUIRE(name[16] == 0);
                REQUIRE(name[17] == 1);
                REQUIRE(name[18] == 0);
                REQUIRE(name[19] == 1);
                free(name);
            }
        }
    }
}

SCENARIO("DNSQuestion from raw conversion") {
    GIVEN("raw data") {
        char data[] = {
                3, 'w', 'w', 'w',
                6, 'g', 'o', 'o', 'g', 'l', 'e',
                3, 'c', 'o', 'm', 0,
                0, 1, 0, 1
        };
        WHEN("DNSQuestion converts it to string") {
            DNSQuestion question;
            int size = question.setRaw(data);
            THEN("name is correctly converted") {
                std::string domainName = question.getDomainName();
                REQUIRE(domainName == "www.google.com");
                REQUIRE(size == sizeof data);
            }
        }
    }
    GIVEN("raw data") {
        char data[] = {
                3, 'w', 'w', 'w',
                4, 'e', 'l', 'k', 'a',
                2, 'p', 'w',
                3, 'e', 'd', 'u',
                2, 'p', 'l', 0,
                0, 1, 0, 1
        };
        WHEN("DNSQuestion converts it to string") {
            DNSQuestion question;
            int size = question.setRaw(data);
            THEN("name is correctly converted") {
                std::string domainName = question.getDomainName();
                REQUIRE(domainName == "www.elka.pw.edu.pl");
                REQUIRE(size == sizeof data);
            }
        }
    }
}

SCENARIO("DNSAuthorityServer from raw conversion") {
    GIVEN("raw data") {
        unsigned char data[] = {
                0xc0, 0x17, 0x00, 0x02, 0x00, 0x01, 0x00, 0x02, 0xa3, 0x00,
                0x00, 0x14,
                0x01, 0x6d, 0x0c, 0x67, 0x74, 0x6c, 0x64, 0x2d, 0x73, 0x65,
                0x72, 0x76, 0x65,
                0x72, 0x73, 0x03, 0x6e, 0x65, 0x74, 0x00
        };
        WHEN("DNSAuthorityServer converts it") {
            DNSAuthoritativeNameServer authoritativeNameServer;
            authoritativeNameServer.fromRaw(data, 0, nullptr);
            THEN("conversion is correct") {
                int size = authoritativeNameServer.getSize();
                REQUIRE(size == sizeof data);
            }
        }
    }
    GIVEN("raw data") {
        unsigned char data[] = {
                0xc0,
                0x17,
                0x00,
                0x02,
                0x00,
                0x01,
                0x00,
                0x02,
                0xa3,
                0x00,
                0x00,
                0x04,
                0x01,
                0x6c,
                0xc0,
                0x2e
        };
        WHEN("DNSAuthorityServer converts it") {
            DNSAuthoritativeNameServer authoritativeNameServer;
            authoritativeNameServer.fromRaw(data, 0, nullptr);
            THEN("conversion is correct") {
                int size = authoritativeNameServer.getSize();
                REQUIRE(size == sizeof data);
            }
        }
    }
}

SCENARIO("DNSAditional from raw conversion") {
    GIVEN("raw data") {
        unsigned char data[] = {
                0xc0,0x2c, 0x00,0x01,0x00,0x01,0x00,0x02,0xa3,0x00,0x00,0x04,
                0xc0,0x37,0x53,0x1e
        };
        WHEN("DNSAdditional converts it") {
            DNSAdditionalRecord additionalRecord;
            additionalRecord.fromRaw(data,0);
            THEN("conversion is correct") {
                int size = additionalRecord.getSize();
                REQUIRE(size == sizeof data);
                int ip = additionalRecord.getIP();
                REQUIRE(ip == 0x1e5337c0);
            }
        }
    }
}

SCENARIO("FQDN from raw conversion") {
    GIVEN("raw data") {
        unsigned char data[] = {
                0xc0,0x2c
        };
        WHEN("FQDN converts it") {
            FQDN fqdn;
            fqdn.fromRaw(data);
            THEN("conversion is correct") {
                REQUIRE(fqdn.getPointer()== 0x2c);
            }
        }
    }
    GIVEN("raw data") {
        unsigned char data[] = {
                0xc0,0x2d
        };
        WHEN("FQDN converts it") {
            FQDN fqdn;
            fqdn.fromRaw(data);
            THEN("conversion is correct") {
                REQUIRE(fqdn.getPointer()== 0x2d);
            }
        }
    }
}
