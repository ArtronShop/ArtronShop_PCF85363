#include "ArtronShop_PCF85363A.h"


ArtronShop_PCF85363A::ArtronShop_PCF85363A(TwoWire *wire) : _wire(wire) {
    // ----
}

bool ArtronShop_PCF85363A::write_reg(uint8_t reg, uint8_t *value, size_t len) {
    this->_wire->beginTransmission(this->_addr);
    this->_wire->write(reg);
    this->_wire->write(value, len);
    return this->_wire->endTransmission() == 0;
}

bool ArtronShop_PCF85363A::write_reg(uint8_t reg, uint8_t value) {
    return this->write_reg(reg, &value, 1);
}

bool ArtronShop_PCF85363A::read_reg(uint8_t reg, uint8_t *value, size_t len) {
    this->_wire->beginTransmission(this->_addr);
    this->_wire->write(reg);
    if (this->_wire->endTransmission(false) != 0) {
        return false;
    }

    int n = this->_wire->requestFrom(this->_addr, len);
    if (n != len) {
        return false;
    }

    this->_wire->readBytes(value, len);

    return true;
}

uint8_t ArtronShop_PCF85363A::read_reg(uint8_t reg) {
    uint8_t value = 0;
    this->read_reg(reg, &value, 1);
    return value;
}

bool ArtronShop_PCF85363A::stop(bool stop) {
    return this->write_reg(0x2E, stop ? 0x01 : 0x00);
}

uint8_t ArtronShop_PCF85363A::bcd2bin(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0xFF);
}

uint8_t ArtronShop_PCF85363A::bin2bcd(uint8_t bin) {
    return ((bin / 10) << 4) | (bin % 10);
}

#define CHECK_OK(OP) { \
    if (!(OP)) { \
        return false; \
    } \
}

bool ArtronShop_PCF85363A::begin() {
    CHECK_OK(this->stop(false)); // clear STOP bit

    /* 
     * Function register:
     *   [7]   100TH: 0 ->  disabled
     *   [6:5] Periodic interrupt: 0 -> no periodic interrupt
     *   [4]   RTCM: 0 -> real-time clock mode
     *   [3]   STOPM: 0 -> RTC stop is controlled by STOP bit only
     *   [2:0] Clock output frequency: 000 (Default value)
     */
    CHECK_OK(this->write_reg(0x28, 0x00)); 
    
    // Battery switch register
    CHECK_OK(this->write_reg(0x26, 0x00)); // enable battery switch feature

    return true;
}

typedef struct __attribute__ ((__packed__)) {
    uint32_t OS : 1;
    uint32_t SEC : 7;
    uint32_t EMON : 1;
    uint32_t MIN : 7;
    uint32_t _unused1 : 2;
    uint32_t HOUR : 6;
    uint32_t _unused2 : 2;
    uint32_t DAY : 6;
    uint32_t _unused3 : 5;
    uint32_t WEEKDAY : 3;
    uint32_t _unused4 : 3;
    uint32_t MON : 5;
    uint32_t YEAR : 8;
} Time_Date_Register_t;

bool ArtronShop_PCF85363A::setTime(struct tm t) {
    Time_Date_Register_t reg_data;
    memset(&reg_data, 0, sizeof(reg_data));
    reg_data.SEC = bin2bcd(t.tm_sec) & 0x7F;
    reg_data.MIN = bin2bcd(t.tm_min) & 0x7F;
    reg_data.HOUR = bin2bcd(t.tm_hour) & 0x3F;
    reg_data.DAY = bin2bcd(t.tm_mday - 1) & 0x3F;
    reg_data.WEEKDAY = bin2bcd(t.tm_wday) & 0x03;
    reg_data.MON = bin2bcd(t.tm_mon) & 0x1F;
    reg_data.YEAR = bin2bcd((t.tm_year + 1900) % 100);

    CHECK_OK(this->stop(true));
    CHECK_OK(this->write_reg(0x01, (uint8_t *) &reg_data, sizeof(reg_data)));
    CHECK_OK(this->stop(false));

    return true;
}

bool ArtronShop_PCF85363A::getTime(struct tm *t) {
    Time_Date_Register_t reg_data;
    CHECK_OK(this->read_reg(0x01, (uint8_t *) &reg_data, sizeof(reg_data)));

    t->tm_sec = bcd2bin(reg_data.SEC);
    t->tm_min = bcd2bin(reg_data.MIN);
    t->tm_hour = bcd2bin(reg_data.HOUR);
    t->tm_mday = bcd2bin(reg_data.DAY) + 1;
    t->tm_wday = bcd2bin(reg_data.WEEKDAY);
    t->tm_mon = bcd2bin(reg_data.MON);
    t->tm_year = bcd2bin(reg_data.YEAR) + 2000 - 1900;

    return true;
}

bool ArtronShop_PCF85363A::writeRAM(uint8_t address, uint8_t value) {
    return this->writeRAM(address, &value, 1);
}

size_t ArtronShop_PCF85363A::writeRAM(uint8_t address, uint8_t *value, size_t len) {
    if (address > 63) { // Oversize of 64-bytes RAM
        return 0;
    }

    if ((address + len) > 63) { // Data size over RAM size
        len = 63 - address;
    }

    if (!this->write_reg(0x40 + address, value, len)) {
        return 0;
    }

    return len;
}

bool ArtronShop_PCF85363A::readRAM(uint8_t address, uint8_t *value, size_t len) {
    if (address > 63) { // Oversize of 64-bytes RAM
        return false;
    }

    if ((address + len) > 63) { // Data size over RAM size
        len = 63 - address;
    }

    return this->read_reg(0x40 + address, value, len);
}

uint8_t ArtronShop_PCF85363A::readRAM(uint8_t address) {
    uint8_t value = 0xFF;
    this->readRAM(address, &value, 1);
    return value;
}
