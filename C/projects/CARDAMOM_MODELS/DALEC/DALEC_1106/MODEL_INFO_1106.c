#pragma once
#include "PARS_INFO_1106.c"
#include "DALEC_1106.c"
#include "EDC1_1106.c"
#include "EDC2_1106.c"
#include "../../../COST_FUNCTION/MODEL_LIKELIHOOD_FUNCTIONS/DALEC_MLF_DEDC.c"
#include "../../../COST_FUNCTION/MODEL_LIKELIHOOD_FUNCTIONS/DALEC_MLF.c"
#include "../../../DALEC_CODE/EDCs/EDCSETUP.c"
#include "../../../CARDAMOM_GENERAL/CARDAMOM_MODEL_LIBRARY.c"

int MODEL_INFO_1106(DATA * DATA){

/*Step 1. Declare structure*/
/*"static" ensures that the memory is declared in one instance and visible to all functions (I think)*/
static DALEC DALECmodel;DALEC_1106_MODCONFIG(&DALECmodel);
static MLF MLF;DALEC_MLF_DEDC_MODCONFIG(&MLF);

/*Step 2: Fill structure with model-specific info*/

//Corresponding likelihood function 



/*Short-term: copy quantities into DATA structure to reduce dependencies in CARDAMOM_MODEL_LIBRARY.c*/
/*Long-term: remove dependencies on DATA.nofluxes... etc. in CARDAMOM_READ_BINARY_DATA and DALEC_ALL_LIKELIHOOD.c*/
DATA->nopools=DALECmodel.nopools;
DATA->nopars=DALECmodel.nopars;
DATA->nofluxes=DALECmodel.nofluxes;
DATA->nolikelihoods=MLF.nolikelihoods;


/*All model functions*/
/*User is able to add further functions as deemed necessary*/
/*Function names are declared in ../DALEC_ALL/DALEC_MODULE.c*/
/*Consider starting new module for radically different model structures*/
DALECmodel.edc1=EDC1_1106;
DALECmodel.edc2=EDC2_1106;





/*Initialize parameter fields*/
/*initializing parmin and parmax fields*/
/*Currently assigned to "DATA", since MCMC needs info separately*/

/*
DATA->parmin=calloc(DATA->nopars,sizeof(double));
DATA->parmax=calloc(DATA->nopars,sizeof(double));
*/
INITIALIZE_PARAMETER_FIELDS(DATA);

PARS_INFO_1106(DATA);

//oksofar("about to declare EDCD");
//printf("DALECmodel.EDCD = %p\n",DALECmodel.EDCD); //DALECmodel.EDCD Needs to be initilized!
/*Initialize the EDCD structure*/
EDCSETUP(*DATA,&DALECmodel.EDCD);




/*initializing model*/
DATA->MODEL=&DALECmodel;
/*Pointing to MLF*/
DATA->MLF=MLF.mlf;
/*Pointing to EMLF*/
DATA->EMLF=EDC_DALEC_MLF_BINARY;




return 0;}
