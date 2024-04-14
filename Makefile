all: procesoPrincipal

procesoPrincipal: procesoPrincipal.c procesoMemoria procesoDisco
	@gcc procesoPrincipal.c -o procesoPrincipal


procesoMemoria: procesoMemoria.c
	@gcc procesoMemoria.c -o procesoMemoria

procesoDisco: procesoDisco.c
	@gcc procesoDisco.c -o procesoDisco


clean: 
	@rm procesoDisco procesoMemoria procesoPrincipal