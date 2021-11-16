%STep 1. Load US-NR1


nccbffilename1100='../DATA/CARDAMOM_DEMO_DRIVERS_prototype.cbf.nc';

CBF=CARDAMOM_READ_NC_CBF_FILE(nccbffilename1100);

%STep 2. Subset July
CBFsubset=CARDAMOM_SUBSET_NC_CBF_STRUCTURE(CBF, 7);


CBFsubset1100=CBFsubset;

%Step 3. Prescribe only Cfoliar and LAI parameters
MD1100=CARDAMOM_MODEL_LIBRARY(1100);
MD1005=CARDAMOM_MODEL_LIBRARY(1005);


LAI=0.1:0.1:10;
%pars1100=repmat(exp(log(MD.parmin)*0.5+log(MD.parmax)*0.5),[numel(LAI),1]);
pars1100=repmat(exp(log(MD1100.parmin)*0.5+log(MD1100.parmax)*0.5),[numel(LAI),1]);
pars1005=repmat(exp(log(MD1005.parmin)*0.5+log(MD1005.parmax)*0.5),[numel(LAI),1]);


%ones(numel(LAI),54);

g1=2.;
vcmax25=43;
tminmin=-20+273;
tminmax=1+273;
ga=1.0;
%Tupp=315;
Tupp=299;
%Tdown=275;
Tdown=263;
C3_frac=1.;
CI=0.48;
LSA=0.1;
max_infilt=.1;
maxPevap=.5;

pars1100(:,44)=g1;
pars1100(:,45)=vcmax25;
pars1100(:,46)=tminmin;
pars1100(:,47)=tminmax;
pars1100(:,48)=ga;
pars1100(:,49)=Tupp;
pars1100(:,50)=Tdown;
pars1100(:,51)=C3_frac;
pars1100(:,52)=CI;
pars1100(:,53)=LSA;
pars1100(:,35)=max_infilt;
pars1100(:,18)=100;%LCMA
pars1100(:,20)=pars1100(:,18).*LAI';
pars1100(:,63)=maxPevap;

CBR1100=CARDAMOM_RUN_MODEL(CBFsubset1100,pars1100);

CBFsubset1005=CBFsubset;CBFsubset1005.ID.values=1005;

%pars1005=ones(numel(LAI),37);
pars1005(:,17)=100;
pars1005(:,11)=20;
pars1005(:,19)=pars1005(:,17).*LAI';

CBR1005=CARDAMOM_RUN_MODEL(CBFsubset1005,pars1005);


figure(1);clf
subplot(2,2,1)
hold on
plot(LAI,CBR1100.GPP,'b.-');
plot(LAI,CBR1005.GPP,'r.-');
legend('New GPP','ACM GPP')
xlabel('LAI [m2/m2]')
ylabel('GPP [gC/m2/d]')
subplot(2,2,2)
hold on
%plot(LAI,CBR1100.ET,'b.-');
plot(LAI,CBR1100.FLUXES(:,:,33),'b.-');
plot(LAI,CBR1005.ET,'r.-');
legend('New ET','ACM ET')
xlabel('LAI [m2/m2]')
ylabel('ET [mm/d]')
subplot(2,2,3)
hold on
%plot(LAI,CBR1100.ET,'b.-');
plot(LAI,CBR1100.FLUXES(:,:,38),'b.-');
plot(LAI,CBR1100.FLUXES(:,:,39),'r.-');
legend('New T','New E')
xlabel('LAI [m2/m2]')
ylabel('ET [mm/d]')
subplot(2,2,4)
hold on
%plot(LAI,CBR1100.ET,'b.-');
plot(LAI,CBR1100.FLUXES(:,:,54),'b.-');
legend('Soil moisture')
xlabel('LAI [m2/m2]')
ylabel('Soil moisture')

