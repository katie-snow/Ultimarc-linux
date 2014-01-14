/*
 ============================================================================
 Name        : main.c
 Author      : Robert Abram, Katie Snow
 Version     :
 Copyright   : Copyright 2014 Robert Abram, Katie Snow
 Description : Ultimarc command line utility
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include <libs/ultimarc.h>

int main(void) {
	puts("Nothing to do yet.");
	foo();

	char *test = "{ \"admin\" : true, \"reviewer\" : false, \"author\" : true }";


	validate_json(test);


	return EXIT_SUCCESS;

}
