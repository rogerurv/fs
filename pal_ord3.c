


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
	int id_moviments,*p_moviments;
	int id_retard,*p_retard;
	int id_tecla,*p_tecla;
	int id_cont,*p_cont;
	int id_MAX_MOV,*p_MAX_MOV;
	int id_index,*p_index;
	int id_l_pal,*p_l_pal;
	void *p_win;
	
	int id_ipo_pf,*p_ipo_pf;
	int id_ipo_pc,*p_ipo_pc;
	int id_po_pf,id_v_pal;
	int id_mutex;
	pthread_mutex_t *p_mutex;
	float *p_po_pf,*p_v_pal;
	
	id_win = atoi(ll_args[1]);
	p_win = map_mem(id_win);
	
	n_fil = atoi(ll_args[2]);		/* obtenir dimensions del camp de joc */
	n_col = atoi(ll_args[3]);
	
	win_set(p_win,n_fil,n_col);	/* crea acces a finestra oberta pel proces pare */
	
	
	/* moviments,retard,tecla,cont,MAX_MOV,index,l_pal*/
	
	id_moviments= atoi(ll_args[4]);
	p_moviments = map_mem(id_moviments);	/* obtenir adres. de mem. compartida */
	
	id_retard= atoi(ll_args[5]);
	p_retard = map_mem(id_retard);	/* obtenir adres. de mem. compartida */
	
	id_tecla= atoi(ll_args[6]);
	p_tecla = map_mem(id_tecla);	/* obtenir adres. de mem. compartida */
	
	id_cont= atoi(ll_args[7]);
	p_cont = map_mem(id_cont);	/* obtenir adres. de mem. compartida */
	
	id_MAX_MOV= atoi(ll_args[8]);
	p_MAX_MOV = map_mem(id_MAX_MOV);	/* obtenir adres. de mem. compartida */
	
	id_index= atoi(ll_args[9]);
	p_index = map_mem(id_index);	/* obtenir adres. de mem. compartida */
		
	id_l_pal= atoi(ll_args[10]);
	p_l_pal = map_mem(id_l_pal);	/* obtenir adres. de mem. compartida */	
	
	id_ipo_pf= atoi(ll_args[11]);
	p_ipo_pf = map_mem(id_ipo_pf);	/* obtenir adres. de mem. compartida */	
	
	id_ipo_pc= atoi(ll_args[12]);
	p_ipo_pc = map_mem(id_ipo_pc);	/* obtenir adres. de mem. compartida */	
	
	id_po_pf= atoi(ll_args[13]);
	p_po_pf = map_mem(id_po_pf);	/* obtenir adres. de mem. compartida */	
	
	id_v_pal= atoi(ll_args[14]);
	p_v_pal = map_mem(id_v_pal);	/* obtenir adres. de mem. compartida */	
	
	id_mutex= atoi(ll_args[15]);
	p_mutex = map_mem(id_mutex);	/* obtenir adres. de mem. compartida */	
  
	int f_h;
	/* char rh,rv,rd; */
	int i= (int) p_index; 
	int j=i+1;
	char c = j +'0';
  
do{
  
  f_h = *p_po_pf + *p_v_pal;		/* posicio hipotetica de la paleta */
 
  if (f_h != *p_ipo_pf)	/* si pos. hipotetica no coincideix amb pos. actual */
  {
    if (*p_v_pal > 0.0)			/* verificar moviment cap avall */
    {
	pthread_mutex_lock(&*p_mutex);
	if (win_quincar(f_h+*p_l_pal-1,*p_ipo_pc) == ' ')   /* si no hi ha obstacle */
	{
	  
	  win_escricar(*p_ipo_pf, *p_ipo_pc,' ',NO_INV);      /* esborra primer bloc */
	  pthread_mutex_unlock(&*p_mutex);
	  *p_po_pf += *p_v_pal; *p_ipo_pf = *p_po_pf;		/* actualitza posicio */
	  pthread_mutex_lock(&*p_mutex);
	  win_escricar(*p_ipo_pf+*p_l_pal-1, *p_ipo_pc,c,INVERS); /* impr. ultim bloc */
      pthread_mutex_unlock(&*p_mutex);

	}
	else{		/* si hi ha obstacle, canvia el sentit del moviment */
	   *p_v_pal = -*p_v_pal;
	   p_moviments=p_moviments+1;
       pthread_mutex_unlock(&*p_mutex);

       
     }}
    else			/* verificar moviment cap amunt */
    {
	pthread_mutex_lock(&*p_mutex);	
	if (win_quincar(f_h,*p_ipo_pc) == ' ')        /* si no hi ha obstacle */
	{
	   		 	
	  win_escricar(*p_ipo_pf+*p_l_pal-1,*p_ipo_pc,' ',NO_INV); /* esbo. ultim bloc */
	  pthread_mutex_unlock(&*p_mutex);
	  *p_po_pf += *p_v_pal; *p_ipo_pf = *p_po_pf;		/* actualitza posicio */
	  pthread_mutex_lock(&*p_mutex);
	  win_escricar(*p_ipo_pf, *p_ipo_pc,c,INVERS);	/* impr. primer bloc */
	  pthread_mutex_unlock(&*p_mutex);
	}
	else{		/* si hi ha obstacle, canvia el sentit del moviment */
	   *p_v_pal = -*p_v_pal;
	   p_moviments=p_moviments+1;
       pthread_mutex_unlock(&*p_mutex);   
         
     }}
  }
  else 
  
  *p_po_pf += *p_v_pal; 	/* actualitza posicio vertical real de la paleta */
  
  
  win_retard(*p_retard);
  
	 } while (*p_tecla != TEC_RETURN && (*p_cont==-1) && p_moviments<p_MAX_MOV);

return(0);
}
