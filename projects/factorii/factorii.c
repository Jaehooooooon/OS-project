#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "threads/init.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"

#include "devices/timer.h"

#define FE 1
#define CO 2
#define FEI 3
#define COI 4

struct semaphore *control;
struct semaphore control_;

struct semaphore *f3;
struct semaphore f3_;
struct semaphore *f1;
struct semaphore f1_;
struct semaphore *f2;
struct semaphore f2_;

struct semaphore *ra3_1;
struct semaphore ra3_1_;
struct semaphore *ra3_2;
struct semaphore ra3_2_;

struct semaphore *ra2_1;
struct semaphore ra2_1_;
struct semaphore *ra2_2;
struct semaphore ra2_2_;

struct semaphore *ra1_1;
struct semaphore ra1_1_;
struct semaphore *ra1_2;
struct semaphore ra1_2_;

struct semaphore *belt3;
struct semaphore belt3_;
struct semaphore *belt4;
struct semaphore belt4_;

struct semaphore *belt1;
struct semaphore belt1_;
struct semaphore *belt2;
struct semaphore belt2_;

int exitCondition=0;
int turnTime = -1;

int f3_delay=3;

int cofe=0;
int fei=0;
int coi=0;
int fe=0;
int co=0;

int f3_work=0;
int f3_fe_work = 0;
int f3_co_work = 0;

int f3_fe_num=0;
int f3_co_num=0;

int map[5][10];
int material[4]={0};

int f1_mat=0;
int f2_mat=0;
int belt1_mat[3]={0};
int belt2_mat[3]={0};
int belt3_mat[3]={0};
int belt4_mat[3]={0};

void map_print(){
   for(int a=0; a<5; a++){
      if(a==0 || a==4){
         for(int i=0; i<10; i++)
            printf("X ");
         printf("\n");
      }
      else if(a==1){
         for(int i=0; i<3; i++){
            for(int j=0; j<3; j++){
               if(i==0){
                  if(belt1_mat[j]==FE) printf("o ");
                  else printf("  ");
               }
               else if(i==1){
                  if(j==0 || j==2) printf("@ ");
                  else{
                     if(f1_mat == FE) printf("o ");
                     else if(f1_mat == FEI) printf("i ");
                     else printf("- ");
                  }
               }
               else{
                  if(belt3_mat[j]==FEI) printf("i ");
                  else printf("  ");
               }
            }
         }
         printf("@ \n");
      }
      else if(a==3){
         for(int i=0; i<3; i++){
            for(int j=0; j<3; j++){
               if(i==0){
                  if(belt2_mat[j]==CO) printf("o ");
                  else printf("  ");
               }
               else if(i==1){
                  if(j==0 || j==2) printf("@ ");
                  else{
                     if(f2_mat == CO) printf("o ");
                     else if(f2_mat == COI) printf("i ");
                     else printf("- ");
                  }
               }
               else{
                  if(belt4_mat[j]==COI) printf("i ");
                  else printf("  ");
               }
            }
         }       
         printf("@ \n");  
      }
      else{
         for(int i=0; i<9; i++) printf("X ");
         if(f3_co_num==0 && f3_fe_num == 0 && f3_work == 0) printf("- \n");
         else printf("i \n");
      }
   }
	printf("LEFT: FeOre=%d, CoOre=%d, FeIng=%d, CoIng=%d\n", fe, co, fei, coi);
	printf("MADE: CoFe=%d\n", cofe);
	printf("%d", ++turnTime);
	printf("\n");
	printf("\n");
	printf("\n");
}

void control_unit(void){
   while(1){
      map_print();
      if(cofe == exitCondition){
			printf("end\n");
			return 0;
		}

      sema_up(f3);
      sema_down(control);
      timer_sleep(100);
   }
}

void iF3(void){
   while(1){
      if(f3->value==0) 
         sema_up(f3);
      sema_down(f3);

		if(material[2]==f3_fe_num){
			f3_fe_work=1;	//철은 꽉참
		}
		if(material[3]==f3_co_num){
			f3_co_work=1;	//구리는 꽉참
		}
      if(material[2]==f3_fe_num && material[3]==f3_co_num){
         f3_delay--;
         f3_work = 1;
         if(f3_delay ==0){
            cofe++;
            f3_delay=3;
            f3_fe_num=0;
            f3_co_num=0;
            f3_work=0;
				f3_fe_work = 0;
				f3_co_work = 0;
         }
      }

      sema_up(ra3_1);
      sema_down(f3);
   }
}

void rA3_1(void){
   while(1){
      if(ra3_1->value==0) 
         sema_up(ra3_1);
      sema_down(ra3_1);

      if(belt3_mat[2]==FEI && f3_fe_work == 0 /*&& brz_fin_fe_num < material[2]*/){
         f3_fe_num++;
         belt3_mat[2]=0;
			fei--;
      }

      sema_up(ra3_2);
      sema_down(ra3_1);
   }
}

void rA3_2(void){
   while(1){
      if(ra3_2->value==0) 
         sema_up(ra3_2);
      sema_down(ra3_2);

      if(belt4_mat[2]==COI && f3_co_work==0 && f3_co_num < material[3]){
         f3_co_num++;
         belt4_mat[2]=0;
			coi--;
      }

      sema_up(belt3);
      sema_down(ra3_2);
   }
}

void cBelt3(void){
   while(1){
      if(belt3->value==0) 
         sema_up(belt3);
      sema_down(belt3);
      if(belt3_mat[2]==0){
         if(belt3_mat[1]==FEI){
            belt3_mat[2]=FEI;
            belt3_mat[1]=0;
         }
			if(belt3_mat[1]==0){	
				if(belt3_mat[0]==FEI){
			      belt3_mat[1]=FEI;
			      belt3_mat[0]=0;
				}
			}
      }

      if(belt3_mat[1]==0 && f3_fe_work!=1){	
         if(belt3_mat[0]==FEI){
            belt3_mat[1]=FEI;
            belt3_mat[0]=0;
         }
      }
                
      sema_up(belt4);
      sema_down(belt3);
   }
}

void cBelt4(void){
   while(1){
      if(belt4->value==0) 
         sema_up(belt4);
      sema_down(belt4);

      if(belt4_mat[2]==0){
         if(belt4_mat[1]==COI){
            belt4_mat[2]=COI;
            belt4_mat[1]=0;
         }
			if(belt4_mat[1]==0){
				if(belt4_mat[0]==COI){
			      belt4_mat[1]=COI;
			      belt4_mat[0]=0;
				}
			}
      }
      if(belt4_mat[1]==0 && f3_co_work!=1){
         if(belt4_mat[0]==COI){
            belt4_mat[1]=COI;
            belt4_mat[0]=0;
         }
      }

      sema_up(ra2_1);
      sema_down(belt4);
   }
}

void rA2_1(void) {
   while(1) {
      if(ra2_1->value==0) 
         sema_up(ra2_1);
      sema_down(ra2_1);

      if(f1_mat == FEI && belt3_mat[0] == 0) {
        	f1_mat = 0;
        	belt3_mat[0] =FEI;
      }
      	
      sema_up(ra2_2);
      sema_down(ra2_1);
   }
}

void rA2_2(void) {
   while(1) {
      if(ra2_2 -> value == 0) 
         sema_up(ra2_2);
      sema_down(ra2_2);

      if(f2_mat == COI && belt4_mat[0] == 0){
        	f2_mat = 0;
        	belt4_mat[0]=COI;
      }

      sema_up(f1);
      sema_down(ra2_2);
   }
}

void iF1(void){
   while(1) {
      if(f1->value==0) 
         sema_up(f1);
      sema_down(f1);

      if(f1_mat==FE) {
		   f1_mat=FEI;
		   fei++;
		   fe--;
		}

      sema_up(f2);
      sema_down(f1);
   }
}

void iF2(void){
   while(1){
      if(f2->value==0) 
         sema_up(f2);
      sema_down(f2);

      if(f2_mat == CO) {
		  f2_mat=COI;
		  coi++;
		  co--;
	   }

      sema_up(ra1_1);
      sema_down(f2);
   }
}

void rA1_1(void){
   while(1){
      if(ra1_1->value == 0) 
         sema_up(ra1_1);
      sema_down(ra1_1);

      if(belt1_mat[2]==FE && f1_mat==0){
        	f1_mat=FE;
        	belt1_mat[2]=0;
      }

      sema_up(ra1_2);
      sema_down(ra1_1);
   }
}

void rA1_2(void){
   while(1){
      if(ra1_2->value == 0) 
         sema_up(ra1_2);
      sema_down(ra1_2);

      if(belt2_mat[2]==CO && f2_mat==0){
         f2_mat=CO;
         belt2_mat[2]=0;
      }

      sema_up(belt1);
      sema_down(ra1_2);
   }
}

void cBelt1(void){
   while(1){
      if(belt1->value==0) 
         sema_up(belt1);
      sema_down(belt1);

      if(belt1_mat[2]==0){
         if(belt1_mat[1]==FE){
            belt1_mat[2]=FE;
            belt1_mat[1]=0;
         }
      }
      if(belt1_mat[1]==0){
         if(belt1_mat[0]==FE){
            belt1_mat[1]=FE;
            belt1_mat[0]=0;
         }
      }
      if(belt1_mat[0]==0 && material[0]>0){
         belt1_mat[0]=FE;
         material[0]--;
		   fe++;
      }

      sema_up(belt2);
      sema_down(belt1);
   }
}

void cBelt2(void){
   while(1){
      if(belt2->value==0) 
         sema_up(belt2);
      sema_down(belt2);

      if(belt2_mat[2]==0){
         if(belt2_mat[1]==CO){
            belt2_mat[2]=CO;
            belt2_mat[1]=0;
         }
      }
      if(belt2_mat[1]==0){
         if(belt2_mat[0]==CO){
            belt2_mat[1]=CO;
            belt2_mat[0]=0;
         }
      }
      if(belt2_mat[0]==0 && material[1]>0){
         belt2_mat[0]=CO;
         material[1]--;
		   co++;
      }

      sema_up(control);
      sema_down(belt2);
   }
}

void run_factorii(char **argv){
   for(int i=0; i<4; i++){
      material[i] = atoi(argv[i+1]);
   }

   if (material[0]/material[2] >= material[1]/material[3])
		exitCondition = material[1]/material[3];
	else
		exitCondition = material[0]/material[2];

   void *aux;

   control = &control_;

   f3 = &f3_;
   f1 = &f1_;
   f2 = &f2_;

   ra3_1 = &ra3_1_;
   ra3_2 = &ra3_2_;
   ra1_1 = &ra1_1_;
   ra1_2 = &ra1_2_;
   ra2_1 = &ra2_1_;
   ra2_2 = &ra2_2_;

   belt3 = &belt3_;
   belt4 = &belt4_;
   belt1 = &belt1_;
   belt2 = &belt2_;

   sema_init(control, 0);
  
   sema_init(f3, 0);
   sema_init(f1, 0);
   sema_init(f2, 0);

   sema_init(ra3_1, 0);
   sema_init(ra3_2, 0);
   sema_init(ra1_1, 0);
   sema_init(ra1_2, 0);
   sema_init(ra2_1, 0);
   sema_init(ra2_2, 0);

   sema_init(belt3, 0);
   sema_init(belt4, 0);
   sema_init(belt1, 0);
   sema_init(belt2, 0);

   thread_create("control", PRI_DEFAULT, control_unit, aux);
   thread_create("f3", PRI_DEFAULT, iF3, aux);
   thread_create("ra3_1", PRI_DEFAULT, rA3_1, aux);
   thread_create("ra3_2", PRI_DEFAULT, rA3_2, aux);
   thread_create("line1_2", PRI_DEFAULT, cBelt3, aux);
   thread_create("line2_2", PRI_DEFAULT, cBelt4, aux);
   thread_create("ra2_1", PRI_DEFAULT, rA2_1, aux);
   thread_create("ra2_2", PRI_DEFAULT, rA2_2, aux);
   thread_create("f1", PRI_DEFAULT, iF1, aux);
   thread_create("f2", PRI_DEFAULT, iF2, aux);
   thread_create("ra1_1", PRI_DEFAULT, rA1_1, aux);
   thread_create("ra1_2", PRI_DEFAULT, rA1_2, aux);
   thread_create("line1_1", PRI_DEFAULT, cBelt1, aux);
   thread_create("line2_1", PRI_DEFAULT, cBelt2, aux);
}