#include <Arduino.h>
#include <SimulinkSerial.h>

#define BAUDRATE    115200  // baudrate definido no simulink
#define LEN_RECEIVE 4       // quantidade de elementos recebidos
#define LEN_SEND    4       // quantidade de elementos enviados
#define HEADER      'A'     // header da mensagem
#define FOOTER      '\n'    // footer da mensagem

SimulinkSerial<double> simSerial(BAUDRATE, LEN_RECEIVE, LEN_SEND, HEADER, FOOTER);

void setup(){
    // inicia a comunicação serial
    simSerial.init();

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

        use [index] para acessar os dados do pacote recebido
        e   (index, data) para adicionar dados no pacote enviado

        exemplo: 
        
        // recebendo um valor no index 0
        float data0 = simSerial[0];

        // enviando pi como valor no index 0
        simSerial(0, 3.14159265359);


        voce tambem pode usar um check sum para verificar
        a integridade dos dados recebidos, ex:

        if((simSerial[0] + simSerial[1] + simSerial[2]) != simSerial[3]) return;
    */

    // define os dados a serem enviados
    simSerial(0, simSerial[0]);
    simSerial(1, simSerial[1]);
    simSerial(2, simSerial[2]);
    simSerial(3, simSerial[3]);

    // envia os dados
    simSerial.send_package();
}