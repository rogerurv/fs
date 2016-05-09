


/************************************************************************/
/*				pal_ord4.c				*/
/*									*/
/*   Compilar:								*/
/*      gcc  pal_ord4.c memoria.o winsuport2.o semafor.o missatge.o-o pal_ord3 -lcurses -lpthread	*/
/*									*/
/*	Aquest programa s'executara com un proces fill del programa	*/
/*	'tennis4' (veure fitxer 'tennis4.c').			*/
/************************************************************************/



/* funcio per moure la paleta de l'ordinador autonomament, en funcio de la */
/* velocitat de la paleta (variable global v_pal) */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "memoria.h"
#include "winsuport2.h"
#include <pthread.h>
#include "missatge.h"
#include "semafor.h"
	
	
int main(int n_args, char *ll_args[])
{
	
	int id_win,n_fil,n_col;
	void *p_win;
	int moviments,*p_moviments;
	int retard;
	int tecla,*p_tecla;
	int cont,*p_cont;
	int MAX_MOV;
	int id_sem,id_busties,*busties;
	int l_pal;
	char missatge[1];
	char mis;
	char ord[1];
	
	int ipo_pf;
	int ipo_pc;
	float po_pf;
	float v_pal;
	
	float aux1,aux2;
	int index;
	
	id_win = atoi(ll_args[1]);
	p_win = map_mem(id_win);
	
	n_fil = atoi(ll_args[2]);		/* obtenir dimensions del camp de joc */
	n_col = atoi(ll_args[3]);
	
	id_sem= atoi(ll_args[15]);
	
	id_busties= atoi(ll_args[16]);
	busties = map_mem(id_busties);	/* obtenir adres. de mem. compartida */
	
	
	
	
	/* moviments,retard,tecla,cont,MAX_MOV,index,l_pal*/
	
	moviments= atoi(ll_args[4]);
	p_moviments = map_mem(moviments);	/* obtenir adres. de mem. compartida */
	
	
	
	retard= atoi(ll_args[5]);
	tecla= atoi(ll_args[6]);
	p_tecla = map_mem(tecla);	/* obtenir adres. de mem. compartida */
	
	cont= atoi(ll_args[7]);
	p_cont = map_mem(cont);	/* obtenir adres. de mem. compartida */
	
	
	MAX_MOV= atoi(ll_args[8]);
	index= atoi(ll_args[9]);	
	l_pal= atoi(ll_args[10]);
	ipo_pf= atoi(ll_args[11]);
	ipo_pc= atoi(ll_args[12]);
	aux1= atoi(ll_args[13]);
	aux2= atoi(ll_args[14]);
	
	po_pf=aux1/100;
	v_pal=aux2/100;
	
	waitS(id_sem);
	win_set(p_win,n_fil,n_col);	/* crea acces a finestra oberta pel proces pare */
	signalS(id_sem);
	
	int f_h;
	
	/* char rh,rv,rd; */
	int i= index; 
	int j=i+1;
	char c = j +'0';
	char pal;
	int c_h;
	int bustia_desti;
	
	sprintf(missatge,"%c",'c');
	sendM(busties[index+1],missatge,1);
	
	
	
do{
  j=0;
  i=0;
  pal=' ';
  receiveM(busties[index],ord);
  sscanf(ord,"%c",&mis);
  
  
  
  if(mis=='x'){			/* si pilota ha xocat amb paleta d'ordinador */
			
		if(ipo_pc+2==n_col){
			waitS(id_sem);
			while(i<=l_pal){										/* si arriba al final eliminem paleta*/
				win_escricar(ipo_pf+i-1,ipo_pc,' ',NO_INV);
				i=i+1;
				
				}
			signalS(id_sem);
			ipo_pc=n_col+1;			
						  }
		
		else{
			c_h = ipo_pc + v_pal;
			
				waitS(id_sem);
				pal=win_quincar(ipo_pf,c_h);	/* mirem que hi ha a continuacio de la paleta a totes les files*/
				signalS(id_sem);
				
			
			
			if(pal==' ' ){		/* si no hi ha res i no ha arribat al final */
				i=0;
				waitS(id_sem);
				while(i<=l_pal){									/* si no hi ha un altre paleta a continuacio */
					win_escricar(ipo_pf+i-1,ipo_pc,' ',NO_INV);		/* esborrem posicio actual */
					i=i+1;
								}
				signalS(id_sem);				
				ipo_pc=ipo_pc+1;					/* movem la paleta a la dreta */
						}
			else{
				
				sprintf(missatge,"%c",'c');
				bustia_desti=pal-'0';	/*calculem a quina bustia ho hem d'enviar*/
				sendM(busties[bustia_desti-1],missatge,1);
				}  
			}
		}
	  
  else{
  f_h = po_pf + v_pal;		/* posicio hipotetica de la paleta */
 
  if (f_h != ipo_pf)	/* si pos. hipotetica no coincideix amb pos. actual */
  { 
    if (v_pal > 0.0)			/* verificar moviment cap avall */
    {
	waitS(id_sem);
	if (win_quincar(f_h+l_pal-1,ipo_pc) == ' ')   /* si no hi ha obstacle */
	{
	  
	  win_escricar(ipo_pf, ipo_pc,' ',NO_INV);      /* esborra primer bloc */
	  signalS(id_sem);
	  po_pf += v_pal; ipo_pf = po_pf;		/* actualitza posicio */
	  waitS(id_sem);
	  win_escricar(ipo_pf+l_pal-1, ipo_pc,c,INVERS); /* impr. ultim bloc */
      signalS(id_sem);

	}
	else{		/* si hi ha obstacle, canvia el sentit del moviment */
	   v_pal = -v_pal;
	  *p_moviments=*p_moviments+1;
      signalS(id_sem);

       
     }}
    else			/* verificar moviment cap amunt */
    {
	waitS(id_sem);
	if (win_quincar(f_h,ipo_pc) == ' ')        /* si no hi ha obstacle */
	{
	   			
	  win_escricar(ipo_pf+l_pal-1,ipo_pc,' ',NO_INV); /* esbo. ultim bloc */
	  signalS(id_sem);
	  po_pf += v_pal; ipo_pf = po_pf;		/* actualitza posicio */
	  waitS(id_sem);
	  win_escricar(ipo_pf, ipo_pc,c,INVERS);	/* impr. primer bloc */
	  signalS(id_sem);
	}
	else{		/* si hi ha obstacle, canvia el sentit del moviment */
	   v_pal = -v_pal;
	   *p_moviments=*p_moviments+1;
       signalS(id_sem);
         
     }}
  }
  else 
  
  po_pf += v_pal; 	/* actualitza posicio vertical real de la paleta */
  
  sprintf(missatge,"%c",'c');
  sendM(busties[index+1],missatge,1);
  
  win_retard(retard);
}  
	 } while (*p_tecla != TEC_RETURN && (*p_cont==-1) && *p_moviments<MAX_MOV);

return(0);
}
