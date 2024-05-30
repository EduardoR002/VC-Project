#include <iostream>
#include <string>
#include <chrono>
#include <opencv2\opencv.hpp>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\videoio.hpp>

extern "C" {
#include "vc.h"
}

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

	// Declara uma matriz frame para armazenar cada frame do vídeo.
	cv::Mat frame;

	// Inicia um loop que continua até que a tecla 'q' seja pressionada.
	while (key != 'q') {

		// Lê o próximo frame do vídeo e o armazena na matriz frame.
		capture.read(frame);

		// Verifica se conseguiu ler o frame , se o frame estiver vazio, indica que o vídeo chegou ao seu final ou houve algum erro na leitura.
		if (frame.empty()) break;

		// Aplica um filtro de desfoque gaussiano ao frame. Isso suaviza a imagem, reduzindo o ruído e detalhes excessivos
		cv::GaussianBlur(frame, frame, cv::Size(5, 5), 0);


		/* Número da frame a processar */
		video.nframe = (int)capture.get(cv::CAP_PROP_POS_FRAMES);

		/* Exemplo de inserção texto na frame */
		str = std::string("RESOLUCAO: ").append(std::to_string(video.width)).append("x").append(std::to_string(video.height));
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


		// Cria uma nova imagem IVC
		IVC* image = vc_image_new(video.width, video.height, 3, 255);
		IVC* image_2 = vc_image_new(video.width, video.height, 3, 255);
		IVC* image_3 = vc_image_new(video.width, video.height, 3, 255);
		IVC* image_4 = vc_image_new(video.width, video.height, 1, 255);
		IVC* image_5 = vc_image_new(video.width, video.height, 1, 255);

		IVC* red_segmented_image = vc_image_new(video.width, video.height, 1, 255); 
		IVC* brown_segmented_image = vc_image_new(video.width, video.height, 1, 255); 
		IVC* black_segmented_image = vc_image_new(video.width, video.height, 1, 255); 
		IVC* orange_segmented_image = vc_image_new(video.width, video.height, 1, 255); 
		IVC* green_segmented_image = vc_image_new(video.width, video.height, 1, 255);  
		IVC* blue_segmented_image = vc_image_new(video.width, video.height, 1, 255); 

		// Guarda a memória da imagem
		memcpy(image_2->data, frame.data, video.width* video.height * 3);

		// Converte a codificação de cores BGR (video original) para RGB
		vc_convert_bgr_to_rgb(image_2, image_2); 

		// Converte a codificação de cores RGB para HSV
		vc_rgb_to_hsv(image_2, image_3); 

#pragma region Cor das Resistências

		vc_hsv_segmentation(image_3, image_4, 25, 43, 32, 63, 44, 86);

		// Abertura binária da imagem
		vc_binary_open(image_4, image_5, 3, 3);

		// Fechamento binário da imagem
		vc_binary_close(image_5, image_4, 3, 3);

		// Etiquetagem de blobs
		OVC* blob = nullptr;
		int nblob = 0;
		blob = vc_binary_blob_labelling(image_4, image_5, &nblob);

		vc_binary_blob_info(image_5, blob, nblob);

		for (int i = 0; i < nblob; i++) {
			cv::rectangle(frame, cv::Point(blob[i].x, blob[i].y), cv::Point(blob[i].x + blob[i].width, blob[i].y + blob[i].height), cv::Scalar(0, 0, 255), 2);
			cv::circle(frame, cv::Point(blob[i].xc, blob[i].yc), 5, cv::Scalar(0, 0, 255), -1);
		}

		/*IVC* mask = vc_image_new(video.width, video.height, 1, 255);
		
		// Copia os pixels da imagem fechada para a máscara
		for (int y = 0; y < video.height; y++) {
			for (int x = 0; x < video.width; x++) {
				int index = y * video.width + x;
				// Se o pixel na imagem fechada for ativo (255), define o pixel na máscara como 255 (branco), caso contrário, define como 0 (preto)
				if (image_4->data[index] == 255) {
					mask->data[index] = 255;
				}
				else {
					mask->data[index] = 0;
				}
			}
		}

		// Cria uma matriz OpenCV para exibir a máscara
		cv::Mat mask_mat(mask->height, mask->width, CV_8UC1, mask->data);

		// Mostra a máscara em uma janela separada
		cv::imshow("Mask", mask_mat);*/

#pragma endregion

		//vc_hsv_segmentation(image_3, red_segmented_image, 25, 43, 32, 63, 44, 86); 
		//vc_hsv_segmentation(image_3, brown_segmented_image, 25, 43, 32, 63, 44, 86); 
		//vc_hsv_segmentation(image_3, black_segmented_image, 25, 43, 32, 63, 44, 86);
		//vc_hsv_segmentation(image_3, orange_segmented_image, 25, 43, 32, 63, 44, 86);

#pragma region Cor Verde

		vc_hsv_segmentation(image_3, green_segmented_image, 85, 105, 33, 53, 35, 57);

		// Abertura binária da imagem segmentada
		IVC* green_opened_image = vc_image_new(video.width, video.height, 1, 255);
		vc_binary_open(green_segmented_image, green_opened_image, 3, 3);

		// Fechamento binário da imagem aberta
		IVC* green_closed_image = vc_image_new(video.width, video.height, 1, 255);
		vc_binary_close(green_opened_image, green_closed_image, 3, 3);

		// Etiquetagem de blobs na imagem fechada
		OVC* green_blob = nullptr;
		int green_nblob = 0; 
		green_blob = vc_binary_blob_labelling(green_closed_image, green_segmented_image, &green_nblob);

		IVC* green_mask = vc_image_new(video.width, video.height, 1, 255);

		// Copia os pixels da imagem segmentada para a máscara
		for (int y = 0; y < video.height; y++) {
			for (int x = 0; x < video.width; x++) {
				int index = y * video.width + x;
				// Se o pixel na imagem segmentada for azul (0), define o pixel na máscara como 255 (branco), caso contrário, define como 0 (preto)
				if (green_closed_image->data[index] == 0) {
					green_mask->data[index] = 0;
				}
				else {
					green_mask->data[index] = 255;
				}
			}
		}

		// Cria uma matriz OpenCV para exibir a máscara
		cv::Mat green_mask_mat(green_mask->height, green_mask->width, CV_8UC1, green_mask->data);

		// Mostra a máscara em uma janela separada
		cv::imshow("Green Mask", green_mask_mat);

#pragma endregion

#pragma region Cor Azul

		vc_hsv_segmentation(image_3, blue_segmented_image, 110, 200, 14, 45, 32, 45);

		// Abertura binária da imagem segmentada
		IVC* blue_opened_image = vc_image_new(video.width, video.height, 1, 255);
		vc_binary_open(blue_segmented_image, blue_opened_image, 3, 3);

		// Fechamento binário da imagem aberta
		IVC* blue_closed_image = vc_image_new(video.width, video.height, 1, 255);
		vc_binary_close(blue_opened_image, blue_closed_image, 3, 3);

		// Etiquetagem de blobs na imagem fechada
		OVC* blue_blob = nullptr;
		int blue_nblob = 0;
		blue_blob = vc_binary_blob_labelling(blue_closed_image, blue_segmented_image, &blue_nblob);

		IVC* blue_mask = vc_image_new(video.width, video.height, 1, 255);

		// Copia os pixels da imagem segmentada para a máscara
		for (int y = 0; y < video.height; y++) {
			for (int x = 0; x < video.width; x++) {
				int index = y * video.width + x;
				// Se o pixel na imagem segmentada for azul (0), define o pixel na máscara como 255 (branco), caso contrário, define como 0 (preto)
				if (blue_closed_image->data[index] == 0) {
					blue_mask->data[index] = 0;
				}
				else {
					blue_mask->data[index] = 255;
				}
			}
		}

		// Cria uma matriz OpenCV para exibir a máscara
		cv::Mat blue_mask_mat(blue_mask->height, blue_mask->width, CV_8UC1, blue_mask->data);

		// Mostra a máscara em uma janela separada
		cv::imshow("Blue Mask", blue_mask_mat);

#pragma endregion

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
