/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
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
#include <libcgc.h>
#include "libc.h"
#include "service.h"

void cgc_identityMap(cgc_Message* message)
{

}

void cgc_constantMap(cgc_Message* message)
{
		message->value[PERM_FIELD] = 1;

}

void cgc_absoluteValueMap(cgc_Message* message)
{
	// Vuln: Use unsigned int for absolute value conversion
	unsigned short absoluteVal;
	short mask;
	short i;

#ifdef PATCHED
	for(i = LENGTH_FIELD; i < NUM_VALS; i++) {
#else
	for(i = LENGTH_FIELD; i < NUM_VALS-2; i++) {
#endif

#ifdef PATCHED
		if(SHRT_MIN == message->value[i]) {
			message->value[i] = SHRT_MAX;
		} else {
#else
#endif
			mask = message->value[i] >> (sizeof(short) * CHAR_BIT - 1);
			absoluteVal = (message->value[i] + mask) ^ mask;
			message->value[i] = absoluteVal;
#ifdef PATCHED	
		}
#else
#endif
	}

}

int cgc_modulus(short n, short M) {
	short result;
	if (M == 0)
		return n;
	//result = ((n % M) + M) % M;
	result = n % M;
	return result;
}

void cgc_modulusCoordinatesWithDimensions(cgc_Message* message)
{

	message->value[X_FIELD] = cgc_modulus(message->value[X_FIELD], message->value[LENGTH_FIELD]);
	message->value[Y_FIELD] = cgc_modulus(message->value[Y_FIELD], message->value[WIDTH_FIELD]);

}

void cgc_processMessage(cgc_Worker* worker)
{
	cgc_Message* message;
	message = worker->inbox;

	if(message != NULL) {
		worker->cgc_processMessage(message);
		worker->outbox = message;
		worker->inbox = NULL;
	}
}


void cgc_receiveMessage(cgc_Message* message) {
	cgc_size_t bytes_read = 0;
	cgc_size_t message_size;
	char *message_ptr;

	message_ptr = (char *) message;
	message_size = sizeof(cgc_Message) - sizeof(short**) - 2;

	while(message_size) {
		if(receive(STDIN, message_ptr++, 1, &bytes_read))
			_terminate(ERROR_FAILED_RECV);

		message_size--;
	}
}

void cgc_sendMessage(cgc_Message* message) {

	if(cgc_transmit_all(STDOUT, (char *) message, sizeof(cgc_Message) - sizeof(short**)))
		_terminate(ERROR_FAILED_SEND);

}

void cgc_swap (cgc_Worker *workerA, cgc_Worker *workerB)
{
 	void (* temp)(cgc_Message* );
	temp = workerA->cgc_processMessage;
	workerA->cgc_processMessage = workerB->cgc_processMessage;
	workerB->cgc_processMessage = temp;

}

void cgc_permute(cgc_Worker* list, int start, int end, int* index, int stop)
{
	int j;
	
	if((*index) == stop)
		return;

	if(start == end)
	{
		(*index)++;
	}
	else 
	{
		for(j = start; j <= end; j++)
		{
			cgc_swap(&list[start], &list[j]);
			cgc_permute(list, start+1, end, index, stop);
			if((*index) != stop)
				cgc_swap(&list[start], &list[j]);
			else
				break;
		}
	}

}

void cgc_computeResult(cgc_Message *message)
{
	short x, y;
	x = message->value[X_FIELD];
	y = message->value[Y_FIELD];

	if(message->result == NULL) {
		int ret;
		short *result_data;

		ret = allocate(message->value[LENGTH_FIELD]*sizeof(short*), 0, (void **) &message->result);
		if (ret != 0)
			_terminate(1);

		ret = allocate(message->value[LENGTH_FIELD]*message->value[WIDTH_FIELD]*sizeof(short), 0, (void **) &result_data);
		if (ret != 0)
			_terminate(1);

		int i;
		for(i=0; i<message->value[LENGTH_FIELD]; i++)
			message->result[i] = result_data + i * message->value[WIDTH_FIELD];
	}

	int i;
	for(i=0; i < message->value[LENGTH_FIELD]*message->value[WIDTH_FIELD]; i++)
	{
		message->result[x][y] = 1;

#ifdef PATCHED
		x = message->value[LENGTH_FIELD] - x;
		if( message->value[X_FIELD] >= x)
			x = message->value[X_FIELD] - x;
		else
			x = message->value[LENGTH_FIELD] - x + message->value[X_FIELD];

		y = message->value[WIDTH_FIELD] - y;
		if( message->value[Y_FIELD] >= y)
			y = message->value[Y_FIELD] - y;
		else
			y = message->value[WIDTH_FIELD] - y + message->value[Y_FIELD];
		
#else
		x = cgc_modulus(x+message->value[X_FIELD], message->value[LENGTH_FIELD]);
		y = cgc_modulus(y+message->value[Y_FIELD], message->value[WIDTH_FIELD]);
#endif

	}

}

int main(void) {
	cgc_Message* message = NULL;
	cgc_Worker worker[4] = {0};
	int i;
	int index;
	int ret;

	ret = allocate(sizeof(cgc_Message), 0, (void **) &message);
	if(ret != 0)
		_terminate(1);

	cgc_receiveMessage(message);

	worker[0].cgc_processMessage = cgc_identityMap;
	worker[1].cgc_processMessage = cgc_constantMap;
	worker[2].cgc_processMessage = cgc_absoluteValueMap;
	worker[3].cgc_processMessage = cgc_modulusCoordinatesWithDimensions;

	index = 0; 

	cgc_permute(worker, 0, 3, &index, message->value[PERM_FIELD]);

	while(1) {

		worker[0].inbox = message;

		for(i=3; i>=0; i--) {
			cgc_processMessage(&worker[i]);

			if(i<3)
				worker[i+1].inbox = worker[i].outbox;
		}

		if(worker[3].outbox != NULL) {

			cgc_computeResult(worker[3].outbox);

			cgc_sendMessage(worker[3].outbox);

			ret = deallocate(worker[3].outbox->result[0], worker[3].outbox->value[LENGTH_FIELD]*worker[3].outbox->value[WIDTH_FIELD]*sizeof(short));
			if (ret != 0)
				_terminate(2);

			ret = deallocate(worker[3].outbox->result, worker[3].outbox->value[LENGTH_FIELD]*sizeof(short*));
			if (ret != 0)
				_terminate(2);

			ret = deallocate(worker[3].outbox, sizeof(cgc_Message));
			if (ret != 0)
				_terminate(2);
		}

		ret = allocate(sizeof(cgc_Message), 0, (void **) &message);
		if(ret != 0)
			_terminate(1);

		cgc_receiveMessage(message);
	}


}
