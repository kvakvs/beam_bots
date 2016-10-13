#include "BEAMBots.h"
#include "BotProtocol.h"

void BotProtocol::write_string(const FString &s) {
    FTCHARToUTF8 enc(*s);
    auto dst_length = enc.Length();
    write_u32(dst_length);
    write_bytes(enc.Get(), dst_length);
}

FString BotProtocolReader::read_string() {
    uint64_t length = read_u32();
    length = std::min<uint64_t>(length, size() - offset_);
    
    const char *in = (const char *)(data_.GetData() + offset_);
    TUniquePtr<TCHAR> out(new TCHAR[1 + length]);
    
    TStringConvert<char, TCHAR>::Convert(out.Get(), length, in, length);
    offset_ += sizeof(uint32_t) + length;
    
    return FString(length, out.Get());
}
