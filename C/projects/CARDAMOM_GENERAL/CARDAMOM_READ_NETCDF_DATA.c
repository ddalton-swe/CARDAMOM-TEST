//This file was moddified from CARDAMOM_READ_BINARY_DATA.c
//it attepts to read in a netcdf file and store it
//Note that a loit of this is lifted directly from https://www.unidata.ucar.edu/software/netcdf/docs/simple_nc4_rd_8c-example.html
//TODO: Work on integrating this with the rest of CARDAMOM using CARDAMOM_RUN_MODEL.m



/*INCLUDING THE PARAMETER_INFO structure*/
/*Note: it remains unclear as to where this structure should be defined*/

#pragma once
#include "../../auxi_fun/filediag.c"
#include "../../auxi_fun/oksofar.c"
#include "stdlib.h"
#include "stdio.h"
#include "memory.h"
#include "CARDAMOM_DATA_STRUCTURE.c"

#include <netcdf.h>


#include "CARDAMOM_MODEL_LIBRARY.c"




/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error in $s at $d: %s\n", __FILE__, __LINE__, nc_strerror(e)); exit(ERRCODE);}





/*
 * Function:  ncdf_read_int_attr
 * --------------------
 * Attempts to read a single int that is stored under a particular name in the netCDF file's attributes
 *
 *  ncid: netCDF file ID to pull the data from. This is the id given by nc_open after the netCDF file is opened
 *  context: this is the context in which the attribute was stored. Can be a variable name, or '/' if it is a global attribute
 *  attrName: This is the name of the attibute to read
 *  default: the default value to use if the requested attribute does not exsist
 *
 *  returns: the int attribute read, or default if the requested attribute does not exsist.
 *   if there is an error, the program exits after displaying a message.
 */
int ncdf_read_int_attr(int ncid, const char* context, const char * attrName, int default ){
	int attrResult;
	int varID=NC_GLOBAL;
	if ((context != NULL) && (context[0] != '/')){
		//Attempt to locate the id of the approprate variable
		if ((retval = nc_inq_varid(ncid, context, &varID))){
			//The failure was in locating the context, not the arttr
			ERR(retval);
		}
	}
	if ((retval = nc_get_att_int(ncid, varID, attrName, &attrResult))){
		if (retval ==NC_ENOTATT ){
			return default;
		}
		ERR(retval);
	}
	return attrResult;
}
/*
 * Function:  ncdf_read_double_attr
 * --------------------
 * Attempts to read a single double that is stored under a particular name in the netCDF file's attributes
 *
 *  ncid: netCDF file ID to pull the data from. This is the id given by nc_open after the netCDF file is opened
 *  context: this is the context in which the attribute was stored. Can be a variable name, or '/' if it is a global attribute
 *  attrName: This is the name of the attibute to read
 *  default: the default value to use if the requested attribute does not exsist
 *
 *  returns: the double attribute read. if there is an error, the program exits after displaying a message.
 */
double ncdf_read_double_attr(int ncid, const char* context, const char * attrName, double default ){
	double attrResult;
	int varID=NC_GLOBAL;
	if ((context != NULL) && (context[0] != '/')){
		//Attempt to locate the id of the approprate variable
		if ((retval = nc_inq_varid(ncid, context, &varID))){
			//The failure was in locating the context, not the arttr
			ERR(retval);
		}
	}
	if ((retval = nc_get_att_double(ncid, varID, attrName, &attrResult))){
		if (retval ==NC_ENOTATT ){
			return default;
		}
		ERR(retval);
	}
	return attrResult;
}





/*
 * Function:  ncfd_get_var_info
 * --------------------
 *  Helper method that Attempts to read the length and varID of a 1 dimensional variable that is stored under a particular name in the netCDF file's variables
 *
 *  ncid: netCDF file ID to pull the data from. This is the id given by nc_open after the netCDF file is opened
 *  varName: This is the name of the variable to read
 *  arrayLen: pointer where the total length in number of elements of the resulting array will be written (NOT MEMORY SIZE)
 *  varID: Pointer to where the varID of the requested var will be written
 *
 *  returns: None
 *   if there is an error, the program exits after displaying a message.
 */


void ncfd_get_var_info(int ncid, const char * varName, size_t * arrayLen, int * varID) {
	int numberOfDims;
	int dimensionID;
	//Attempt to locate the id of the approprate variable
	if ((retval = nc_inq_varid(ncid, varName, varID))){
		ERR(retval);
	}
	//check to make sure the variable has only one dim (Critial to assure well-defined behavior for all other ncdf function calls!!!)
	if ((retval = nc_inq_varndims(ncid, *varID, &numberOfDims))){
		ERR(retval);
	}
	if (numberOfDims>1 || numberOfDims<0){
		printf("Error in $s at $d while trying to read var $s: CARDAMOM netCDF only supports 1 and 0 dimensional variables, but var has $d dimensions.\n", __FILE__, __LINE__, varName,numberOfDims);
		exit(1);
	}
	//begin work in preperation for allocating the array where the variable data will be stored.
	if (numberOfDims==0){
		//special case, var is a scalar, so we know the length of the array is 1
		*arrayLen=1;
	}
	if ((retval = nc_inq_vardimid(ncid, *varID, &dimensionID))){
		ERR(retval);
	}
	if ((retval = nc_inq_dimlen(ncid, dimensionID, arrayLen))){
		ERR(retval);
	}
}



/*
 * Function:  ncdf_read_int_var
 * --------------------
 * Attempts to read a 1 dimensional int variable that is stored under a particular name in the netCDF file's variables
 *
 *  ncid: netCDF file ID to pull the data from. This is the id given by nc_open after the netCDF file is opened
 *  varName: This is the name of the variable to read
 *  arrayLen: pointer where the total length in number of elements of the resulting array will be written (NOT MEMORY SIZE)
 *
 *  returns: an array of all the values of the variable,
 *   if there is an error, the program exits after displaying a message.
 */
  int * ncdf_read_int_var(int ncid, const char * varName, size_t * arrayLen ){
	int varID;
	ncfd_get_var_info(ncid, varName, arrayLen, &varID)
	//allocate the actual array we will be returning
	int * resultArray= (int*)calloc(*arrayLen, sizeof(int));

	if ((retval = nc_get_var_int(ncid, varID, resultArray))){
		ERR(retval);
	}
	return resultArray;
}

/*
 * Function:  ncdf_read_double_var
 * --------------------
 * Attempts to read a 1 dimensional double variable that is stored under a particular name in the netCDF file's variables
 *
 *  ncid: netCDF file ID to pull the data from. This is the id given by nc_open after the netCDF file is opened
 *  varName: This is the name of the variable to read
 *  arrayLen: pointer where the total length in number of elements of the resulting array will be written (NOT MEMORY SIZE)
 *
 *  returns: an array of all the values of the variable,
 *   if there is an error, the program exits after displaying a message.
 */
  double * ncdf_read_double_var(int ncid, const char * varName, size_t * arrayLen ){
	int varID;
	ncfd_get_var_info(ncid, varName, arrayLen, &varID)
	//allocate the actual array we will be returning
	int * resultArray= (int*)calloc(*arrayLen, sizeof(double));

	if ((retval = nc_get_var_double(ncid, varID, resultArray))){
		ERR(retval);
	}
	return resultArray;
}

/*
 * Function:  ncdf_read_single_int_var
 * --------------------
 * Attempts to read a 1 dimensional int variable that is stored under a particular name in the netCDF file's variables
 *
 *  ncid: netCDF file ID to pull the data from. This is the id given by nc_open after the netCDF file is opened
 *  varName: This is the name of the variable to read
 *  default: default value to return if the requested variable does not exsist
 *
 *  returns: the value of the variable,
 *   if there is an error, the program exits after displaying a message.
 */
  int ncdf_read_single_int_var(int ncid, const char * varName, int default){
	int varID;
	int result;
	if ((retval = nc_inq_varid(ncid, varName, &varID))){
		if (retval ==NC_ENOTVAR ){
			return default;
		}
	}
	size_t arrayLen;
	ncfd_get_var_info(ncid, varName, &arrayLen, &varID)
	//No need to allocate an array, just check the size and die if it is wrong
	if (arrayLen>1){
		printf("Error in $s at $d while trying to read var $s: ncdf_read_single_int_var only supports one value, but var has $d values.\n", __FILE__, __LINE__, varName,numberOfDims,arrayLen);
		exit(1);
	}

	if ((retval = nc_get_var_int(ncid, varID, &result))){
		ERR(retval);
	}
	return result;
}

/*
 * Function:  ncdf_read_single_double_var
 * --------------------
 * Attempts to read a scalar double variable that is stored under a particular name in the netCDF file's variables
 *
 *  ncid: netCDF file ID to pull the data from. This is the id given by nc_open after the netCDF file is opened
 *  varName: This is the name of the variable to read
 *  default: default value to return if the requested variable does not exsist
 *
 *  returns: the value of the variable,
 *   if there is an error, the program exits after displaying a message.
 */
  double ncdf_read_single_double_var(int ncid, const char * varName, double default ){
	int varID;
	double result;
	if ((retval = nc_inq_varid(ncid, varName, &varID))){
		if (retval ==NC_ENOTVAR ){
			return default;
		}
	}
	size_t arrayLen;
	ncfd_get_var_info(ncid, varName, &arrayLen, &varID)
	//No need to allocate an array, just check the size and die if it is wrong
	if (arrayLen>1){
		printf("Error in $s at $d while trying to read var $s: ncdf_read_single_double_var only supports one value, but var has $d values.\n", __FILE__, __LINE__, varName,numberOfDims,arrayLen);
		exit(1);
	}
	if ((retval = nc_get_var_double(ncid, varID, &result))){
		ERR(retval);
	}
	return result;
}





int CARDAMOM_DATA_CHECKS(DATA *CDATA){
/*General Checks*/
printf("***CBF FILE SUMMARY***");
printf("MODEL ID = %d\n",CDATA->ID);
printf("No days = %d\n",CDATA->nodays);
printf("Mean Rad = %f\n",CDATA->meanrad);
printf("Mean Temp = %f\n",CDATA->meantemp);
printf("Mean Prec = %f\n",CDATA->meanprec);
printf("Latitude = %f\n",CDATA->LAT);
printf("Number of MET drivers%d\n",CDATA->nomet);
printf("Number of GPP obs. = %d\n",CDATA->ngpp);
printf("Number of LAI obs. = %d\n",CDATA->nlai);
printf("Number of NEE obs. = %d\n",CDATA->nnee);
printf("Number of CH4 obs. = %d\n",CDATA->nch4);
printf("Ecological & Dynamic Constraints options\n");
printf("EDC likelihood option = %d\n",CDATA->EDC);
printf("EDC diagnostics option = %d\n",CDATA->EDCDIAG);
printf("*****END OF CBF FILE SUMMARY***");
return 0;}




/*
 * Function:  CARDAMOM_READ_NETCDF_DATA
 * --------------------
 * Attempts to read in the cardamom netcdf file
 *
 * filename: The path of the file to be read
 * DATA: The DATA struct to read the data into
 *
 *
 *	NOTE: if you intend to modify what is read in when reading a netCDF file,
 *  You MUST edit both this method, and the DATA struct in CARDAMOM/C/projects/CARDAMOM_GENERAL/CARDAMOM_DATA_STRUCTURE.c
 */
int CARDAMOM_READ_NETCDF_DATA(char *filename,DATA *DATA)
{

/*NOTE: this function reads data as written by DALEC_FLUXCOM_MCMC_PROJECT_SITELEVEL.m
 * For any adaptations to this function make sure to keep in sync with matlab function*/

/*TEMPLATE FOR ALL DALEC MCMC DATA files*/
/*Static Elements: 1-100 - use as many as needed*/
/*Parameter Priors: 101-150*/
/*Parameter prior uncertainty: 151-200*/
/*Other priors & uncertainties: 201-300*/
/*TEMPORAL DRIVERS & DATA: 301-end*/



/* Open the file. NC_NOWRITE tells netCDF we want read-only access
	 * to the file.*/
int ncid, retval;
if ((retval = nc_open(filename, NC_NOWRITE, &ncid))){
	ERR(retval);
}

/*READING STATIC DATA*/
printf("we got to the netcdf reading part! Killing self!");
exit(1);



/*DALEC model run info*/
DATA->ID=ncdf_read_int_attr(ncid, NC_GLOBAL, "ID" ,0);
DATA->LAT=ncdf_read_double_attr(ncid, NC_GLOBAL, "LAT", 0.0);
DATA->nodays=ncdf_read_int_attr(ncid, NC_GLOBAL, "nodays", 0);
DATA->nomet=ncdf_read_int_attr(ncid, NC_GLOBAL, "nomet", 0);
DATA->noobs=ncdf_read_int_attr(ncid, NC_GLOBAL, "noobs", 0);
DATA->EDC=ncdf_read_int_attr(ncid, NC_GLOBAL, "EDC", 0);
DATA->EDCDIAG=ncdf_read_int_attr(ncid, NC_GLOBAL, "EDCDIAG", 0 );
DATA->gppabs=ncdf_read_int_attr(ncid, NC_GLOBAL, "gppabs", 0 );
/*DALEC MCMC run info*/
/*set to 1 for (a) few and (b) well constrained priors, otherwise 0*/
/*binary file mcmc options (need to add all options HERE except inout files)*/
DATA->edc_random_search=ncdf_read_int_attr(ncid, NC_GLOBAL, "edc_random_search", 0 );
DATA->gppiav=ncdf_read_int_attr(ncid, NC_GLOBAL, "gppiav", 0 );
DATA->laiiav=ncdf_read_int_attr(ncid, NC_GLOBAL, "laiiav", 0 );
DATA->nee_annual_unc=ncdf_read_double_attr(ncid, NC_GLOBAL, "nee_annual_unc", 0.0 );
DATA->et_annual_unc=ncdf_read_double_attr(ncid, NC_GLOBAL, "et_annual_unc", 0.0 );
DATA->nee_obs_unc=ncdf_read_double_attr(ncid, NC_GLOBAL, "nee_obs_unc", 0.5 );
DATA->et_obs_unc=ncdf_read_double_attr(ncid, NC_GLOBAL, "et_obs_unc", 2.0 );
DATA->ewt_annual_unc=ncdf_read_double_attr(ncid, NC_GLOBAL, "ewt_annual_unc", 0.0);
DATA->ewt_obs_unc=ncdf_read_double_attr(ncid, NC_GLOBAL, "ewt_obs_unc", 0.0);
DATA->gpp_annual_unc=ncdf_read_double_attr(ncid, NC_GLOBAL, "gpp_annual_unc", 0.0);
DATA->gpp_obs_unc=ncdf_read_double_attr(ncid, NC_GLOBAL, "gpp_obs_unc", 2.0);
DATA->et_obs_threshold=ncdf_read_double_attr(ncid, NC_GLOBAL, "et_obs_threshold", 0.0);
DATA->gpp_obs_threshold=ncdf_read_double_attr(ncid, NC_GLOBAL, "gpp_obs_threshold", 0.0);
DATA->ch4iav=ncdf_read_int_attr(ncid, NC_GLOBAL, "ch4iav", 0); /*shuang*/
DATA->ch4_annual_unc=ncdf_read_double_attr(ncid, NC_GLOBAL, "ch4_annual_unc", 0.0);  /*shuang*/
DATA->ch4_obs_unc=ncdf_read_double_attr(ncid, NC_GLOBAL, "ch4_obs_unc", 0.5); /*shuang*/
DATA->ch4_obs_threshold=ncdf_read_double_attr(ncid, NC_GLOBAL, "ch4_obs_threshold", 0.0); /*shuang*/

/*UP TO USER to read data and allocate it to DATA structure*/
double parpriors[50],parpriorunc[50],otherpriors[50],otherpriorunc[50];
fread(parpriors,sizeof(double),50,fid);
fread(parpriorunc,sizeof(double),50,fid);
fread(otherpriors,sizeof(double),50,fid);
fread(otherpriorunc,sizeof(double),50,fid);

/*For universal data structure, DATA contains 50 parameter prior spaces
 * use as many as needed!*/
memcpy(DATA->parpriors,parpriors,50*sizeof(double));
memcpy(DATA->parpriorunc,parpriorunc,50*sizeof(double));
memcpy(DATA->otherpriors,otherpriors,50*sizeof(double));
memcpy(DATA->otherpriorunc,otherpriorunc,50*sizeof(double));


/*loading model specific values*/
/*this includes information on the number of pools, number of parameters, etc.
?*this allows an initialization of the model output fields (stored in DATA for simplicity)*/

CARDAMOM_MODEL_LIBRARY(DATA);

/*the following fields (begining by M_) are for storage purposes*/
/*data stored in these fields is over-written at each model run*/
/*Future versions: these should be declared within each model structure
or generically declared for model types (e.g. DALEC, etc).*/
/*Model-specific quantities (nodays, nofluxes, nopools), will be declared in MODEL_INFO for modularity*/
DATA->M_LAI=calloc(DATA->nodays,sizeof(double));
DATA->M_GPP=calloc(DATA->nodays,sizeof(double));
DATA->M_NEE=calloc(DATA->nodays,sizeof(double));

printf("DATA->nodays = %i\n", DATA->nodays);
printf("DATA->nofluxes = %i\n", DATA->nofluxes);

DATA->M_FLUXES=calloc(DATA->nodays*DATA->nofluxes,sizeof(double));
DATA->M_POOLS=calloc((DATA->nodays+1)*DATA->nopools,sizeof(double));
int noedc=100, noprob=1;
DATA->M_EDCD=calloc(noedc,sizeof(int));
DATA->M_P=calloc(noprob,sizeof(double));
DATA->M_leo=calloc(1,sizeof(double));
DATA->M_leo[0]=1.0/0.0;

DATA->M_PARS=calloc(DATA->nopars,sizeof(double));


/*READING TEMPORAL DATA*/
/*For re-use of DATA structure (presumably), the data is only re-read if the fields are set to zero (or initialized)*/
/*Currently not sure why this is here: however, this is harmless*/
/*also it is good practice to initialize pointers in C*/
if (DATA->MET==0){DATA->MET=calloc(DATA->nomet*DATA->nodays,sizeof(double));}
if (DATA->GPP==0){DATA->GPP=calloc(DATA->nodays,sizeof(double));}
if (DATA->NEE==0){DATA->NEE=calloc(DATA->nodays,sizeof(double));}
if (DATA->LAI==0){DATA->LAI=calloc(DATA->nodays,sizeof(double));}
if (DATA->WOO==0){DATA->WOO=calloc(DATA->nodays,sizeof(double));}
if (DATA->ET==0){DATA->ET=calloc(DATA->nodays,sizeof(double));}
if (DATA->EWT==0){DATA->EWT=calloc(DATA->nodays,sizeof(double));}
if (DATA->BAND1==0){DATA->BAND1=calloc(DATA->nodays,sizeof(double));}
if (DATA->BAND2==0){DATA->BAND2=calloc(DATA->nodays,sizeof(double));}
if (DATA->BAND3==0){DATA->BAND3=calloc(DATA->nodays,sizeof(double));}
if (DATA->BAND4==0){DATA->BAND4=calloc(DATA->nodays,sizeof(double));}
if (DATA->SOM==0){DATA->SOM=calloc(DATA->nodays,sizeof(double));}
if (DATA->NEEunc==0){DATA->NEEunc=calloc(DATA->nodays,sizeof(double));}
if (DATA->CH4==0){DATA->CH4=calloc(DATA->nodays,sizeof(double));}

DATA->ngpp=0;
DATA->nlai=0;
DATA->nnee=0;
DATA->nwoo=0;
DATA->net=0;
DATA->newt=0;
DATA->nband1=0;
DATA->nband2=0;
DATA->nband3=0;
DATA->nband4=0;
DATA->nsom=0;
DATA->nneeunc=0;
DATA->nch4=0; /*shuang*/



/*6 met fields for DALEC CDEA*/

int n, nn;
double *metline, *obsline;
metline=calloc(DATA->nomet,sizeof(double));
obsline=calloc(DATA->noobs,sizeof(double));
/*mean met and mean obs line, in case user-defined*/


int frfm,frfo;


printf("reading file...\n");
for (n=0;n<DATA->nodays+1;n++){
	frfm=fread(metline,sizeof(double),DATA->nomet,fid);
	frfo=fread(obsline,sizeof(double),DATA->noobs,fid);
	if (n<DATA->nodays){
	for (nn=0;nn<DATA->nomet;nn++){DATA->MET[n*DATA->nomet+nn]=metline[nn];}

	DATA->GPP[n]=obsline[0];
	DATA->LAI[n]=obsline[1];
	DATA->NEE[n]=obsline[2];
	if (obsline[0]>-9998){DATA->ngpp=DATA->ngpp+1;}
	if (obsline[1]>-9998){DATA->nlai=DATA->nlai+1;}
	if (obsline[2]>-9998){DATA->nnee=DATA->nnee+1;}
	if (DATA->noobs>3){DATA->WOO[n]=obsline[3];
	if (obsline[3]>-9998){DATA->nwoo=DATA->nwoo+1;}}
        if (DATA->noobs>4){DATA->ET[n]=obsline[4];
        if (obsline[4]>-9998){DATA->net=DATA->net+1;}}
        if (DATA->noobs>5){DATA->EWT[n]=obsline[5];
        if (obsline[5]>-9998){DATA->newt=DATA->newt+1;}}

        if (DATA->noobs>6){DATA->BAND1[n]=obsline[6];
        if (obsline[6]>-9998){DATA->nband1=DATA->nband1+1;}}

        if (DATA->noobs>7){DATA->BAND2[n]=obsline[7];
        if (obsline[7]>-9998){DATA->nband2=DATA->nband2+1;}}

        if (DATA->noobs>8){DATA->BAND3[n]=obsline[8];
        if (obsline[8]>-9998){DATA->nband3=DATA->nband3+1;}}

        if (DATA->noobs>9){DATA->BAND4[n]=obsline[9];
        if (obsline[9]>-9998){DATA->nband4=DATA->nband4+1;}}

        if (DATA->noobs>10){DATA->SOM[n]=obsline[10];
        if (obsline[10]>-9998){DATA->nsom=DATA->nsom+1;}}

        if (DATA->noobs>11){DATA->NEEunc[n]=obsline[11];
        if (obsline[11]>-9998){DATA->nneeunc=DATA->nneeunc+1;}}

        if (DATA->noobs>12){DATA->CH4[n]=obsline[12];
        if (obsline[12]>-9998){DATA->nch4=DATA->nch4+1;}}        /*shuang*/

};

}


DATA->deltat=DATA->MET[DATA->nomet]-DATA->MET[0];



fclose(fid);

if (DATA->ngpp>0){DATA->gpppts=calloc(DATA->ngpp,sizeof(int));}
if (DATA->nlai>0){DATA->laipts=calloc(DATA->nlai,sizeof(int));}
if (DATA->nnee>0){DATA->neepts=calloc(DATA->nnee,sizeof(int));}
if (DATA->nwoo>0){DATA->woopts=calloc(DATA->nwoo,sizeof(int));}
if (DATA->net>0){DATA->etpts=calloc(DATA->net,sizeof(int));}
if (DATA->newt>0){DATA->ewtpts=calloc(DATA->newt,sizeof(int));}
if (DATA->nband1>0){DATA->band1pts=calloc(DATA->nband1,sizeof(int));}
if (DATA->nband2>0){DATA->band2pts=calloc(DATA->nband2,sizeof(int));}
if (DATA->nband3>0){DATA->band3pts=calloc(DATA->nband3,sizeof(int));}
if (DATA->nband4>0){DATA->band4pts=calloc(DATA->nband4,sizeof(int));}
if (DATA->nsom>0){DATA->sompts=calloc(DATA->nsom,sizeof(int));}
if (DATA->nneeunc>0){DATA->neeuncpts=calloc(DATA->nneeunc,sizeof(int));}
if (DATA->nch4>0){DATA->ch4pts=calloc(DATA->nch4,sizeof(int));}       /*shuang*/

/*Deriving laipts, gpppts, neepts*/
int c;
c=0;for (n=0;n<DATA->nodays;n++){if (DATA->GPP[n]>-9998){DATA->gpppts[c]=n;c=c+1;}};
c=0;for (n=0;n<DATA->nodays;n++){if (DATA->LAI[n]>-9998){DATA->laipts[c]=n;c=c+1;}};
c=0;for (n=0;n<DATA->nodays;n++){if (DATA->NEE[n]>-9998){DATA->neepts[c]=n;c=c+1;}};
if (DATA->noobs>3){c=0;for (n=0;n<DATA->nodays;n++){if (DATA->WOO[n]>-9998){DATA->woopts[c]=n;c=c+1;}}}

if (DATA->noobs>4){c=0;for (n=0;n<DATA->nodays;n++){if (DATA->ET[n]>-9998){DATA->etpts[c]=n;c=c+1;}}}

if (DATA->noobs>5){c=0;for (n=0;n<DATA->nodays;n++){if (DATA->EWT[n]>-9998){DATA->ewtpts[c]=n;c=c+1;}}}

if (DATA->noobs>6){c=0;for (n=0;n<DATA->nodays;n++){if (DATA->BAND1[n]>-9998){DATA->band1pts[c]=n;c=c+1;}}}

if (DATA->noobs>7){c=0;for (n=0;n<DATA->nodays;n++){if (DATA->BAND2[n]>-9998){DATA->band2pts[c]=n;c=c+1;}}}

if (DATA->noobs>8){c=0;for (n=0;n<DATA->nodays;n++){if (DATA->BAND3[n]>-9998){DATA->band3pts[c]=n;c=c+1;}}}

if (DATA->noobs>9){c=0;for (n=0;n<DATA->nodays;n++){if (DATA->BAND4[n]>-9998){DATA->band4pts[c]=n;c=c+1;}}}

if (DATA->noobs>10){c=0;for (n=0;n<DATA->nodays;n++){if (DATA->SOM[n]>-9998){DATA->sompts[c]=n;c=c+1;}}}

if (DATA->noobs>11){c=0;for (n=0;n<DATA->nodays;n++){if (DATA->NEEunc[n]>-9998){DATA->neeuncpts[c]=n;c=c+1;}}}

if (DATA->noobs>12){c=0;for (n=0;n<DATA->nodays;n++){if (DATA->CH4[n]>-9998){DATA->ch4pts[c]=n;c=c+1;}}} /*shuang*/

/*deriving mean temp and mean rad*/

DATA->meantemp=0;
DATA->meanrad=0;
DATA->meanprec=0;

/*2 options:*/
/*1. derive mean met values based on "MET" (frfm = DATA->nomet)*/
/*2. prescribe user-provided mean met values (frfm = 0)*/

if (frfm==0){
for (n=0;n<DATA->nodays;n++){DATA->meantemp+=0.5*DATA->MET[DATA->nomet*n+1]/(double)DATA->nodays;}
for (n=0;n<DATA->nodays;n++){DATA->meantemp+=0.5*DATA->MET[DATA->nomet*n+2]/(double)DATA->nodays;}
for (n=0;n<DATA->nodays;n++){DATA->meanrad+=DATA->MET[DATA->nomet*n+3]/(double)DATA->nodays;}
/*only if no met > 6*/
if (DATA->nomet>8){
for (n=0;n<DATA->nodays;n++){DATA->meanprec+=DATA->MET[DATA->nomet*n+8]/(double)DATA->nodays;}};
printf("No prescribed met reference means, calculating based on driver data\n");
}
else if (frfm==DATA->nomet){
DATA->meantemp=0.5*metline[1] + 0.5*metline[2];
DATA->meanrad=metline[3];
if (DATA->nomet>6){DATA->meanprec=metline[8];}
printf("Using prescribed met reference means\n");
}

printf("frf = %i\n",frfm);


CARDAMOM_DATA_CHECKS(DATA);

free(metline);
free(obsline);

return 0;



}




int INITIALIZE_DATA_STRUCT(DATA *CDATA){

/*initialising array pointers as zero*/
/*NOTE: These may need to be set to NULL*/
CDATA->MET=0;
CDATA->GPP=0;
CDATA->LAI=0;
CDATA->NEE=0;
CDATA->WOO=0;
CDATA->ET=0;
CDATA->EWT=0;
CDATA->BAND1=0;
CDATA->BAND2=0;
CDATA->BAND3=0;
CDATA->BAND4=0;
CDATA->SOM=0;
CDATA->NEEunc=0;
CDATA->CH4=0; /*shuang*/

return 0;

}


int FREE_DATA_STRUCT(DATA DATA){


if (DATA.ngpp>0){free(DATA.gpppts);}
if (DATA.nlai>0){free(DATA.laipts);}
if (DATA.nnee>0){free(DATA.neepts);}
if (DATA.nwoo>0){free(DATA.woopts);}
if (DATA.net>0){free(DATA.etpts);}
if (DATA.newt>0){free(DATA.ewtpts);}
if (DATA.nband1>0){free(DATA.band1pts);}
if (DATA.nband2>0){free(DATA.band2pts);}
if (DATA.nband3>0){free(DATA.band3pts);}
if (DATA.nband4>0){free(DATA.band4pts);}
if (DATA.nsom>0){free(DATA.sompts);}
if (DATA.nneeunc>0){free(DATA.neeuncpts);}
if (DATA.nch4>0){free(DATA.ch4pts);} /*shuang*/

free(DATA.MET);
free(DATA.LAI);
free(DATA.NEE);
free(DATA.WOO);
free(DATA.ET);
free(DATA.GPP);
free(DATA.EWT);
free(DATA.BAND4);
free(DATA.BAND3);
free(DATA.BAND2);
free(DATA.BAND1);
free(DATA.SOM);
free(DATA.NEEunc);
free(DATA.CH4);/*shuang*/

free(DATA.M_PARS);
free(DATA.M_LAI);
free(DATA.M_GPP);
free(DATA.M_FLUXES);
free(DATA.M_NEE);
free(DATA.M_POOLS);
free(DATA.M_P);
free(DATA.M_EDCD);

free(DATA.parmin);
free(DATA.parmax);
free(DATA.M_leo);

return 0;

}
