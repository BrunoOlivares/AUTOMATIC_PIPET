// Inclusão das Bibliotecas e Arquivos Auxiliares Necessários

    #include "mbed.h"
    #include "Arduino.h"
    #include <MCUFRIEND_kbv.h>
    #include <TouchScreen_kbv_mbed.h>
    #include <cstdio> 
    #include "pipe.h"
    #define velo 0.0015

// Definição dos pino I/O e suas definições

    // Declaração Da Serial
    
        Serial pc(USBTX, USBRX); 

    // Botão De Emergência

        InterruptIn botao1(PB_13, PullUp); 

    // Joystick

        AnalogIn xAxis(PC_5); // Sentido X
        AnalogIn yAxis(PC_4); // Sentido Y

    // Motores de Passo X, Y e Z

        DigitalOut Enable_x(PB_2); //Ativação do Motor X
        DigitalOut Enable(PB_1); //Ativação do Motor Y
        DigitalOut Enable_z(PA_13); //Ativação do Motor Z

        DigitalOut Sentido_x(PB_12); // Sentido de Rotação do Motor X
        DigitalOut Sentido(PB_15); // Sentido de Rotação do Motor Y
        DigitalOut Sentido_z(PC_12); // Sentido de Rotação do Motor Z

        DigitalOut Passos(PB_14); // Clock Compartilhado dos Motores

    // Rele da Pipeta

        DigitalOut rele1(PC_2);

    // Estado de Emergência

        InterruptIn estadoemergencia(PC_8);

    // Sensores FDC

        DigitalIn FDC_A_X(PD_2); // Porta Comum FDC X
        DigitalIn FDC_A_X_MENOS(PC_9); // Porta NA FCD X
        DigitalIn FDC_A_Y(PB_7); // Porta Comum FDC Y
        DigitalIn FDC_A_Y_MENOS(PC_10); // Porta NA FCD Y
        DigitalIn FDC_A_Z(PC_3); // Porta Comum FDC Y
        DigitalIn FDC_A_Z_MENOS(PC_11); // Porta NA FCD Z

    // Definição da Função Touch do TFT LCD Shield

        TSPoint_kbv touch;

        const PinName X = D8, Y = A3, XM = A2, YM = D9;   
        TouchScreen_kbv ts = TouchScreen_kbv(X, Y, XM, YM, 300);

    // Definição da Função de Edição da Tela do TFT LCD Shield

        MCUFRIEND_kbv tft;

    // Pré Definições das Telas Utilizadas do LCD Shield

        uint8_t Orientation =1;
        int larg=tft.width();
        int altura=tft.height();

    // Definição do Tempo de Debounce

        Timer debounce; 

    // Definição de Cores

        #define WHITE 0xFFFF
        #define BLACK 0x0000
        #define BLUE 0x001F
        #define GRAY 0x8410
        #define CYAN 0x07FF
        #define MAGENTA 0xF81F
        #define RED 0xF800
        #define GREEN 0x07E0

    // Variáveis Correspondentes a Exibição de Cada Tela

        int tela0 = 0;
        int tela1 = 1; 
        int tela2 = 0; 
        int tela3 = 0; 
        int tela4 = 0; 
        int tela5 = 0; 
        int tela6 = 0;

    // Variáveis que controlam a troca única entre telas

        int fixo_0 = 1;
        int fixo_1 = 1; 
        int fixo_2 = 1; 
        int fixo_3 = 1; 
        int fixo_4 = 1; 
        int fixo_5 = 1; 
        int fixo_pipeta = 0;

    // Realiza a verificação dos botões selecionados em tela

        int lado_tela0 = 0;
        int lado_tela1 = 0; 
        int lado_tela2 = 0; 
        int lado_tela3 = 0; 
        int lado_tela5 = 0; 
        int lado_tela6 = 0;

    // Listas e Variáveis que guardam e sinalizam as posições e volumes utilizados no processo de pipetagem

        int lista_x[10] = {0,0,0,0,0,0,0,0,0,0}; 
        int lista_y[10] = {0,0,0,0,0,0,0,0,0,0}; 
        int lista_z[10]; 
        int lista_vol[10] = {0,0,0,0,0,0,0,0,0,0}; 
        int index = 0; 
        int estado = 0;

    // Variáveis de Controle para quando um botão é pressionado

        int botao_teste = 0; 
        int botao_teste_tela0 = 0; 
        int botao_teste_tela2 = 0; 
        int botao_teste_tela3 = 0; 
        int botao_teste_tela4 = 0; 
        int botao_teste_tela5 = 0;
        int botao_teste_tela6 = 0;

    // Definição da Sensibilidade/Tempo de Espera

        int sensibilidade_x = 1000; 

    // Controle do Botão de Emergência

        bool estadoBE = 0; 

    // Definição dos Limites Inferior e Superior de Volumes Escolhíveis

        int volume_inicial = 0; 
        int volume_final = 10;

    // Variáveis de Leitura do Joystick

        int x,y;

    // Estado de Emergência 

        int fixo_emergencia = 1;

    // Contador de Passos da Etapa Atual de X, Y e Z
        
        int contador_x = 0;    
        int contador_y = 0; 
        int contador_z = 0;

    // Liberação do Controle Manual XY e Z

        int libera = 0; 
        int libera_2 = 0;

    // Controle do Volume da Posição de Pipetagem e da Quantidade de Volume Depositada

        int variavel_pipeta = 0; 
        int index_pipeta = 1; 

    // Controle da Pipeta

        int controle_da_pipeta = 1;

    // Controle dos FDC de todos os eixos

        bool limite_x = 0;
        bool limite_y = 0;
        bool limite_z= 0;

    // Variável de Controle Para Mudanças Estéticas de Botões Selecionados

        int teste_y = 1; 
        int teste_y_2 = 1; 
        int teste_x_2 = 1; 
        int teste_x_0 = 1; 
        int teste_x_5 = 1;

    // Volume Predefinido de Volume Permitido de Pipetagem por vez

        int vol = 1; 

    // Controle da realização de referenciamento

        bool ref=0;

    // Valor Inicial de Y

        int valor_inicial_y;

// TELAS BÁSICAS 

    // Mapa De coordenadas 

        void mapa_p3(){

            // Inicialização da Tela

                tft.reset();
                tft.begin();
                tft.setRotation(Orientation);

            // Leitura de Pressão

                int pressao = 0;

            // Coordenadas X e Y analógicas e pós interpolação

                int x_m = 0;
                int y_m = 0;
                int x_d = 0;
                int y_d = 0;
            
            // Coordenadas de Referência X e Y

                int coord_x_m = (contador_x / 16000) * 200 + 20;
                int coord_y_m = (contador_y / 16000) * 200 + 20;

            // Quantidade de passos a serem dados em X e Y pelo Mapa
                
                int p_total_x = 0;
                int p_total_y = 0;

            // Contador de passos dados em X e Y

                int contador_xm = 0;
                int contador_ym = 0;   

            // Caminho digital a ser trilhado no mapa

                int caminho_xm = 0;
                int caminho_ym = 0;

            // Comunicação com a quantidade de passos dados pelo mapa e controle manual em conjunto

                int q_passos_x;
                int q_passos_y;

            // Controle de Finalização do Movimento no eixo X ou Y
                
                int eixo_x;
                int eixo_y;

            // Criação do Grid da Tela

                tft.setRotation(Orientation);
                tft.fillScreen(GRAY);

                tft.fillRoundRect(2,2, altura-4,larg-4,0, WHITE);
                tft.fillRoundRect(6,6,altura-12,larg-12,0, GRAY);
                tft.drawRect(20, 20, 200, 200, WHITE);

                for (int f = 1; f < 11; f++){

                    for (int i = 1; i < 11; i++) {

                        tft.drawRect(20 * i, 20 * f, 20, 20, WHITE);

                    }

                } 

            // Criação do Botão de Volta

                tft.fillRoundRect(270, 170, 30, 30, 0, MAGENTA);

            // Criação de Loop de funcionamento do Mapa

                while(botao_teste_tela4 == 0 && estadoBE == 0){
                    
                    // Leitura da Pressão

                        pressao = ts.pressure(); 

                    // Filtro de Pressão
                    
                        if (pressao > 200) { 

                            // Leitura do Ponto Clicado e Conversão por Interpolação para ponto inserido na tela

                                contador_x = 0;

                                contador_y = 0;

                                touch = ts.getPoint();

                                x = touch.y;

                                y = touch.x;

                                x_d = int(-41.666 + 0.4166666 * x);

                                y_d = int(240 - (-25.961538465 + 0.2884615385 * y));
                        }

                    // Seleção de Ponto dentro do Grid

                        if (x_d >= 20 & y_d >= 20 & x_d <= 220 & y_d <= 220) {

                            // Geração de Ponto Clicado Visível na Tela
                            
                                tft.fillRect(x_d, y_d, 10, 10, MAGENTA);

                            // Geração do caminho digital a ser percorrido pela pipeta e setup das variáveis de controle da movimentação

                                caminho_xm = x_d - coord_x_m;

                                caminho_ym = y_d - coord_y_m;

                                q_passos_x = lista_x[index];
                                q_passos_y = lista_y[index];

                                Enable = 1;
                                Enable_x = 1;

                                eixo_x = 1;
                                eixo_y = 1;

                            // Movimentação no Eixo X e Y para a Esquerda

                                if (caminho_xm < 0 & caminho_ym < 0) {

                                    // Cálculo de Passos a serem dados em X e Y

                                        p_total_x = int((abs(caminho_xm) * 80)); 
                                        p_total_y = int((abs(caminho_ym) * 80)); 

                                    // Loop para movimentação da pipeta

                                        while(contador_x <= p_total_x || contador_y <= p_total_y && estadoBE==0){

                                            // Eixo X

                                                if(contador_x <= p_total_x) {

                                                    // Ativação e Seleção de Sentido do Motor
                                                    
                                                        Passos = !Passos;
                                                        Enable_x = 0;
                                                        Sentido_x = 0;

                                                    // Controle de Passos Local e Global

                                                        contador_x++;
                                                        q_passos_x--;    

                                                } else {

                                                    // Desativação do Eixo X

                                                        Enable_x = 1;
                                                        eixo_x = 0;

                                                }

                                            // Eixo Y

                                                if(contador_y <= p_total_y) {

                                                    // Ativação e Seleção de Sentido do Motor

                                                        Passos = !Passos;
                                                        Enable = 0;
                                                        Sentido = 0;

                                                    // Controle de Passos Global e Local

                                                        contador_y++;
                                                        q_passos_y--;

                                                    
                                                } else {

                                                    // Desativação do Eixo Y

                                                        Enable = 1;
                                                        eixo_y = 0;

                                                }

                                            // Tempo de Espera até o próximo passo

                                                wait_us((sensibilidade_x * (eixo_x + eixo_y)) + 1);

                                            }


                                    // Estabelecimento de novo ponto de referência 

                                        coord_x_m = x_d;
                                        coord_y_m = y_d;

                                }

                            // Movimentação de X e Y para a Direita

                                if (caminho_xm > 0 & caminho_ym > 0) {

                                    // Cálculo de Passos a serem dados em X e Y

                                        p_total_x = int((caminho_xm) * 80); 
                                        p_total_y = int((caminho_ym) * 80);

                                    // Loop para movimentação da pipeta

                                        while(contador_x <= p_total_x || contador_y <= p_total_y ){

                                            // Eixo X

                                                if(contador_x <= p_total_x) {

                                                    // Ativação e Seleção de Sentido do Motor

                                                        Passos = !Passos;
                                                        Enable_x = 0;
                                                        Sentido_x = 1;

                                                    // Controle de Passos Local e Global

                                                        contador_x++;
                                                        q_passos_x++;    

                                                } else {

                                                    // Desativação do eixo X

                                                        Enable_x = 1;
                                                        eixo_x = 0;

                                                }

                                            // Eixo Y

                                                if(contador_y <= p_total_y) {

                                                    // Ativação e Seleção do Sentido do Motor

                                                        Passos = !Passos;
                                                        Enable = 0;
                                                        Sentido = 1;

                                                    // Controle de Passos Local e Global 

                                                        contador_y++;
                                                        q_passos_y++;
                                                    
                                                } else {

                                                    // Desativação do eixo Y

                                                        Enable = 1;
                                                        eixo_y = 0;

                                                }

                                            // Tempo De Espera até o próximo passo

                                                wait_us((sensibilidade_x * (eixo_x + eixo_y)) + 1);

                                            }

                                    // Estabelecimento de novo ponto de referencia

                                        coord_x_m = x_d;
                                        coord_y_m = y_d;


                                }

                            // Movimentação de X para a Direita e Y para a Esquerda

                                if (caminho_xm > 0 & caminho_ym < 0) {

                                    // Cálculo de Passos a serem dados em X e Y

                                        p_total_x = int((caminho_xm) * 80);
                                        p_total_y = int(abs(caminho_ym) * 80);

                                    // Loop para movimentação da pipeta

                                        while(contador_x <= p_total_x || contador_y <= p_total_y){

                                            // Eixo X

                                                if(contador_x <= p_total_x) {

                                                    // Ativação e seleção do sentido do Motor

                                                        Passos = !Passos;
                                                        Enable_x = 0;
                                                        Sentido_x = 1;

                                                    // Controle de passos Local e global

                                                        contador_x++;
                                                        q_passos_x++;    

                                                } else {

                                                    // Desativação do eixo X

                                                        Enable_x = 1;
                                                        eixo_x = 0;

                                                }

                                            // Eixo Y 

                                                if(contador_y <= p_total_y) {

                                                    // Ativação e seleção do sentido do Motor

                                                        Passos = !Passos;
                                                        Enable = 0;
                                                        Sentido = 0;
                                                    
                                                    // Controle de passos Local e Global

                                                        contador_y++;
                                                        q_passos_y--;
                                                    
                                                } else {

                                                    // Desativação do Eixo Y

                                                        Enable = 1;
                                                        eixo_y = 0;

                                                }

                                            // Tempo de espera até o próximo passo

                                                wait_us((sensibilidade_x * (eixo_x + eixo_y)) + 1);

                                            }

                                    // Estabelecimento de novo ponto de referencia

                                        coord_x_m = x_d;
                                        coord_y_m = y_d;

                                }

                            // Movimentação de X para a Esquerda e Y para a Direita

                                if (caminho_xm < 0 & caminho_ym > 0) {

                                    // Cálculo de passos a serem dados em X e Y
                                    
                                        p_total_x = int(abs(caminho_xm) * 80);
                                        p_total_y = int((caminho_ym) * 80);

                                    // Loop para movimentação da pipeta

                                        while(contador_x <= p_total_x || contador_y <= p_total_y){

                                            // Eixo X

                                                if(contador_x <= p_total_x) {

                                                    // Ativação e seleção do sentido do motor

                                                        Passos = !Passos;
                                                        Enable_x = 0;
                                                        Sentido_x = 0;

                                                    // Controle de passos Local e Global

                                                        contador_x++;
                                                        q_passos_x--;    

                                                } else {

                                                    // Desativação do eixo X

                                                        Enable_x = 1;
                                                        eixo_x = 0;

                                                }

                                            // Eixo Y 

                                                if(contador_y <= p_total_y) {

                                                    // Ativação e seleção do sentido do motor
                                                    
                                                        Passos = !Passos;
                                                        Enable = 0;
                                                        Sentido = 1;

                                                    // Controle de Passos Local e Global 

                                                        contador_y++;
                                                        q_passos_y++;

                                                } else {

                                                    // Desativação do Eixo Y

                                                        Enable = 1;
                                                        eixo_y = 0;

                                                }

                                            // Tempo de espera até o próximo passo

                                                wait_us((sensibilidade_x * (eixo_x + eixo_y)) + 1);

                                            }

                                    // Estabelecimento de novo ponto de referencia

                                        coord_x_m = x_d;
                                        coord_y_m = y_d;

                                }

                        }

                    // Definição Global de Passos Dados a Partir do Ponto de Referência

                        lista_x[index] = q_passos_x;
                        lista_y[index] = q_passos_y; 

                    // Clique no Botão de Volta

                        if (x_d >= 240 & x_d <= 300 & y_d >= 140 & y_d <= 200) {

                            botao_teste_tela4 = 1;
                            tft.reset();

                        }

                }

        
        }


    // Tela Inicial 

        void tela(){

            // Criação da Tela Base 

                tft.fillRoundRect(2, 2, altura-4, larg-4, 0, WHITE);
                tft.fillRoundRect(6, 6, altura-12, larg-12, 0, GRAY);
                tft.fillRoundRect(15, 13, altura-30, 45, 0, WHITE);
                tft.setTextSize(2);
                tft.setTextColor(BLACK);
                tft.setCursor(90, 28); 
                tft.println("PIPETAGEM");
                tft.drawRGBBitmap(220, 16, pipeta, 40, 40);
                tft.fillRoundRect(larg-150, 80, 150, 40, 15, WHITE); 
                tft.fillRoundRect(larg-150, 140, 150, 40, 15, WHITE);
                tft.setCursor(larg-115, 90);
                tft.println("Posicao");
                tft.setCursor(larg-140, 150);
                tft.println("Referenciar");

            // Mudanças na Tela pelo Estado de Referenciamento

                // Referenciamento não Realizado

                    if(ref == 0){

                        tft.setCursor(10, 210);
                        tft.println("Referenciamento");
                        tft.setTextColor(RED);
                        tft.setCursor(200, 210);
                        tft.println("pendente");
                        
                        }

                // Referenciamento Realizado

                    else{

                        tft.setCursor(10, 210);
                        tft.println("Referenciamento");
                        tft.setTextColor(GREEN);
                        tft.setCursor(200, 210);
                        tft.println("concluido");

                    }

        }

    // Movimentação na Tela Inicial 

        void mov_tela1(){

            // Leitura do Joystick no vetor Y

                y = yAxis.read()*1000;

            // Mudança Estética de Botão Selecionado

                // Botão "Posição" Selecionado

                    if(y < 300){

                        if(lado_tela1 != 1){

                            y = yAxis.read()*1000;
                            tft.setTextColor(BLACK);
                            tft.fillRoundRect(larg-150, 80, 150, 40, 15, CYAN); //botao
                            tft.fillRoundRect(larg-150, 140, 150, 40, 15, WHITE);
                            lado_tela1 = 1;
                            tft.setCursor(larg-115, 90);
                            tft.println("Posicao");
                            tft.setCursor(larg-140, 150);
                            tft.println("Referenciar");
                            wait_ms(300);

                        }
                        
                    } 

                // Botão "Referenciar" Selecionado

                    if(y > 800){

                        if(lado_tela1 != 2){

                            tft.setTextColor(BLACK);
                            y = yAxis.read()*1000;
                            tft.fillRoundRect(larg-150, 80, 150, 40, 15, WHITE); 
                            tft.fillRoundRect(larg-150, 140, 150, 40, 15, CYAN);
                            tft.setCursor(larg-115, 90);
                            tft.println("Posicao");
                            tft.setCursor(larg-140, 150);
                            tft.println("Referenciar");
                            wait_ms(300);
                            lado_tela1 = 2;
                        
                        }

                    }

        }

    // Criação da Tela Menu

        void sensi(){

            tft.begin();
            tft.setRotation(Orientation);
            tft.fillScreen(GRAY);
            tft.fillRoundRect(2, 2, altura-4, larg-4, 0, WHITE);
            tft.fillRoundRect(6, 6, altura-12, larg-12, 0, GRAY);
            tft.fillRoundRect(10, 15, 35, 45, 0, WHITE);
            tft.fillRoundRect(60, 15, 45, 45, 0, WHITE);
            tft.fillRoundRect(110, 15, 35, 45, 0, WHITE);
            tft.fillRoundRect(160, 15, 45, 45, 0, WHITE);
            tft.fillRoundRect(210, 15, 35, 45, 0, WHITE);
            tft.fillRoundRect(260, 15, 45, 45, 0, WHITE);
            tft.setTextSize(2);
            tft.setTextColor(BLACK);
            tft.setCursor(22, 30);
            tft.println("X :");
            tft.setCursor(122, 30);
            tft.println("Y :");
            tft.setCursor(222, 30);
            tft.println("Z :");
            tft.fillRoundRect(35, 100, 120, 50, 5, WHITE);
            tft.fillRoundRect(170, 100, 120, 50, 5, WHITE);
            tft.fillRoundRect(103, 165, 120, 50, 5, WHITE);
            tft.fillRoundRect(30, 185, 40, 40, 5, WHITE);
            tft.setCursor(42, 200);
            tft.println("<");
            tft.setCursor(70, 115);
            tft.println("MAPA");
            tft.setCursor(195, 115);
            tft.println("MANUAL");
            tft.setCursor(120, 180);
            tft.println("DEFINIR");
            tft.setCursor(190, 75);
            tft.println(vol);
            tft.setCursor(220, 75);
            tft.println("ml");
            

            }

    // Criação da Tela Controle Manual

        void manual(){

            tft.fillScreen(GRAY);
            tft.fillRoundRect(2 , 2, altura-4, larg-4, 0, WHITE);
            tft.fillRoundRect(6, 6, altura-12, larg-12, 0, GRAY);
            tft.fillRoundRect(15, 13, altura-30, 45, 0, WHITE);
            tft.setTextSize(2);
            tft.setTextColor(BLACK);
            tft.setCursor(70, 28);
            tft.println("CONTROLE MANUAL");
            tft.fillRoundRect(30, 185, 40, 40, 5, CYAN);
            tft.fillRoundRect(100, 100, 60, 60, 5, WHITE);
            tft.fillRoundRect(210, 100, 60, 60, 5, WHITE);
            tft.setCursor(230, 120);
            tft.println("Jz");
            tft.setCursor(125, 120);
            tft.println("J");
            tft.setCursor(42, 200);
            tft.println("<");
            tft.setTextColor(WHITE);
            tft.setCursor(70, 120);
            tft.println("-X");
            tft.setCursor(165, 120);
            tft.println("+X");
            tft.setCursor(120, 80);
            tft.println("-Y");
            tft.setCursor(120, 170);
            tft.println("+Y");
            tft.setCursor(230, 80);
            tft.println("-Z");
            tft.setCursor(230, 170);
            tft.println("+Z");
                
        }

    // Criação da Tela Volume de Deposição

        void volume(){

            tft.fillScreen(GRAY);
            tft.fillRoundRect(2, 2, altura-4, larg-4, 0, WHITE);
            tft.fillRoundRect(6, 6, altura-12, larg-12, 0, GRAY);
            tft.fillRoundRect(15, 13, altura-30, 45, 0, WHITE);
            tft.setTextSize(2);
            tft.setTextColor(BLACK);
            tft.setCursor(130, 28);
            tft.println("VOLUME");
            tft.fillRoundRect(140, 100, 60, 60, 5, CYAN);
            tft.fillRoundRect(30, 185, 40, 40, 5, WHITE);
            tft.fillRoundRect(250, 185, 40, 40, 5, WHITE);
            tft.setCursor(42, 200);
            tft.println("<");
            tft.setCursor(265, 200);
            tft.println(">");

        }

    // Criação da Tela de Posições

        void historico(){

            tft.fillScreen(GRAY);
            tft.fillRoundRect(2, 2, altura-4, larg-4, 0, WHITE);
            tft.fillRoundRect(6, 6, altura-12, larg-12, 0, GRAY);
            tft.fillRoundRect(30, 10, 260, 25, 0, WHITE);
            tft.setTextSize(2);
            tft.setTextColor(BLACK);
            tft.setCursor(55, 14);
            tft.println("X");
            tft.setCursor(110, 14);
            tft.println("Y");
            tft.setCursor(165, 14);
            tft.println("Z");
            tft.setCursor(220, 14);
            tft.println("VOL");
            tft.fillRoundRect(10, 39, 145, 25, 0, CYAN); 
            tft.fillRoundRect(10, 69, 145, 25, 0, WHITE); 
            tft.fillRoundRect(10, 99, 145, 25, 0, WHITE);
            tft.fillRoundRect(10, 129, 145, 25, 0, WHITE); 
            tft.fillRoundRect(10, 159, 145, 25, 0, WHITE);
            tft.fillRoundRect(165, 39, 145, 25, 0, WHITE); 
            tft.fillRoundRect(165, 69, 145, 25, 0, WHITE); 
            tft.fillRoundRect(165, 99, 145, 25, 0, WHITE);
            tft.fillRoundRect(165, 129, 145, 25, 0, WHITE); 
            tft.fillRoundRect(165, 159, 145, 25, 0, WHITE);
            tft.fillRoundRect(30, 190, 40, 40, 5, WHITE);
            tft.fillRoundRect(110, 190, 60, 40, 5, WHITE);
            tft.fillRoundRect(190, 190, 100, 40, 5, WHITE);
            tft.setCursor(200, 200);
            tft.println("pipetar");
            tft.setCursor(120, 200);
            tft.println("nova");
            tft.setCursor(42, 200);
            tft.println("<");
            
        }

    // Criação da Tela de Emergência

        void emergencia(){

            tft.begin();
            tft.setRotation(Orientation);
            tft.fillRoundRect(2, 2, altura-4, larg-4, 0, WHITE);
            tft.fillRoundRect(6, 6, altura-12, larg-12, 0, GRAY);
            tft.fillRoundRect(15, 13, altura-30, 45, 0, RED);
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.setCursor(90, 28);
            tft.println("EMERGENCIA");
            tft.fillRoundRect(100, 70, 120, 130, 0, WHITE);
            tft.drawRGBBitmap(110, 80, kaique, 100, 107);

        }

    // Criação da Tela de Pipetagem

        void tela_pipetagem(){

            // Inicio do Ciclo de Pipetagem e Disposição de Botões

                if(fixo_pipeta == 0 && estadoBE != 1){

                    tft.fillRoundRect(2, 2, altura-4, larg-4, 0, WHITE);
                    tft.fillRoundRect(6, 6, altura-12, larg-12, 0, GRAY);
                    tft.fillRoundRect(15, 13, altura-30, 45, 0, WHITE);
                    tft.setTextSize(2);
                    tft.setTextColor(BLACK);
                    tft.setCursor(50, 28);
                    tft.println("PIPETAGEM INICIADA");
                    tft.fillRoundRect(40, 180, 100, 50, 15, WHITE);
                    tft.fillRoundRect(170, 180, 100, 50, 15, WHITE);
                    tft.setCursor(50, 200);
                    tft.println("Repetir");
                    tft.setCursor(195, 190);
                    tft.println("Novo");
                    tft.setCursor(195, 210);
                    tft.println("Ciclo");
                    
                    fixo_pipeta = 1;

                }

            //  Sinalização de Volume Necessário na Posição Atual em Tempo Real

                if(lista_vol[index_pipeta] > 0 && estadoBE != 1){

                    tft.setCursor(80, 100);
                    tft.println("POSICAO:");
                    tft.fillRoundRect(170, 90, 30, 40, 0, GRAY);
                    tft.setCursor(180, 100);
                    tft.println(index_pipeta);
                    tft.setCursor(80, 150);
                    tft.println("FALTAM:");
                    tft.setCursor(170, 150);
                    tft.fillRoundRect(165, 140, 30, 40, 0, GRAY);
                    tft.println(lista_vol[index_pipeta]-variavel_pipeta);
                    tft.setCursor(190, 150);
                    tft.println("ml");

                }


            // Sinalização de Finalização Completa do Processo

                if(lista_vol[index_pipeta] == 0 && estadoBE != 1){

                    tft.fillRoundRect(15, 13, altura-30, 45, 0, WHITE);
                    tft.setTextSize(2);
                    tft.setTextColor(BLACK);
                    tft.setCursor(50, 28);
                    tft.println("PIPETAGEM FINALIZADA");

                }

        }

// FUNÇÕES BÁSICAS

    // Função da Movimentação Jog XY

        void jogManual(){

            // Variáveis Locais De Passos

                int valor_maximo_y;
                int valor_maximo_x;

            // Leitura da Movimentação do Joystick

                y = yAxis.read()*1000;
                x = xAxis.read()*1000;

            // Movimentação em Y

                // Joystick Y+

                    if(y > 750){

                        y = yAxis.read()*1000;

                        // Leitura do FDC de Y  Direito e Controle do Máximo de Passos Possíveis

                            if(FDC_A_Y_MENOS == 1){

                                valor_maximo_y = contador_y;
                                Enable = 1;
                                
                            }
                        
                        // Ativação do Eixo Y no Sentido Desejado e Contagem de passos

                            else{

                                Enable = 0;
                                Sentido = 1;
                                
                                contador_y += 1;
                            
                            }

                    }

                // Joystick Y- 

                    if(y < 100){

                        y = yAxis.read()*1000;

                        // Leitura do FDC de Y  Direito e Controle do Mínimo de Passos Possíveis

                            if(FDC_A_Y == 1 || contador_y <= valor_inicial_y){

                                valor_inicial_y = 0;
                                contador_y = valor_inicial_y;
                                Enable = 1;
                                
                            }

                        // Ativação do Eixo Y no Sentido Desejado e Contagem de passos

                            else {

                                Enable = 0;
                                Sentido = 0;
                                contador_y -= 1;

                            }

                    }

            // Movimentação em X

                // Movimentação X-

                    if(x < 200){

                        x = xAxis.read()*1000;

                        // Leitura do FDC Esquerdo e Controle do Mínimo de Passos Possíveis

                            if(FDC_A_X==1 || contador_x==0){

                                contador_x = 0;
                                Enable_x = 1;
                            
                            }
                        
                        // Ativação do Eixo X no Sentido Desejado e Contagem de Passos

                            else {

                                Enable_x = 0;
                                Sentido_x = 0;

                                contador_x -= 1;
                            
                            }
                        
                    }
                
                // Movimentação X+
                
                    if(x > 800){

                        x=xAxis.read()*1000;

                        // Leitura do FDC Direito 

                            if(FDC_A_X_MENOS == 1){

                                Enable_x = 1;
                                
                            }

                        // Ativação do Eixo X no Sentido Desejado e Contagem de Passos

                            else{

                                Enable_x = 0;
                                Sentido_x = 1;
                                contador_x += 1;
                            }
                            
                    }
            
            // Mudança de Clock

                Passos=1;
                wait_us(sensibilidade_x);
                Passos=0;
                wait_us(sensibilidade_x);

        }


    // Função da Movimentação Jog Z

        void novo_z(){

            // Leitura da Movimentação do Joystick 

                y = yAxis.read()*1000;

            // Movimentação Z-

                while(y < 100 && FDC_A_Z == 0 && estadoBE == 0){

                    // Ativação do Motor do Eixo Z 

                        y = yAxis.read()*1000;
                        Enable_z = 1;
                        Sentido_z = 1;

                    // Mudança de Clock

                        Passos = 1;
                        wait_us(sensibilidade_x);
                        Passos = 0;
                        wait_us(sensibilidade_x); 

                    // Contagem de Passo em Z

                        contador_z -= 1;
                
                }

            // Movimentação Z+

                while(y > 900 && FDC_A_Z == 0 && estadoBE==0){

                    // Ativação do Motor do eixo Z

                        y = yAxis.read()*1000;
                        Enable_z = 1;
                        Sentido_z = 0;

                    // Mudança do Clock

                        Passos = 1;
                        wait_us(sensibilidade_x);
                        Passos = 0;
                        wait_us(sensibilidade_x);

                    // Contagem de Passo em Z

                        contador_z += 1;

                }
        
        }


    // Referenciamento do Eixo Z
        
        void zera_z(){

            // Variável de Controle de Referenciamento

                limite_z = 0;

            // Referenciamento

                while(limite_z == 0 && estadoBE==0){

                    // Ativação do Motor do Eixo Z
                    
                        Enable_z = 1;
                        Sentido_z = 1;

                    // Mudança de Clock

                        Passos = 1;
                        wait_us(sensibilidade_x);
                        Passos=0;
                        wait_us(sensibilidade_x);

                    // Ativação do FDC e Ação corretiva até desativação do mesmo

                        while (FDC_A_Z == 1 && estadoBE==0) {

                            // Mudança de Sentido do Motor 

                                Sentido_z = 0;

                            // Mudança de Clock

                                Passos = 1;
                                wait_us(sensibilidade_x);
                                Passos = 0;
                                wait_us(sensibilidade_x);

                            // Desativação do FDC e da ação corretiva

                                if (FDC_A_Z == 0) {

                                    // Desativação do Motor Z e Finalização do Referenciamento
                                    
                                        limite_z = 1;
                                        Enable_z = 0;
                                        contador_z = 0;

                                }

                        }

                }

        }


    // Funções de Definição do Estado de Emergência

        void toggle_emergencia1(void){

            estadoBE = 1;
            
        }

        void toggle_emergencia2(void){

            estadoBE = 0;

        }

    // Criação de Função Debounce

        void toggle(void);

        void toggle(){

            // Redução de Ruído de Aperto de Botões para evitar Bugs 
            
                if(debounce.read_ms() > 50){

                    if (tela3 == 1 && lado_tela3 == 2){

                        libera = !libera;

                    }

                    if (tela3 == 1 && lado_tela3 == 3){

                        libera_2 = !libera_2;

                    }

                    if(tela1 == 1 && lado_tela1 == 1){

                        botao_teste=1;

                    }

                    if(tela1 == 1 && lado_tela1 == 2){

                        botao_teste = 2;

                    }

                    if(tela0 == 1 && lado_tela0 == 1){

                        botao_teste_tela0 = 1;

                    }

                    if(tela0 == 1 && lado_tela0 == 2){

                        botao_teste_tela0 = 2;

                    }

                    if(tela2 == 1 && lado_tela2 == 1){

                        botao_teste_tela2 = 1;

                    }

                    if(tela2 == 1 && lado_tela2 == 2){

                        botao_teste_tela2 = 2;

                    }

                    if(tela2 == 1 && lado_tela2 == 3){

                        botao_teste_tela2 = 3;

                    }

                    if(tela2 == 1 && lado_tela2 == 4){

                        botao_teste_tela2=4;

                    }

                    if(tela3==1 && lado_tela3 == 1){

                        botao_teste_tela3 = 1;

                    }

                    if(tela4 == 1){

                        botao_teste_tela4 = 1;

                    }

                    if(tela5 == 1 && lado_tela5 == 1){

                        botao_teste_tela5 = 1;

                    }

                    if(tela5 == 1 && lado_tela5 == 2){

                        botao_teste_tela5 = 2;
                        
                    }

                    if(tela5 == 1 && lado_tela5 == 3){

                        botao_teste_tela5 = 3;
                    }

                    if(tela6 == 1 && lado_tela6 == 1){

                        botao_teste_tela6 = 1;

                    }

                    if(tela6 == 1 && lado_tela6 == 2){

                        botao_teste_tela6 = 2;

                    }

                }

                debounce.reset();

        }

    // Criação da Função de Seleção de Volume

        void volumes(){

            // Leitura do Joystick

                y = yAxis.read()*1000;

            // Joystick Y-

                if(y < 300){

                    // Verificação e Limitação Superior de Volume
                    
                        if(vol >= volume_final){

                            vol = volume_final;

                        }

                    // Adição de Volume e Alteração de Tela

                        else{

                            vol = vol + 1;
                            
                            tft.fillRoundRect(140, 100, 60, 60, 5, CYAN);
                            tft.setCursor(165, 120);
                            tft.println(vol);
                            fixo_0 = 0;
                            wait_ms(300);

                        }
                
                }

            // Joystick Y+

                if(y > 900){

                    // Verificação do Limite Inferior de Volume

                        if(vol <= volume_inicial){

                            vol = volume_inicial;
                            
                        }

                    // Subtração de Volume e Alteração da Tela

                        else{
                        
                            vol = vol - 1;
                        
                            tft.fillRoundRect(140, 100, 60, 60, 5, CYAN);
                            tft.setCursor(165, 120);
                            tft.println(vol);
                            fixo_0 = 0;
                            wait_ms(300);
                        
                        }
                
                }
        
        }

    // Função de Pipetagem

        void pipetar(void){

            // Ativação e Desativação do Rele de Contato Seco da pipeta

                rele1 = 0;
                wait(0.1);
                rele1 = 1;

        }

    // Função de Deposição Automática de Líquido em XY

        void solta(int index){

            // Loop para movimentação da pipeta nos eixos XY de forma interpolada

                while((contador_x != lista_x[index] || contador_y != lista_y[index]) && estadoBE == 0){

                    // X-

                        if(contador_x-lista_x[index] < 0){

                            // Ativação dos Motores e Controle de Passo

                                Enable_x = 0;
                                Sentido_x = 1;
                                contador_x += 1;
                        
                        }

                    // X+

                        if(contador_x-lista_x[index] > 0){

                            // Ativação dos Motores e Controle de Passo
                            
                                Enable_x = 0;
                                Sentido_x = 0;
                                contador_x -= 1;
                        
                        }

                    // Y-

                        if(contador_y-lista_y[index] < 0){

                            // Ativação dos Motores e Controle de Passo

                                Enable = 0;
                                Sentido = 1;
                                contador_y += 1;
                        
                        }

                    // Y+

                        if(contador_y-lista_y[index] > 0){

                            // Ativação dos Motores e Controle de Passo

                                Enable = 0;
                                Sentido = 0;
                                contador_y -= 1;
                            
                        }

                    // Desativação dos Motores X e Y finalizado o processo
                    
                        if(contador_x == lista_x[index]){

                            Enable_x = 1;
                        
                        }

                        if(contador_y == lista_y[index]){
                            
                            Enable = 1;
                        
                        }

                    // Mudança do Clock

                        Passos=1;
                        wait_us(sensibilidade_x);
                        Passos=0;
                        wait_us(sensibilidade_x);

                }
        }

    // Função de Coleta Automática de Líquido XY

        void pega(){

            while((contador_x != lista_x[0] || contador_y != lista_y[0]) && estadoBE ==0){

                // X-

                    if(contador_x-lista_x[0] < 0){

                        // Ativação do Motor e Contagem de Passos

                            Enable_x = 0;
                            Sentido_x = 1;
                            contador_x += 1;
                    
                    }

                // X+

                    if(contador_x-lista_x[0] > 0){

                        // Ativação do Motor e Contagem de Passos

                            Enable_x = 0;
                            Sentido_x = 0;
                            contador_x -= 1;
                    
                    }

                // Y-

                    if(contador_y-lista_y[0] < 0){

                        // Ativação do Motor e Contagem de Passos

                            Enable = 0;
                            Sentido = 1;
                            contador_y += 1;
                    
                    }

                // Y+

                    if(contador_y-lista_y[0] > 0){

                        // Ativação do Motor e Contagem de Passos

                            Enable = 0;
                            Sentido = 0;
                            contador_y -= 1;
                    
                    }

                // Desativação dos Motores finalizada a operação

                    if(contador_x == lista_x[0]){

                        Enable_x = 1;
                        
                    }

                    if(contador_y == lista_y[0]){

                        Enable = 1;
                        
                    }

                // Mudança do Clock

                    Passos=1;
                    wait_us(sensibilidade_x);
                    Passos=0;
                    wait_us(sensibilidade_x);

            }
            
        }

    // Criação da Função de Referenciamento

        void referenciamento() {

            // Loop para movimentação da pipeta

                while (limite_x == 0 || limite_y == 0 && estadoBE==0) {

                    // Eixo X

                        if(limite_x == 0){

                            // Ativação do Motor

                                Enable_x = 0;
                                Sentido_x = 0;

                            // Mudança do clock

                                Passos = 1;
                                wait_us(sensibilidade_x);
                                Passos = 0;
                                wait_us(sensibilidade_x);

                            // Detecção do FDC X para ação corretiva

                                while (FDC_A_X == 1 && estadoBE==0) {

                                    // Mudança de sentido de rotação do motor

                                        Sentido_x = 1;

                                    // Mudança do clock

                                        Passos = 1;
                                        wait_us(sensibilidade_x);
                                        Passos = 0;
                                        wait_us(sensibilidade_x);

                                    // detecção da desativação do FDC e finalização da ação corretiva

                                        if (FDC_A_X == 0) {

                                            limite_x = 1;
                                            Enable_x = 1;
                                            contador_x = 0;

                                        }

                                }

                        }

                    // Eixo Y

                        if(limite_y == 0){

                            // Ativação do Motor 
                        
                                Enable = 0;
                                Sentido = 0;

                            // Mudança do Clock

                                Passos = 1;
                                wait_us(sensibilidade_x);
                                Passos= 0;
                                wait_us(sensibilidade_x);

                            // Detecção do FDC para ação corretiva

                                while (FDC_A_Y == 1 && estadoBE==0) {

                                    // Mudança do sentido de rotação do motor

                                        Sentido = 1;

                                    // Mudança do Clock

                                        Passos = 1;
                                        wait_us(sensibilidade_x);
                                        Passos = 0;
                                        wait_us(sensibilidade_x);
                                    

                                    // Detecção da desativação do FDc e finalização da ação corretiva

                                        if (FDC_A_Y == 0) {

                                            limite_y = 1;
                                            Enable = 1;
                                            contador_y = 0;

                                        }

                                }

                        }

                }

        }

    // Referenciamento Total

        void referenciamento_total(){

            // Realização do Referenciamento dos eixo Z e XY na mesma ordem

                zera_z();
                referenciamento();

        } 

    // Função de Coleta Automática de Líquido Z

        void posicao_z_pega(){

            // Loop para movimentação da pipeta

                while(contador_z != lista_z[0] && estadoBE==0){

                    // Z- Automático

                        if(contador_z-lista_z[0] > 0){

                            // Ativação do motor e controle de passos

                                y = yAxis.read()*1000;
                                Enable_z = 1;
                                Sentido_z = 1;
                                contador_z -= 1;

                        }

                    // Z+ Automático

                        if(contador_z-lista_z[0] < 0){

                            // Ativação do motor e controle de passos

                                y = yAxis.read()*1000;
                                Enable_z = 1;
                                Sentido_z = 0;
                                contador_z += 1;

                        }

                    // Finalização da movimentação em Z

                        if(contador_z == lista_z[0]){

                            // Desativação do motor

                                Enable_z = 0;
                            
                        }

                    // Mudança do Clock

                        Passos=1;
                        wait_us(sensibilidade_x);
                        Passos=0;
                        wait_us(sensibilidade_x);

                }

        }

    // Função de Deposição Automática de Líquido Z

        void posicao_z_solta(int index){

            // Loop para moviemntação automática da pipeta

                while(contador_z != lista_z[index] && estadoBE==0){

                    // Z- Automático

                        if(contador_z-lista_z[index] > 0){

                            // Ativação do motor e contagem de passos  

                                y = yAxis.read()*1000;
                                Enable_z = 1;
                                Sentido_z = 1;
                                contador_z -= 1;

                        }

                    // Z+ automático

                        if(contador_z-lista_z[index] < 0){

                            // Ativação do motor e contagem de passos

                                y = yAxis.read()*1000;
                                Enable_z = 1;
                                Sentido_z = 0;
                                contador_z += 1;

                        }

                    // Finalização da movimentação 

                        if(contador_z == lista_z[index]){

                            // Desativação do motor

                                Enable_z = 0;
                            
                        }

                    // Mudança do Clock

                        Passos = 1;
                        wait_us(sensibilidade_x);
                        Passos = 0;
                        wait_us(sensibilidade_x);

                }

        }

    // Pipetagem Completa
        
        void teste_pipeta(){

            // Loop para percorrer todas posições selecionadas

                while(index_pipeta < 9 && estadoBE ==0){

                    // Início de Pipetagem para Posição Atual

                        if(variavel_pipeta < lista_vol[index_pipeta]){

                            // Inicialização das Funções de Coleta e Deposição Automática em todos os eixos

                                // Coleta 

                                    zera_z();
                                    pega();
                                    posicao_z_pega();
                                    wait(1);
                                    pipetar();

                                // Tempo de Coleta da Pipeta

                                wait(2.5);

                                // Deposição
                                    tela_pipetagem();
                                    zera_z();
                                    solta(index_pipeta);
                                    posicao_z_solta(index_pipeta);
                                    wait(1);
                                    pipetar();

                                // Tempo de Deposição 

                                    wait(2.5);

                                // Contador de Volume

                                    variavel_pipeta += 1;

                                // Mudança Estética da Tela 

                                    tela_pipetagem();
                            
                        }

                    // Mudança de Posição

                        else{

                            // Zeramento de Volume e Mudança de Posição

                                variavel_pipeta = 0;
                                index_pipeta += 1;

                            // Mudança Estética da Tela
                                zera_z();
                                tela_pipetagem();

                        }

                }

        }


// CICLO DE FUNCIONAMENTO PRINCIPAL

    void principal(void){

        // Estabelecimento da Interrupção pelo Botão de Emergência

            botao1.fall(&toggle);
            estadoemergencia.fall(&toggle_emergencia1);
            estadoemergencia.rise(&toggle_emergencia2);
            debounce.start();

        // Loop Principal

            while(1){

                // Setagem de Estado Off dos Componentes Eletrônicos por padrão 

                    rele1=1;
                    Enable=1;
                    Enable_x=1;
                    Enable_z=0;
                    Sentido=0;
                
                // Leitura do Joystick

                    y=yAxis.read()*1000;
                    x=xAxis.read()*1000;
                
                // Funcionamento caso Botão de Emergência não esteja pressionado

                    // Variável de Emergência Desativada

                        if(estadoBE == 0){

                            fixo_emergencia=1;

                    // TELA DE INICIO
                        
                        if (tela1 == 1){

                            // Incialização da Tela Base

                                if(fixo_1 == 1){

                                    tela();
                                    fixo_1 = 0;

                                }

                            // Inicialização da Função de Movimentação da Tela

                                mov_tela1();


                            // Clique do Botão "Posição"

                                if (botao_teste == 1 && index > 0 && ref == 1){

                                    // Não permissão de mudança de tela

                                        tela1 = 0;
                                        fixo_0 = 1;
                                        tela0 = 1;
                                        lado_tela1 = 0;
                                        botao_teste = 0;
                                
                                }

                                if (botao_teste == 1 && index == 0 && ref == 1){

                                    // Mudança para tela seguinte

                                        tela1 = 0;
                                        fixo_2 = 1;
                                        tela2 = 1;
                                        lado_tela1 = 0;
                                        botao_teste = 0;
                                
                                }

                            // Clique do Botão "Referenciar"

                                if(botao_teste == 2){

                                    // Inicialização da função de Referenciamento Total

                                        referenciamento_total();
                                        ref = 1;
                                        fixo_1 = 1;
                                        botao_teste = 5;

                                }

                        }


                    // TELA DE VOLUME
                        
                        if(tela0 == 1){

                            // Inicialização da Tela de Volume

                                if(fixo_0==1){

                                    // inicialização da Tela Base

                                        volume();
                                        tft.fillRoundRect(140,100,60,60,5, CYAN);
                                        tft.setCursor(160,120);
                                        tft.println(vol);
                                        fixo_0=0;

                                }

                            // Inicialização da Função de Movimentação da Tela

                                volumes();

                            // Mudança de Botões selecionados na Tela

                                if (x < 50 && teste_x_0 > 0){

                                    wait_ms(300);
                                    teste_x_0 = teste_x_0-1;

                                }

                                if(x > 950 && teste_x_0 < 2){

                                    wait_ms(300);
                                    teste_x_0 = teste_x_0+1;

                                }

                            // Mudança Estética dos Botões caso Selecionados

                                if(teste_x_0==1){

                                    if(lado_tela0!=1){

                                        tft.fillRoundRect(30,185,40,40,5, CYAN);
                                        tft.fillRoundRect(250,185,40,40,5, WHITE);
                                        tft.setCursor(42,200);
                                        tft.println("<");
                                        tft.setCursor(265,200);
                                        tft.println(">");
                                        lado_tela0=1;

                                    }

                                }

                                if(teste_x_0==2){

                                    if(lado_tela0!=2){

                                        tft.fillRoundRect(250,185,40,40,5, CYAN);
                                        tft.fillRoundRect(30,185,40,40,5, WHITE);
                                        tft.setCursor(42,200);
                                        tft.println("<");
                                        tft.setCursor(265,200);
                                        tft.println(">");
                                        lado_tela0=2;

                                    }

                                }

                            // Clique do Botão "Volta"

                                if (botao_teste_tela0 == 1){

                                    // Troca para tela anterior

                                        tela0 = 0;
                                        fixo_1 = 1;
                                        tela1 = 1;
                                        lado_tela0 = 0;
                                        botao_teste_tela0 = 0;

                                }

                            // Clique do Botão "Seguinte"

                                if (botao_teste_tela0 == 2){

                                    // Troca para tela posterior

                                        tela0 = 0;
                                        fixo_2 = 1;
                                        tela2 = 1;
                                        lado_tela0 = 0;
                                        botao_teste_tela0 = 0;
                                
                                } 
                            
                        }

                    // TELA DE OPÇÕES

                        if(tela2==1){

                            //Inicialização da Tela

                                if(fixo_2 == 1){

                                    // Inicialização da Tela Base

                                        sensi();

                                    // Edição da Tela 

                                        tft.fillRoundRect(120, 70, 50, 25, 0, WHITE);
                                        tft.setCursor(130, 75);
                                        tft.println("vol:");
                                        tft.fillRoundRect(180, 70, 60, 25, 0, WHITE);
                                        tft.setCursor(190, 75);
                                        tft.println(vol);
                                        tft.setCursor(220, 75);
                                        tft.println("ml");
                                        tft.fillRoundRect(60, 15, 35, 45, 0, WHITE);
                                        tft.setCursor(65, 30);
                                        tft.println(contador_x/100);
                                        tft.fillRoundRect(160, 15, 35, 45, 0, WHITE);
                                        tft.setCursor(165, 30);
                                        tft.println(contador_y/100);
                                        tft.fillRoundRect(260, 15, 35, 45, 0, WHITE);
                                        tft.setCursor(265, 30);
                                        tft.println(contador_z/100); 
                                        tft.setCursor(42, 200);
                                        tft.println("<");
                                        fixo_2 = 0;

                                }

                            // Verificação de qual botão está sendo selecionado

                                if (y < 10 && teste_y_2 > 0){

                                    wait_ms(300);
                                    teste_y_2 = teste_y_2-1;
                                } 

                                if(y > 999 && teste_y_2 < 2){

                                    wait_ms(300);
                                    teste_y_2 = teste_y_2+1;
                                }

                                if (x < 50 && teste_x_2 > 0){

                                    wait_ms(300);
                                    teste_x_2 = teste_x_2-1;
                                }

                                if(x > 950 && teste_x_2 < 2){

                                    wait_ms(300);
                                    teste_x_2 = teste_x_2+1;

                                }
                            
                            // Mudança Estética da Seleção dos Botões

                                if (teste_y_2==1 & teste_x_2==1){

                                    if(lado_tela2 != 1){

                                        tft.fillRoundRect(35, 100, 120, 50, 5, CYAN);
                                        tft.fillRoundRect(170, 100, 120, 50, 5, WHITE);
                                        tft.fillRoundRect(103, 165, 120, 50, 5, WHITE);
                                        tft.fillRoundRect(30, 185, 40, 40, 5, WHITE);
                                        tft.setCursor(70, 115);
                                        tft.println("MAPA");
                                        tft.setCursor(195, 115);
                                        tft.println("MANUAL");
                                        tft.setCursor(120, 180);
                                        tft.println("DEFINIR");
                                        lado_tela2 = 1;
                                        tft.setCursor(42, 200);
                                        tft.println("<");
                                        wait_ms(300);

                                    }

                                }
    
                                if (teste_y_2 == 1 & teste_x_2 == 2){

                                    if(lado_tela2!=2){

                                        tft.fillRoundRect(35, 100, 120, 50, 5, WHITE);
                                        tft.fillRoundRect(170, 100, 120, 50, 5, CYAN);
                                        tft.fillRoundRect(103, 165, 120, 50, 5, WHITE);
                                        tft.fillRoundRect(30, 185, 40, 40, 5, WHITE);
                                        tft.setCursor(70, 115);
                                        tft.println("MAPA");
                                        tft.setCursor(195, 115);
                                        tft.println("MANUAL");
                                        tft.setCursor(120, 180);
                                        tft.println("DEFINIR");
                                        tft.setCursor(42, 200);
                                        tft.println("<");
                                        lado_tela2 = 2;
                                        wait_ms(300);

                                    }
                                }
                        
                                if (teste_y_2 == 2 & teste_x_2 == 2){

                                    if(lado_tela2!=3){

                                        tft.fillRoundRect(35, 100, 120, 50, 5, WHITE);
                                        tft.fillRoundRect(170, 100, 120, 50, 5, WHITE);
                                        tft.fillRoundRect(103, 165, 120, 50, 5, CYAN);
                                        tft.fillRoundRect(30, 185, 40, 40, 5, WHITE);
                                        tft.setCursor(70, 115);
                                        tft.println("MAPA");
                                        tft.setCursor(195, 115);
                                        tft.println("MANUAL");
                                        tft.setCursor(120, 180);
                                        tft.println("DEFINIR");
                                        tft.setCursor(42, 200);
                                        tft.println("<");
                                        lado_tela2 = 3;
                                        wait_ms(300);

                                    }

                                }

                                if (teste_y_2 == 2 & teste_x_2 == 1){

                                    if(lado_tela2!=4){

                                        tft.fillRoundRect(35, 100, 120, 50, 5, WHITE);
                                        tft.fillRoundRect(170, 100, 120, 50, 5, WHITE);
                                        tft.fillRoundRect(103, 165, 120, 50, 5, WHITE);
                                        tft.fillRoundRect(30, 185, 40, 40, 5, CYAN);
                                        tft.setCursor(70, 115);
                                        tft.println("MAPA");
                                        tft.setCursor(195, 115);
                                        tft.println("MANUAL");
                                        tft.setCursor(120, 180);
                                        tft.println("DEFINIR");
                                        tft.setCursor(42, 200);
                                        tft.println("<");
                                        lado_tela2 = 4;
                                        wait_ms(300);

                                    }

                                }

                            // Clique dos Botões 

                                // Clique do Botão "Manual"

                                    if(botao_teste_tela2 == 2){

                                        // Troca para Tela de Controle Manual

                                            tela2 = 0;
                                            fixo_3 = 1;
                                            tela3 = 1; 
                                            lado_tela2 = 0; 
                                            botao_teste_tela2 = 0;

                                    }

                                // Clique no Botão "Mapa"

                                    if(botao_teste_tela2 == 1){

                                        // Troca para a Tela "Mapa de Coordenadas"

                                            tela2 = 0;
                                            fixo_4 = 1;
                                            tela4 = 1;
                                            botao_teste_tela2 = 0;
                                    
                                    }

                                // Clique no Botão "Volta" em caso de posição de deposição

                                    if(botao_teste_tela2==4 && index>0){

                                        // Troca para a tela de volumes

                                            tela2=0;
                                            fixo_0=1;
                                            tela0=1;
                                            lado_tela2=0;
                                            botao_teste_tela2=0;
                                    
                                    }

                                // Clique no Botão "Volta" em caso de posição de coleta 
                                    
                                    if(botao_teste_tela2==4 && index==0){

                                        // Troca para a tela incial
                                        
                                            tela2=0;
                                            fixo_1=1;
                                            tela1=1;
                                            lado_tela2=0;
                                            botao_teste_tela2=0;
                                    
                                    }

                                // Clique no Botão "Definir"
                                
                                    if(botao_teste_tela2==3){

                                        // Troca para a Tela de posições
                                        
                                            tela2 = 0;
                                            fixo_5 = 1;
                                            tela5 = 1;
                                            lado_tela2 = 0;
                                            botao_teste_tela2 = 0;
                                    
                                    }

                        }
                    // TELA DO CONTROLE MANUAL

                        if (tela3==1){

                            // Inicialização da Tela

                                tft.setTextColor(BLACK);

                                if (fixo_3==1){

                                    // Inicialização da função de movimentação na tela

                                        manual(); 
                                        fixo_3=0;
                                        libera=0;

                                }

                            // Verificação de Botão Sleecionado

                                if (x < 10 && teste_y > 0 & libera == 0 & libera_2 == 0){

                                    wait_ms(300);
                                    teste_y = teste_y-1;

                                }  

                                if(x > 950 && teste_y < 4 & libera == 0 & libera_2 == 0){

                                    wait_ms(300);
                                    teste_y = teste_y+1;

                                }

                            // Mudança Estética dos Botões Selecionados

                                // JOG XY Selecionado

                                    if (teste_y == 2){ 

                                        if(lado_tela3 != 2){

                                            tft.setTextColor(BLACK);
                                            tft.fillRoundRect(30, 185, 40, 40, 5, WHITE);
                                            tft.fillRoundRect(100, 100, 60, 60, 5, CYAN);
                                            tft.fillRoundRect(210, 100, 60, 60, 5, WHITE);
                                            tft.setCursor(230, 120);
                                            tft.println("Jz");
                                            tft.setCursor(125, 120);
                                            tft.println("J");
                                            tft.setCursor(42, 200);
                                            tft.println("<");
                                            lado_tela3 = 2;

                                        }

                                    }

                                // Botão "Voltar" Selecionado

                                    if (teste_y == 1){

                                        if(lado_tela3 != 1){

                                            tft.setTextColor(BLACK);
                                            tft.fillRoundRect(30, 185, 40, 40, 5, CYAN);
                                            tft.fillRoundRect(100, 100, 60, 60, 5, WHITE);
                                            tft.fillRoundRect(210, 100, 60, 60, 5, WHITE);
                                            tft.setCursor(230, 120);
                                            tft.println("Jz");
                                            tft.setCursor(125, 120);
                                            tft.println("J");
                                            tft.setCursor(42, 200);
                                            tft.println("<");
                                            lado_tela3 = 1;

                                        }

                                    }

                                // JOG Z Selecionado 

                                    if(teste_y == 3){ 

                                        if(lado_tela3 != 3){

                                            tft.setTextColor(BLACK);
                                            tft.fillRoundRect(30, 185, 40, 40, 5, WHITE);
                                            tft.fillRoundRect(100, 100, 60, 60, 5, WHITE);
                                            tft.fillRoundRect(210, 100, 60, 60, 5, CYAN);
                                            tft.setCursor(230, 120);
                                            tft.println("Jz");
                                            tft.setCursor(125, 120);
                                            tft.println("J");
                                            tft.setCursor(42, 200);
                                            tft.println("<");
                                            lado_tela3 = 3;

                                        }

                                    }

                            // Clique dos Botões 

                                // Clique no Botão "Voltar"

                                    if(botao_teste_tela3 == 1){

                                        // Troca para a tela de menu

                                            tela3 = 0;
                                            fixo_2 = 1;
                                            tela2 = 1;
                                            lado_tela3 = 0;
                                            teste_x_2 = 1;
                                            teste_y_2 = 1;
                                            botao_teste_tela3 = 0;
                                        
                                    }

                                // Clique no Botão "JOG XY"

                                    if (libera == 1){

                                        // Inicialização da função de movimentação da pipeta em XY

                                            jogManual();

                                    }
                                
                                // Clique no Botão "JOG Z"

                                    if(libera_2 == 1){

                                        // Inicialização da função de movimentação da pipeta em Z

                                            novo_z();

                                    }        

                        }

                    // TELA DO MAPA 

                        if (tela4==1){

                            // Inicialização da Tela 

                                if(fixo_4 == 1){

                                    // Inicialização da função de funcionamento do mapa

                                        mapa_p3();
                                        fixo_4 = 0;

                                }

                            // Clique no Botão "Volta"

                                if(botao_teste_tela4==1){

                                    // Troca para a tela de menu

                                        tela4 = 0;
                                        fixo_2 = 1;
                                        tela2 = 1;
                                        lado_tela2 = 0;
                                        botao_teste_tela4 = 0;
                                    
                                }

                        }


                    // TELA DE HISTÓRICO 

                        if (tela5==1){

                            // Inicialização da Tela

                                if(fixo_5==1){

                                    // Inicialização da função de movimentação da tela

                                        historico();
                                        fixo_5=0;

                                    // Salvamento da quantidade de passos e de volumes em suas respectivas listas

                                        lista_x[index] = contador_x;
                                        lista_y[index] = contador_y;
                                        lista_z[index] = contador_z;
                                        lista_vol[index] = vol;

                                    // Exibição das Posições Atuais para os 10 slots de posicionamento (1 de coleta / 9 de deposição) na tela

                                        // Posição 1
                                
                                            tft.setCursor(11, 43);
                                            tft.println(lista_x[0]/100);
                                            tft.setCursor(55, 43);
                                            tft.println(lista_y[0]/100);
                                            tft.setCursor(99, 43);
                                            tft.println(lista_z[0]/100);
                                            tft.setCursor(140, 43);
                                            tft.println('-'); 

                                        // Posição 2

                                            tft.setCursor(11, 73);
                                            tft.println(lista_x[1]/100);
                                            tft.setCursor(55, 73);
                                            tft.println(lista_y[1]/100);
                                            tft.setCursor(99, 73);
                                            tft.println(lista_z[1]/100);
                                            tft.setCursor(140, 73);
                                            tft.println(lista_vol[1]);

                                        // Posição 3

                                            tft.setCursor(11, 103);
                                            tft.println(lista_x[2]/100);
                                            tft.setCursor(55, 103);
                                            tft.println(lista_y[2]/100);
                                            tft.setCursor(99, 103);
                                            tft.println(lista_z[2]/100);
                                            tft.setCursor(140, 103);
                                            tft.println(lista_vol[2]);

                                        // Posição 4

                                            tft.setCursor(11, 133);
                                            tft.println(lista_x[3]/100);
                                            tft.setCursor(55, 133);
                                            tft.println(lista_y[3]/100);
                                            tft.setCursor(99, 133);
                                            tft.println(lista_z[3]/100);
                                            tft.setCursor(140, 133);
                                            tft.println(lista_vol[3]);

                                        // Posição 5

                                            tft.setCursor(11, 166);
                                            tft.println(lista_x[4]/100);
                                            tft.setCursor(55, 166);
                                            tft.println(lista_y[4]/100);
                                            tft.setCursor(99, 166);
                                            tft.println(lista_z[4]/100);
                                            tft.setCursor(140, 166);
                                            tft.println(lista_vol[4]);

                                        // Posição 6

                                            tft.setCursor(170, 43);
                                            tft.println(lista_x[5]/100);
                                            tft.setCursor(210, 43);
                                            tft.println(lista_y[5]/100);
                                            tft.setCursor(250, 43);
                                            tft.println(lista_z[5]/100);
                                            tft.setCursor(290, 43);
                                            tft.println(lista_vol[5]);

                                        // Posição 7

                                            tft.setCursor(170, 73);
                                            tft.println(lista_x[6]/100);
                                            tft.setCursor(210, 73);
                                            tft.println(lista_y[6]/100);
                                            tft.setCursor(250, 73);
                                            tft.println(lista_z[6]/100);
                                            tft.setCursor(290, 73);
                                            tft.println(lista_vol[6]);

                                        // Posição 8 

                                            tft.setCursor(170, 103);
                                            tft.println(lista_x[7]/100);
                                            tft.setCursor(210, 103);
                                            tft.println(lista_y[7]/100);
                                            tft.setCursor(250, 103);
                                            tft.println(lista_z[7]/100);
                                            tft.setCursor(290, 103);
                                            tft.println(lista_vol[7]);

                                        // Posição 9

                                            tft.setCursor(170, 133);
                                            tft.println(lista_x[8]/100);
                                            tft.setCursor(210, 133);
                                            tft.println(lista_y[8]/100);
                                            tft.setCursor(250, 133);
                                            tft.println(lista_z[8]/100);
                                            tft.setCursor(290, 133);
                                            tft.println(lista_vol[8]);

                                        // Posição 10

                                            tft.setCursor(170, 166);
                                            tft.println(lista_x[9]/100);
                                            tft.setCursor(210, 166);
                                            tft.println(lista_y[9]/100);
                                            tft.setCursor(250, 166);
                                            tft.println(lista_z[9]/100);
                                            tft.setCursor(290, 166);
                                            tft.println(lista_vol[9]);

                                }

                            // Verificação de qual botão é selecionado
                            
                                if (x < 50 && teste_x_5 > 0){

                                    wait_ms(300);
                                    teste_x_5 = teste_x_5-1;

                                }

                                if(x > 950 && teste_x_5 < 3){

                                    wait_ms(300);
                                    teste_x_5 = teste_x_5+1;

                                }

                            // Mudança Estética do Botão selecionado

                                if(teste_x_5 == 1){

                                    if(lado_tela5 != 1){

                                        tft.fillRoundRect(30, 190, 40, 40, 5, CYAN);
                                        tft.fillRoundRect(110, 190, 60, 40, 5, WHITE);
                                        tft.fillRoundRect(190, 190, 100, 40, 5, WHITE);
                                        tft.setCursor(42, 200);
                                        tft.println("<");
                                        tft.setCursor(200, 200);
                                        tft.println("pipetar");
                                        tft.setCursor(120, 200);
                                        tft.println("nova");
                                        lado_tela5 = 1;

                                    }

                                }

                                if(teste_x_5 == 2){

                                    if(lado_tela5 != 2){

                                        tft.fillRoundRect(30, 190, 40, 40, 5, WHITE);
                                        tft.fillRoundRect(110, 190, 60, 40, 5, CYAN);
                                        tft.fillRoundRect(190, 190, 100, 40, 5, WHITE);
                                        tft.setCursor(42, 200);
                                        tft.println("<");
                                        tft.setCursor(200, 200);
                                        tft.println("pipetar");
                                        tft.setCursor(120, 200);
                                        tft.println("nova");
                                        lado_tela5 = 2;

                                    }

                                }
                        
                                if(teste_x_5 == 3){

                                    if(lado_tela5 != 3){

                                        tft.fillRoundRect(30, 190, 40, 40, 5, WHITE);
                                        tft.fillRoundRect(110, 190, 60, 40, 5, WHITE);
                                        tft.fillRoundRect(190, 190, 100, 40, 5, CYAN);
                                        tft.setCursor(200, 200);
                                        tft.println("pipetar");
                                        tft.setCursor(120, 200);
                                        tft.println("nova");
                                        tft.setCursor(42, 200);
                                        tft.println("<");
                                        lado_tela5 = 3;

                                    }

                                }

                            // Clique dos Botões Selecionados
                            
                                // Clique Botão "Voltar"

                                    if(botao_teste_tela5==1){

                                        // Troca para a tela de menu
                                        
                                            tela5=0;
                                            fixo_2=1;
                                            tela2=1;
                                            teste_y_2=1;
                                            botao_teste_tela5=0;
                                        
                                    }

                                // Clique Botão "Nova"

                                    if(botao_teste_tela5==2){

                                        // Troca para a tela de volume

                                            tela5=0;
                                            fixo_0=1;
                                            tela0=1;
                                            teste_y_2=1;
                                            botao_teste_tela5=0; 

                                        // Início de Processo de Seleção para Nova Posição 
                                        
                                            if(index>-1 & index<10){
                                                
                                                index=index+1;
                                                
                                            }
                                            
                                    }

                                // Clique Botão "Pipetar"

                                    if(botao_teste_tela5==3){

                                        // Troca para a tela de pipetagem

                                            tela5=0;
                                            tela6=1;
                                            botao_teste_tela5=0;
                                            controle_da_pipeta=1;
                                        
                                    }
                                
                                
                        }

                    // TELA DE PIPETAGEM

                        if (tela6 == 1){
                            
                            // Inicialização da Tela

                                if (controle_da_pipeta == 1){

                                    // Inicialização da função de pipetagem

                                        teste_pipeta();
                                        botao_teste_tela6 = 0;

                                } 

                            // Desativação da Pipeta

                                controle_da_pipeta = 0;

                            // Detecção de Botão Selecionado e mudança estética dos botões

                                if(x < 10){

                                    if(lado_tela6 != 1){

                                    tft.fillRoundRect(40, 180, 100, 50, 15, CYAN);
                                    tft.fillRoundRect(170, 180, 100, 50, 15, WHITE);
                                    tft.setCursor(50, 200);
                                    tft.println("Repetir");
                                    tft.setCursor(195, 190);
                                    tft.println("Novo");
                                    tft.setCursor(195, 210);
                                    tft.println("Ciclo");
                                    lado_tela6 = 1;
                                    
                                    }
                                    
                                }

                                if(x > 950){        

                                    if(lado_tela6 != 2){

                                    tft.fillRoundRect(40, 180, 100, 50, 15, WHITE);
                                    tft.fillRoundRect(170, 180, 100, 50, 15, CYAN);
                                    tft.setCursor(50, 200);
                                    tft.println("Repetir");
                                    tft.setCursor(195, 190);
                                    tft.println("Novo");
                                    tft.setCursor(195, 210);
                                    tft.println("Ciclo");
                                    lado_tela6 = 2;
                                    
                                    }
                                    
                                }

                            // Clique dos botões selecionados

                                // Clique do Botão "Nova Pipetagem"

                                    if(botao_teste_tela6==2){

                                        // Zeramento de todas as variáveis existentes

                                            fixo_1 = 1;
                                            tela1 = 1;
                                            tela0 = 0; 
                                            tela2 = 0;
                                            tela3 = 0; 
                                            tela4 = 0; 
                                            tela5 = 0; 
                                            tela6 = 0;
                                            ref = 0;
                                            limite_x = 0; 
                                            limite_y = 0;
                                            limite_z=0;
                                            index = 0;
                                            fixo_pipeta = 0;
                                            index_pipeta = 1;
                                            variavel_pipeta = 0;
                                            controle_da_pipeta = 1;
                                            vol = 1;
                                        
                                        // Zeramento de todas as listas existentes

                                            for(int a = 0; a < 10; a++){

                                                lista_y[a] = 0;
                                                lista_vol[a] = 0;
                                                lista_x[a] = 0;
                                                lista_z[a] = 0;

                                            }
                                    
                                    }  

                                // Clique no Botão "Refazer"

                                    if(botao_teste_tela6==1){

                                        // Reinicialização do processo de pipetagem com as posições previamente obtidas

                                            tft.fillRoundRect(15, 13, altura-30, 45, 0, WHITE);
                                            tft.setTextSize(2);
                                            tft.setTextColor(BLACK);
                                            tft.setCursor(50, 28);
                                            tft.println("PIPETAGEM INICIADA");
                                            index_pipeta = 1;
                                            variavel_pipeta = 0;
                                            controle_da_pipeta = 1;
                                    
                                    }

                        }

                }

                // Funcionamento caso o Botão de Emergência for Pressionado

                    if(estadoBE == 1){

                        // Acionamento da Tela de Emergência

                            if (fixo_emergencia == 1){

                                    emergencia();

                                fixo_emergencia = 0;
                            
                            }

                        // Zeramento de Todas as variáveis existentes                        

                            fixo_1 = 1;
                            tela1 = 1;
                            tela0 = 0; 
                            tela2 = 0;
                            tela3 = 0; 
                            tela4 = 0; 
                            tela5 = 0; 
                            tela6 = 0;
                            ref = 0;
                            limite_x = 0; 
                            limite_y = 0;
                            index = 0;
                            fixo_pipeta = 0;
                            index_pipeta = 1;
                            variavel_pipeta = 0;
                            controle_da_pipeta = 1;
                            vol = 1;

                        // Zeramento de Todas as listas existentes

                            for(int a = 0; a < 10; a++){

                                lista_y[a] = 0;
                                lista_vol[a] = 0;
                                lista_x[a] = 0;
                                lista_z[a] = 0;

                            }

                    } 

                }
    
    }

// ROTINA DE INICIALIZAÇÃO

    int main(){ 

        // Inicialização do Sistema

            tft.reset();
            tft.begin();
            tft.setRotation(Orientation);

        // Inicialização do ciclo principal

            principal();

    }

// Void Loop

    void loop(){}