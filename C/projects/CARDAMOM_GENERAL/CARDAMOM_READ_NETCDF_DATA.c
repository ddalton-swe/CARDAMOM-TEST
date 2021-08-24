//This file was moddified from CARDAMOM_READ_BINARY_DATA.c
//it attepts to read in a netcdf file and store it
//Note that a lot of this is derived from https://www.unidata.ucar.edu/software/netcdf/docs/simple_nc4_rd_8c-example.html

#pragma once
#include "../../auxi_fun/filediag.c"
#include "../../auxi_fun/oksofar.c"
#include "stdlib.h"
#include "stdio.h"
#include "memory.h"
#include <math.h>
#include "CARDAMOM_NETCDF_DATA_STRUCTURE.c"

#include <netcdf.h>


#define DEFAULT_DOUBLE_VAL -9999.0
#define DEFAULT_INT_VAL -9999


//NOTE ABOUT THIS MACRO:
//If set to 1, netCDF methods will continue to run and return with default values if they fail to find the requested variable or attribute
//if set to 0, they will instantly die on failing to find any variable or attribute
//This is meant as a debugging tool more than anything else, and can be used with a "fully complete" netcdf cardamom file containing every variable in order to check if there were typeos in the c code.
//In the production enviroment, this should always be set to 1
#define ALLOW_DEFAULTS 1


//#include "CARDAMOM_MODEL_LIBRARY.c"




/* Handle errors by printing an error message and exiting with a
 * non-zero status.*/

#define ERRCODE 2
#define ERR(e) {printf("Error in %s at %d: %s\n", __FILE__, __LINE__, nc_strerror(e)); exit(ERRCODE);}
//Helper for errors in variable-getting methods
#define ERR_VAR(e, var) {printf("Error in %s at %d with variable \"%s\": %s\n", __FILE__, __LINE__, var, nc_strerror(e)); exit(ERRCODE);}
//Helper for errors in attribute-getting methods
#define ERR_ATTR_AND_CONTEXT(e, attr, context, varID) {printf("Error in %s at %d with attribute \"%s\" in the context \"%s\" (%d):  %s\n", __FILE__, __LINE__, attr,context,varID, nc_strerror(e)); exit(ERRCODE);}





/*
 * Function:  ncdf_read_int_attr
 * --------------------
 * Attempts to read a single int that is stored under a particular name in the netCDF file's attributes
 *
 *  ncid: netCDF file ID to pull the data from. This is the id given by nc_open after the netCDF file is opened
 *  context: this is the context in which the attribute was stored. Can be a variable name, or '/' if it is a global attribute
 *  attrName: This is the name of the attibute to read
 *
 *  returns: the int attribute read, or DEFAULT_INT_VAL if the requested attribute does not exsist.
 *   if there is an error, the program exits after displaying a message.
 */



int ncdf_read_int_attr(int ncid, const char* context, const char * attrName){
	int retval =0; //Return value variable for NCDF calls.
	int attrResult;
	int varID=NC_GLOBAL;
	if ((context != NULL) && (context[0] != '/')){
		//Attempt to locate the id of the approprate variable
		if ((retval = nc_inq_varid(ncid, context, &varID))){
			if (retval ==NC_ENOTVAR && ALLOW_DEFAULTS){
				//we failed to find the context (and so we never even looked for the attribute!)
				//we will return the default double value
				return DEFAULT_INT_VAL;
			}
			//The failure was in locating the context, not the arttr
			ERR_ATTR_AND_CONTEXT(retval,attrName, context,varID );
		}
	}
	if ((retval = nc_get_att_int(ncid, varID, attrName, &attrResult))){
		if (retval ==NC_ENOTATT && ALLOW_DEFAULTS ){
			return DEFAULT_INT_VAL;
		}
		ERR_ATTR_AND_CONTEXT(retval,attrName, context, varID);
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
 *
 *  returns: the double attribute read. if there is an error, the program exits after displaying a message.
 */
double ncdf_read_double_attr(int ncid, const char* context, const char * attrName ){
  int retval =0; //Return value variable for NCDF calls.
	double attrResult;
	int varID=NC_GLOBAL;
	if ((context != NULL) && (context[0] != '/')){
		//Attempt to locate the id of the approprate variable
		if ((retval = nc_inq_varid(ncid, context, &varID))){
			if (retval ==NC_ENOTVAR && ALLOW_DEFAULTS){
				//we failed to find the context (and so we never even looked for the attribute!)
				//we will return the default double value
				return DEFAULT_DOUBLE_VAL;
			}
			//Note that The failure was in locating the context, not the attr
			ERR_ATTR_AND_CONTEXT(retval,attrName, context,varID );
		}
	}
	if ((retval = nc_get_att_double(ncid, varID, attrName, &attrResult))){
		if (retval ==NC_ENOTATT && ALLOW_DEFAULTS){
			//we failed to find the attribute, so we should return the default double value
			return DEFAULT_DOUBLE_VAL;
		}
		ERR_ATTR_AND_CONTEXT(retval,attrName, context,varID);
	}
	return attrResult;
}





/*
 * Function:  ncfd_get_var_info
 * --------------------
 *  Helper method that Attempts to read the length and varID of a 1 or 0 dimensional variable that is stored under a particular name in the netCDF file's variables
 *
 *  ncid: netCDF file ID to pull the data from. This is the id given by nc_open after the netCDF file is opened
 *  varName: This is the name of the variable to read
 *  arrayLen: pointer where the total length in number of elements of the resulting array will be written (NOT MEMORY SIZE)
 *  varID: Pointer to where the varID of the requested var will be written
 *
 *  returns: 0 if the variable does not exsist (and the macro ALLOW_DEFAULTS is set), 1 if we found the variable and functioned normally
 *   if there is an error, the program exits after displaying a message.
 */


int ncfd_get_var_info(int ncid, const char * varName, size_t * arrayLen, int * varID) {
  int retval =0; //Return value variable for NCDF calls.
	int numberOfDims;
	int dimensionID;
	//Attempt to locate the id of the approprate variable
	if ((retval = nc_inq_varid(ncid, varName, varID))){
		//Test if the variable just does not exsist, and if we are allowed to just give a default.
		if (retval ==NC_ENOTVAR && ALLOW_DEFAULTS){
			//We did not find anything, but this is still a valid result. Make sure everything is set.
			*arrayLen = 0;
			*varID=-1;
			return 0; //Return false to note our failure to find something
		}else{
		ERR_VAR(retval, varName); //Die. either we did not find anything and that was marked as unacceptable in the macro ALLOW_DEFAULTS, or something else wrong happened.
		}
	}
	//check to make sure the variable has only one dim (Critial to assure well-defined behavior for all other ncdf function calls!!!)
	if ((retval = nc_inq_varndims(ncid, *varID, &numberOfDims))){
		ERR_VAR(retval, varName);
	}
	if (numberOfDims>1 || numberOfDims<0){
		printf("Error in %s at %d while trying to read var %s: CARDAMOM ncfd_get_var_info only supports 1 and 0 dimensional variables, but var has %d dimensions.\n", __FILE__, __LINE__, varName,numberOfDims);
		exit(1);
	}
	//begin work in preperation for allocating the array where the variable data will be stored.
	if (numberOfDims==0){
		//special case, var is a scalar, so we know the length of the array is 1
		*arrayLen=1;
		//No need to inspect dimensions or anything else, we have a scalar
		return 1;
	}
	if ((retval = nc_inq_vardimid(ncid, *varID, &dimensionID))){
		ERR_VAR(retval, varName);
	}
	if ((retval = nc_inq_dimlen(ncid, dimensionID, arrayLen))){
		ERR_VAR(retval, varName);
	}
	//If we got here, we were sucessful, so return true
	return 1;
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
	int retval =0; //Return value variable for NCDF calls.
	int varID;
	if ( !ncfd_get_var_info(ncid, varName, arrayLen, &varID)){
		//Return a null pointer due to no content
		return NULL;
	}
	//allocate the actual array we will be returning
	int * resultArray= (int*)calloc(*arrayLen, sizeof(int));

	if ((retval = nc_get_var_int(ncid, varID, resultArray))){
		ERR_VAR(retval, varName);
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
	int retval =0; //Return value variable for NCDF calls.
	int varID;
	if ( !ncfd_get_var_info(ncid, varName, arrayLen, &varID)){
		//Return a null pointer due to no content
		return NULL;
	}
	//allocate the actual array we will be returning
	double * resultArray= (double*)calloc(*arrayLen, sizeof(double));

	if ((retval = nc_get_var_double(ncid, varID, resultArray))){
		ERR_VAR(retval, varName);
	}
	return resultArray;
}




/*
 * Function:  ncdf_read_double_2D
 * --------------------
 * Attempts to read a 2 dimensional double variable that is stored under a particular name in the netCDF file's variables
 *
 *  ncid: netCDF file ID to pull the data from. This is the id given by nc_open after the netCDF file is opened
 *  varName: This is the name of the variable to read
 *  dimLen: pointer to a 2 element array where the total length in number of elements of the resulting arrays will be written (NOT MEMORY SIZE)
 							Note: this memory MUST be allocated allready, and should be of type size_t[2].
 *
 *  returns: a 2D array of all the values of the variable, represented by an array of pointers to arrays. (IE, a double**, or double[][])
 *   if the requested var does not exsist (and ALLOW_DEFAULTS is set), the return value will be
 *   if there is an error, the program exits after displaying a message.
 */
double ** ncdf_read_double_2D(int ncid, const char * varName, size_t * dimLen ){
	int retval =0; //Return value variable for NCDF calls.
	int varID;
	int numberOfDims;
	int dimensionIDs[2] = {-1,-1}; //array where we store the dimensionIDs. default to -1
	//Attempt to locate the id of the approprate variable
	if ((retval = nc_inq_varid(ncid, varName, &varID))){
		//Test if the variable just does not exsist, and if we are allowed to just give a default.
		if (retval ==NC_ENOTVAR && ALLOW_DEFAULTS){
			//We did not find anything, but this is still a valid result. Make sure lengths are set to 0
			dimLen[0]=0;
			dimLen[1]=0;
			return NULL; //Return null to mark failure
		}else{
		ERR_VAR(retval, varName); //Die. either we did not find anything and that was marked as unacceptable in the macro ALLOW_DEFAULTS, or something else wrong happened.
		}
	}
	//check to make sure the variable has only one dim (Critial to assure well-defined behavior for all other ncdf function calls!!!)
	if ((retval = nc_inq_varndims(ncid, varID, &numberOfDims))){
		ERR_VAR(retval, varName);
	}
	if (numberOfDims!=2){
		printf("Error in %s at %d while trying to read var %s: CARDAMOM netCDF was trying to read 2D var, but var has %d dimensions.\n", __FILE__, __LINE__, varName,numberOfDims);
		exit(1);
	}
	//Now store all the dimensionIDs (There should be 2)
	if ((retval = nc_inq_vardimid(ncid, varID, dimensionIDs))){
		ERR_VAR(retval, varName);
	}
	//Dim 1
	if ((retval = nc_inq_dimlen(ncid, dimensionIDs[0], dimLen))){
		ERR_VAR(retval, varName);
	}
	//Dim 2
	if ((retval = nc_inq_dimlen(ncid, dimensionIDs[1], dimLen+1))){ //Beware, dimLen+1 is pointer arithmatic, NOT normal addition
		ERR_VAR(retval, varName);
	}

	size_t startIndexes[2] = {0,0}; //This is just the indexes we want to start at, which we pass to nc_get_vara. We will begin at 0,0, and increment only the first dim
	size_t readCount[2] = {1,-1}; //This is just the number of elements we want to read at a time, which we pass to nc_get_vara.
	readCount[1]= dimLen[1]; //We want to read all the elements that are there, so we set it to the len of the second dim
	//allocate the array we will be returning
	double ** resultArray= (double**)calloc(dimLen[0], sizeof(double*));
	for(int idx=0; idx<dimLen[0]; idx++){
		//Allocate and populate the child arrays that hold the data
		resultArray[idx] = (double*)calloc(dimLen[1], sizeof(double));
		if ((retval = nc_get_vara(ncid, varID, startIndexes, readCount, resultArray[idx] ))){
			ERR_VAR(retval, varName);
		}
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
 *
 *  returns: the value of the variable,
 *   if there is an error, the program exits after displaying a message.
 */
int ncdf_read_single_int_var(int ncid, const char * varName){
	int retval =0; //Return value variable for NCDF calls.
	int varID;
	int result;
	if ((retval = nc_inq_varid(ncid, varName, &varID))){
		if (retval ==NC_ENOTVAR && ALLOW_DEFAULTS ){
			return DEFAULT_INT_VAL;
		}else{
			ERR_VAR(retval, varName);
		}
	}
	size_t arrayLen;
	ncfd_get_var_info(ncid, varName, &arrayLen, &varID);
	//No need to allocate an array, just check the size and die if it is wrong
	if (arrayLen>1){
		printf("Error in %s at %d while trying to read var %s: ncdf_read_single_int_var only supports one value, but var has %lu values.\n", __FILE__, __LINE__, varName,arrayLen);
		exit(1);
	}

	if ((retval = nc_get_var_int(ncid, varID, &result))){
		ERR_VAR(retval, varName);
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
 *
 *  returns: the value of the variable,
 *   if there is an error, the program exits after displaying a message.
 */
double ncdf_read_single_double_var(int ncid, const char * varName ){
	int retval =0; //Return value variable for NCDF calls.
	int varID;
	double result;
	if ((retval = nc_inq_varid(ncid, varName, &varID))){
		if (retval ==NC_ENOTVAR && ALLOW_DEFAULTS ){
			return DEFAULT_DOUBLE_VAL;
		}else{
			ERR_VAR(retval, varName);
		}
	}
	size_t arrayLen;
	ncfd_get_var_info(ncid, varName, &arrayLen, &varID);
	//No need to allocate an array, just check the size and die if it is wrong
	if (arrayLen>1){
		printf("Error in %s at %d while trying to read var %s: ncdf_read_single_double_var only supports one value, but var has %lu values.\n", __FILE__, __LINE__, varName,arrayLen);
		exit(1);
	}
	if ((retval = nc_get_var_double(ncid, varID, &result))){
		ERR_VAR(retval, varName);
	}
	return result;
}




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



//Function for reading these
/*OBS_STRUCT READ_NETCDF_OBS_FIELDS(int ncid, char * OBSNAME){
OBS_STRUCT OBS;
printf("Struct declared OK\n");

printf("OBSNAME = %s\n",OBSNAME);


char uncsf[50],OBSunc[50];
strcpy(OBSunc,OBSNAME);
strcpy(uncsf,"unc");
strcat(OBSunc,uncsf);



size_t unclength;
OBS.unc = ncdf_read_double_var(ncid, OBSunc , &unclength);




OBS.values = ncdf_read_double_var(ncid, OBSNAME , &(OBS.length));
OBS.opt_log_transform=ncdf_read_int_attr(ncid, OBSNAME,"opt_log_transform");
OBS.opt_normalization=ncdf_read_int_attr(ncid, OBSNAME,"opt_normalization");
OBS.opt_mean_only=ncdf_read_int_attr(ncid, OBSNAME,"opt_mean_only");
OBS.opt_structural_error=ncdf_read_int_attr(ncid, OBSNAME,"opt_structural_error");
OBS.min_threshold_value=ncdf_read_double_attr(ncid, OBSNAME,"min_threshold_value");
OBS.single_monthly_unc=ncdf_read_double_attr(ncid, OBSNAME,"single_monthly_unc");
OBS.single_annual_unc=ncdf_read_double_attr(ncid, OBSNAME,"single_annual_unc");
OBS.structural_unc=ncdf_read_double_attr(ncid, OBSNAME,"structural_unc");
return OBS;
};
*/



int CARDAMOM_READ_NETCDF_DATA(char *filename,NETCDF_DATA *DATA)
{
	int retval =0; //Return value variable for NCDF calls.
	int ncid;
	if ((retval = nc_open(filename, NC_NOWRITE, &ncid))){
 		ERR(retval);
 	}

	DATA->ABGB.values=ncdf_read_double_var(ncid, "ABGB", &(DATA->ABGB.length));



	DATA->CH4.values=ncdf_read_double_var(ncid, "CH4", &(DATA->CH4.length));
		DATA->CH4.Uncertainty=ncdf_read_double_attr(ncid, "CH4","Uncertainty");
		DATA->CH4.Annual_Uncertainty=ncdf_read_double_attr(ncid, "CH4","Annual_Uncertainty");
		DATA->CH4.Uncertainty_Threshold=ncdf_read_double_attr(ncid, "CH4","Uncertainty_Threshold");



	DATA->EDC=ncdf_read_single_double_var(ncid, "EDC");

	DATA->EDCDIAG=ncdf_read_single_double_var(ncid, "EDCDIAG");


	

//Read data
DATA->ET=READ_NETCDF_OBS_FIELDS(ncid, "ET");






printf("DATA->ET.values[0] = %2.2f\n",DATA->ET.values[1]);
printf("DATA->ET.unc[0] = %2.2f\n",DATA->ET.unc[1]);
printf("DATA->ET.length[0] = %i\n",(int)DATA->ET.length);
printf("**********************\n");




	//DATA->ET.Uncertainty=ncdf_read_double_attr(ncid, "ET","Uncertainty");
//	DATA->ET.Annual_Uncertainty=ncdf_read_double_attr(ncid, "ET","Annual_Uncertainty");
//	DATA->ET.Uncertainty_Threshold=ncdf_read_double_attr(ncid, "ET","Uncertainty_Threshold");








	DATA->EWT.values=ncdf_read_double_var(ncid, "EWT", &(DATA->EWT.length));
		DATA->EWT.Uncertainty=ncdf_read_double_attr(ncid, "EWT","Uncertainty");
		DATA->EWT.Annual_Uncertainty=ncdf_read_double_attr(ncid, "EWT","Annual_Uncertainty");

	DATA->GPP.values=ncdf_read_double_var(ncid, "GPP", &(DATA->GPP.length));
		DATA->GPP.Uncertainty=ncdf_read_double_attr(ncid, "GPP","Uncertainty");
		DATA->GPP.Annual_Uncertainty=ncdf_read_double_attr(ncid, "GPP","Annual_Uncertainty");
		DATA->GPP.gppabs=ncdf_read_double_attr(ncid, "GPP","gppabs");
		DATA->GPP.Uncertainty_Threshold=ncdf_read_double_attr(ncid, "GPP","Uncertainty_Threshold");

	DATA->ID=ncdf_read_single_double_var(ncid, "ID" );

	DATA->LAI.values=ncdf_read_double_var(ncid, "LAI", &(DATA->LAI.length));

	DATA->LAT=ncdf_read_single_double_var(ncid, "LAT" );

	DATA->Mean_Biomass.values=ncdf_read_double_var(ncid, "Mean_Biomass", &(DATA->Mean_Biomass.length));
		DATA->Mean_Biomass.Uncertainty=ncdf_read_double_attr(ncid, "Mean_Biomass","Uncertainty");

	DATA->Mean_Fire.values=ncdf_read_double_var(ncid, "Mean_Fire", &(DATA->Mean_Fire.length));
		DATA->Mean_Fire.Uncertainty=ncdf_read_double_attr(ncid, "Mean_Fire","Uncertainty");

	DATA->Mean_GPP.values=ncdf_read_double_var(ncid, "Mean_GPP", &(DATA->Mean_GPP.length));
		DATA->Mean_GPP.Uncertainty=ncdf_read_double_attr(ncid, "Mean_GPP","Uncertainty");

	DATA->Mean_LAI.values=ncdf_read_double_var(ncid, "Mean_LAI", &(DATA->Mean_LAI.length));
		DATA->Mean_LAI.Uncertainty=ncdf_read_double_attr(ncid, "Mean_LAI","Uncertainty");

	DATA->CH4.values=ncdf_read_double_var(ncid, "CH4", &(DATA->CH4.length));
		DATA->CH4.Uncertainty=ncdf_read_double_attr(ncid, "CH4","Uncertainty");
		DATA->CH4.Annual_Uncertainty=ncdf_read_double_attr(ncid, "CH4","Annual_Uncertainty");

	DATA->SOM.values=ncdf_read_double_var(ncid, "SOM", &(DATA->SOM.length));
		DATA->SOM.Uncertainty=ncdf_read_double_attr(ncid, "SOM","Uncertainty");

	DATA->NBE.values=ncdf_read_double_var(ncid, "NBE", &(DATA->NBE.length));
		DATA->NBE.Uncertainty=ncdf_read_double_attr(ncid, "NBE","Uncertainty");
		DATA->NBE.Annual_Uncertainty=ncdf_read_double_attr(ncid, "NBE","Annual_Uncertainty");

	DATA->PARPRIORS.values=ncdf_read_double_var(ncid, "PARPRIORS", &(DATA->PARPRIORS.length));
	DATA->PARPRIORUNC.values=ncdf_read_double_var(ncid, "PARPRIORUNC", &(DATA->PARPRIORUNC.length));
	DATA->OTHERPRIORS.values=ncdf_read_double_var(ncid, "OTHERPRIORS", &(DATA->OTHERPRIORS.length));
	DATA->OTHERPRIORSUNC.values=ncdf_read_double_var(ncid, "OTHERPRIORSUNC", &(DATA->OTHERPRIORSUNC.length));

	DATA->SSRD.values=ncdf_read_double_var(ncid, "SSRD", &(DATA->SSRD.length));
		DATA->SSRD.reference_mean=ncdf_read_double_attr(ncid, "SSRD","reference_mean");

	DATA->T2M_MAX.values=ncdf_read_double_var(ncid, "T2M_MAX", &(DATA->T2M_MAX.length));
		DATA->T2M_MAX.reference_mean=ncdf_read_double_attr(ncid, "T2M_MAX","reference_mean");

	DATA->T2M_MIN.values=ncdf_read_double_var(ncid, "T2M_MIN", &(DATA->T2M_MIN.length));
		DATA->T2M_MIN.reference_mean=ncdf_read_double_attr(ncid, "T2M_MIN","reference_mean");

	DATA->TIME_INDEX.values=ncdf_read_double_var(ncid, "TIME_INDEX", &(DATA->TIME_INDEX.length));
		DATA->TIME_INDEX.reference_mean=ncdf_read_double_attr(ncid, "TIME_INDEX","reference_mean");

	DATA->TOTAL_PREC.values=ncdf_read_double_var(ncid, "TOTAL_PREC", &(DATA->TOTAL_PREC.length));
		DATA->TOTAL_PREC.reference_mean=ncdf_read_double_attr(ncid, "TOTAL_PREC","reference_mean");

	DATA->VPD.values=ncdf_read_double_var(ncid, "VPD", &(DATA->VPD.length));
		DATA->VPD.reference_mean=ncdf_read_double_attr(ncid, "VPD","reference_mean");

	DATA->BURNED_AREA.values=ncdf_read_double_var(ncid, "BURNED_AREA", &(DATA->BURNED_AREA.length));
                DATA->BURNED_AREA.reference_mean=ncdf_read_double_attr(ncid, "BURNED_AREA","reference_mean");

	DATA->CO2.values=ncdf_read_double_var(ncid, "CO2", &(DATA->CO2.length));
		DATA->CO2.reference_mean=ncdf_read_double_attr(ncid, "CO2","reference_mean");

	DATA->DOY.values=ncdf_read_double_var(ncid, "DOY", &(DATA->DOY.length));
		DATA->DOY.reference_mean=ncdf_read_double_attr(ncid, "DOY","reference_mean");

/*
	DATA->EDC=ncdf_read_single_double_var(ncid, "EDC", -9999.0);
	DATA->EDCDIAG=ncdf_read_single_double_var(ncid, "EDCDIAG", -9999.0);
	DATA->ET=ncdf_read_single_double_var(ncid, "ET", -9999.0);
	DATA->EWT=ncdf_read_single_double_var(ncid, "EWT", -9999.0);
	DATA->GPP.values=ncdf_read_double_var(ncid, "GPP", &(DATA->GPP.length));
	DATA->GPP.Uncertainty=ncdf_read_double_attr(ncid, "GPP","Uncertainty", -9999.0 );
	DATA->GPP.Annual_Uncertainty=ncdf_read_double_attr(ncid, "GPP","Annual_Uncertainty", -9999.0 );
	DATA->GPP.gppabs=ncdf_read_double_attr(ncid, "GPP","gppabs", -9999.00 );
	DATA->GPP.obs_unc_threshold=ncdf_read_double_attr(ncid, "GPP","obs_unc_threshold", -9999.0 );


	DATA->ID=ncdf_read_single_double_var(ncid, "ID", -9999.0);
	DATA->LAI.values=ncdf_read_double_var(ncid, "LAI", &(DATA->LAI.length));
*/

	return 0;
}
