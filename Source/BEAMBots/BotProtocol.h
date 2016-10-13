#pragma once

#include <algorithm>
#include <stdint.h>

/// Writes big endian data types into a growing array buffer
class BotProtocol {
    FArrayWriter writer_;
public:
    BotProtocol() = default;
    BotProtocol(int estimated_size) : writer_() {
        writer_.Reserve(estimated_size);
    }
    void write_u8(uint8_t v) {
        writer_.Serialize(&v, sizeof(v));
    }
    void write_u64(uint64_t v) {
        if (FGenericPlatformProperties::IsLittleEndian()) {
            v = BYTESWAP_ORDER64(v);
        }
        writer_.Serialize(&v, sizeof(v));
    }
    void write_u32(uint32_t v) {
        if (FGenericPlatformProperties::IsLittleEndian()) {
            v = BYTESWAP_ORDER32(v);
        }
        writer_.Serialize(&v, sizeof(v));
    }
    const void *data() const {
        return static_cast<const void *>(writer_.GetData());
    }
    void write_string(const FString &s);
    void write_bytes(const void *src, uint64_t size) {
        writer_.Serialize(const_cast<void *>(src), size);
    }
    int64_t size() {
        return writer_.TotalSize();
    }
    void write_f32(float value) {
        uint32_t tmp;
        *((float *)&tmp) = value;
        write_u32(tmp);
    }
};

class BotProtocolReader {
    TArray<uint8_t> data_;
    uint64_t offset_ = 0;
public:
    BotProtocolReader(const uint8_t *data, uint64_t size): data_() {
        data_.SetNumZeroed(size);
        std::copy(data, data+size, data_.GetData());
    }
    uint8_t read_u8() {
        return data_[offset_++];
    }
    uint64_t read_u64() {
        uint64_t result;
        if (FGenericPlatformProperties::IsLittleEndian()) {
            result = ((uint64_t)data_[offset_] << 56) |
            ((uint64_t)data_[offset_+1] << 48) |
            ((uint64_t)data_[offset_+2] << 40) |
            ((uint64_t)data_[offset_+3] << 32) |
            ((uint64_t)data_[offset_+4] << 24) |
            ((uint64_t)data_[offset_+5] << 16) |
            ((uint64_t)data_[offset_+6] << 8) |
            data_[offset_+7];
        } else {
            result = *((uint64_t *)(data_.GetData() + offset_));
        }
        offset_ += sizeof(uint64_t);
        return result;
    }
    uint32_t read_u32() {
        uint32_t result;
        if (FGenericPlatformProperties::IsLittleEndian()) {
            result = ((uint32_t)data_[offset_  ] << 24) |
            ((uint32_t)data_[offset_+1] << 16) |
            ((uint32_t)data_[offset_+2] << 8) |
            data_[offset_+3];
        } else {
            result = *((uint32_t *)(data_.GetData() + offset_));
        }
        offset_ += sizeof(uint32_t);
        return result;
    }
    float read_f32() {
        float result;
        *((uint32_t *)&result) = read_u32();
        return result;
    }
    int64_t size() {
        return data_.Num();
    }
    FString read_string();
};
