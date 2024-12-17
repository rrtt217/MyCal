#pragma once
#include "exprparser.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct node
{
    char* str_data;
    int str_length;
    double result;
   struct node* prev;
   struct node* next;
}node;
//create an empty linked list.
node* create_list()
{
    node* head_node = (node*)malloc(sizeof(node));
    return head_node;
}
//append an item to the bottom.
node* append_item(node* head,char str_data[],int length)
{
    node *p = head;
    while(p->next)
        p = p->next;
    node* ptr_item = (node*)malloc(sizeof(node));
    ptr_item->str_data = (char*)malloc(sizeof(char)*length);
    memcpy(ptr_item->str_data,str_data,length);
    p->next = ptr_item;
    ptr_item->prev = p;
    ptr_item->next = NULL;
    ptr_item->result = nan("1");
    return head;
}
void delete_item(node* ptr_item)
{
    if(ptr_item->prev)
    {
        ptr_item->prev->next = ptr_item->next;
        free(ptr_item->str_data);
        free(ptr_item);
    }
    else
    {
        errno = 1;
    }
}
node* auto_delete(node *head,node* current)
{
    node* p = head , *q;
    while(p->next)
    {
        p = p->next;
        if(p->str_data && p != current && !(p->str_data[0]) && p->prev)
        {
                q = p;
                p = p->prev;
                delete_item(q); 
        }
    }
    return head;
}
int print_list_segment(node* start,node* end,int start_index,int x)
{
    if(start == NULL)
    {
        printf("ERROR\n");
        return -1;        
    }
    else
    {
        while(start)
        {
            printf("Input[%d]:%s\n",start_index,start->str_data);
            if(start->result == nan("1"))
                start->result = expr_parse(start->str_data,start->str_length,x);
            if(!isnan(start->result))
                printf("Output[%d]:%F\n",start_index,start->result);
            else
            {
            if(errno > 1024 && errno)
            {
                if(errno-1024 == '(')
                    puts("Error:Unmatched or unclosed brackets");
                else
                    printf("Error:Unexpected symbol \"%c\"\n",errno-1024);
            }
            else if(errno == 1024)
            {
                printf("Error:Undefined syntax error\n");
            }
            else if(errno > 1024-128)
            {
                if(str_of_wrapped_function(errno-1024))
                    printf("Error:Function %s\() has no brackets matched\n",str_of_wrapped_function(errno-1024));
            }
            else if(errno != 0)
            {
                perror("Error");
            }
            }
            start = start->next;
            start_index++;
            if(start == end)
                break;
        }
        return start_index;
    }
}