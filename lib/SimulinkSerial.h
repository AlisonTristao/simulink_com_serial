#ifndef SIMULINKSERIAL
#define SIMULINKSERIAL
#include <Arduino.h>

// Autor: Alison de Oliveira Tristao
// Email: AlisonTristao@hotmail.com

// converte byte para o valor
template <class TYPE>
union typeUnion_t {
    TYPE value;                 // valor recebido
    byte bytes[sizeof(TYPE)];   // valor em bytes
};

template <class TYPE>
class SimulinkSerial {
private:
    const uint32_t BaudRate;
    const uint8_t len_receive, len_send, len_bytes, sampleTime;
    const char header, footer;
    uint32_t currentTime = 0;
    String receive;
    typeUnion_t<TYPE>* data_send; 
    typeUnion_t<TYPE>* data_receive; 
public:
    SimulinkSerial(const uint32_t BaudRate,    // BaudRate da comunicação serial
                   const uint8_t len_receive,  // quantidade de elementos recebidos
                   const uint8_t len_send,     // quantidade de elementos enviados
                   const char header,          // header da mensagem enviada e recebida
                   const char footer,          // footer da mensagem enviada e recebida
                   const uint8_t sampleTime);  // tempo de amostragem em millis 

    // destructor
    virtual ~SimulinkSerial(){
        delete[] data_receive;
        delete[] data_send;
    };

    // recebe o array de dados
    bool receive_package();
    TYPE received(uint8_t index);
    TYPE operator[](uint8_t index){ 
        return received(index); 
    }

    // envia um array de dados
    void to_send(uint8_t index, TYPE data);
    void send_package();
    void operator()(uint8_t index, TYPE data) { 
        to_send(index, data); 
    }

    // espera o tempo de amostragem
    void wait_sample();
};

template <class TYPE>
SimulinkSerial<TYPE>::SimulinkSerial(
                const uint32_t BaudRate,
                const uint8_t len_receive, 
                const uint8_t len_send, 
                const char header, 
                const char footer, 
                const uint8_t sampleTime):  
                BaudRate(BaudRate), 
                len_receive(len_receive), 
                len_send(len_send),
                len_bytes(sizeof(TYPE)), 
                header(header), 
                footer(footer), 
                sampleTime(sampleTime)
{
    // inicia a comunicação serial
    Serial.begin(BaudRate);
    // define o tamanho do array recebido 
    data_receive    = new typeUnion_t<TYPE>[len_receive];
    data_send       = new typeUnion_t<TYPE>[len_send];
}

template <class TYPE>
bool SimulinkSerial<TYPE>::receive_package(){
    // recebe a mensagem inteira
    receive = Serial.readStringUntil(footer);
    // verifica a integridade da mensagem
    if (receive[0] == header && receive.length() == len_receive*len_bytes + 1) {
        // extrai os dados recebidos
        for (uint8_t i = 0; i < len_receive; i++)       // percorre os dados recebidos
            for (uint8_t j = 0; j < len_bytes; j++)  // percorre os bytes do dado
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
    // envia o header
    Serial.write('A');
    // envia os dados
    for (uint8_t i = 0; i < len_send; i++)
        for (uint8_t j = 0; j < len_bytes; j++)
            Serial.write(data_send[i].bytes[j]);
    // envia o footer
    Serial.write(footer);
}

template <class TYPE>
void SimulinkSerial<TYPE>::wait_sample(){
    // espera o tempo de amostragem
    while((millis() - currentTime) < sampleTime);
    currentTime = millis();
}

#endif
