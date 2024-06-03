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

bool compare(const int(&a)[2], const int(&b)[2]) {
	return a[0] < b[0];
}


// Função vc_timer que mede e exibe o tempo decorrido entre chamadas consecutivas.
void vc_timer(void) {

	// A variável "running" é um booleano que indica se o temporizador está em execução.
	static bool running = false;
	// A variável "previousTime" armazena o tempo do ponto anterior no tempo(quando a fun��o foi chamada pela última vez).
	static std::chrono::steady_clock::time_point previousTime = std::chrono::steady_clock::now();

	// Se running é false, significa que a função está sendo chamada pela primeira vez. A vari�vel running é então definida como true para indicar que o temporizador está agora em execução.
	if (!running) {
		running = true;
	}
	else {
		//Tempo Atual: currentTime é capturado usando std::chrono::steady_clock::now()
		std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
		//Tempo Decorrido: elapsedTime é a diferen�a entre currentTime e previousTime
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

	// Variável inicializada com o nome do video, este array é utilizado para armazenar o caminho/nome do arquivo de video que será processado
	char videofile[20] = "video_resistors.mp4";

	// Objeto para captura de video, este objeto será utilizado para abrir, ler e manipular o video especificado.
	cv::VideoCapture capture;

	// Estrutura para armazenar informações do video
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



	/* Leitura de video de um ficheiro */
	/* NOTA IMPORTANTE: O ficheiro video.avi deverá estar localizado no mesmo directório que o ficheiro de código fonte. */

	// Tenta abrir o arquivo de video, e caso não consiga apresenta uma mensagem ao utilizador a informar o erro
	if (!capture.open(videofile)) {
		std::cerr << "Erro ao abrir o ficheiro de v�deo!" << std::endl;
		return -1;
	}

	/* Em alternativa, abrir captura de v�deo pela Webcam #0 */
	//capture.open(0, cv::CAP_DSHOW); // Pode-se utilizar apenas capture.open(0);


	// Obtém o número total de frames  no video 
	video.ntotalframes = (int)capture.get(cv::CAP_PROP_FRAME_COUNT);

	// Obtém a taxa de frames por segundo (fps) do video
	video.fps = (int)capture.get(cv::CAP_PROP_FPS);

	// Obtém a largura do v�deo em pixels
	video.width = (int)capture.get(cv::CAP_PROP_FRAME_WIDTH);

	// Obtém a altura do v�deo em pixels
	video.height = (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT);

	// Cria uma janela para exibir o video 
	cv::namedWindow("VC - VIDEO", cv::WINDOW_AUTOSIZE);

	// Inicia um temporizador para medir o tempo decorrido entre chamadas consecutivas, útil para monitorar o desempenho do processamento de video
	vc_timer();
	//capture.set(cv::CAP_PROP_POS_FRAMES, 500);

	// Declara uma matriz frame para armazenar cada frame do video.
	cv::Mat frame;

	int contador = 0;

	int ultimoy = 0;

	std::string value_text;

	// Inicia um loop que continua até que a tecla 'q' seja pressionada.
	while (key != 'q') {

		// Lê o próximo frame do video e o armazena na matriz frame.
		capture.read(frame);

		// Verifica se conseguiu ler o frame , se o frame estiver vazio, indica que o video chegou ao seu final ou houve algum erro na leitura.
		if (frame.empty()) break;

		// Aplica um filtro de desfoque gaussiano ao frame. Isso suaviza a imagem, reduzindo o ruido e detalhes excessivos
		//cv::GaussianBlur(frame, frame, cv::Size(5, 5), 0);


		// Obtém o indice da frame atual no video utilizando o método get() do objeto capture. Ele retorna a posição atual do video em termos de número de frames
		video.nframe = (int)capture.get(cv::CAP_PROP_POS_FRAMES);


		//Exemplo de inserção texto na frame, cria uma string com o texto "RESOLUCAO: ", converte a largura e altura do video em pixels para uma string
		str = std::string("RESOLUCAO: ").append(std::to_string(video.width)).append("x").append(std::to_string(video.height));

		//  Adiciona texto a frame do video, e recebe como parametros, o frame onde vai ser inserido, a string a inserir, as coordenadas deste, tipo e tamanho da fonte, cor e espessura da linha do texto
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
		/* Mesmo processo de adi��o de texto até aqui */


		/*Código abaixo cria várias imagens IVC para armazenar diferentes tipos de segmentações de imagem. Cada imagem IVC é uma estrutura de dados que contém informações sobre a imagem, como largura,
		altura, número de canais de cor e o próprio buffer de pixels. */
		
		// Cria uma imagem colorida com 3 canais de cor (RGB) e alocada com 255 bytes para cada canal, ou seja, 24 bits por pixel (8 bits por canal). Esta imagem é destinada a armazenar frames de v�deo.
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

		// Cria uma imagem em escala de cinza para armazenar a segmentação de todas as cores
		IVC* coresjuntas = vc_image_new(video.width, video.height, 1, 255);

		// Guarda a memória da imagem, copia os dados de pixels da frame do video para a imagem image_2, permitindo que ela contenha o conteúdo visual da frame atual do video
		memcpy(image_2->data, frame.data, video.width* video.height * 3);


		//  Esta função converte a codificação de cores da imagem image_2 de BGR (azul-verde-vermelho) para RGB (vermelho-verde-azul).
		vc_convert_bgr_to_rgb(image_2, image_2); 


		// Converte a codificação de cores RGB para HSV (Matiz-Saturaçãoo-Valor)
		// O resultado da conversação é armazenado na estrutura image_3
		vc_rgb_to_hsv(image_2, image_3);

// Região pra manipular a cor das resistências
#pragma region Cor das Resistências

		// Segmentação da imagem com abse nas cores da resitência
		vc_hsv_segmentation(image_3, image_4, 25, 43, 32, 63, 44, 86);

		// copia ara o coresjuntas
		copy_image(image_4, coresjuntas);


#pragma endregion


// Região pra manipular a identificação da cor verde nas resistências
#pragma region Cor Verde
		vc_hsv_segmentation(image_3, green_segmented_image, 85, 105, 33, 53, 35, 57); 
		OVC* blobGreen = nullptr;
		int nblobGreen = 0;
		IVC* greenBlobImage = vc_image_new(video.width, video.height, 1, 255);

		// Verifica os blobs na imagem desta cor e faz a etiquetagem dos blobs
		blobGreen = vc_binary_blob_labelling(green_segmented_image, greenBlobImage, &nblobGreen); 

		// Obter informações sobre os blobs rotulados
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
		vc_hsv_segmentation(image_3, orange_segmented_image,12, 16, 65, 80, 70, 93);
		OVC* blobOrange = nullptr;
		int nblobOrange = 0;
		IVC* orangeBlobImage = vc_image_new(video.width, video.height, 1, 255);
		blobOrange = vc_binary_blob_labelling(orange_segmented_image, orangeBlobImage, &nblobOrange);
		vc_binary_blob_info(orangeBlobImage, blobOrange, nblobOrange);
		copy_image(orange_segmented_image, coresjuntas);
#pragma endregion

// Regi�o pra manipular a identificação da cor castanha nas resistências
#pragma region Cor Castanho
		// Segmentaçãoo HSV para a cor castanho
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

		//cv::Mat coresconjuntas(coresjuntasD->height, coresjuntasD->width, CV_8UC1, coresjuntasD->data);
		//cv::imshow("Cores Juntas", coresconjuntas);

		// Etiquetagem de blobs na imagem fechada
		OVC* coresjuntas_blob = nullptr;
		int coresjuntas_nblob = 0;
		IVC* coresjuntas2 = vc_image_new(image->width, image->height, 1, 255);

		coresjuntas_blob = vc_binary_blob_labelling(coresjuntasD, coresjuntas2, &coresjuntas_nblob);
		vc_binary_blob_info(coresjuntas2, coresjuntas_blob, coresjuntas_nblob);
		

		// percorre o número de blobs encontrados no video todo
		for (int i = 0; i < coresjuntas_nblob; i++) {
			int positiony = 0;
			int cores[2];
			std::vector<std::tuple<int, int, char>> posicao_cores;
			int helper = 0;

			// Verifica se o blob é significativo para evitar ruídos pequenos
			if (coresjuntas_blob[i].area > 4000 && coresjuntas_blob[i].width > 150 && coresjuntas_blob[i].height < 90 && coresjuntas_blob[i].height > 40) {
				cv::rectangle(frame, cv::Point(coresjuntas_blob[i].x, coresjuntas_blob[i].y), cv::Point(coresjuntas_blob[i].x + coresjuntas_blob[i].width, coresjuntas_blob[i].y + coresjuntas_blob[i].height), cv::Scalar(0, 0, 255), 2);
				cv::circle(frame, cv::Point(coresjuntas_blob[i].xc, coresjuntas_blob[i].yc), 5, cv::Scalar(0, 0, 255), -1);
				int area = coresjuntas_blob[i].area * (coresjuntas_blob[i].width / (double)coresjuntas2->width) * (coresjuntas_blob[i].height / (double)coresjuntas2->height);
				// Mostrar �rea ao lado da bounding box
				std::string area_text = "Area: " + std::to_string(area);
				std::string altura_text = "Altura: " + std::to_string(coresjuntas_blob[i].height);
				std::cout << altura_text << std::endl;
				cv::putText(frame, area_text, cv::Point(coresjuntas_blob[i].x + coresjuntas_blob[i].width + 5, coresjuntas_blob[i].y + 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
				//cv::putText(frame, altura_text, cv::Point(coresjuntas_blob[i].x + coresjuntas_blob[i].width + 5, coresjuntas_blob[i].y + 40), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);	

				// Ver quantas resistencias existe
				if (coresjuntas_blob[i].yc > 399 && coresjuntas_blob[i].yc < 407)
				{
					contador++;
				}
				for (int j = 0; j < nblobOrange; j++)
				{
					if (blobOrange[j].xc >= coresjuntas_blob[i].x && blobOrange[j].xc <= (coresjuntas_blob[i].x + coresjuntas_blob[i].width)) {
						posicao_cores.emplace_back(blobOrange[j].xc, blobOrange[j].yc, 'o');
						positiony = blobOrange[j].yc;
						helper++;
						break;
					}
				}
				for (int j = 0; j < nblobGreen; j++)
				{
					if (blobGreen[j].xc >= coresjuntas_blob[i].x && blobGreen[j].xc <= (coresjuntas_blob[i].x + coresjuntas_blob[i].width)) {
						posicao_cores.emplace_back(blobGreen[j].xc, blobGreen[j].yc, 'g');
						positiony = blobGreen[j].yc;
						helper++;
						break;
					}
				}
				for (int j = 0; j < nblobRed; j++)
				{
					if (blobRed[j].xc >= coresjuntas_blob[i].x && blobRed[j].xc <= (coresjuntas_blob[i].x + coresjuntas_blob[i].width)) { 
						posicao_cores.emplace_back(blobRed[j].xc, blobRed[j].yc, 'r');
						positiony = blobRed[j].yc;
						helper++; 
						break;
					}
				}
				for (int j = 0; j < nblobBrown; j++)
				{
					if (blobBrown[j].xc >= coresjuntas_blob[i].x && blobBrown[j].xc <= (coresjuntas_blob[i].x + coresjuntas_blob[i].width)) {
						posicao_cores.emplace_back(blobBrown[j].xc, blobBrown[j].yc, 'br');
						positiony = blobBrown[j].yc;
						helper++;
						break;
					}
				}
				for (int j = 0; j < nblobBlack; j++)
				{
					if (blobBlack[j].xc >= coresjuntas_blob[i].x && blobBlack[j].xc <= (coresjuntas_blob[i].x + coresjuntas_blob[i].width)) {
						posicao_cores.emplace_back(blobBlack[j].xc, blobBlack[j].yc,'bl');
						positiony = blobBlack[j].yc;
						helper++;
						break;
					}
				}
				for (int j = 0; j < nblobBlue; j++)
				{
					if (blobBlue[j].xc >= coresjuntas_blob[i].x && blobBlue[j].xc <= (coresjuntas_blob[i].x + coresjuntas_blob[i].width)) {
						posicao_cores.emplace_back(blobBlue[j].xc, blobBlue[j].yc,'b');
						positiony = blobBlue[j].yc;
						helper++;
						break;
					}
				}
				// ordena o vetor para sabermos a posição da cor
				std::sort(posicao_cores.begin(), posicao_cores.end(), [](const std::tuple<int, int, char>& a, const std::tuple<int, int, char>& b) {
					return std::get<0>(a) < std::get<0>(b);
					});

				// vetor tamanho 3
				for (int j = 0; j < posicao_cores.size(); j++)
				{
					if (j != 2)
					{
						switch (std::get<2>(posicao_cores[j])) {
						case 'g':
							cores[j] = 5;
							break;
						case 'b':
							cores[j] = 6;
							break;
						case 'r':
							cores[j] = 7;
							break;
						case 'br':
							cores[j] = 1;
							break;
						case 'bl':
							cores[j] = 0;
							break;
						case 'o':
							cores[j] = 3;
							break;
						default:
							break;
						}
					}
					else {
						switch (std::get<2>(posicao_cores[j])) {
						case 'g':
							cores[j] = 100000;
							break;
						case 'b':
							cores[j] = 1000000;
							break;
						case 'r':
							cores[j] = 100;
							break;
						case 'br':
							cores[j] = 10;
							break;
						case 'bl':
							cores[j] = 1;
							break;
						case 'o':
							cores[j] = 1000;
						default:
							break;
						}
					}
				}
				if (posicao_cores.size() != 0)
				{
					if (ultimoy == 0)
					{
						ultimoy = positiony;
						std::string val_str = std::to_string(cores[0]) + std::to_string(cores[1]);
						int value = std::stoi(val_str) * cores[2];
						value_text = "Valor: " + std::to_string(value);	
					}
					else if(abs(ultimoy - positiony) > 500)
					{
						ultimoy = positiony;
						std::string val_str = std::to_string(cores[0]) + std::to_string(cores[1]);
						int value = std::stoi(val_str) * cores[2];
						value_text = "Valor: " + std::to_string(value);
					}
				}
				cv::putText(frame, value_text, cv::Point(coresjuntas_blob[i].x + coresjuntas_blob[i].width + 5, coresjuntas_blob[i].y + 50), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
			}
			
		}

		// Apresenta o value no video
		std::string contador_text = "Numero resistencias: " + std::to_string(contador);
		cv::putText(frame, contador_text, cv::Point(450, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 255), 2);



		// Exibe a frame
		cv::imshow("VC - VIDEO", frame);

		// Sai da aplicação, se o utilizador premir a tecla 'q'
		key = cv::waitKey(1);


		// Imagens libertadas da memória
		vc_image_free(image);
		vc_image_free(image_2);
		vc_image_free(image_3);
		vc_image_free(image_4);
		vc_image_free(image_5);
		vc_image_free(red_segmented_image);
		vc_image_free(black_segmented_image);
		vc_image_free(brown_segmented_image);
		vc_image_free(green_segmented_image);
		vc_image_free(orange_segmented_image);
		vc_image_free(blue_segmented_image);
		vc_image_free(coresjuntas);
		vc_image_free(coresjuntasD);
		vc_image_free(greenBlobImage);
		vc_image_free(blueBlobImage);
		vc_image_free(redBlobImage);
		vc_image_free(brownBlobImage);
		vc_image_free(orangeBlobImage);
		vc_image_free(blackBlobImage);
		vc_image_free(coresjuntas2);

	}

	/* Exibe o tempo decorrido */
	vc_timer();

	/* Fecha a janela */
	cv::destroyWindow("VC - VIDEO");

	/* Fecha o ficheiro de video */
	capture.release();

	return 0;
}
