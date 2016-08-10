#pragma once

void* _NSConcreteStackBlock;
void* _NSConcreteGlobalBlock;

void cgc__Block_object_assign(void* dst, const void *src, int flag);
void cgc__Block_object_dispose(void* arg, int flag);
