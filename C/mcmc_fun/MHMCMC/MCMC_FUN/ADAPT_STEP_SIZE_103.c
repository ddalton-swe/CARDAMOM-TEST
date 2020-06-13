#pragma once
int ADAPT_STEP_SIZE_103(double *PARSALL, PARAMETER_INFO PI, COUNTERS N, MCMC_OPTIONS MCO){



/*first adjusting all step sizes*/

int n;
double fac=2, adapfac=(1-(double)N.ACC/(double)MCO.nOUT/2)*(MCO.nADAPT/100)/100+1;

/*
double adapfac=1.5;*/
/*double minstepsize=100./(double)N.ITER;
if (minstepsize>0.01){minstepsize=0.01;}
if (minstepsize<MCO.minstepsize){minstepsize=MCO.minstepsize;}
*/
double minstepsize=MCO.minstepsize;
N.ACCRATE=(double)N.ACCLOC/(double)MCO.nADAPT;

/*N.ACCRATE=(double)N.ACC/(double)N.ITER;*/

/*MAKE THIS COMPATIBLE WITH STRUCTS AND CODE*/

if (N.ACCRATE<0.23){
for (n=0;n<PI.npars;n++){PI.stepsize[n]=PI.stepsize[n]/adapfac;}}
else if(N.ACCRATE>0.23){
for (n=0;n<PI.npars;n++){PI.stepsize[n]=PI.stepsize[n]*adapfac;}}


/*Dimension specific adjustments*/
if (N.ACCLOC>3){

	/*need to remove this!!*/
	double norparstd;
	double *norparvec=calloc(N.ACCLOC,sizeof(double));
	double parrange;
	int p, i;
	/*std of each parameter (for all recently N.ACCLOCepted parameters)*/
	for (p=0;p<PI.npars;p++){
		for (n=0;n<N.ACCLOC;n++){
			/*transforming parameters and storing in pointer*/
			norparvec[n]=par2nor(PARSALL[PI.npars*n+p],PI.parmin[p],PI.parmax[p]);}
			
		/*calculating variability*/
		norparstd=std(norparvec,N.ACCLOC);
		/*reducing if the stepsize is too small*/
 		if (PI.stepsize[p]<norparstd)
		/*disabled step-dim-adapt*/
		{PI.stepsize[p]=PI.stepsize[p]*sqrt(adapfac);}
		else
                {PI.stepsize[p]=PI.stepsize[p]/sqrt(adapfac);}

		}

	free(norparvec);

	}


/*OVERALL CHECK*/
for (n=0;n<PI.npars;n++){
if (PI.stepsize[n]>1){PI.stepsize[n]=PI.stepsize[n]/adapfac;}
if (PI.stepsize[n]<minstepsize){PI.stepsize[n]=PI.stepsize[n]*adapfac;}
if (PI.stepsize[n]<minstepsize){PI.stepsize[n]=minstepsize;}
/*printf("STEPSIZE = %8.6f (minstepsize = %8.6f, Niterations = %10i)\n",PI.stepsize[n],minstepsize,N.ITER);*/
}






return 0;

}





