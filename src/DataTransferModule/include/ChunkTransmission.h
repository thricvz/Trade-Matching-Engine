    #pragma once
    #include <sys/socket.h>
    #include <vector>
    #include <cstdint>

    using std::vector;
    using std::uint8_t;

    const int CHUNK_SIZE = 128;
    const int BUFFER_SIZE = CHUNK_SIZE;

    void sendChunk(int senderFD,vector<uint8_t> byteStream);
    void receiveChunks(int senderFD,vector<uint8_t> &byteStream);