#include <assert.h>
#include "../../auxi_fun/oksofar.c"
#include "../../auxi_fun/okcheck.c"
#include "../../auxi_fun/seedrandomnumber.c"

/*defines all the structures, i.e. DATA, MCOPT, PI*/

#include "../../mcmc_fun/MHMCMC/MCMC_FUN/MCMCOPT.c"
#include "MCMC_SETUP/MCMC_MODULES.c"


#include "../../mcmc_fun/MHMCMC/MCMC_FUN/MHMCMC.c"
/*#include "../../mcmc_fun/MHMCMC/MCMC_FUN/DEMCMC.c"
*/


int main(int argc,char *CLA[]){
/*To correctly set-up the MHMCMC*/

/*inputs*/
/*1. met file in*/
/*2. results file out*/
/*3. number of MCMC solutions requested*/
/*4. print-to-screen frequency*/
/*5. write-to-file frequency*/




/*SETTING number of command line inputs as char in CLA[0]*/
sprintf(CLA[0],"%d",argc-1);
/*declaring CARDAMOM Binary Format (.cbf) file*/
char CBFfile[200];
/*setting default filename*/
/*this mode can be routinely used for testing*/
if (argc-1<1){strcpy(CBFfile,"MCMC_SETUP/TEST_BINARY_DATASET.cbf");}
/*otherwise first argument is filename*/
else {strcpy(CBFfile,CLA[1]);}





/*defining MCMC_OPTIONS structure*/
MCMC_OPTIONS MCOPT;


/*defining the MCMC output structure*/
MCMC_OUTPUT MCOUT;


/*These lines guarantee high frequency random generator seeding*/
if (argc-1<2){seedrandomnumber(CBFfile);}else{seedrandomnumber(CLA[2]);}


/*Defining all MCMC components*/
/*USER DEFINED: SETUP MCMC - templates provides*/
/*NOTE  : READ_PARI_DATA function is stored in DALEC_CDEA_TEMPLATE/MCMC_SETUP/MCMC_MODULES.c*/
/*TO DO : (a) read CARDADATA first - note that this includes model specific fields, such as nomet, nopars, etc.
        these are all loaded via the CARDAMOM_MODEL_LIBRARY(CARDADATA) function*/
/*      : (b) read PI based on CARDADATA*/



/*OK is output flag from all functions*/
int OK;

/***********CARDATA STRUCTURE*************/

/*defining data structure*/
DATA CARDADATA;
/*Initialize data structure - this function is found in CARDAMOM_READ_BINARY_DATA*/
OK=INITIALIZE_DATA_STRUCT(&CARDADATA);
okcheck(OK,"Main data structure initialized");
/*read cardamom data from file*/
/*Function also performs and displays basic checks*/
OK=CARDAMOM_READ_BINARY_DATA(CBFfile,&CARDADATA);
okcheck(OK,"Main data structure read successfully");

printf("CARDAMOM MODEL ID = %i\n",CARDADATA.ID);


/***************PI STRUCTURE AND MLF*****************/


/*defining parameter structure*/
/*this structure is defined as part of the MCMC
Full function can be found in mcmc_fun/MHMCMC/MCMC_FUN/MCMC_OPT.c*/
PARAMETER_INFO PI;

/*initializing structure with correct PI fields (as required by MHMCMC)*/
/*Function is in MCMC_MODULES.c*/
OK=INITIALIZE_PI_STRUCT(&PI,&CARDADATA);
okcheck(OK,"Parameter info structure initialized");


/*choose model likelihood here*/
/*the MLF function determines the probability of any model output based on data and/or other constraints*/
/*In future versions: this should be done inside CARDAMOM_READ_BINARY_DATA*/



/*READ_PARI_DATA and READ_MCOPT should now be generic for all model types*/
OK=READ_PARI_DATA(&PI, &CARDADATA, &MCOUT, CLA);
okcheck(OK,"READ_PARI_DATA successfully executed");
OK=READ_MCOPT(&MCOPT,CLA);
okcheck(OK,"Main data structure read successfully");


/*calling the MHMCMC here*/
oksofar("Pre-MCMC step size");
int n;
for (n=0;n<PI.npars;n++){printf("%6.6f ",PI.stepsize[n]);}
printf("\n");
printf("CARDADATA->PCrotate = %i\n",CARDADATA.PCrotate);

MHMCMC(CARDADATA.MLF,CARDADATA,PI,MCOPT,&MCOUT);

/*???????*/
/*User Defined function needed to clean up memory*/
MEMORY_CLEANUP(CARDADATA,PI,MCOPT,MCOUT);

return 0;

}





