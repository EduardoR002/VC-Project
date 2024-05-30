#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "vc.h"

// Função para alocar memória para uma nova imagem.
IVC* vc_image_new(int width, int height, int channels, int levels)
{
	// Aloca memória para a estrutura de imagem.
	IVC* image = (IVC*)malloc(sizeof(IVC));

	// Verifica se a alocação de memória foi bem-sucedida.
	if (image == NULL)
		return NULL;

	// Verifica se o número de níveis de intensidade é válido.
	if ((levels <= 0) || (levels > 255))
		return NULL;

	// Define os parâmetros da imagem na estrutura alocada.
	image->width = width;
	image->height = height;
	image->channels = channels;
	image->levels = levels;
	image->bytesperline = image->width * image->channels;

	// Aloca memória para os dados da imagem.
	image->data = (unsigned char*)malloc(image->width * image->height * image->channels * sizeof(char));

	// Verifica se a alocação de memória para os dados da imagem foi bem-sucedida.
	if (image->data == NULL)
	{
		return vc_image_free(image); // Libera a memória alocada para a estrutura de imagem.
	}

	return image; // Retorna a estrutura de imagem alocada.
}

// Função para liberar memória de uma imagem.
IVC* vc_image_free(IVC* image)
{
	// Verifica se a imagem não é nula.
	if (image != NULL)
	{
		// Verifica se os dados da imagem não são nulos e libera a memória alocada para eles.
		if (image->data != NULL)
		{
			free(image->data);
			image->data = NULL;
		}

		// Libera a memória alocada para a estrutura de imagem.
		free(image);
		image = NULL;
	}

	return image; // Retorna a imagem (pode ser nula após a liberação de memória).
}

// Função para converter de formato de cor BGR para RGB.
int vc_convert_bgr_to_rgb(IVC* src, IVC* dst)
{
	unsigned char* data = (unsigned char*)src->data;
	unsigned char* datadst = (unsigned char*)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->width * src->channels;
	int channels = src->channels;
	int channels_dst = dst->channels;
	int x, y, i, j;
	long int pos;

	// Verifica se os parâmetros da imagem de entrada são válidos.
	if ((width <= 0) || (height <= 0) || (data == NULL)) return 0;
	if (channels != 3 || channels_dst != 3) return 0;

	// Loop pelos pixels da imagem.
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			// Calcula a posição do pixel na imagem de destino.
			pos = y * dst->bytesperline + x * dst->channels;

			// Calcula a posição do pixel na imagem de destino.
			pos = y * dst->bytesperline + x * dst->channels;
			int* aux = datadst[pos];
			datadst[pos] = data[pos + 2];
			//datadst[pos + 1] = data[pos + 1];
			datadst[pos + 2] = aux;
		}
	}

	return 1; // Retorna sucesso.
}

// Função para converter de formato de cor RGB para HSV.
int vc_rgb_to_hsv(IVC* src, IVC* dst) {
	// Declaração de variáveis locais
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;
	int width = src->width;
	int height = src->height;
	long int pos;
	float rf, gf, bf;
	float value;
	float max;
	float min;
	float sat;
	float hue;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst->height)) return 0;
	if ((src->channels != 3) || (dst->channels != 3)) return 0;

	// Loop pelos pixels da imagem de origem
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			// Cálculo da posição do pixel na imagem
			pos = y * bytesperline_src + x * channels_src;

			// Obtenção dos valores dos canais RGB do pixel
			rf = (float)src->data[pos];
			gf = (float)src->data[pos + 1];
			bf = (float)src->data[pos + 2];

			// Encontrar os valores máximo e mínimo entre os canais RGB
			if ((rf >= gf) && (rf >= bf))
				max = rf;
			else if ((gf >= rf) && (gf >= bf))
				max = gf;
			else
				max = bf;
			value = max;

			if ((rf <= gf) && (rf <= bf))
				min = rf;
			else if ((gf <= rf) && (gf <= bf))
				min = gf;
			else
				min = bf;

			// Cálculo dos valores HSV para o pixel
			if (max == 0) {
				sat = 0;
				hue = 0;
			}
			else {
				sat = (max - min) / value;

				if ((max == rf) && gf > bf)
					hue = 60.0f * (gf - bf) / (max - min);
				else if ((max == rf) && (bf >= gf))
					hue = 360.0f + 60.0f * (gf - bf) / (max - min);
				else if ((max == gf))
					hue = 120.0f + 60.0f * (bf - rf) / (max - min);
				else if ((max == bf))
					hue = 240.0f + 60.0f * (rf - gf) / (max - min);
				else if (max == min)
					hue = 0;
			}

			// Atribuição dos valores HSV no pixel correspondente na imagem de destino
			dst->data[pos] = (unsigned char)((hue / 360.0f) * 255.0f);
			dst->data[pos + 1] = (unsigned char)(sat * 255.0f);
			dst->data[pos + 2] = (unsigned char)value;
		}
	}
	return 1;
}

// Função para segmentação de imagem HSV.
int vc_hsv_segmentation(IVC* src, IVC* dst, int hmin, int hmax, int smin, int smax, int vmin, int vmax) {
	// Declaração de variáveis locais
	unsigned char* datasrc = (unsigned char*)src->data;
	unsigned char* datadst = (unsigned char*)dst->data;
	int bytesperline_src = src->width * src->channels;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_src = src->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	float h, s, v;
	long int pos_src, pos_dst;

	int x, y;

	// Verificação de erros
	if ((width <= 0) || (height <= 0) || (datasrc == NULL)) return 0;
	if (width != dst->width || height != dst->height) return 0;
	if (channels_src != 3 || dst->channels != 1) return 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			// Cálculo da posição do pixel na imagem
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;

			// Obtenção dos valores HSV do pixel
			h = ((float)datasrc[pos_src]) / 255.0f * 360.0f;
			s = ((float)datasrc[pos_src + 1]) / 255.0f * 100.0f;
			v = ((float)datasrc[pos_src + 2]) / 255.0f * 100.0f;

			// Verificação se o pixel satisfaz os critérios de segmentação HSV
			if (h >= hmin && h <= hmax && s >= smin && s <= smax && v >= vmin && v <= vmax) {
				datadst[pos_dst] = (unsigned char)255;
			}
			else {
				datadst[pos_dst] = (unsigned char)0;
			}
		}
	}
	return 1;
}

// Funçao que executa a operação de dilatação binária em uma imagem.
int vc_binary_dilate(IVC* src, IVC* dst, int kernel)
{
	// Ponteiros para os dados de entrada e saída
	unsigned char* datasrc = (unsigned char*)src->data;
	unsigned char* datadst = (unsigned char*)dst->data;
	// Dimensões da imagem de entrada
	int width = src->width;
	int height = src->height;
	// Informações sobre a estrutura da imagem
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	// Variáveis de iteração
	int x, y, kx, ky;
	// Offset usado para determinar a vizinhança do kernel
	int offset = (kernel - 1) / 2;
	// Variáveis auxiliares
	long int pos, posk;
	// Flag para verificar se foi encontrado um pixel branco na vizinhança
	int whiteFound;

	// Verificação de erros básica
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels))
		return 0;
	if (channels != 1) // Apenas imagens em tons de cinza são suportadas
		return 0;

	// Iteração sobre todos os pixels da imagem
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			// Posição do pixel atual na imagem
			pos = y * bytesperline + x * channels;

			// Se o pixel que está a ser analisado for preto (valor 0)
			if ((int)datasrc[pos] == 0)
			{
				// Iteração sobre a vizinhança definida pelo kernel
				for (ky = -offset, whiteFound = 0; ky <= offset && !whiteFound; ky++)
				{
					for (kx = -offset; kx <= offset && !whiteFound; kx++)
					{
						// Verifica se o vizinho está dentro dos limites da imagem
						if ((y + ky >= 0) && (y + ky < height) && (x + kx >= 0) && (x + kx < width))
						{
							// Posição do pixel vizinho na imagem
							posk = (y + ky) * bytesperline + (x + kx) * channels;

							// Se pelo menos um vizinho no kernel for branco
							if ((int)datasrc[posk] != 0)
								whiteFound = 1; // Indica que um pixel branco foi encontrado
						}
					}
				}
				// Adicionar no centro branco se um pixel branco foi encontrado na vizinhança
				if (whiteFound)
					datadst[pos] = (unsigned char)255; // Pixel central é marcado como branco
				else
					datadst[pos] = (unsigned char)0; // Pixel central permanece preto
			}
			else
				datadst[pos] = (unsigned char)255; // Se o pixel não for preto, é mantido branco
		}
	}

	return 1; // Operação de dilatação concluída com sucesso
}

// Função que executa a operação de erosão binária em uma imagem.
int vc_binary_erosion(IVC* src, IVC* dst, int kernel)
{
	// Ponteiros para os dados de entrada e saída
	unsigned char* datasrc = (unsigned char*)src->data;
	unsigned char* datadst = (unsigned char*)dst->data;
	// Dimensões da imagem de entrada
	int width = src->width;
	int height = src->height;
	// Informações sobre a estrutura da imagem
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	// Variáveis de iteração
	int x, y, kx, ky;
	// Offset usado para determinar a vizinhança do kernel
	int offset = (kernel - 1) / 2;
	// Variáveis auxiliares
	long int pos, posk;
	// Flag para verificar se foi encontrado um pixel preto na vizinhança
	int blackFound;

	// Verificação de erros básica
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels))
		return 0;
	if (channels != 1) // Apenas imagens em tons de cinza são suportadas
		return 0;

	// Iteração sobre todos os pixels da imagem
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			// Posição do pixel atual na imagem
			pos = y * bytesperline + x * channels;

			// Se o pixel que está sendo analisado for branco (valor 255)
			if ((int)datasrc[pos] != 0)
			{
				// Iteração sobre a vizinhança definida pelo kernel
				for (ky = -offset, blackFound = 0; ky <= offset && !blackFound; ky++)
				{
					for (kx = -offset; kx <= offset && !blackFound; kx++)
					{
						// Verifica se o vizinho está dentro dos limites da imagem
						if ((y + ky >= 0) && (y + ky < height) && (x + kx >= 0) && (x + kx < width))
						{
							// Posição do pixel vizinho na imagem
							posk = (y + ky) * bytesperline + (x + kx) * channels;

							// Se pelo menos um vizinho no kernel for preto
							if ((int)datasrc[posk] == 0)
								blackFound = 1; // Indica que um pixel preto foi encontrado
						}
					}
				}

				// Se pelo menos um pixel preto foi encontrado na vizinhança, marca o pixel central como preto (0)
				if (blackFound)
					datadst[pos] = (unsigned char)0;
				else // Caso contrário, mantém o pixel central como branco (255)
					datadst[pos] = (unsigned char)255;
			}
			else // Se o pixel for preto, mantém como preto (0)
				datadst[pos] = (unsigned char)0;
		}
	}

	return 1; // Operação de erosão binária concluída com sucesso
}

// Função que 1eecuta a operação de abertura binária em uma imagem, que consiste em uma erosão seguida de uma dilatação.
int vc_binary_open(IVC* src, IVC* dst, int kernelErosion, int kernelDilation)
{
	int ret = 1; // Variável para armazenar o resultado da operação, inicializada como 1 (verdadeiro)
	IVC* imageAux = vc_image_new(src->width, src->height, src->channels, src->levels); // Criação de uma imagem auxiliar

	// Realiza a operação de erosão binária na imagem de entrada e armazena o resultado na imagem auxiliar
	ret &= vc_binary_erosion(src, imageAux, kernelErosion);
	// Realiza a operação de dilatação binária na imagem resultante da erosão e armazena o resultado na imagem de saída
	ret &= vc_binary_dilate(imageAux, dst, kernelDilation);

	// Libera a memória alocada para a imagem auxiliar
	vc_image_free(imageAux);

	return ret; // Retorna o resultado da operação, 1 se for bem-sucedida e 0 se houver algum erro
}

// Função que executa a operação de fechamento binário em uma imagem, que consiste em uma dilatação seguida de uma erosão.
int vc_binary_close(IVC* src, IVC* dst, int kernelDilation, int kernelErosion)
{
	int ret = 1; // Variável para armazenar o resultado da operação, inicializada como 1 (verdadeiro)
	IVC* imageAux = vc_image_new(src->width, src->height, src->channels, src->levels); // Criação de uma imagem auxiliar

	// Realiza a operação de dilatação binária na imagem de entrada e armazena o resultado na imagem auxiliar
	ret &= vc_binary_dilate(src, imageAux, kernelDilation);
	// Realiza a operação de erosão binária na imagem resultante da dilatação e armazena o resultado na imagem de saída
	ret &= vc_binary_erosion(imageAux, dst, kernelErosion);

	// Libera a memória alocada para a imagem auxiliar
	vc_image_free(imageAux);

	return ret; // Retorna o resultado da operação, 1 se for bem-sucedida e 0 se houver algum erro
}

// Função que realiza a etiquetagem de blobs em uma imagem binária.
OVC* vc_binary_blob_labelling(IVC* src, IVC* dst, int* nlabels)
{
	unsigned char* datasrc = (unsigned char*)src->data;
	unsigned char* datadst = (unsigned char*)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, a, b;
	long int i, size;
	long int posX, posA, posB, posC, posD;
	int labeltable[256] = { 0 };
	int labelarea[256] = { 0 };
	int label = 1; // Etiqueta inicial.
	int num, tmplabel;
	OVC* blobs; // Apontador para array de blobs (objectos) que sera retornado desta funcao.

	// Verificacao de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels))
		return NULL;
	if (channels != 1)
		return NULL;

	// Copia dados da imagem bin�ria para imagem grayscale
	memcpy(datadst, datasrc, bytesperline * height);

	// Todos os pixeis de plano de fundo devem obrigat�riamente ter valor 0
	// Todos os pixeis de primeiro plano devem obrigat�riamente ter valor 255
	// Ser�o atribu�das etiquetas no intervalo [1,254]
	// Este algoritmo est� assim limitado a 254 labels
	for (i = 0, size = bytesperline * height; i < size; i++)
	{
		if (datadst[i] != 0)
			datadst[i] = 255;
	}

	// Limpa os rebordos da imagem bin�ria
	for (y = 0; y < height; y++)
	{
		datadst[y * bytesperline + 0 * channels] = 0;
		datadst[y * bytesperline + (width - 1) * channels] = 0;
	}
	for (x = 0; x < width; x++)
	{
		datadst[0 * bytesperline + x * channels] = 0;
		datadst[(height - 1) * bytesperline + x * channels] = 0;
	}

	// Efectua a etiquetagem
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			// Kernel:
			// A B C
			// D X

			posA = (y - 1) * bytesperline + (x - 1) * channels; // A
			posB = (y - 1) * bytesperline + x * channels;		// B
			posC = (y - 1) * bytesperline + (x + 1) * channels; // C
			posD = y * bytesperline + (x - 1) * channels;		// D
			posX = y * bytesperline + x * channels;				// X

			// Se o pixel foi marcado
			if (datadst[posX] != 0)
			{
				if ((datadst[posA] == 0) && (datadst[posB] == 0) && (datadst[posC] == 0) && (datadst[posD] == 0))
				{
					datadst[posX] = label;
					labeltable[label] = label;
					label++;
				}
				else
				{
					num = 255;

					// Se A est� marcado
					if (datadst[posA] != 0)
						num = labeltable[datadst[posA]];
					// Se B est� marcado, e � menor que a etiqueta "num"
					if ((datadst[posB] != 0) && (labeltable[datadst[posB]] < num))
						num = labeltable[datadst[posB]];
					// Se C est� marcado, e � menor que a etiqueta "num"
					if ((datadst[posC] != 0) && (labeltable[datadst[posC]] < num))
						num = labeltable[datadst[posC]];
					// Se D est� marcado, e � menor que a etiqueta "num"
					if ((datadst[posD] != 0) && (labeltable[datadst[posD]] < num))
						num = labeltable[datadst[posD]];

					// Atribui a etiqueta ao pixel
					datadst[posX] = num;
					labeltable[num] = num;

					// Actualiza a tabela de etiquetas
					if (datadst[posA] != 0)
					{
						if (labeltable[datadst[posA]] != num)
						{
							for (tmplabel = labeltable[datadst[posA]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posB] != 0)
					{
						if (labeltable[datadst[posB]] != num)
						{
							for (tmplabel = labeltable[datadst[posB]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posC] != 0)
					{
						if (labeltable[datadst[posC]] != num)
						{
							for (tmplabel = labeltable[datadst[posC]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posD] != 0)
					{
						if (labeltable[datadst[posD]] != num)
						{
							for (tmplabel = labeltable[datadst[posC]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
				}
			}
		}
	}

	// Volta a etiquetar a imagem
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			posX = y * bytesperline + x * channels; // X

			if (datadst[posX] != 0)
			{
				datadst[posX] = labeltable[datadst[posX]];
			}
		}
	}

	// printf("\nMax Label = %d\n", label);

	// Contagem do n�mero de blobs
	// Passo 1: Eliminar, da tabela, etiquetas repetidas
	for (a = 1; a < label - 1; a++)
	{
		for (b = a + 1; b < label; b++)
		{
			if (labeltable[a] == labeltable[b])
				labeltable[b] = 0;
		}
	}
	// Passo 2: Conta etiquetas e organiza a tabela de etiquetas, para que n�o hajam valores vazios (zero) entre etiquetas
	*nlabels = 0;
	for (a = 1; a < label; a++)
	{
		if (labeltable[a] != 0)
		{
			labeltable[*nlabels] = labeltable[a]; // Organiza tabela de etiquetas
			(*nlabels)++;						  // Conta etiquetas
		}
	}

	// Se n�o h� blobs
	if (*nlabels == 0)
		return NULL;

	// Cria lista de blobs (objectos) e preenche a etiqueta
	blobs = (OVC*)calloc((*nlabels), sizeof(OVC));
	if (blobs != NULL)
	{
		for (a = 0; a < (*nlabels); a++)
			blobs[a].label = labeltable[a];
	}
	else
		return NULL;

	return blobs;
}

// Função que calcula informações sobre os blobs presentes em uma imagem binária.
int vc_binary_blob_info(IVC* src, OVC* blobs, int nblobs)
{
	unsigned char* data = (unsigned char*)src->data; // Ponteiro para os dados da imagem
	int width = src->width; // Largura da imagem
	int height = src->height; // Altura da imagem
	int bytesperline = src->bytesperline; // Número de bytes por linha
	int channels = src->channels; // Número de canais de cor
	int x, y, i; // Variáveis de iteração
	long int pos; // Posição do pixel na matriz
	int xmin, ymin, xmax, ymax; // Coordenadas para bounding box
	long int sumx, sumy; // Soma das coordenadas para o centro de gravidade

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL))
		return 0; // Retorna 0 se a imagem estiver vazia ou as dimensões forem inválidas
	if (channels != 1)
		return 0; // Retorna 0 se a imagem não for em tons de cinza

	// Conta área de cada blob
	for (i = 0; i < nblobs; i++)
	{
		xmin = width - 1;
		ymin = height - 1;
		xmax = 0;
		ymax = 0;

		sumx = 0;
		sumy = 0;

		blobs[i].area = 0; // Inicializa a área do blob

		for (y = 1; y < height - 1; y++)
		{
			for (x = 1; x < width - 1; x++)
			{
				pos = y * bytesperline + x * channels; // Calcula a posição do pixel na matriz

				if (data[pos] == blobs[i].label) // Verifica se o pixel pertence ao blob atual
				{
					// Área
					blobs[i].area++; // Incrementa a área do blob

					// Centro de Gravidade
					sumx += x; // Soma das coordenadas x
					sumy += y; // Soma das coordenadas y

					// Bounding Box
					if (xmin > x)
						xmin = x;
					if (ymin > y)
						ymin = y;
					if (xmax < x)
						xmax = x;
					if (ymax < y)
						ymax = y;

					// Perímetro
					// Se pelo menos um dos quatro vizinhos não pertence ao mesmo label, então é um pixel de contorno
					if ((data[pos - 1] != blobs[i].label) || (data[pos + 1] != blobs[i].label) || (data[pos - bytesperline] != blobs[i].label) || (data[pos + bytesperline] != blobs[i].label))
					{
						blobs[i].perimeter++;
					}
				}
			}
		}

		// Bounding Box
		blobs[i].x = xmin;
		blobs[i].y = ymin;
		blobs[i].width = (xmax - xmin) + 1;
		blobs[i].height = (ymax - ymin) + 1;

		// Centro de Gravidade
		// blobs[i].xc = (xmax - xmin) / 2;
		// blobs[i].yc = (ymax - ymin) / 2;
		blobs[i].xc = sumx / MAX(blobs[i].area, 1); // Calcula o centro de gravidade x
		blobs[i].yc = sumy / MAX(blobs[i].area, 1); // Calcula o centro de gravidade y
	}

	return 1; // Retorna 1 para indicar sucesso
}