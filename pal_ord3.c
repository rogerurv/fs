


/************************************************************************/
/*				pal_ord3.c				*/
/*									*/
/*   Compilar:								*/
/*      gcc -Wall pal_ord3.c memoria.o winsuport2.o -o pal_ord3 -lcurses	*/
/*									*/
/*	Aquest programa s'executara com un proces fill del programa	*/
/*	'tennis3' (veure fitxer 'tennis3.c').			*/
/************************************************************************/



/* funcio per moure la paleta de l'ordinador autonomament, en funcio de la */
/* velocitat de la paleta (variable global v_pal) */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "memoria.h"
#include "winsuport2.h"
#include <pthread.h>

	
	
int main(int n_args, char *ll_args[])
{
	
	int id_win,n_fil,n_col;
	void *p_win;
	int moviments,*p_moviments;
	int retard;
	int tecla,*p_tecla;
	int cont,*p_cont;
	int MAX_MOV;
	
	int l_pal;
	
	
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
	
	win_set(p_win,n_fil,n_col);	/* crea acces a finestra oberta pel proces pare */
	
	
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
	
	
	
	
	
	int f_h;
	
	/* char rh,rv,rd; */
	int i= index; 
	int j=i+1;
	char c = j +'0';
  
do{
  
  f_h = po_pf + v_pal;		/* posicio hipotetica de la paleta */
 
  if (f_h != ipo_pf)	/* si pos. hipotetica no coincideix amb pos. actual */
  { 
    if (v_pal > 0.0)			/* verificar moviment cap avall */
    {
	
	if (win_quincar(f_h+l_pal-1,ipo_pc) == ' ')   /* si no hi ha obstacle */
	{
	  
	  win_escricar(ipo_pf, ipo_pc,' ',NO_INV);      /* esborra primer bloc */
	 
	  po_pf += v_pal; ipo_pf = po_pf;		/* actualitza posicio */
	  
	  win_escricar(ipo_pf+l_pal-1, ipo_pc,c,INVERS); /* impr. ultim bloc */
    

	}
	else{		/* si hi ha obstacle, canvia el sentit del moviment */
	   v_pal = -v_pal;
	  *p_moviments=*p_moviments+1;
      

       
     }}
    else			/* verificar moviment cap amunt */
    {

	if (win_quincar(f_h,ipo_pc) == ' ')        /* si no hi ha obstacle */
	{
	   			
	  win_escricar(ipo_pf+l_pal-1,ipo_pc,' ',NO_INV); /* esbo. ultim bloc */
	 
	  po_pf += v_pal; ipo_pf = po_pf;		/* actualitza posicio */
	  
	  win_escricar(ipo_pf, ipo_pc,c,INVERS);	/* impr. primer bloc */
	 
	}
	else{		/* si hi ha obstacle, canvia el sentit del moviment */
	   v_pal = -v_pal;
	   *p_moviments=*p_moviments+1;
        
         
     }}
  }
  else 
  
  po_pf += v_pal; 	/* actualitza posicio vertical real de la paleta */
  
  
  win_retard(retard);
  
	 } while (*p_tecla != TEC_RETURN && (*p_cont==-1) && *p_moviments<MAX_MOV);

return(0);
}
