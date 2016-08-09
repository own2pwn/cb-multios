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
#include "vm.h"
#include "stack.h"

void cgc_invalid_command() {
	int ret;

	ret = cgc_transmit_all(STDOUT, INVALID_CMD_STR, sizeof(INVALID_CMD_STR));
    if (ret != 0)
        _terminate(3);
}

int cgc_parseCmd(char* command, char* buffer) {

	int command_size; 

	command_size = cgc_strnlen(command, MAX_LINE_SIZE);

	if(cgc_strncmp(buffer, command, command_size) == 0) {
		return command_size;
	} else
		return 0;
}

int cgc_receiveProgramSize(socket) {
	char buf[1024] ={0};
	int bytes_read;
	int arg_pos;
	int size;

	bytes_read = cgc_recvline(socket, buf, sizeof(buf)-1);
	if (bytes_read <= 0)
		_terminate(4);

	arg_pos = cgc_parseCmd(PRGM_SIZE_CMD, buf);
	if (!arg_pos) {
		cgc_invalid_command();
		_terminate(5);
	}

	size = cgc_strn2int(buf+arg_pos, MAX_ARG_SIZE);

	return size;

}

void cgc_initProgram(cgc_Program **program_ptr, int socket) {

	char buf[MAX_LINE_SIZE] ={0};
	int size;
	int ret;
	int bytes_read;

	size = cgc_receiveProgramSize(socket);

	if(size <= 0) {
		return;
	}

	if(size > MAX_PROGRAM_SIZE)
		return;

	ret = allocate(sizeof(cgc_Program) + MAX_LINE_SIZE*size, 0, (void**) program_ptr);
	if (ret != 0)
		_terminate(6);

	cgc_Program *program;
	program = *program_ptr;
	program->lineNumber = -1;
	program->size = size;

	int i;
	for (i = 0; i < size; i++) {
		bytes_read = cgc_recvline(socket, buf, MAX_LINE_SIZE);
		if (bytes_read <= 0)
			_terminate(7);

		cgc_addLine(program, buf);
	}

	program->lineNumber = 0;

	return;
	
}

int cgc_isProgramFull(cgc_Program *program) {
	return program->lineNumber >= program->size - 1;
}

void cgc_addLine(cgc_Program *program, char* buf) {
	int ret;

	if(cgc_isProgramFull(program)) {
		ret = cgc_transmit_all(STDOUT, TOO_MANY_LINES_STR, sizeof(TOO_MANY_LINES_STR));
    	if (ret != 0)
        	_terminate(10);
        return;
	}

    program->lineNumber++;
	cgc_memcpy(&program->lines[program->lineNumber*MAX_LINE_SIZE], buf, MAX_LINE_SIZE);

}

void cgc_executeProgram(cgc_Program* program) {

	cgc_Stack stack;
	int arg_pos;

	cgc_initStack(&stack, program->size * 16, MAX_ARG_SIZE);
	if(stack.elements == NULL)
		return;

	program->lineNumber = 0;

	while(program->lineNumber < program->size) {
		char* line;
		
		line = &program->lines[program->lineNumber*MAX_LINE_SIZE];

		if((arg_pos = cgc_parseCmd(PUSH_CMD, line)) > 0) {
			cgc_pushElement(&stack, line+arg_pos);
			program->lineNumber++;
		}
		else if((arg_pos = cgc_parseCmd(POP_CMD, line)) > 0)	{
			char* element;
			int len;

			element = (char *) cgc_popElement(&stack);

			len = cgc_strnlen(element, MAX_ARG_SIZE);
			cgc_memset(element, 0, len);

			program->lineNumber++;
		}
		else if((arg_pos = cgc_parseCmd(ADD_CMD, line)) > 0) {
			int a, b, ret;
			char *a_str;
			char *a_element;
			char *b_element;
			int len;

			a_element = cgc_popElement(&stack);
			b_element = cgc_popElement(&stack);

			a = cgc_strn2int(a_element, MAX_ARG_SIZE);
			b = cgc_strn2int(b_element, MAX_ARG_SIZE);
			a += b;
			a_str = cgc_itoaB10(a); 

			len = cgc_strnlen(a_element, MAX_ARG_SIZE);
			cgc_memset(a_element, 0, len);

			len = cgc_strnlen(b_element, MAX_ARG_SIZE);
			cgc_memset(b_element, 0, len);

			len = cgc_strlen(a_str);
			cgc_pushElement(&stack, a_str);
			
			ret = deallocate(a_str, len); 
			if (ret != 0)
				_terminate(11);

			program->lineNumber++;
		}
		else if((arg_pos = cgc_parseCmd(PRINT_CMD, line)) > 0) {
			int ret, len;
			char *element;

			element = (char *) cgc_popElement(&stack);
			ret = cgc_transmit_all(STDOUT, element, cgc_strnlen(element, MAX_ARG_SIZE));
			if (ret != 0)
				_terminate(14);			
			ret = cgc_transmit_all(STDOUT, NEWLINE, cgc_strnlen(NEWLINE, MAX_ARG_SIZE));
			if (ret != 0)
				_terminate(15);	

			len = cgc_strnlen(element, MAX_ARG_SIZE);
			cgc_memset(element, 0, len);

			program->lineNumber++;
		}
		else if((arg_pos = cgc_parseCmd(COPY_CMD, line)) > 0) {
			int copy_num;
			int ret, i;

			copy_num = cgc_strn2int(line+arg_pos, MAX_ARG_SIZE);
			if(copy_num < 1) {
				program->lineNumber++;
				continue;
			}

			char *element;
			element = cgc_popElement(&stack);

			for(i=0; i<copy_num; i++) {
				cgc_pushElement(&stack, element);
			}

			program->lineNumber++;
		} else
			program->lineNumber++;

	}

	cgc_destroyStack(&stack);

}
