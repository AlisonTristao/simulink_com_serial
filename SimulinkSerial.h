#ifndef SIMULINKSERIAL
#define SIMULINKSERIAL
#include <Arduino.h>

// Author: Alison de Oliveira Tristao
// Email: AlisonTristao@hotmail.com

// converte byte para o valor
template <class TYPE>
union typeUnion_t {
    TYPE value;                 // received value
    byte bytes[sizeof(TYPE)];   // received bytes
};

template <class TYPE>
class SimulinkSerial {
private:
    const uint32_t BaudRate;
    const uint8_t len_receive, len_send, len_bytes;
    const char header, footer;
    typeUnion_t<TYPE>* data_send; 
    typeUnion_t<TYPE>* data_receive; 
public:
    SimulinkSerial(const uint32_t BaudRate,    // BaudRate of the serial communication
                   const uint8_t len_receive,  // number of elements received
                   const uint8_t len_send,     // number of elements sent
                   const char header,          // header of the message sent and received
                   const char footer);         // footer of the message sent and received

    void init(){
        // begin the serial communication
        Serial.begin(BaudRate);

        // init the arrays
        data_send = new typeUnion_t<TYPE>[len_send];
        data_receive = new typeUnion_t<TYPE>[len_receive];
        for(uint8_t i = 0; i < len_send; i++) data_send[i].value = 0;
        for(uint8_t i = 0; i < len_receive; i++) data_receive[i].value = 0;
    }

    // destructor
    virtual ~SimulinkSerial(){
        delete[] data_receive;
        delete[] data_send;
    };

    // receive a package of bytes
    bool receive_package();
    TYPE received(uint8_t index);               // return received bytes converted to value
    TYPE operator[](uint8_t index){ 
        return received(index); 
    }

    // send a package of bytes
    void send_package();
    void to_send(uint8_t index, TYPE data);     // set the value to send converted to bytes
    void operator()(uint8_t index, TYPE data) { 
        to_send(index, data); 
    }
};

template <class TYPE>
SimulinkSerial<TYPE>::SimulinkSerial(
                const uint32_t BaudRate,
                const uint8_t len_receive, 
                const uint8_t len_send, 
                const char header, 
                const char footer):  
                BaudRate(BaudRate), 
                len_receive(len_receive), 
                len_send(len_send),
                len_bytes(sizeof(TYPE)), 
                header(header), 
                footer(footer) 
{
    // memory allocation for the data
    data_receive    = new typeUnion_t<TYPE>[len_receive];
    data_send       = new typeUnion_t<TYPE>[len_send];
}

template <class TYPE>
bool SimulinkSerial<TYPE>::receive_package(){
    // receive the message until the footer
    String receive = Serial.readStringUntil(footer);
    // verify the header and the length of the message
    if (receive[0] == header && receive.length() == len_receive*len_bytes + 1) {
        // convert the bytes to the value
        for (uint8_t i = 0; i < len_receive; i++)       // walk through the data
            for (uint8_t j = 0; j < len_bytes; j++)     // walk through the bytes
                data_receive[i].bytes[j] = (byte) receive[i*len_bytes + j + 1];
        return false;
    }
    return true;
}

template <class TYPE>
TYPE SimulinkSerial<TYPE>::received(uint8_t index){
    if(index < 0 || index >= len_receive) return static_cast<TYPE>(0);
    return data_receive[index].value;
}

template <class TYPE>
void SimulinkSerial<TYPE>::to_send(uint8_t index, TYPE data){
    if(index < 0 || index >= len_send) return;
    data_send[index].value = data;
}

template <class TYPE>
void SimulinkSerial<TYPE>::send_package(){
    // send the header
    Serial.write('A');
    // send the data converted to bytes
    for (uint8_t i = 0; i < len_send; i++)
        for (uint8_t j = 0; j < len_bytes; j++)
            Serial.write(data_send[i].bytes[j]);
    // send the footer
    Serial.write(footer);
}

#endif
