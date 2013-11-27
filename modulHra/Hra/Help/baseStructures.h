#pragma once

/** @file baseStructures.h
 * V tomto s�bore su definovan� z�kladn� strukt�ry na pr�cu s text�rami.
 * 
 */

typedef struct
{
	unsigned char r; /**< Hodnota cervenej farby v 1 pixeli */
	unsigned char g; /**< Hodnota zelenej farby v 1 pixeli */
	unsigned char b; /**< Hodnota modrej farby v 1 pixeli */
} Bod;

typedef struct
{
	Bod* data; /**< Predstavuje vsetky 3 zlozky RGB v jednej strukture */
	int x; /**< Umiestnenie pixelu na osi X */
	int y; /**< Umiestnenie pixelu na osi Y */
} Obraz;
