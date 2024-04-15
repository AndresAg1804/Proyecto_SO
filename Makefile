all: procesoPrincipal

procesoPrincipal: procesoPrincipal.c procesoMemoria procesoDisco procesoCPU
	@gcc procesoPrincipal.c -o procesoPrincipal


procesoMemoria: procesoMemoria.c
	@gcc procesoMemoria.c -o procesoMemoria

procesoDisco: procesoDisco.c
	@gcc procesoDisco.c -o procesoDisco

procesoCPU: procesoCPU.c
	@gcc procesoCPU.c -o procesoCPU

clean: 
	@rm procesoDisco procesoMemoria procesoPrincipal