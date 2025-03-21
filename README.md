# simulink_com_serial
 - Serial communication library with Esp32/Arduino and matlab simulink

ATENÇÃO: Ao utilizar arduino, double e float possuem 4 bytes, utilize o formato single (alterando o bloco de conversão no simulink) para converter double de 8 bytes para 4.

DICA: Para tirar os ruidos, você pode implementar um checksum em sua mensagem, verificando a integridade dos números recebidos.

Se utilizou a biblioteca e gostou, peço que me de uma estrelinha por favorzin. Se tiver dicas de mudanças ou problemas, pode entrar em contato. Muito Obrigado :)

## Configurando a ESP (código disponível na pasta "Example")

```cpp
#include <SimulinkSerial.h>

#define BAUDRATE    115200  // baudrate definido no simulink
#define LEN_RECEIVE 4       // quantidade de elementos recebidos
#define LEN_SEND    4       // quantidade de elementos enviados
#define HEADER      'A'     // header da mensagem
#define FOOTER      '\n'    // footer da mensagem

SimulinkSerial<float> simSerial(BAUDRATE, LEN_RECEIVE, LEN_SEND, HEADER, FOOTER);

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
```

### Defina as variáveis que deseja usar (você precisará defini-las no arquivo do Simulink depois):

```cpp
#define BAUDRATE    115200  // baudrate definido no Simulink
#define LEN_RECEIVE 4       // quantidade de elementos recebidos
#define LEN_SEND    4       // quantidade de elementos enviados
#define HEADER      'A'     // header da mensagem
#define FOOTER      '\n'    // footer da mensagem
```

- BAUDRATE é a velocidade da comunicação serial desejada.
- LEN_RECEIVE e LEN_SEND são a quantidade de dados que deseja enviar e receber na comunicação.
- HEADER e FOOTER são usados para identificar o começo e o final dos bytes enviados.

### Crie o objeto serial que vai usar no seu código, definindo o tipo de dado que deseja enviar e receber:

```cpp
SimulinkSerial<float> simSerial(BAUDRATE, LEN_RECEIVE, LEN_SEND, HEADER, FOOTER);
```
No exemplo, estamos usando float, mas você pode definir com int8_t, uint8_t, int16_t, etc.
Mas cuidado, não esqueca de mudar no bloco *"Serial Send"*. 

### no serial setup use a função init para iniciar o serial com o baudrate definido:

```cpp
simSerial.init();
```
Mas cuidado, não defina novamente o serial com um baudrate diferente.

### Para receber o pacote de bytes, você usa:
```cpp
bool erro = simSerial.receive_package();
```
A função retorna um valor booleano indicando se recebeu os dados corretamente utilizando o header e o footer.

### Para adicionar dados ao pacote que deseja enviar, você pode utilizar:

[index] para acessar os dados do pacote recebido.
(index, data) para adicionar dados no pacote enviado.

```cpp
simSerial(0, simSerial[0]);
simSerial(1, simSerial[1]);
simSerial(2, simSerial[2]);
simSerial(3, simSerial[3]);
```

### E então, para enviar o pacote por serial, use:

```cpp
simSerial.send_package();
```

## Configurando a comunicação no Simulink (arquivo disponível na pasta "Example")

![Diagrama](imagens/config_diagram.png)
1. Configure os blocos "*Serial Configuration*", "*Serial Send*" e "*Serial Receive*" com as configurações definidas no código.
2. Use "*Vector Concatenate*" para concatenar todos os dados que deseja enviar.
3. "*Zero-Order Hold*" interpola os dados recebido com a taxa de amostragem que deseja (usei 10 millis no exemplo).
4. Converta seu vector de dados para o tipo definido no código (single = float).
5. Envie os dados utilizando "*Serial Send*".
6. Leia os bytes recebidos utilizando "*Serial Receive*".
7. Converta os valores recebidos para double.
8. Separe os bytes recebidos usando "*Demux*".

## Configuração dos Blocos

### *Serial Configuration*
![Config](imagens/config_serial.png)
- Defina a porta serial da sua esp32 conectada

### *Zero-Order Hold*
![Sample](imagens/sample.png)
- Crie uma variavel para salvar a taxa de amostragem definida

### *Serial Send*
![Config](imagens/com_send.png)
- O *Header* 'A' definido no codigo é 65 em ASCII e o *Footer* '\n' é a quebra de linha.

### *Serial Receive*
![Config](imagens/com_receive.png)
- Defina o tipo do dado recebido, o *header* 'A', o *footer* '\n' e em *data size* a quatidade de dados recebidos em uma matriz coluna.
- Defina o tempo de *block* como a metade do tempo de amostragem.

# Utilizando o código e o arquivo de exemplo, ao abrir o scope, você terá esse plot:
![Config](imagens/plot_ex.png)
