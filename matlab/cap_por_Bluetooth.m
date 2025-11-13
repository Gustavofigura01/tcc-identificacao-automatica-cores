clear; clc;
close all; % Fecha figuras anteriores

% --- Configurações ---
porta_serial = "COM4"; % Verifique se esta é a porta correta
baud_rate = 115200;
timeout_segundos = 60; % <<< NOVO: Tempo máximo de espera por quadro (em segundos)

N = 1546;     % Nº de pixels por quadro
M = 2;       % Nº de quadros por cor

% Lista de cores a capturar
cores = {'Chantilly', 'Eternidade', 'Quintal_de_Casa', ...
         'Romance_Sereno', 'Vulcao_Osorno', 'Nectar_de_Uva', ...
         'Brilhante_Bruto', 'Banho_de_Lua', 'Coala', ...
         'Papel_de_Seda', 'Aromaterapia', 'Venus', ...
         'Flor_de_lis', 'Roxo_Impecavel', 'Cruzeiro_Maritimo', ...
         'Azul_Sereia', 'Vermelho_Oriental', 'Timidez'};

% Limpa conexões seriais antigas
delete(instrfindall);

try
    % Conecta via porta serial Bluetooth
    fprintf('Conectando à porta serial %s...\n', porta_serial);
    s = serialport(porta_serial, baud_rate);
    s.ByteOrder = 'big-endian'; % Opcional: para simplificar a leitura (ver sugestão)
    fprintf('Conexão estabelecida.\n');

    for c = 1:length(cores)
        rotulo = cores{c};
        X = zeros(M, N);
        Y = repmat({rotulo}, M, 1);
        
        fprintf("\n--> Capturando %d quadros da cor: %s <--\n", M, rotulo);
        
        for i = 1:M
            fprintf("Aguardando quadro %d/%d...", i, M);
            
            bytes_esperados = 2 * N;
            
            % --- LOOP DE ESPERA MELHORADO ---
            inicio_espera = tic; % Inicia o cronômetro do timeout
            while s.NumBytesAvailable < bytes_esperados
                % Verifica se o timeout foi atingido
                if toc(inicio_espera) > timeout_segundos
                    error('Timeout! Nao foram recebidos dados suficientes em %d segundos.', timeout_segundos);
                end
                
                % <<< NOVO: Barra de progresso na mesma linha
                progresso = (s.NumBytesAvailable / bytes_esperados) * 100;
                fprintf('\rAguardando quadro %d/%d... [%.1f%%]', i, M, progresso);
                
                pause(0.1); % Pausa para não sobrecarregar a CPU
            end
            fprintf('\rQuadro %d/%d recebido! Processando...            \n', i, M); % Limpa a linha
            
            % Leitura e processamento dos dados
            rx = read(s, bytes_esperados, 'uint8');
            for n = 1:N
                X(i, n) = rx(2*n - 1) * 256 + rx(2*n);
            end
            % Alternativa mais simples se s.ByteOrder = 'big-endian' for usado:
            % X(i, :) = read(s, N, 'uint16');
            
            % Plota os dados recebidos
            figure(1); clf;
            plot(X(i, :));
            title(sprintf("Captura %d/%d - Cor: %s", i, M, rotulo));
            xlabel("Pixel"); ylabel("Intensidade");
            ylim([-100, 2000]);
            grid on;
            drawnow; % Força a atualização do gráfico
        end
        
        % Prepara e salva os dados
        fprintf("Salvando dados da cor %s...\n", rotulo);
        dados = [Y, num2cell(X)];
        cabecalho = [{'classe'}, arrayfun(@(i) sprintf('pixel_%d', i), 1:N, 'UniformOutput', false)];
        arquivo_csv = sprintf("dados_%s.csv", rotulo);
        writecell([cabecalho; dados], arquivo_csv);
        fprintf("✔️ Dados salvos em %s\n", arquivo_csv);
        
        if c < length(cores)
             fprintf("Por favor, posicione a próxima cor. Pausa de 1.5 segundos...\n");
             pause(5); % Tempo para trocar a cor
        end
    end

    disp("✅ Todas as cores foram capturadas com sucesso.");

catch ME % Captura qualquer erro que ocorrer
    fprintf(2, '\n❌ ERRO DURANTE A EXECUÇÃO: %s\n', ME.message);
    if exist('ME.stack', 'var')
        fprintf(2, 'Erro na linha %d do arquivo %s\n', ME.stack(1).line, ME.stack(1).name);
    end
    
finally
    % Este bloco SEMPRE será executado, com ou sem erro
    fprintf('Fechando todas as conexões seriais...\n');
    delete(instrfindall);
end