#pragma once

#include "motor.h"

#include <netdb.h>

namespace obot {

  constexpr static uint16_t kSoF = 0xCAFE;
  constexpr static uint8_t kFrameId = 4;
  constexpr static uint8_t kHeaderLength = 4;
  constexpr static uint8_t kMaxPayloadLength = 255;
  constexpr static uint8_t kCrcLength = 2;

class UDPFile : public TextFile {
 public:
    struct ObotPacket {
        ObotPacket() : sof{kSoF}, frame_id{kFrameId} {}
        uint16_t sof;
        uint8_t frame_id;
        uint8_t payload_length;
        uint8_t data[kMaxPayloadLength + kCrcLength];
    };
    UDPFile(std::string address) {
        int n = address.find(":");
        if (n == std::string::npos) {
            ip_ = address;
            port_ = "7770";
        } else {
            ip_ = address.substr(0, n);
            port_ = address.substr(n+1,-1);
            if (!port_.size()) {
                port_ = "7770";
            }
        }
        open();
    }
    void open();
    int poll();
    virtual void flush();
    virtual ssize_t read(char * /* data */, unsigned int /* length */);
    virtual ssize_t write(const char * /* data */, unsigned int /* length */);
    virtual ssize_t writeread(const char * /* *data_out */, unsigned int /* length_out */, char * /* data_in */, unsigned int /* length_in */);
    int fd_;
    int timeout_ms_ = 10;
    std::string port_;
    std::string ip_;
    std::string addrstr_;
 private:
    sockaddr_in addr_ = {};
};

class MotorIP : public Motor {
 public:
    MotorIP(std::string address) : realtime_communication_(address) {
        motor_txt_ = std::move(std::unique_ptr<UDPFile>(new UDPFile(address)));
        fd_ = realtime_communication_.fd_;
        connect();
    }
    
    virtual void set_timeout_ms(int timeout_ms) override;
    void connect();

    std::string ip() const { return realtime_communication_.ip_; }
    uint32_t port() const { return std::atol(realtime_communication_.port_.c_str()); }
    virtual ssize_t read() override;
    virtual ssize_t write() override;

 private:
    UDPFile realtime_communication_;
};

}; // namespace obot