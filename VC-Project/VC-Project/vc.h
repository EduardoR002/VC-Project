#define VC_DEBUG
#ifndef VC_H
#define VC_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                                ESTRUTURA DE UMA IMAGEM                                     //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

// Definição da estrutura para representação de uma imagem.
typedef struct
{
	unsigned char* data;   // Ponteiro para os dados da imagem.
	int width, height;     // Largura e altura da imagem.
	int channels;          // Número de canais de cor (Binário/Cinza=1; RGB=3).
	int levels;            // Número de níveis de intensidade (Binário=1; Cinza [1,255]; RGB [1,255]).
	int bytesperline;      // Número de bytes por linha (largura * canais).
} IVC;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                                ESTRUTURA DE UM BLOB (OBJECTO)                              //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//


// Definição da estrutura para representação de um objeto (blob) numa imagem binária.
typedef struct
{
	int x, y, width, height; // Coordenadas e dimensões da caixa delimitadora (Bounding Box).
	int area;                 // Área do objeto.
	int xc, yc;               // Coordenadas do centro de massa.
	int perimeter;            // Perímetro do objeto.
	int label;                // Etiqueta do objeto.
} OVC;



// Macro para retornar o máximo entre dois valores.
#define MAX(a, b) ((a) > (b) ? a : b)


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                               PROTÓTIPOS DE FUNÇÕES                                        //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//


// > FUNÇÃO: Criar uma nova imagem
// > DESCRIÇÃO: Aloca memória para uma nova estrutura de imagem com as dimensões especificadas.
// > PARÂMETROS:
//		- width: largura da imagem.
//		- height: altura da imagem.
//		- channels: número de canais de cor da imagem (ex: 1 para tons de cinza, 3 para RGB).
//		- levels: número máximo de níveis de intensidade dos pixels (ex: 256 para 8 bits por canal).
// > RETORNO: Ponteiro para a nova estrutura de imagem alocada.
IVC* vc_image_new(int width, int height, int channels, int levels);


// > FUNÇÃO: Libertar memória de uma imagem
// > DESCRIÇÃO: Liberta a memória previamente alocada para uma estrutura de imagem.
// > PARÂMETROS:
//		- image: ponteiro para a estrutura de imagem a ser liberada.
// > RETORNO: Nenhum.
IVC* vc_image_free(IVC* image);

// > FUNÇÃO: Converter de formato de cor BGR para RGB
// > DESCRIÇÃO: Converte uma imagem de formato BGR (Blue, Green, Red) para o formato RGB (Red, Green, Blue).
// > PARÂMETROS:
//		- src: imagem de entrada no formato BGR.
//		- dst: imagem de saída no formato RGB.
// > RETORNO: 0 se a conversão for bem-sucedida, -1 caso contrário.
int vc_convert_bgr_to_rgb(IVC* src, IVC* dst);

// > FUNÇÃO: Converter de formato de cor RGB para HSV
// > DESCRIÇÃO: Converte uma imagem de formato RGB (Red, Green, Blue) para o formato HSV (Hue, Saturation, Value).
// > PARÂMETROS:
//		- src: imagem de entrada no formato RGB.
//		- dst: imagem de saída no formato HSV.
// > RETORNO: 0 se a conversão for bem-sucedida, -1 caso contrário.
int vc_rgb_to_hsv(IVC* src, IVC* dst);

// > FUNÇÃO: Segmentação de imagem HSV
// > DESCRIÇÃO: Segmenta uma imagem HSV com base nos intervalos especificados para os canais H, S e V.
// > PARÂMETROS:
//		- src: imagem de entrada no formato HSV.
//		- dst: imagem de saída, onde os pixels dentro dos intervalos são mantidos e os outros são eliminados.
//		- hmin, hmax: intervalo para o canal H (matiz).
//		- smin, smax: intervalo para o canal S (saturação).
//		- vmin, vmax: intervalo para o canal V (valor).
// > RETORNO: 0 se a segmentação for bem-sucedida, -1 caso contrário.
int vc_hsv_segmentation(IVC* src, IVC* dst, int hmin, int hmax, int smin, int smax, int vmin, int vmax);

// > FUNÇÃO: Dilatação binária de uma imagem
// > DESCRIÇÃO: Aplica a operação de dilatação binária a uma imagem binária.
// > PARÂMETROS:
//		- src: imagem binária de entrada.
//		- dst: imagem binária de saída após a dilatação.
//		- kernel: tamanho do elemento estruturante (kernel) para a dilatação.
// > RETORNO: 0 se a dilatação for bem-sucedida, -1 caso contrário.
int vc_binary_dilate(IVC* src, IVC* dst, int kernel);

// FUNÇÃO: Erosão binária de uma imagem
// DESCRIÇÃO: Aplica a operação de erosão binária a uma imagem binária.
// PARÂMETROS:
//		- src: imagem binária de entrada.
//		- dst: imagem binária de saída após a erosão.
//		- kernel: tamanho do elemento estruturante (kernel) para a erosão.
// RETORNO: 0 se a erosão for bem-sucedida, -1 caso contrário.
int vc_binary_erosion(IVC* src, IVC* dst, int kernel);


// FUNÇÃO: Abertura binária de uma imagem
// DESCRIÇÃO: Aplica a operação de abertura binária (erosão seguida de dilatação) a uma imagem binária.
// PARÂMETROS:
//		- src: imagem binária de entrada.
//		- dst: imagem binária de saída após a abertura.
//		- kernelErosion: tamanho do elemento estruturante (kernel) para a erosão na abertura.
//		- kernelDilation: tamanho do elemento estruturante (kernel) para a dilatação na abertura.
// RETORNO: 0 se a abertura for bem-sucedida, -1 caso contrário.
int vc_binary_open(IVC* src, IVC* dst, int kernelErosion, int kernelDilation);


// FUNÇÃO: Fechamento binário de uma imagem
// DESCRIÇÃO: Aplica a operação de fechamento binário (dilatação seguida de erosão) a uma imagem binária.
// PARÂMETROS:
//		- src: imagem binária de entrada.
//		- dst: imagem binária de saída após o fechamento.
//		- kernelDilation: tamanho do elemento estruturante (kernel) para a dilatação no fechamento.
//		- kernelErosion: tamanho do elemento estruturante (kernel) para a erosão no fechamento.
// RETORNO: 0 se o fechamento for bem-sucedido, -1 caso contrário.
int vc_binary_close(IVC* src, IVC* dst, int kernelDilation, int kernelErosion);


// FUNÇÃO: Rotulagem de blobs em uma imagem binária
// DESCRIÇÃO: Rotula os blobs (regiões conectadas de pixels) em uma imagem binária.
// PARÂMETROS:
//		- src: imagem binária de entrada contendo os blobs.
//		- dst: imagem de saída, onde cada blob é representado por uma cor única.
//		- nlabels: ponteiro para armazenar o número total de blobs encontrados.
// RETORNO: Ponteiro para a estrutura de blobs rotulados.
OVC* vc_binary_blob_labelling(IVC* src, IVC* dst, int* nlabels);


// FUNÇÃO: Obter informações sobre blobs rotulados em uma imagem binária
// DESCRIÇÃO: Calcula informações estatísticas sobre os blobs rotulados em uma imagem binária.
// PARÂMETROS:
//		- src: imagem binária de entrada contendo os blobs rotulados.
//		- blobs: estrutura de blobs rotulados.
//		 - nblobs: número total de blobs rotulados.
// RETORNO: 0 se as informações dos blobs forem obtidas com sucesso, -1 caso contrário.
int vc_binary_blob_info(IVC* src, OVC* blobs, int nblobs);

int copy_image(IVC* src, IVC* dst);

int vc_write_image(char* filename, IVC* image);


long int unsigned_char_to_bit(unsigned char datauchar, unsigned chardatabit, int width, int height);


#endif /* VC_H */