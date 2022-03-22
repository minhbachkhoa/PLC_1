#include <stdio.h>
#include<stdint.h>
#include<string.h>
#include<stdlib.h>
//dfaf
FILE *ConvFrTxtFile(uint32_t *row); // Tạo và đọc địa chỉ file TXT đã xóa comment
struct Link
{   int size_of_data ;
    char *data ;
    struct Link *next ;
    struct Link *prev ;

};    // Cấu trúc 1 phần tử 
typedef struct Link LinkList ;
LinkList *First , *Last ,*First_F , *Last_F ; // First và Last dùng cho  ConvFrList ()   ; First_F , Last_F  là chuỗi cuối cùng cần tìm
void ConvFrList(FILE *fopen,uint32_t Row_Of_File); // Chuyển về List 1 gồm tập hợp các ký tự có loại bỏ các ký tự ko cần thiết
void ListFinal(void);   // Gom các ký tự câu lệnh (biến) vào 1 data , bổ sung H vào biến thường đóng , bổ xung "sl" nếu là suòn lên hoặc "sx" nếu là sườn xuống
void BranchWithFirst_F(void) ; // Tách nhánh . Nếu chưa phải là đầu ra thì cho vào ()
int main(void)
{   
    FILE  *fptest = NULL ;LinkList *p ;
    uint32_t row = 0U;
    fptest =ConvFrTxtFile(&row);
    ConvFrList(fptest,row);
    ListFinal();
     BranchWithFirst_F();
    p = First_F ;
    while(p != NULL)
    {
        printf("%s\n",p->data);
        p = p->next ;
    }
    fclose(fptest);
    return 0;
}
FILE *ConvFrTxtFile(uint32_t *Row_Of_File)
{   FILE * fp = NULL , * fp1 =NULL ;
   
    char arr[20];
    
      fp1= fopen("PLC.txt", "w");
    //Mở file bằn hàm fopen
    fp= fopen("PLC.awl", "r");
    if (fp==NULL)
    {
        printf("Can not open file");
        return NULL ;
    }

    //Đọc từng dòng từ file cho tới khi gặp NULL
    fp= fopen("PLC.awl", "r");
  while(fgets(arr, 128, fp)!=NULL )
  {  
        *Row_Of_File = *Row_Of_File + 1 ; 
        
        if(*Row_Of_File>5 )
        {       
            
            if(strncmp(arr,"Network",7)==0 )
            {
                fputs("N\n",fp1);
            }
             else if (strncmp(arr,"END",3)==0 )
            {
                fputs("N\n",fp1);
                break;
            }
            else
            {
                fputs(arr,fp1); 
            }

        }
       
  
  }
  *Row_Of_File = *Row_Of_File - 5;

  fclose(fp);
  fclose(fp1);
  return fopen("PLC.txt", "r") ;

}

void ConvFrList(FILE *fopen,uint32_t Row_Of_File)
{
    LinkList *p,*q ; char arr[20];
    First = NULL ;

    for (int i = 0; i <Row_Of_File; i++)
    {  
         fgets(arr,20, fopen) ;
        for(int k =0 ; k<20 ; k++)
        {   
             p = (LinkList *)malloc(sizeof(LinkList));
             p->next = NULL ;
              p->data = (char *)calloc(20, sizeof(char));
              int count = 0 ; // Tính size data thực
              for (int x= 0 ; x<20;x++)
              {     
                   
                if (arr[k]==',')
                    {   
                         k++ ;
                         x = x-1 ;
                        continue ;
                    }
                if (arr[k]=='.')
                    {   
                        (p->data)[x]='_';
                        k++ ;
                        count ++ ;
                        continue ;
                    }

                if(arr[k] == '\n'  ) 
                   {    if (count !=1 ) // Xuống dòng nhưng chưa kết thúc 1 Network
                        {
                            (p->data)[x]=' ';
                            (p->data)[x+1]=')';
                            (p->data)[x+2]=' ';
                            (p->data)[x+3]='(';
                            count = count +4 ;
                            p->size_of_data = count ;
                            break ;
                        }
                        // Kết thúc 1 Net Work thì thêm '('
                        (p->data)[x]=' ';
                        (p->data)[x+1]='(';
                        p->size_of_data = count +2 ;
                        break ;
                   }
                (p->data)[x]=arr[k];
                k++ ;
                count ++ ;   
              }
            if(First ==NULL)
            {
                p->prev = NULL ;
                First = p;
                Last = p ;
            }
            else
            {
                q= Last ;
                q->next = p ;
                p->prev = q ;
                Last = p ;
            }
            if (arr[k]=='\n')
            {
                break ;
            } 
            
        }  
    }
        
}

void ListFinal(void)
{       LinkList *p,*q , *k ;
     p = First ;
     First_F = NULL ;
     int a = 0; // Đếm size token 
     int b = 0 ; // Xác định tiếp điểm thường đóng , sườn lên , sườn xuống
    while(p != NULL)
    {   
        char * token = strtok(p->data, " ");
        while( token != NULL ) 
        {  
                    a = strlen(token) ;
                    
                q = (LinkList *)malloc(sizeof(LinkList));
                
                
                q->next = NULL ;
                if(b == 0)
                {   q->size_of_data = a ;
                    q->data = (char *)calloc(a, sizeof(char));
                    strcpy(q->data,token );
                }
                else 
                {   q->size_of_data = a+1 ;
                    q->data = (char *)calloc(a+1, sizeof(char));
                    for (int i =0 ;i<a ; i++)
                    {
                       (q->data)[i]=token[i]; 
                    }
                    (q->data)[a] ='H' ;
                    b =0;

                }
                
                if(First_F == NULL)
                {
                    q->prev = NULL ;
                First_F = q ;
                Last_F = q ;
                }
                else
                {   k = Last_F ;
                    k->next = q ;
                    q->prev = k ;
                    Last_F = q ;
                }
            
             if(strncmp(token,"LDN",3)==0 ||   strncmp(token,"AN",2)==0 || strncmp(token,"ON",2)==0 )
             {
                 b = 1;
             }
             if(strncmp(token,"EU",2)==0)
            {   LinkList *r1 ,*r2,*r3  ;
                r1 = q->prev ;
                r2 = r1->prev ;
                r1 = r2->prev ;
                r2 = r1->prev ;
                r3= (LinkList *)malloc(sizeof(LinkList));
                r3 ->next = r1->next ;
                r3 ->prev = r1->prev ;
                r2 ->next = r3 ;
                r3->size_of_data = r1->size_of_data +2 ;
                r3->data = (char *)calloc(a+1, sizeof(char));
                for (int i =0 ;i<a ; i++)
                {
                (r3->data)[i] = (r1->data)[i];  
                }
                 (r3->data)[a] ='s' ;
                 (r3->data)[a+1] ='l' ;
                free(r1) ; 

            }
            if(strncmp(token,"ED",2)==0)
            {   LinkList *r1 ,*r2,*r3  ;
                r1 = q->prev ;
                r2 = r1->prev ;
                r1 = r2->prev ;
                r2 = r1->prev ;
                r3= (LinkList *)malloc(sizeof(LinkList));
                r3->next = r1->next ;
                r3->prev = r1->prev ;
                r2->next = r3 ;
                r3->size_of_data = r1->size_of_data +2 ;
                r3->data = (char *)calloc(a+1, sizeof(char));
                for (int i =0 ;i<a ; i++)
                {
                (r3->data)[i] = (r1->data)[i]; 
                }
                 (r3->data)[a] ='s' ;
                 (r3->data)[a+1] ='x' ;
                free(r1) ; 
            }


           
            token = strtok(NULL, " ");
        }
        p=p->next ;
    }
}

void BranchWithFirst_F(void) 
{
    LinkList *p , *k,*r ,*z ; // p dungf duyệt danh sách , q là biến tạm thời lưu dữ liệu
    p = First_F ;
    int found = 0 ; // Tìm thấy 'N' thì bằng 1
    int a = 0 ;
    while(p !=NULL)
    {       
            if (p == Last_F)  // Xóa ký tự '(' cuối đi
            {   
                
                Last_F = k ;
                k->next = NULL ;
            }
           if (p == First_F) // Thêm ký tự '(' vào đầu
        {
            k = (LinkList *)malloc(sizeof(LinkList));
            k->size_of_data =1 ;
            k->data = (char *)calloc(1, sizeof(char));
            k->data[0]='(' ;
            k->next = p ;
            p->prev = k ;
            k->prev = NULL ;
            First_F =k ;
        }
        while(found == 0 && p->next != NULL) // Kiểm tra xem đã tìm đc "N" chưa
        {
            if(p->size_of_data == 1 && strncmp(p->data,"N",1) == 0)
            {
                found = 1 ;
            }
            else
            {   
                r = k ;
                k = p ;
                p = p->next ;
            }
        }
        if(found == 1) // (
        {   int temp1 = 0, temp2 = 0 ;LinkList *a,*b ;
            z = r->prev ;
            z->next = p ;
            p->prev = z ;
            free(r) ;
            free(k); // Xóa ký tự '(' trước dữ liệu "N" đi
            k= p ;
            while(found ==1)
            {
                if(temp1 ==0)
                {    
                    p= p->prev ;
                   
                }
                while(temp2 ==0 )
               {
                    if(strncmp(p->data,"(",1) == 0) 
                    {
                        temp2 =1 ;
                    }
                    else
                    {
                        a= p ;
                        p = p->prev ;
                    }
               }
               if(temp2 ==1)
               {    b= p->prev ;
                    a->prev =b;
                    b->next = a ;
                    found = 0 ;
               }
            }
            p = k ;
        }
        p= p->next ;
    } 
    
}





