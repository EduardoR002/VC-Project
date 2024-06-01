#include <iostream>
#include <string>
#include <chrono>
#include <opencv2\opencv.hpp>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\videoio.hpp>
#include <vector>
#include <map>

extern "C" {
#include "vc.h"
}

struct Blob {
	int x, y, width, height;
};

// Função vc_timer que mede e exibe o tempo decorrido entre chamadas consecutivas.
void vc_timer(void) {

	// A variável "running" é um booleano que indica se o temporizador está em execução.
	static bool running = false;
	// A variável "previousTime" armazena o tempo do ponto anterior no tempo(quando a função foi chamada pela última vez).
	static std::chrono::steady_clock::time_point previousTime = std::chrono::steady_clock::now();

	// Se running é false, significa que a função está sendo chamada pela primeira vez. A variável running é então definida como true para indicar que o temporizador está agora em execução.
	if (!running) {
		running = true;
	}
	else {
		//Tempo Atual: currentTime é capturado usando std::chrono::steady_clock::now()
		std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
		//Tempo Decorrido: elapsedTime é a diferença entre currentTime e previousTime
		std::chrono::steady_clock::duration elapsedTime = currentTime - previousTime;

		// Conversão para Segundos: time_span converte elapsedTime para uma duração em segundos.
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(elapsedTime);
		double nseconds = time_span.count();

		std::cout << "Tempo decorrido: " << nseconds << "segundos" << std::endl;
		std::cout << "Pressione qualquer tecla para continuar...\n";
		std::cin.get();
	}
}

// Função main
int main(void) {

	// Variável inicializada com o nome do vídeo, este array é utilizado para armazenar o caminho/nome do arquivo de vídeo que será processado
	char videofile[20] = "video_resistors.mp4";

	// Objeto para captura de vídeo, este objeto será utilizado para abrir, ler e manipular o vídeo especificado.
	cv::VideoCapture capture;

	// Estrutura para armazenar informações do vídeo
	struct
	{
		int width, height;
		int ntotalframes;
		int fps;
		int nframe;
	} video;


	// Outras variáveis que vão ser usadas mais á frente
	std::string str;
	int key = 0;



	/* Leitura de vídeo de um ficheiro */
	/* NOTA IMPORTANTE: O ficheiro video.avi deverá estar localizado no mesmo directório que o ficheiro de código fonte. */

	// Tenta abrir o arquivo de vídeo, e caso não consiga apresenta uma mensagem ao utilizador a informar o erro
	if (!capture.open(videofile)) {
		std::cerr << "Erro ao abrir o ficheiro de vídeo!" << std::endl;
		return -1;
	}

	/* Em alternativa, abrir captura de vídeo pela Webcam #0 */
	//capture.open(0, cv::CAP_DSHOW); // Pode-se utilizar apenas capture.open(0);


	// Obtém o número total de frames  no vídeo 
	video.ntotalframes = (int)capture.get(cv::CAP_PROP_FRAME_COUNT);

	// Obtém a taxa de frames por segundo (fps) do vídeo
	video.fps = (int)capture.get(cv::CAP_PROP_FPS);

	// Obtém a largura do vídeo em pixels
	video.width = (int)capture.get(cv::CAP_PROP_FRAME_WIDTH);

	// Obtém a altura do vídeo em pixels
	video.height = (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT);

	// Cria uma janela para exibir o vídeo 
	cv::namedWindow("VC - VIDEO", cv::WINDOW_AUTOSIZE);

	// Inicia um temporizador para medir o tempo decorrido entre chamadas consecutivas, útil para monitorar o desempenho do processamento de vídeo
	vc_timer();
	//capture.set(cv::CAP_PROP_POS_FRAMES, 500);

	// Declara uma matriz frame para armazenar cada frame do vídeo.
	cv::Mat frame;

	int contador = 0;
	std::vector<Blob> blobs_detectados;

	// Inicia um loop que continua até que a tecla 'q' seja pressionada.
	while (key != 'q') {

		// Lê o próximo frame do vídeo e o armazena na matriz frame.
		capture.read(frame);

		// Verifica se conseguiu ler o frame , se o frame estiver vazio, indica que o vídeo chegou ao seu final ou houve algum erro na leitura.
		if (frame.empty()) break;

		// Aplica um filtro de desfoque gaussiano ao frame. Isso suaviza a imagem, reduzindo o ruído e detalhes excessivos
		//cv::GaussianBlur(frame, frame, cv::Size(5, 5), 0);


		// Obtém o índice da frame atual no vídeo utilizando o método get() do objeto capture. Ele retorna a posição atual do vídeo em termos de número de frames
		video.nframe = (int)capture.get(cv::CAP_PROP_POS_FRAMES);


		//Exemplo de inserção texto na frame, cria uma string com o texto "RESOLUCAO: ", converte a largura e altura do vídeo em pixels para uma string
		str = std::string("RESOLUCAO: ").append(std::to_string(video.width)).append("x").append(std::to_string(video.height));

		//  Adiciona texto à frame do vídeo, e recebe como parametros, o frame onde vai ser inserido, a string a inserir, as coordenadas deste, tipo e tamanho da fonte, cor e espessura da linha do texto
		cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("TOTAL DE FRAMES: ").append(std::to_string(video.ntotalframes));
		cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("FRAME RATE: ").append(std::to_string(video.fps));
		cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("N. DA FRAME: ").append(std::to_string(video.nframe));
		cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		/* Mesmo processo de adição de texto até aqui */


		/*Código abaixo cria várias imagens IVC para armazenar diferentes tipos de segmentações de imagem. Cada imagem IVC é uma estrutura de dados que contém informações sobre a imagem, como largura,
		altura, número de canais de cor e o próprio buffer de pixels. */
		
		// Cria uma imagem colorida com 3 canais de cor (RGB) e alocada com 255 bytes para cada canal, ou seja, 24 bits por pixel (8 bits por canal). Esta imagem é destinada a armazenar frames de vídeo.
		IVC* image = vc_image_new(video.width, video.height, 3, 255);
		IVC* image_2 = vc_image_new(video.width, video.height, 3, 255);
		IVC* image_3 = vc_image_new(video.width, video.height, 3, 255);

		// Cria uma imagem em escala de cinza com apenas um canal de cor (nível de cinza) e alocada com 255 bytes para o canal, ou seja, 8 bits por pixel.
		IVC* image_4 = vc_image_new(video.width, video.height, 1, 255);
		IVC* image_5 = vc_image_new(video.width, video.height, 1, 255);

		// Cria uma imagem em escala de cinza para armazenar a segmentação de pixels vermelhos.
		IVC* red_segmented_image = vc_image_new(video.width, video.height, 1, 255); 

		// Cria uma imagem em escala de cinza para armazenar a segmentação de pixels castanhos
		IVC* brown_segmented_image = vc_image_new(video.width, video.height, 1, 255); 
		// Cria uma imagem em escala de cinza para armazenar a segmentação de pixels pretos
		IVC* black_segmented_image = vc_image_new(video.width, video.height, 1, 255); 

		// Cria uma imagem em escala de cinza para armazenar a segmentação de pixels laranjas
		IVC* orange_segmented_image = vc_image_new(video.width, video.height, 1, 255); 

		// Cria uma imagem em escala de cinza para armazenar a segmentação de pixels verdes
		IVC* green_segmented_image = vc_image_new(video.width, video.height, 1, 255);  

		// Cria uma imagem em escala de cinza para armazenar a segmentação de pixels azuis
		IVC* blue_segmented_image = vc_image_new(video.width, video.height, 1, 255); 

		IVC* coresjuntas = vc_image_new(video.width, video.height, 1, 255);
		// Guarda a memória da imagem, copia os dados de pixels da frame do vídeo para a imagem image_2, permitindo que ela contenha o conteúdo visual da frame atual do vídeo
		memcpy(image_2->data, frame.data, video.width* video.height * 3);


		//  Esta função converte a codificação de cores da imagem image_2 de BGR (azul-verde-vermelho) para RGB (vermelho-verde-azul).
		vc_convert_bgr_to_rgb(image_2, image_2); 


		// Copia os dados do quadro de vídeo (frame) para a estrutura de imagem (image_2)
		// O tamanho dos dados copiados é calculado multiplicando a largura, altura e 3 (bytes por pixel)
		memcpy(image_2->data, frame.data, video.width * video.height * 3);


		// Converte a codificação de cores BGR (Azul-Verde-Vermelho) para RGB (Vermelho-Verde-Azul)
		// Os dados da imagem são convertidos diretamente na estrutura image_2
		vc_convert_bgr_to_rgb(image_2, image_2);


		// Converte a codificação de cores RGB para HSV (Matiz-Saturação-Valor)
		// O resultado da conversão é armazenado na estrutura image_3
		vc_rgb_to_hsv(image_2, image_3);

// Região pra manipular a cor das resistências
#pragma region Cor das Resistências

		vc_hsv_segmentation(image_3, image_4, 25, 43, 32, 63, 44, 86);

		copy_image(image_4, coresjuntas);


#pragma endregion

		//vc_hsv_segmentation(image_3, red_segmented_image, 25, 43, 32, 63, 44, 86); 
		//vc_hsv_segmentation(image_3, brown_segmented_image, 25, 43, 32, 63, 44, 86); 
		//vc_hsv_segmentation(image_3, black_segmented_image, 25, 43, 32, 63, 44, 86);
		//vc_hsv_segmentation(image_3, orange_segmented_image, 25, 43, 32, 63, 44, 86);



// Região pra manipular a identificação da cor verde nas resistências
#pragma region Cor Verde
		vc_hsv_segmentation(image_3, green_segmented_image, 85, 105, 33, 53, 35, 57); 
		OVC* blobGreen = nullptr;
		int nblobGreen = 0;
		IVC* greenBlobImage = vc_image_new(video.width, video.height, 1, 255);
		blobGreen = vc_binary_blob_labelling(green_segmented_image, greenBlobImage, &nblobGreen);
		vc_binary_blob_info (greenBlobImage, blobGreen, nblobGreen);
		copy_image(green_segmented_image, coresjuntas);
#pragma endregion

// Região pra manipular a identificação da cor azul nas resistências
#pragma region Cor Azul
		vc_hsv_segmentation(image_3, blue_segmented_image, 110, 200, 14, 45, 32, 45);
		OVC* blobBlue = nullptr;
		int nblobBlue = 0;
		IVC* blueBlobImage = vc_image_new(video.width, video.height, 1, 255);
		blobBlue = vc_binary_blob_labelling(blue_segmented_image, blueBlobImage, &nblobBlue);
		vc_binary_blob_info(blueBlobImage, blobBlue, nblobBlue);
		copy_image(blue_segmented_image, coresjuntas);
#pragma endregion

// Região pra manipular a identificação da cor vermelha nas resistências
#pragma region Cor Vermelha
		// Segmentação HSV para a cor vermelha
		vc_hsv_segmentation(image_3, red_segmented_image, 0, 11, 45, 69, 55, 89);
		OVC* blobRed = nullptr;
		int nblobRed = 0;
		IVC* redBlobImage = vc_image_new(video.width, video.height, 1, 255);
		blobRed = vc_binary_blob_labelling(red_segmented_image, redBlobImage, & nblobRed);
		vc_binary_blob_info(redBlobImage, blobRed, nblobRed);
		copy_image(red_segmented_image, coresjuntas);
#pragma endregion

// Região pra manipular a identificação da cor preto nas resistências
#pragma region Cor Preto
		// Segmentação HSV para a cor preta
		vc_hsv_segmentation(image_3, black_segmented_image, 40, 210, 4, 38, 20, 35);
		OVC* blobBlack = nullptr;
		int nblobBlack = 0;
		IVC* blackBlobImage = vc_image_new(video.width, video.height, 1, 255);
		blobBlack = vc_binary_blob_labelling(black_segmented_image, blackBlobImage, &nblobBlack);
		vc_binary_blob_info(blackBlobImage, blobBlack, nblobBlack);
		copy_image(black_segmented_image, coresjuntas);
#pragma endregion

// Região pra manipular a identificação da cor laranja nas resistências
#pragma region Cor Laranja
		// Segmentação HSV para a cor laranja
		vc_hsv_segmentation(image_3, orange_segmented_image,6, 10, 65, 80, 70, 93);
		OVC* blobOrange = nullptr;
		int nblobOrange = 0;
		IVC* orangeBlobImage = vc_image_new(video.width, video.height, 1, 255);
		blobOrange = vc_binary_blob_labelling(orange_segmented_image, orangeBlobImage, &nblobOrange);
		vc_binary_blob_info(orangeBlobImage, blobOrange, nblobOrange);
		copy_image(orange_segmented_image, coresjuntas);
#pragma endregion

// Região pra manipular a identificação da cor castanha nas resistências
#pragma region Cor Castanho
		// Segmentação HSV para a cor castanho
		vc_hsv_segmentation(image_3, brown_segmented_image, 11, 26, 26, 46, 30, 50);
		OVC* blobBrown = nullptr;
		int nblobBrown = 0;
		IVC* brownBlobImage = vc_image_new(video.width, video.height, 1, 255);
		blobBrown = vc_binary_blob_labelling(brown_segmented_image, brownBlobImage, &nblobBrown);
		vc_binary_blob_info(brownBlobImage, blobBrown, nblobBrown);
		copy_image(brown_segmented_image, coresjuntas);
#pragma endregion

		//cv::morphologyEx(coresconjuntas, coresconjuntas, cv::MORPH_CLOSE, cv::Mat(), cv::Point(-1, -1), 2);
		IVC* coresjuntasD = vc_image_new(image->width, image->height, 1, 255);
		//vc_binary_dilate(coresjuntas, coresjuntasD, 7);
		vc_binary_open(coresjuntas, coresjuntasD, 1, 7);
		cv::Mat coresconjuntas(coresjuntasD->height, coresjuntasD->width, CV_8UC1, coresjuntasD->data);
		cv::imshow("Cores Juntas", coresconjuntas);

		// Etiquetagem de blobs na imagem fechada
		OVC* coresjuntas_blob = nullptr;
		int coresjuntas_nblob = 0;
		IVC* coresjuntas2 = vc_image_new(image->width, image->height, 1, 255);
		coresjuntas_blob = vc_binary_blob_labelling(coresjuntasD, coresjuntas2, &coresjuntas_nblob);
		vc_binary_blob_info(coresjuntas2, coresjuntas_blob, coresjuntas_nblob);

		for (int i = 0; i < coresjuntas_nblob; i++) {
			int cores[2];
			std::vector<std::pair<int, char>> posicao_cores;
			int helper = 0;
			// Verifica se o blob é significativo para evitar ruídos pequenos
			if (coresjuntas_blob[i].area > 4000 && coresjuntas_blob[i].width > 150 && coresjuntas_blob[i].height < 90) {
				cv::rectangle(frame, cv::Point(coresjuntas_blob[i].x, coresjuntas_blob[i].y), cv::Point(coresjuntas_blob[i].x + coresjuntas_blob[i].width, coresjuntas_blob[i].y + coresjuntas_blob[i].height), cv::Scalar(0, 0, 255), 2);
				cv::circle(frame, cv::Point(coresjuntas_blob[i].xc, coresjuntas_blob[i].yc), 5, cv::Scalar(0, 0, 255), -1);
				int area = coresjuntas_blob[i].area * (coresjuntas_blob[i].width / (double)coresjuntas2->width) * (coresjuntas_blob[i].height / (double)coresjuntas2->height);
				// Mostrar área ao lado da bounding box
				std::string area_text = "Area: " + std::to_string(area);
				//std::string altura_text = "Altura: " + std::to_string(coresjuntas_blob[i].height);
				cv::putText(frame, area_text, cv::Point(coresjuntas_blob[i].x + coresjuntas_blob[i].width + 5, coresjuntas_blob[i].y + 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
				//cv::putText(frame, altura_text, cv::Point(coresjuntas_blob[i].x + coresjuntas_blob[i].width + 5, coresjuntas_blob[i].y + 40), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);	

				if (coresjuntas_blob[i].yc > 399 && coresjuntas_blob[i].yc < 407)
				{
					contador++;
				}
				for (int j = 0; j < nblobGreen; j++)
				{
					if (blobGreen[j].xc >= coresjuntas_blob[i].x && blobGreen[j].xc <= (coresjuntas_blob[i].x + coresjuntas_blob[i].width)) {
						posicao_cores.push_back(std::make_pair(blobGreen[j].xc, 'g'));
						helper++;
						break;
					}
				}
				for (int j = 0; j < nblobBlue; j++)
				{
					if (blobBlue[j].xc >= coresjuntas_blob[i].x && blobBlue[j].xc <= (coresjuntas_blob[i].x + coresjuntas_blob[i].width)) {
						posicao_cores.push_back(std::make_pair(blobBlue[j].xc, 'b'));
						helper++;
						break;
					}
				}
				for (int j = 0; j < nblobRed; j++)
				{
					if (blobRed[j].xc >= coresjuntas_blob[i].x && blobRed[j].xc <= (coresjuntas_blob[i].x + coresjuntas_blob[i].width)) { 
						posicao_cores.push_back(std::make_pair(blobRed[j].xc, 'r')); 
						helper++; 
						break;
					}
				}
				std::sort(posicao_cores.begin(), posicao_cores.end());
				for (int j = 0; j < posicao_cores.size(); j++)
				{
					if (j != 2)
					{
						switch (posicao_cores[j].second) {
						case 'g':
							cores[j] = 5;
							break;
						case 'b':
							cores[j] = 6;
							break;
						case 'r':
							cores[j] = 7;
							break;
						default:
							break;
						}
					}
					else {
						switch (posicao_cores[j].second) {
						case 'g':
							cores[j] = 100000;
							break;
						case 'b':
							cores[j] = 1000000;
							break;
						case 'r':
							cores[j] = 100;
							break;
						default:
							break;
						}
					}
				}
				if (posicao_cores.size() != 0)
				{
					std::string val_str = std::to_string(cores[0]) + std::to_string(cores[1]);
					int value = std::stoi(val_str) * cores[2];
					std::string value_text = "Valor: " + std::to_string(value);
					cv::putText(frame, value_text, cv::Point(coresjuntas_blob[i].x + coresjuntas_blob[i].width + 5, coresjuntas_blob[i].y + 50), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
				}
			}
			
		}
		std::string contador_text = "Numero resistencias: " + std::to_string(contador);
		cv::putText(frame, contador_text, cv::Point(450, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 255), 2);



		// Exibe a frame
		cv::imshow("VC - VIDEO", frame);

		// Sai da aplicação, se o utilizador premir a tecla 'q'
		key = cv::waitKey(1);
	}

	/* Para o timer e exibe o tempo decorrido */
	vc_timer();

	/* Fecha a janela */
	cv::destroyWindow("VC - VIDEO");

	/* Fecha o ficheiro de vídeo */
	capture.release();

	return 0;
}
