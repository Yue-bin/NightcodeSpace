#include <stdio.h>
#include <stdlib.h>
struct node
{
    int data;
    struct node* next;
};
int main()
{
    int n, m;
    scanf("%d %d", &n, &m);
    int k;
    scanf("%d", &k);
    struct node* head, * tail, * p,*q;
    head = (struct node*)malloc(sizeof(struct node));
    if (head == NULL)
    {
        exit(1);
    }
    head->data = -1;
    head->next = NULL;
    tail = head;   
    int i = 0;
    for (i = 0; i < n; i++)
    {
        p = (struct node*)malloc(sizeof(struct node));
        if (p == NULL)
        {
            exit(1);
        }
        p->data = i + 1;
        p->next = head->next;
        tail->next = p;
        tail = p;

    }

    p = head;
    if (p == NULL)
    {
        exit(1);
    }
    q = tail;
    for (i = 1;i < k; i++)
    {
        q = p;
        p = p->next;
        
    }
    
    int count = 1;
    while (p != q)
    {
        if (count == m)
        {

            q->next = q->next->next;
            free(p);
            p = q->next;
            count = 1;
        }
        else {
            q = p; p = p->next; count++;
        }
    }
    int a = p->data;
    free(p);
    head->next = NULL;
    printf("%d", a);
    return 0;
}