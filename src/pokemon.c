#include <stdlib.h>
#include "pokemon.h"
#include "tipo.h"
#include <stdio.h>
#include "ataque.h"
#include <string.h>
#include <stdbool.h>
#define MAX_ATAQUE 5
#define MAX_LINEA 100
typedef struct ataque ataque_t;

struct pokemon {
	char nombre[30];
	enum TIPO tipo;
	ataque_t ataques[MAX_ATAQUE];
	int cant_ataques;
};

struct info_pokemon {
	struct pokemon *pokemones;
	int cant_pokemones;
};

/*
 * Cuenta y devuelve la cantidad de delimitadores que tiene el string 
*/
int cantidad_delimitadores(const char *string)
{
	int contador = 0;
	size_t longitud = strlen(string);
	for (size_t i = 0; i < longitud; i++) {
		if (string[i] == ';') {
			(contador)++;
		}
	}

	return contador;
}

/*
 * Parsea el tipo devolviendo el enum correspondiente
*/
enum TIPO parsear_tipo(char tipo)
{
	enum TIPO nuevo_tipo = 0;
	if (tipo == 'N') {
		nuevo_tipo = NORMAL;
	} else if (tipo == 'F') {
		nuevo_tipo = FUEGO;
	} else if (tipo == 'A') {
		nuevo_tipo = AGUA;
	} else if (tipo == 'P') {
		nuevo_tipo = PLANTA;
	} else if (tipo == 'E') {
		nuevo_tipo = ELECTRICO;
	} else if (tipo == 'R') {
		nuevo_tipo = ROCA;
	}
	return nuevo_tipo;
}

/*
 * Valida tipo  	
*/
bool tipo_valido(char tipo)
{
	return tipo == 'N' || tipo == 'F' || tipo == 'A' || tipo == 'P' ||
	       tipo == 'E' || tipo == 'R';
}
/**
 * Ordena alfabeticamente a informacion_pokemon_t
**/
informacion_pokemon_t *ordenar_alfabeticamente(informacion_pokemon_t *ip)
{
	pokemon_t pokemon_aux;
	int min = 0;

	for (int i = 0; i < ip->cant_pokemones; i++) {
		min = i;
		for (int j = i + 1; j < ip->cant_pokemones; j++) {
			int comparacion = strcmp(ip->pokemones[j].nombre,
						 ip->pokemones[min].nombre);
			if (comparacion < 0) {
				min = j;
			}
		}
		pokemon_aux = ip->pokemones[i];
		ip->pokemones[i] = ip->pokemones[min];
		ip->pokemones[min] = pokemon_aux;
	}

	return ip;
}

/**
 * Carga el vector info->pokemones con los datos validos del pokemon_aux
**/
struct info_pokemon *cargar_poke_valido(struct pokemon *pokemon_aux,
					struct info_pokemon *info)
{
	info->pokemones =
		realloc(info->pokemones,
			sizeof(struct pokemon) *
				(unsigned long)(1 + info->cant_pokemones));
	if (info->pokemones == NULL) {
		free(pokemon_aux);
		free(info->pokemones);
	}
	strcpy(info->pokemones[info->cant_pokemones].nombre,
	       pokemon_aux->nombre);
	info->pokemones[info->cant_pokemones].tipo = pokemon_aux->tipo;
	info->pokemones[info->cant_pokemones] = *pokemon_aux;
	info->cant_pokemones++;

	return info;
}

informacion_pokemon_t *pokemon_cargar_archivo(const char *path)
{
	if (path == NULL) {
		return NULL;
	}

	FILE *archivo = fopen(path, "r");

	if (archivo == NULL) {
		return NULL;
	}

	struct info_pokemon *info = malloc(sizeof(struct info_pokemon));
	struct pokemon pokemon_aux;

	if (info == NULL) {
		fclose(archivo);
		return NULL;
	}

	info->pokemones = NULL;
	info->cant_pokemones = 0;
	pokemon_aux.cant_ataques = 0;
	char linea[MAX_LINEA];
	char nombre[30];
	char nombre_ataque[30];
	char tipo;
	char tipo_ataque;
	unsigned int poder;
	bool seguir_leyendo = true;

	while (fgets(linea, MAX_LINEA, archivo) != NULL &&
	       seguir_leyendo == true) {
		int contador_delimitadores = cantidad_delimitadores(linea);

		if (contador_delimitadores == 1) {
			if (pokemon_aux.cant_ataques != 0) {
				if (info->cant_pokemones == 0) {
					seguir_leyendo = false;
					free(info);
					fclose(archivo);
					return NULL;
				} else {
					seguir_leyendo = false;
					fclose(archivo);
					return info;
				}

			} else {
				int valores_validos = sscanf(
					linea, "%[^;];%c\n", nombre, &tipo);
				if (valores_validos == 2 && tipo_valido(tipo)) {
					strcpy(pokemon_aux.nombre, nombre);
					pokemon_aux.tipo = parsear_tipo(tipo);
				} else if ((valores_validos != 2 ||
					    !tipo_valido(tipo)) &&
					   info->cant_pokemones == 0) {
					seguir_leyendo = false;
					free(info);
					fclose(archivo);
					return NULL;
				} else {
					seguir_leyendo = false;
					if (info->cant_pokemones == 0) {
						free(info);
						fclose(archivo);
						return NULL;
					} else {
						fclose(archivo);
						return info;
					}
				}
			}

		} else if (contador_delimitadores == 2) {
			int ataques_valores_validos =
				sscanf(linea, "%[^;];%c;%u\n", nombre_ataque,
				       &tipo_ataque, &poder);
			if (ataques_valores_validos == 3 &&
			    tipo_valido(tipo_ataque)) {
				strcpy(pokemon_aux
					       .ataques[pokemon_aux.cant_ataques]
					       .nombre,
				       nombre_ataque);
				pokemon_aux.ataques[pokemon_aux.cant_ataques]
					.tipo = parsear_tipo(tipo_ataque);
				pokemon_aux.ataques[pokemon_aux.cant_ataques]
					.poder = poder;
				pokemon_aux.cant_ataques++;
				if (pokemon_aux.cant_ataques == 3) {
					info = cargar_poke_valido(&pokemon_aux,
								  info);
					pokemon_aux.cant_ataques = 0;
				}

			} else if ((ataques_valores_validos != 3 ||
				    !tipo_valido(tipo_ataque)) &&
				   info->cant_pokemones == 0) {
				seguir_leyendo = false;
				free(info->pokemones);
				free(info);
				fclose(archivo);
				return NULL;
			} else if (!tipo_valido(tipo_ataque) &&
				   info->cant_pokemones != 0) {
				seguir_leyendo = false;
				fclose(archivo);
				return info;
			}
		} else {
			free(info);
			fclose(archivo);
			return NULL;
		}
	}

	fclose(archivo);
	return info;
}

pokemon_t *pokemon_buscar(informacion_pokemon_t *ip, const char *nombre)
{
	if (ip == NULL || nombre == NULL) {
		return NULL;
	}
	informacion_pokemon_t *ip_ordenado = ordenar_alfabeticamente(ip);
	for (int i = 0; i < ip_ordenado->cant_pokemones; i++) {
		if (strcmp(ip_ordenado->pokemones[i].nombre, nombre) == 0) {
			return &ip_ordenado->pokemones[i];
		}
	}

	return NULL;
}

int pokemon_cantidad(informacion_pokemon_t *ip)
{
	if (ip == NULL) {
		return 0;
	}
	return ip->cant_pokemones;
}

const char *pokemon_nombre(pokemon_t *pokemon)
{
	if (pokemon == NULL) {
		return NULL;
	}

	return pokemon->nombre;
}

enum TIPO pokemon_tipo(pokemon_t *pokemon)
{
	if (pokemon == NULL) {
		return 0;
	}
	return pokemon->tipo;
}

const struct ataque *pokemon_buscar_ataque(pokemon_t *pokemon,
					   const char *nombre)
{
	if (pokemon == NULL || nombre == NULL) {
		return NULL;
	}

	for (int i = 0; i < pokemon->cant_ataques; i++) {
		if (strcmp(pokemon->ataques[i].nombre, nombre) == 0) {
			return &pokemon->ataques[i];
		}
	}

	return NULL;
}

int con_cada_pokemon(informacion_pokemon_t *ip, void (*f)(pokemon_t *, void *),
		     void *aux)
{
	if (ip == NULL || f == NULL) {
		return 0;
	}

	int contador = 0;
	informacion_pokemon_t *ip_ordenado = ordenar_alfabeticamente(ip);

	for (int i = 0; i < ip_ordenado->cant_pokemones; i++) {
		f(&ip_ordenado->pokemones[i], aux);
		contador++;
	}

	return contador;
}

int con_cada_ataque(pokemon_t *pokemon,
		    void (*f)(const struct ataque *, void *), void *aux)
{
	if (pokemon == NULL || f == NULL) {
		return 0;
	}

	int contador = 0;
	for (int i = 0; i < pokemon->cant_ataques; i++) {
		f(&pokemon->ataques[i], aux);
		contador++;
	}

	return contador;
}

void pokemon_destruir_todo(informacion_pokemon_t *ip)
{
	if (ip != NULL) {
		free(ip->pokemones);
		free(ip);
	}
}
