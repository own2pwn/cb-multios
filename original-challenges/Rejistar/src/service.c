/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "service.h"
#include "cbstring.h"
#include "cbstdio.h"
#include "atox.h"
#include <libcgc.h>
#include "comms.h"
#include "memcpy.h"
#include "malloc.h"
#include "strtok.h"
#include "atoi.h"
#include "strncmp.h"
#include "certificate.h"
#include "token.h"
#include "util.h"

/**
* Add a service
* 
* @param services The list of services
* @param name The name of the new service to add
* @param callback The callback address of the service to add
*
* @return None
*/
void cgc_addService(cgc_Service** services, char* name, void (* callback)(int, char*, unsigned int*)) {
	cgc_size_t size;
	cgc_Service* newService;
	if(!(newService = cgc_malloc(sizeof(cgc_Service))))
		_terminate(ALLOCATE_ERROR);
	cgc_bzero((char *) newService, sizeof(cgc_Service));
	newService->callback = callback;
	size = cgc_strlen(name);
	if(!(newService->name = cgc_malloc(size+1)))
		_terminate(ALLOCATE_ERROR);
	cgc_bzero(newService->name, size+1);
	cgc_memcpy(newService->name, name, size);
	newService->next = *services;
	*services = newService;
}

/**
* Add all services
* 
* @param services The list of services
*
* @return None
*/
void cgc_initServices(cgc_Service** services) {

	cgc_addService(services, TOKEN_CMD, cgc_requestToken);
	cgc_addService(services, ENROLL_CMD, cgc_enroll);
	cgc_addService(services, REENROLL_CMD, cgc_reenroll);
	cgc_addService(services, CERTS_CMD, cgc_crls);
	cgc_addService(services, REVOKE_CERT_CMD, cgc_revokeCert);
	cgc_addService(services, REVOKE_TOKEN_CMD, cgc_revokeToken);
	cgc_addService(services, REFRESH_CMD, cgc_refreshToken);

}

/**
* Deallocate the memory used for the message
* 
* @param message The address of the message to deallocate
*
* @return None
*/
void cgc_freeMessage(cgc_Message *message) {
	cgc_free(message->credential);
	cgc_free(message->body);
	cgc_free(message);
}

/**
* Get the next message from requestor
* 
* @return The address of the message received
*/
cgc_Message* cgc_getMessage() {
	cgc_size_t bytes, size;
	cgc_Message *message;
	char *cmd_str, *auth_str, *id_str, *cred_str, *body_str;
	char buffer[1024];
	cgc_bzero(buffer,1024);

	if(!(message = cgc_malloc(sizeof(cgc_Message))))
		_terminate(ALLOCATE_ERROR);
	cgc_bzero((char *)message, sizeof(cgc_Message));
	if(!message) 
		_terminate(1);

	if(cgc_read_until_delim_or_n(STDIN, buffer, '!', sizeof(buffer), &bytes))
		_terminate(2);

	cmd_str = cgc_strtok(buffer, ",");
	if(!cmd_str)
		return NULL;

	size = cgc_strlen(cmd_str);

	if(size > MAX_CMD_SIZE)
		size = MAX_CMD_SIZE;

	if(!(message->command = cgc_malloc(size+1)))
		_terminate(ALLOCATE_ERROR);
	cgc_bzero(message->command, size+1);
	cgc_memcpy(message->command, cmd_str, size);

	auth_str = cgc_strtok(0, ",");
	if(!auth_str) 
		return NULL;

	size = cgc_strlen(auth_str);

	if(size > MAX_AUTH_TYPE_SIZE)
		size = MAX_AUTH_TYPE_SIZE;

	cgc_memcpy(message->auth, auth_str, size);

	id_str = cgc_strtok(0, ",");

	if(id_str)
		message->id = cgc_atoi(id_str);
	else
		return NULL;

	cred_str = cgc_strtok(0, ",");
	if(!cred_str)
		return NULL;

	size = cgc_strlen(cred_str);
	if(!(message->credential = cgc_malloc(size+1)))
		_terminate(ALLOCATE_ERROR);

	cgc_bzero(message->credential, size+1);
	cgc_memcpy(message->credential, cred_str, size);

	body_str = cgc_strtok(0, "!");
	if(body_str) {
		size = cgc_strlen(body_str);
#ifdef PATCHED_1
		if(!(message->body = cgc_malloc(size+1)))
			_terminate(ALLOCATE_ERROR);
#else
		if(!(message->body = cgc_malloc(size)))
			_terminate(ALLOCATE_ERROR);
#endif
		cgc_bzero(message->body, size+1);
		cgc_memcpy(message->body, body_str, size);		
	}

	return message;
}

/**
* Authorize the command for the requestor using the credential
* 
* @param command The command that is requested
* @param auth_type The type of authentication used
* @param credential The credential used to cgc_authenticate
*
* @return 1 if authenticated, 0 if not
*/
int cgc_authenticate(char* command, char* auth_type, char* credential) {
	if(!cgc_strncmp(auth_type, CERT_AUTH_TYPE, cgc_strlen(CERT_AUTH_TYPE))
		&& cgc_isCertCommand(command)) {
		cgc_Certificate *cert;
		cert = cgc_parseCertificate(credential);

		if(!cgc_checkCertUse(command, cert->use)) {
			return 0;
		}

		if(cgc_validateCert(cert, command, &expiration_date)) {
			return 1;
		}

		return 0;

	} else if(!cgc_strncmp(auth_type, TOKEN_AUTH_TYPE, cgc_strlen(TOKEN_AUTH_TYPE))) {
		cgc_Token *token;
		token = cgc_parseToken(credential);
		
		if(!cgc_checkTokenUse(command, token->use)) {
			return 0;
		}

		if(cgc_validateToken(token, &expiration_date)) {
			return 1;
		}

		return 0;

	} else if(!cgc_strncmp(auth_type, UP_AUTH_TYPE, cgc_strlen(UP_AUTH_TYPE))
		&& !cgc_strncmp(command, TOKEN_CMD, cgc_strlen(TOKEN_CMD))) {
		char *user, *pass;

		user = cgc_strtok(credential, "/");
		if(!cgc_strncmp(user, DEFAULT_USER, cgc_strlen(DEFAULT_USER))) {
			pass = cgc_strtok(0,"!");
			if(!cgc_strncmp(pass, DEFAULT_PASS, cgc_strlen(DEFAULT_PASS))) {
				return 1;
			}
		}

		return 0;
	}

	return 0;
}

/**
* cgc_Run the service requested
* 
* @param serviceList The list of services
* @param message The requesting message
*
* @return None
*/
void cgc_runService(cgc_Service* serviceList, cgc_Message *message) {
	cgc_Service* service;
	for(service = serviceList; service!=NULL; service=service->next) {
		if(!cgc_strncmp(message->command, service->name, cgc_strlen(service->name))) {
			if(cgc_authenticate(message->command, message->auth, message->credential)) {
				service->callback(message->id, message->body, &expiration_date);				
			}
			else {
				cgc_sendErrorResponse(RESPONSE_ERR_NO_AUTH);				
			}
			return;
		}

	}

	cgc_sendErrorResponse(RESPONSE_ERR_NO_SERVICE);
	return;

}

int main(void) {
	cgc_size_t bytes;
	cgc_size_t size=0;
	char* vuln_buf;
	char* out;
	char* filler_buf[510];
	int ret;
	cgc_Service* serviceList = NULL;
	cgc_Message* message;

	if((ret = cgc_random(private_key, sizeof(private_key), &bytes)))
		_terminate(1);

	cgc_initServices(&serviceList);

	while(1) {
		if(!(message = cgc_getMessage())) {
			cgc_sendErrorResponse(RESPONSE_ERR_NO_MESSAGE);
			return 0;
		}

		if(!cgc_strncmp(message->command, QUIT_CMD, cgc_strlen(QUIT_CMD))) {

			if((ret = transmit(STDOUT, "!", cgc_strlen("!"), &bytes))) 
				_terminate(1);
			return 0;
		}

		cgc_runService(serviceList, message);
		cgc_freeMessage(message);
	}

	return 0;
}