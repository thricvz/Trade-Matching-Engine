#pragma once
#include "RequestClassByteCodes.hpp"
#include <cstdint>
#include <vector>


using std::vector;


class request{

    public:
        request()=default;
        request(RequestCommand command,vector<const char*> textArgs,vector<int32_t> numericArgs);
        ~request();
        
        request(const request&) = delete;
        request& operator=(const request&) =delete;


        vector<uint8_t> serialize();
        void deserialize(vector<uint8_t>);
        bool operator==(request const &lhs) const;

        vector<const char*> getTextData() const;
        vector<int32_t> getNumericData() const;
        RequestCommand getCommand() const;

    private:
        RequestCommand command{};
        vector<int32_t> numericData{ };
        vector<const char*> textData{ };

        
        void serialize_args(vector<uint8_t> &stream,std::vector<const char*> args_);
        void serialize_args(vector<uint8_t> &stream,std::vector<int32_t> args_);

        
        void deserializeIntegerList(vector<uint8_t> &Data,int listSize,int &listIndex);
        void deserializeStringList(vector<uint8_t> &Data,int listSize,int &listIndex);


};
