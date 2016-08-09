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
#include <libcgc.h>
#include "libc.h"
#include "service.h"

void cgc_newReport(cgc_Report *report) {
	char character = 0;
	char* string = NULL;
	short pos = 0, prev_pos=0;
	short num_fields = 0;
	cgc_Field* new_field = NULL;

	while(END_CHAR != character) {
		if(SPLIT_CHAR == character) {
			new_field = cgc_malloc(sizeof(cgc_Field));
#ifdef PATCHED
			if(new_field == NULL)
				_terminate(ALLOCATE_ERROR);
#endif
			new_field->start = prev_pos-num_fields;
			new_field->size = pos-prev_pos-1;
#ifdef PATCHED
			if(new_field->size <= 0 || new_field->size > MAX_FIELD_SIZE)
				_terminate(FIELD_SIZE_ERROR);
#endif
			new_field->next = report->fields;
			report->fields = new_field;
			prev_pos = pos;
			num_fields++;
		}
		cgc_recv(STDIN, &character, sizeof(character));
		pos++;
#ifdef PATCHED
        if(pos < 0)
            _terminate(FIELD_SIZE_ERROR);
#endif
	}
	new_field = cgc_malloc(sizeof(cgc_Field));
#ifdef PATCHED
	if(new_field == NULL)
		_terminate(ALLOCATE_ERROR);
#endif
	new_field->start = prev_pos-num_fields;
	new_field->size = pos-prev_pos-1;
#ifdef PATCHED
			if(new_field->size <= 0 || new_field->size > MAX_FIELD_SIZE)
				_terminate(FIELD_SIZE_ERROR);
#endif
	new_field->next = report->fields;
	report->fields = new_field;
	num_fields++;

	report->record_size = pos-num_fields;

#ifdef PATCHED
	if(report->record_size <= 0 || report->record_size > MAX_REPORT_SIZE || report->record_size <= new_field->size+1)
		_terminate(REPORT_SIZE_ERROR);
#endif
}

void cgc_sendReport(cgc_Report *report) {
	cgc_Record* nextRecord = NULL;
	int ret=0;

	for(nextRecord=report->head; nextRecord != NULL; nextRecord = nextRecord->next) {
		if(cgc_transmit_all(STDOUT, nextRecord->data, report->record_size)) 
			_terminate(TRANSMIT_ERROR);
	}
}

void cgc_splitReport(cgc_Record *source, cgc_Record **front, cgc_Record **back) {
	cgc_Record *fast;
	cgc_Record *slow;

	if(!source || !source->next) {
		*front = source;
		*back = NULL;
	} else {
		slow = source;
		fast = source->next;

		while (fast != NULL) {
			fast = fast->next;
			if(fast != NULL) {
				slow = slow->next;
				fast = fast->next;
			}
		}

		*front = source;
		*back = slow->next;
		slow->next = NULL;
	}

}

cgc_Record* cgc_mergeReport(cgc_Record* a, cgc_Record* b, cgc_Field* field) {
	cgc_Record* result = NULL;
	int compare;

	if(!a)
		return b;

	if(!b)
		return a;

	compare = cgc_strncmp(&a->data[field->start], &b->data[field->start], field->size);

	if(compare <= 0) {
		result = a;
		result->next = cgc_mergeReport(a->next, b, field);
	} else {
		result = b;
		result->next = cgc_mergeReport(a, b->next, field);
	}

	return result;
}

void cgc_sortReport(cgc_Record **headRef, cgc_Field* field) {
	cgc_Record *head = *headRef;
	cgc_Record *a=NULL, *b=NULL;

	if(!head || !head->next)
		return;

	cgc_splitReport(head, &a, &b);
	cgc_sortReport(&a, field);
	cgc_sortReport(&b, field);

	*headRef = cgc_mergeReport(a, b, field);

}

void cgc_filterReport(cgc_Report *report, cgc_Report* filteredReport, char* filterString) {
	cgc_Record* nextRecord = NULL;
	cgc_Field* nextField = NULL;
	int compare = 0;

	for(nextRecord = report->head; nextRecord != NULL; nextRecord = nextRecord->next) {
		for(nextField = report->fields; nextField->next != NULL; nextField = nextField->next) {
			compare = cgc_strncmp(&nextRecord->data[nextField->start], &filterString[nextField->start], nextField->size);
			if(!compare) {
				cgc_Record* record;
				record = cgc_malloc(sizeof(cgc_Record));
#ifdef PATCHED
				if(record == NULL)
					_terminate(ALLOCATE_ERROR);
#endif
				record->next = filteredReport->head;
				record->data = nextRecord->data;
				filteredReport->head = record;
				break;
			}
		}
	}

}


int cgc_newRecord(cgc_Report *report) {
	cgc_Record* record = NULL;
	short second_field_size = 0;
	char* receive_buf = NULL;
	cgc_Field* nextField = NULL;
	short field_index = 0;

	record = cgc_malloc(sizeof(cgc_Record));
#ifdef PATCHED
	if(record == NULL)
		_terminate(ALLOCATE_ERROR);
#endif
	record->next = NULL;
	record->data = cgc_malloc(report->record_size*2); 
#ifdef PATCHED
	if(record->data == NULL)
		_terminate(ALLOCATE_ERROR);
#endif
	receive_buf = cgc_malloc(report->record_size*2);
#ifdef PATCHED
	if(receive_buf == NULL)
		_terminate(ALLOCATE_ERROR);
#endif

	cgc_memset(record->data, 0, report->record_size);
    
    for(nextField = report->fields; nextField != NULL; nextField = nextField->next) {
    	cgc_memset(receive_buf, 0, report->record_size);
    	cgc_recv(STDIN, receive_buf, nextField->size);

    	cgc_strcpy(&record->data[field_index], receive_buf);
    	field_index += nextField->size;
    }

	cgc_free(receive_buf);

	if(cgc_startswith(record->data, "END")) {
		cgc_free(record->data);
		cgc_free(record);
		return 0;
	} else if(cgc_startswith(record->data, "REPORT")) {
		cgc_sendReport(report);
		cgc_free(record->data);
		cgc_free(record);
		return 1;
	} else if(cgc_startswith(record->data, "SORT")) {
		unsigned short field_index;
		cgc_Field* key;

		field_index = record->data[sizeof("SORT")-1];
		for(key = report->fields; key != NULL; key=key->next) {
			if(field_index == 0) {
				break;
			}
			field_index--;
		}

		if(key)
			cgc_sortReport(&report->head, key);

		cgc_free(record->data);
		cgc_free(record);

		return 1;
	} else if(cgc_startswith(record->data, "FREPORT")) {
		cgc_Report filteredReport = {NULL, 0, 0};
		filteredReport.record_size = report->record_size;
		cgc_filterReport(report, &filteredReport, record->data);
		cgc_sendReport(&filteredReport);
		cgc_free(record->data);
		cgc_free(record);

		return 1;
	} else {
		char* command;
		command = &record->data[report->fields->start];

		if(cgc_startswith(command, "ERROR")) {
#ifdef PATCHED
			cgc_free(record->data);
#else
			cgc_free(command);
#endif
			cgc_free(record);
			return 1;
		}
	}

	record->next = report->head;
	report->head = record;

	return 1;

}

int main(void) {
	cgc_Report report = {NULL, 0, 0};
	char* field = NULL;

	cgc_newReport(&report);
 	while(cgc_newRecord(&report));

	return 0;
}
