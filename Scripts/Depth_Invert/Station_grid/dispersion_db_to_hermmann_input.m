% dispersion_db_to_hermmann_input.m

% From Rob Porritt

% Code to convert from a dispersion database (usarray_ant_dispersion.db) in
% the format longitude, latitude, period, groupvel, phasevel
% to the format needed in the Bob Hermmann's joint96 program. 
% The difference between the C program and this version is that this
% version uses a station list file from funclab (station_list.txt) to
% source the location of the station and the station name.
% I should probably use the c code directly for a surf only inversion

datadir = '/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/Station_grid/Alaska_stations_plus_ghost_500_AK_TA';
dispersionDataBaseFileName = '/home/rmartinshort/Documents/Berkeley/Ambient_Noise/Depth_invert/phase_data/500/Alaska_ALL.db';
cd(sprintf('%s',datadir));

% File IO and read.
% This file is generated using the script 'get-stations.py'
stationInfoFileName = 'Alaska_stations.txt';


fid = fopen(stationInfoFileName,'r');
StationInfoCell = textscan(fid,'%f %f %s %s %d');
fclose(fid);

fid = fopen(dispersionDataBaseFileName,'r');
DispersionInfoCell = textscan(fid,'%f %f %f %f');
fclose(fid);
dispersionLongitude = DispersionInfoCell{1};
dispersionLatitude = DispersionInfoCell{2};
dispersionPeriod = DispersionInfoCell{3};
dispersionPhaseVel = DispersionInfoCell{4};

% Make interpolation functions for group and phase velocity
disp('Creating the dispersion functions')
PhaseVelocityFunction = scatteredInterpolant(dispersionLongitude, dispersionLatitude, dispersionPeriod, dispersionPhaseVel);
PhaseVelocityFunction.Method = 'natural';
PhaseVelocityFunction.ExtrapolationMethod = 'nearest';

% Set of periods to look up
periodArray = [8:130];

disp('Begining main loop')
% Loop over stations
for idx=1:length(StationInfoCell{1})
    stlo = StationInfoCell{1}(idx)+360;
    stla = StationInfoCell{2}(idx);
    stnm = StationInfoCell{4}(idx);
    
    mkdir(sprintf('%s',stnm{1}));
    
    fid = fopen([stnm{1}, '/', 'nnall.dsp'],'w');
    
    for iper = 1:length(periodArray)
        per = periodArray(iper);
        phasevel = PhaseVelocityFunction(stlo, stla, per);
        fprintf(fid,'SURF96 R C X 0 %f %f 0.2 20.0\n',per, phasevel);
    end
    
    fclose(fid);
    
end
disp('Done')