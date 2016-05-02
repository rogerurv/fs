/*****************************************************************************/
/*									     */
/*				     Tennis0.c				     */
/*									     */
/*  Programa inicial d'exemple per a les practiques 2 i 3 de ISO.	     */
/*     Es tracta del joc del tennis: es dibuixa un camp de joc rectangular    */
/*     amb una porteria a cada costat, una paleta per l'usuari, una paleta   */
/*     per l'ordinador i una pilota que va rebotant per tot arreu; l'usuari  */
/*     disposa de dues tecles per controlar la seva paleta, mentre que l'or- */
/*     dinador mou la seva automaticament (amunt i avall). Evidentment, es   */
/*     tracta d'intentar col.locar la pilota a la porteria de l'ordinador    */
/*     (porteria de la dreta), abans que l'ordinador aconseguixi col.locar   */
/*     la pilota dins la porteria de l'usuari (porteria de l'esquerra).      */
/*									     */
/*  Arguments del programa:						     */
/*     per controlar la posicio de tots els elements del joc, cal indicar    */
/*     el nom d'un fitxer de text que contindra la seguent informacio:	     */
/*		n_fil n_col m_por l_pal					     */
/*		pil_pf pil_pc pil_vf pil_vc				     */
/*		ipo_pf ipo_pc po_vf					     */
/*									     */
/*     on 'n_fil', 'n_col' son les dimensions del taulell de joc, 'm_por'    */
/*     es la mida de les dues porteries, 'l_pal' es la longitud de les dues  */
/*     paletes; 'pil_pf', 'pil_pc' es la posicio inicial (fila,columna) de   */
/*     la pilota, mentre que 'pil_vf', 'pil_vc' es la velocitat inicial;     */
/*     finalment, 'ipo_pf', 'ipo_pc' indicara la posicio del primer caracter */
/*     de la paleta de l'ordinador, mentre que la seva velocitat vertical    */
/*     ve determinada pel parametre 'po_fv'.				     */
/*									     */
/*     A mes, es podra afegir un segon argument opcional per indicar el      */
/*     retard de moviment de la pilota i la paleta de l'ordinador (en ms);   */
/*     el valor d'aquest parametre per defecte es 100 (1 decima de segon).   */
/*									     */
/*  Compilar i executar:					  	     */
/*     El programa invoca les funcions definides en 'winsuport.o', les       */
/*     quals proporcionen una interficie senzilla per a crear una finestra   */
/*     de text on es poden imprimir caracters en posicions especifiques de   */
/*     la pantalla (basada en CURSES); per tant, el programa necessita ser   */
/*     compilat amb la llibreria 'curses':				     */
/*									     */
/*	   $ gcc tennis0.c winsuport.o -o tennis0 -lcurses		     */
/*	   $ tennis0 fit_param [retard]					     */
/*									     */
/*  Codis de retorn:						  	     */
/*     El programa retorna algun dels seguents codis al SO:		     */
/*	0  ==>  funcionament normal					     */
/*	1  ==>  numero d'arguments incorrecte 				     */
/*	2  ==>  fitxer no accessible					     */
/*	3  ==>  dimensions del taulell incorrectes			     */
/*	4  ==>  parametres de la pilota incorrectes			     */
/*	5  ==>  parametres d'alguna de les paletes incorrectes		     */
/*	6  ==>  no s'ha pogut crear el camp de joc (no pot iniciar CURSES)   */
/*****************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "memoria.h"
#include "winsuport2.h"
#include <pthread.h>

#define MIN_FIL 7		/* definir limits de variables globals */
#define MAX_FIL 25
#define MIN_COL 10
#define MAX_COL 80
#define MIN_PAL 3
#define MIN_VEL -1.0
#define MAX_VEL 1.0
#define MAX_PROCS 10



/* variables globals */
int n_fil, n_col, m_por;	/* dimensions del taulell i porteries */
int l_pal;			/* longitud de les paletes */


int ipu_pf, ipu_pc;      	/* posicio del la paleta d'usuari */

int ipil_pf, ipil_pc;		/* posicio de la pilota, en valor enter */
float pil_pf, pil_pc;		/* posicio de la pilota, en valor real */
float pil_vf, pil_vc;		/* velocitat de la pilota, en valor real*/

int retard;		/* valor del retard de moviment, en mil.lisegons */
int cont;
int tecla;
int moviments=0;
int MAX_MOV=100;		/* maxim nombre de moviments de paleta */
int n_proc=0;
pid_t taula[MAX_PROCS];		/* taula d'identificadors dels processos fill */
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER; /* crea un sem. Global*/


struct paleta_ordinador{

int ipo_pf;
int ipo_pc;      	/* posicio del la paleta de l'ordinador */
float po_pf;	/* pos. vertical de la paleta de l'ordinador, en valor real */
float v_pal;			/* velocitat de la paleta del programa */
};

struct paleta_ordinador ordinador[MAX_PROCS];

char a1[20],a2[20],a3[20],a4[20],a5[20],a6[20],a7[20],a8[20],a9[20],a10[20],a11[20],a12[20],a13[20],a14[20],a15[20];
int id_win,id_moviments,*p_moviments,id_retard,*p_retard,id_tecla,*p_tecla;
int id_cont,*p_cont,id_MAX_MOV,*p_MAX_MOV,id_l_pal,*p_l_pal;

int id_ipo_pf,*p_ipo_pf,id_ipo_pc,*p_ipo_pc;
int id_po_pf,id_v_pal,id_mutex;
float *p_po_pf,*p_v_pal;

void *p_win;
pthread_mutex_t *p_mutex;
	
	
/* funcio per realitzar la carrega dels parametres de joc emmagatzemats */
/* dins un fitxer de text, el nom del qual es passa per referencia en   */
/* 'nom_fit'; si es detecta algun problema, la funcio avorta l'execucio */
/* enviant un missatge per la sortida d'error i retornant el codi per-	*/
/* tinent al SO (segons comentaris del principi del programa).		*/
void carrega_parametres(const char *nom_fit)
{
  FILE *fit;
  
  fit = fopen(nom_fit,"rt");		/* intenta obrir fitxer */
  if (fit == NULL)
  {	fprintf(stderr,"No s'ha pogut obrir el fitxer \'%s\'\n",nom_fit);
  	exit(2);
  }

  if (!feof(fit)) fscanf(fit,"%d %d %d %d\n",&n_fil,&n_col,&m_por,&l_pal);
  if ((n_fil < MIN_FIL) || (n_fil > MAX_FIL) ||
	(n_col < MIN_COL) || (n_col > MAX_COL) || (m_por < 0) ||
	 (m_por > n_fil-3) || (l_pal < MIN_PAL) || (l_pal > n_fil-3))
  {
	fprintf(stderr,"Error: dimensions del camp de joc incorrectes:\n");
	fprintf(stderr,"\t%d =< n_fil (%d) =< %d\n",MIN_FIL,n_fil,MAX_FIL);
	fprintf(stderr,"\t%d =< n_col (%d) =< %d\n",MIN_COL,n_col,MAX_COL);
	fprintf(stderr,"\t0 =< m_por (%d) =< n_fil-3 (%d)\n",m_por,(n_fil-3));
	fprintf(stderr,"\t%d =< l_pal (%d) =< n_fil-3 (%d)\n",MIN_PAL,l_pal,(n_fil-3));
	fclose(fit);
	exit(3);
  }

  if (!feof(fit)) fscanf(fit,"%d %d %f %f\n",&ipil_pf,&ipil_pc,&pil_vf,&pil_vc);
  if ((ipil_pf < 1) || (ipil_pf > n_fil-3) ||
	(ipil_pc < 1) || (ipil_pc > n_col-2) ||
	(pil_vf < MIN_VEL) || (pil_vf > MAX_VEL) ||
	(pil_vc < MIN_VEL) || (pil_vc > MAX_VEL))
  {
	fprintf(stderr,"Error: parametre pilota incorrectes:\n");
	fprintf(stderr,"\t1 =< ipil_pf (%d) =< n_fil-3 (%d)\n",ipil_pf,(n_fil-3));
	fprintf(stderr,"\t1 =< ipil_pc (%d) =< n_col-2 (%d)\n",ipil_pc,(n_col-2));
	fprintf(stderr,"\t%.1f =< pil_vf (%.1f) =< %.1f\n",MIN_VEL,pil_vf,MAX_VEL);
	fprintf(stderr,"\t%.1f =< pil_vc (%.1f) =< %.1f\n",MIN_VEL,pil_vc,MAX_VEL);

	fclose(fit);
	exit(4);
  }

while(n_proc<MAX_PROCS && !feof(fit)){

  if (!feof(fit)) fscanf(fit,"%d %d %f\n",&ordinador[n_proc].ipo_pf,&ordinador[n_proc].ipo_pc,&ordinador[n_proc].v_pal);
  
  
  if ((ordinador[n_proc].ipo_pf < 1) || (ordinador[n_proc].ipo_pf+l_pal > n_fil-2) ||
	(ordinador[n_proc].ipo_pc < 5) || (ordinador[n_proc].ipo_pc > n_col-2) ||
	(ordinador[n_proc].v_pal < MIN_VEL) || (ordinador[n_proc].v_pal > MAX_VEL))
    {
	fprintf(stderr,"Error: parametres paleta ordinador incorrectes:\n");
	fprintf(stderr,"\t1 =< ordinador[n_proc].ipo_pf (%d) =< n_fil-l_pal-3 (%d)\n",ordinador[n_proc].ipo_pf,
			(n_fil-l_pal-3));
	fprintf(stderr,"\t5 =< ordinador[n_proc].ipo_pc (%d) =< n_col-2 (%d)\n",ordinador[n_proc].ipo_pc,
			(n_col-2));
	fprintf(stderr,"\t%.1f =< ordinador[n_proc].v_pal (%.1f) =< %.1f\n",MIN_VEL,ordinador[n_proc].v_pal,MAX_VEL);
	fclose(fit);
	exit(5);
    }
    n_proc++;
    }
  fclose(fit);			/* fitxer carregat: tot OK! */
}




/* funcio per inicialitar les variables i visualitzar l'estat inicial del joc */
int inicialitza_joc(void)
{
  int i, i_port, f_port, retwin,j,k;
  char strin[51];

  retwin = win_ini(&n_fil,&n_col,'+',INVERS);   /* intenta crear taulell */
  
  id_win = ini_mem(retwin);	/* crear zona mem. compartida */
  p_win = map_mem(id_win);	/* obtenir adres. de mem. compartida */
  
  
  win_set(p_win,n_fil,n_col);	/* crea acces a finestra oberta pel proces pare */
  
  
  /* variables a pasar als processos */
  
  
	id_moviments= ini_mem(sizeof(moviments));
	p_moviments = map_mem(id_moviments);	/* obtenir adres. de mem. compartida */
	
	id_retard= ini_mem(sizeof(retard));
	p_retard = map_mem(id_retard);	/* obtenir adres. de mem. compartida */
	
	id_tecla= ini_mem(sizeof(tecla));
	p_tecla = map_mem(id_tecla);	/* obtenir adres. de mem. compartida */
	
	id_cont= ini_mem(sizeof(cont));
	p_cont = map_mem(id_cont);	/* obtenir adres. de mem. compartida */
	
	id_MAX_MOV= ini_mem(sizeof(MAX_MOV));
	p_MAX_MOV = map_mem(id_MAX_MOV);	/* obtenir adres. de mem. compartida */
	
		
	id_v_pal= ini_mem(sizeof(l_pal));
	p_l_pal = map_mem(id_v_pal);	/* obtenir adres. de mem. compartida */	
	
	id_mutex= ini_mem(sizeof(mutex));
	p_mutex = map_mem(id_mutex);	/* obtenir adres. de mem. compartida */	
	
  
  if (retwin < 0)       /* si no pot crear l'entorn de joc amb les curses */
  { fprintf(stderr,"Error en la creacio del taulell de joc:\t");
    switch (retwin)
    {   case -1: fprintf(stderr,"camp de joc ja creat!\n");
                 break;
        case -2: fprintf(stderr,"no s'ha pogut inicialitzar l'entorn de curses!\n");
 		 break;
        case -3: fprintf(stderr,"les mides del camp demanades son massa grans!\n");
                 break;
        case -4: fprintf(stderr,"no s'ha pogut crear la finestra!\n");
                 break;
     }
     return(retwin);
  }

  i_port = n_fil/2 - m_por/2;	    /* crea els forats de la porteria */
  if (n_fil%2 == 0) i_port--;
  if (i_port == 0) i_port=1;
  f_port = i_port + m_por -1;
  for (i = i_port; i <= f_port; i++)
  {	win_escricar(i,0,' ',NO_INV);
	win_escricar(i,n_col-1,' ',NO_INV);
  }


  ipu_pf = n_fil/2; ipu_pc = 3;		/* inicialitzar pos. paletes */
  if (ipu_pf+l_pal >= n_fil-3) ipu_pf = 1;
  
  for(j=0;j<n_proc;j++){
  for (i=0; i< l_pal; i++)	    /* dibuixar paleta inicialment */
 
  {	win_escricar(ipu_pf +i, ipu_pc, '0',INVERS);
	k=j+1;
	char c = k +'0';
	win_escricar(ordinador[j].ipo_pf +i,ordinador[j].ipo_pc, c,INVERS);
  }
  ordinador[j].po_pf = ordinador[j].ipo_pf;		/* fixar valor real paleta ordinador */
}
  pil_pf = ipil_pf; pil_pc = ipil_pc;	/* fixar valor real posicio pilota */
  win_escricar(ipil_pf, ipil_pc, '.',INVERS);	/* dibuix inicial pilota */

  sprintf(strin,"Tecles: \'%c\'-> amunt, \'%c\'-> avall, RETURN-> sortir.",
		TEC_AMUNT, TEC_AVALL);
  win_escristr(strin);
  return(0);
}



/* funcio per moure la pilota; retorna un valor amb alguna d'aquestes	*/
/* possibilitats:							*/
/*	-1 ==> la pilota no ha sortit del taulell			*/
/*	 0 ==> la pilota ha sortit per la porteria esquerra		*/
/*	>0 ==> la pilota ha sortit per la porteria dreta		*/
void * moure_pilota(void * cap)
{
  
  win_set(p_win,n_fil,n_col);	/* crea acces a finestra oberta pel proces pare */
  
  	
  int f_h, c_h;
  char rh,rv,rd;

do{
	
  f_h = pil_pf + pil_vf;		/* posicio hipotetica de la pilota */
  c_h = pil_pc + pil_vc;
  cont = -1;		/* inicialment suposem que la pilota no surt */
  rh = rv = rd = ' ';
  if ((f_h != ipil_pf) || (c_h != ipil_pc))
  {		/* si posicio hipotetica no coincideix amb la pos. actual */
    if (f_h != ipil_pf)		/* provar rebot vertical */
    {	
		pthread_mutex_lock(&*p_mutex);
		rv = win_quincar(f_h,ipil_pc);	/* veure si hi ha algun obstacle */
		pthread_mutex_unlock(&*p_mutex);
		
	if (rv != ' ')			/* si no hi ha res */
	{   pil_vf = -pil_vf;		/* canvia velocitat vertical */
	    f_h = pil_pf+pil_vf;	/* actualitza posicio hipotetica */
	}
    }
    if (c_h != ipil_pc)		/* provar rebot horitzontal */
    {	
		pthread_mutex_lock(&*p_mutex);
		rh = win_quincar(ipil_pf,c_h);	/* veure si hi ha algun obstacle */
		pthread_mutex_unlock(&*p_mutex);
		
	if (rh != ' ')			/* si no hi ha res */
	{    pil_vc = -pil_vc;		/* canvia velocitat horitzontal */
	     c_h = pil_pc+pil_vc;	/* actualitza posicio hipotetica */
	}
    }
    if ((f_h != ipil_pf) && (c_h != ipil_pc))	/* provar rebot diagonal */
		
    {	
		pthread_mutex_lock(&*p_mutex);
		rd = win_quincar(f_h,c_h);
		pthread_mutex_unlock(&*p_mutex);
		
	if (rd != ' ')				/* si no hi ha obstacle */
	{    pil_vf = -pil_vf; pil_vc = -pil_vc;	/* canvia velocitats */
	     f_h = pil_pf+pil_vf;
	     c_h = pil_pc+pil_vc;		/* actualitza posicio entera */
	}
    } 
    pthread_mutex_lock(&*p_mutex);
    if (win_quincar(f_h,c_h) == ' ')	/* verificar posicio definitiva */
    {						/* si no hi ha obstacle */
   
	win_escricar(ipil_pf,ipil_pc,' ',NO_INV);	/* esborra pilota */
	pthread_mutex_unlock(&*p_mutex);
	
	pil_pf += pil_vf; pil_pc += pil_vc;
	ipil_pf = f_h; ipil_pc = c_h;		/* actualitza posicio actual */
	if ((ipil_pc > 0) && (ipil_pc <= n_col)){	/* si no surt */
		
		pthread_mutex_lock(&*p_mutex);
		win_escricar(ipil_pf,ipil_pc,'.',INVERS); /* imprimeix pilota */
		pthread_mutex_unlock(&*p_mutex);
		}
	else{
		
		cont = ipil_pc;	/* codi de finalitzacio de partida */
    }
    
    }
    pthread_mutex_unlock(&*p_mutex);
  }
  else { 
	  
	  pil_pf += pil_vf; pil_pc += pil_vc; }
  
  win_retard(*p_retard);
  
   } while ((*p_tecla != TEC_RETURN) && (*p_cont==-1) && *p_moviments<*p_MAX_MOV);
  
  return(0);
}




/* funcio per moure la paleta de l'usuari en funcio de la tecla premuda */
void * mou_paleta_usuari(void * cap)
{
   win_set(p_win,n_fil,n_col);	/* crea acces a finestra oberta pel proces pare */
  
   do{
 
  *p_tecla = win_gettec();
 
  if (tecla != 0){
  pthread_mutex_lock(&*p_mutex);
  if ((tecla == TEC_AVALL) && (win_quincar(ipu_pf+*p_l_pal,ipu_pc) == ' '))
  {
	
	
    win_escricar(ipu_pf,ipu_pc,' ',NO_INV);	   /* esborra primer bloc */
    pthread_mutex_unlock(&*p_mutex);
   
    ipu_pf++;					   /* actualitza posicio */

	pthread_mutex_lock(&*p_mutex);
    win_escricar(ipu_pf+*p_l_pal-1,ipu_pc,'0',INVERS); /* impri. ultim bloc */
    
    
  }
  pthread_mutex_unlock(&*p_mutex);
  pthread_mutex_lock(&*p_mutex);
  if ((*p_tecla == TEC_AMUNT) && (win_quincar(ipu_pf-1,ipu_pc) == ' '))
  {
	
    win_escricar(ipu_pf+*p_l_pal-1,ipu_pc,' ',NO_INV); /* esborra ultim bloc */
    pthread_mutex_unlock(&*p_mutex);
    
    ipu_pf--;					    /* actualitza posicio */
    
    pthread_mutex_lock(&*p_mutex);
    win_escricar(ipu_pf,ipu_pc,'0',INVERS);	    /* imprimeix primer bloc */

    
  }
  pthread_mutex_unlock(&*p_mutex);
  p_moviments=p_moviments+1;
  }
  
  win_retard(*p_retard);
  
   } while ((*p_tecla != TEC_RETURN) && (*p_cont==-1) && *p_moviments<*p_MAX_MOV);
  
  return(0);
}



/* programa principal				    */
int main(int n_args, const char *ll_args[])
{
	
  pthread_t threads;	
 
	
  int i,n;
  
  if ((n_args != 3) && (n_args !=4))
  {	fprintf(stderr,"Comanda: tennis0 fit_param max_mov [retard]\n");
  	exit(1);
  }
  carrega_parametres(ll_args[1]);
  MAX_MOV=atoi(ll_args[2]);
  if (n_args == 4) retard = atoi(ll_args[3]);
  else retard = 100;
  
  if (inicialitza_joc() !=0)    /* intenta crear el taulell de joc */
     exit(4);   /* aborta si hi ha algun problema amb taulell */

  
  win_set(p_win,n_fil,n_col);	/* crea acces a finestra oberta pel proces pare */
  
  pthread_create(&threads,NULL,moure_pilota,(void *)NULL);
  pthread_create(&threads,NULL,mou_paleta_usuari,(void *)NULL);
  
  
  /* moviments,retard,tecla,cont,MAX_MOV,index,l_pal*/
		
	sprintf(a1,"%i",(id_win));
	sprintf(a2,"%i",(n_fil));
	sprintf(a3,"%i",(n_col));
	sprintf(a4,"%i",(id_moviments));
	sprintf(a5,"%i",(id_retard));
	sprintf(a6,"%i",(id_tecla));
	sprintf(a7,"%i",(id_cont));
	sprintf(a8,"%i",(id_MAX_MOV));
	
	sprintf(a10,"%i",(id_l_pal));
  
  
  n = 0;
  for ( i = 0; i < n_proc; i++)
  {
    taula[n] = fork();		/* crea un nou proces */
    if (taula[n] == (pid_t) 0)		/* branca del fill */
    {
	
	/* variables del struct ordinador */
	
	
	id_ipo_pf= ini_mem(sizeof(ordinador[i].ipo_pf));
	p_ipo_pf = map_mem(id_ipo_pf);	/* obtenir adres. de mem. compartida */	
	
	id_ipo_pc= ini_mem(sizeof(ordinador[i].ipo_pc));
	p_ipo_pc = map_mem(id_ipo_pc);	/* obtenir adres. de mem. compartida */	
	
	id_po_pf= ini_mem(sizeof(ordinador[i].po_pf));
	p_po_pf = map_mem(id_po_pf);	/* obtenir adres. de mem. compartida */	
	
	id_v_pal= ini_mem(sizeof(ordinador[i].v_pal));
	p_v_pal = map_mem(id_v_pal);	/* obtenir adres. de mem. compartida */	
	
	
	
	sprintf(a9,"%i",(i));
	sprintf(a11,"%i",(id_ipo_pf));
	sprintf(a12,"%i",(id_ipo_pc));
	sprintf(a13,"%i",(id_po_pf));
	sprintf(a14,"%i",(id_v_pal));
	sprintf(a15,"%i",(id_mutex));
	
	execlp("./pal_ord3", "pal_ord3", a1, a2, a3, a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,(char *)0);
	fprintf(stderr,"error: no puc executar el process fill \'pal_ord3\'\n");
	exit(0);
    }
    else if (taula[n] > 0) n++;		/* branca del pare */
  }
  
  
  
  do				/********** bucle principal del joc **********/
  {	
	  win_retard(*p_retard);
	  win_update();
	  
  } while ((*p_tecla != TEC_RETURN) && (*p_cont==-1) && *p_moviments<*p_MAX_MOV);
  
  win_fi(); 
  

  elim_mem(id_win);		/* elimina zones de memoria compartida */
  
  /*elim_mem(id_moviments);		
  elim_mem(id_retard);		
  elim_mem(id_tecla);		
  elim_mem(id_cont);		
  elim_mem(id_MAX_MOV);		
  elim_mem(id_l_pal);		
  elim_mem(id_ipo_pc);		
  elim_mem(id_ipo_pf);		
  elim_mem(id_v_pal);		
  elim_mem(id_mutex);		
  elim_mem(id_po_pf);		
  
  */
  
  pthread_mutex_destroy(&mutex);

  if (tecla == TEC_RETURN) printf("S'ha aturat el joc amb la tecla RETURN!\n");
  else { 
	  
	  if(moviments==MAX_MOV) printf ("S'ha arribat al maxim nombre de moviments de paleta \n");
	  else{
	  if (cont == 0) printf("Ha guanyat l'ordinador!\n");
         else printf("Ha guanyat l'usuari!\n"); }}
 
        
  return(0);
}
