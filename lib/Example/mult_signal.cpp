#include <Arduino.h>
#include <SimulinkSerial.h>

#define BAUDRATE    115200  // baudrate definido no simulink
#define LEN_RECEIVE 4       // quantidade de elementos recebidos
#define LEN_SEND    4       // quantidade de elementos enviados
#define HEADER      'A'     // header da mensagem
#define FOOTER      '\n'    // footer da mensagem
#define SAMPLETIME  20      // tempo de amostragem em millis

SimulinkSerial<double> simSerial(BAUDRATE, LEN_RECEIVE, LEN_SEND, HEADER, FOOTER, SAMPLETIME);

void setup(){
    // nao precisa fazer nada

    // CUIDADO!
    // nao use Serial.begin() com outro baudrate aqui
}

void loop(){
    // recebe os dados
    bool erro = simSerial.receive_package();

    // caso tenha algum problema na comunicação erro = true
    // if(erro) return;

    /*
        seu codigo manipulando os dados recebidos vai aqui

        use [index] ou .received(index) para acessar os dados do pacote recebido
        e   (index, data) ou .to_send(index, data) para adicionar dados no pacote enviado
    
        check sum ex:
        if((simSerial[0] + simSerial[1] + simSerial[2]) != simSerial[3]) return;
    */

    // define os dados a serem enviados
    simSerial(0, simSerial[0]);
    simSerial(1, simSerial.received(1));
    simSerial.to_send(2, simSerial[2]);
    simSerial.to_send(3, simSerial.received(3));

    // envia os dados
    simSerial.send_package();

    // espera o tempo de amostragem
    simSerial.wait_sample();
}